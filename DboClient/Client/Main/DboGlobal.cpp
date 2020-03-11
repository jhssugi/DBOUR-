#include "precomp_dboclient.h"
#include "DboGlobal.h"

// shared
#include "NtlClientNet.h"
#include "NtlXMLDoc.h"

// core
#include "NtlDebug.h"

// simulation
#include "NtlNetSender.h"
#include "NtlSobAvatar.h"
#include "NtlSobManager.h"
#include "NtlSobStatusAnimSyncManager.h"
#include "NtlNetSender.h"

// dbo
#include "DboDef.h"
#include "DboVirtualServer.h"
#include "DumpCommand.h"
#include "PacketProc.h"

CDboGlobal*			CDboGlobal::m_pInstance = 0;
const char*			CDboGlobal::DISCORD_APPLICATION_ID = "580180321329479681";
int64_t				CDboGlobal::m_pStartTime = 0;
DiscordRichPresence CDboGlobal::m_pDiscordPresence = {0};
char                CDboGlobal::discordCharName[17] = "";
char                CDboGlobal::discordClassName[20] = "";

#define dCONTRACT_FILE			"script\\CJI_Contract.htm"


CallbackLoadContract g_fnCallback_LoadContract = NULL;

void LinkLoadContract(CallbackLoadContract fn)
{
	g_fnCallback_LoadContract = fn;
}

void UnLinkLoadContract()
{
	g_fnCallback_LoadContract = NULL;
}


void SConnectData::Init(void)
{
	bChangingGameServer = FALSE;

	sAuthCon.Init();
	sCharCon.Init();
	sGameCon.Init();
	sChatCon.Init();

	listCharServer.clear();	
}

void SConnectData::Destroy(void)
{
	ResetCharacterServer();	
}

void SConnectData::ResetCharacterServer(void)
{
	SCharServerInfo *pInfo;
	ListCharServer::iterator it;
	
	for(it = listCharServer.begin(); it != listCharServer.end(); it++)
	{
		pInfo = (*it);
		NTL_DELETE(pInfo);
	}

	listCharServer.clear();
}

void SConnectData::AddCharacterServer(const RwChar *pServerIP, RwUInt16 wServerPort, RwUInt32 uiLoad)
{
	SCharServerInfo *pAddInfo = NTL_NEW SCharServerInfo;

	strcpy_s(pAddInfo->chServerIP, pServerIP);
	pAddInfo->wServerPort	= wServerPort;
	pAddInfo->uiLoad		= uiLoad;
	
	listCharServer.push_back(pAddInfo);
}

RwBool SConnectData::PopCharacterServer(SCharServerInfo *pInfo)
{
	if(listCharServer.size() == 0)
		return FALSE;

	SCharServerInfo *pBestInfo = NULL;

	RwUInt32 uiLoad = 1000000000;
	ListCharServer::iterator it;
	ListCharServer::iterator itbest;
	for(it = listCharServer.begin(); it != listCharServer.end(); it++)
	{
		if((*it)->uiLoad < uiLoad)
		{
			itbest = it;
			pBestInfo = (*it);
		}
	}

	listCharServer.erase(itbest);
	memcpy(pInfo, pBestInfo, sizeof(SCharServerInfo));

	NTL_DELETE(pBestInfo);

	return TRUE;
}

CDboGlobal::CDboGlobal()
:m_bySelectedServerHandle(INVALID_SERVERFARMID)
,m_byPostSelectedServerHandle(INVALID_SERVERFARMID)
,m_pNetwork(NULL)
,m_pLoginNetSender(NULL)
,m_pLobbyNetSender(NULL)
,m_pGameNetSender(NULL)
,m_pChatNetSender(NULL)
,m_pLoginPacketGenerator(NULL)
,m_pLobbyPacketGenerator(NULL)
,m_pGamePacketGenerator(NULL)
,m_pChatPacketGenerator(NULL)
,m_bySelectedCharIndex(INVALID_BYTE)
,m_bEnterTutorial(FALSE)
,m_bDirectionFirstConnect(FALSE)
,m_pDBOUIConfig(NULL)
,m_pContractInfo(NULL)
{
	NTL_PRE(m_pInstance == 0);
	m_pInstance = this;	
	m_sUser.Init();
	memset(&m_sConfig, 0, sizeof(SConfigData));
	ZeroMemory(&m_sDBCSheduleInfo, sizeof(SNtlEventDBCSchedule_Info));
	m_sConnect.Init();
	m_sGame.Init();		
}

CDboGlobal::~CDboGlobal()
{
	if( m_pContractInfo )
	{
		NTL_ARRAY_DELETE(m_pContractInfo->pcContract);
		NTL_ARRAY_DELETE(m_pContractInfo->pwcContract);
		NTL_DELETE(m_pContractInfo);
	}

	m_sConnect.Destroy();
}	

CDboGlobal* CDboGlobal::GetInstance(void)
{
	return m_pInstance;
}



RwBool CDboGlobal::Create(HWND hWnd)
{
	NTL_FUNCTION("CDboGlobal::Create");
	

	InitDiscord();
	//Start by making it naked
	memset(&m_pDiscordPresence, 0, sizeof(m_pDiscordPresence));

	m_pNetwork = NTL_NEW CNtlClientNet;
	if(!m_pNetwork->Create(NULL, NetworkEventHander, NetworkLogCallback))
	{
		DBO_FAIL("Fail create client network");
		NTL_RETURN(FALSE);
	}

	if(!LoadConfigOptions("ConfigOptions.xml"))
	{
		DBO_FAIL("Fail load ConfigOptions.xml");
		NTL_RETURN(FALSE);
	}

	if(!LoadUserOptions("UserOptions.xml"))
	{
		DBO_FAIL("Fail load UserOptions.xml");
		NTL_RETURN(FALSE);
	}

#ifdef NET_SERVER_SYNC
	
	m_pLoginNetSender = NTL_NEW CNtlNetSender;					
	m_pLobbyNetSender = NTL_NEW CNtlNetSender;					
	m_pGameNetSender = NTL_NEW CNtlNetSender;					
	m_pChatNetSender = NTL_NEW CNtlNetSender;					

#else

	m_pLoginNetSender = NTL_NEW CNtlVirtualNetSender;					
	m_pLobbyNetSender = NTL_NEW CNtlVirtualNetSender;					
	m_pGameNetSender = NTL_NEW CNtlVirtualNetSender;					
	m_pChatNetSender = NTL_NEW CNtlVirtualNetSender;	

	CNtlVirtualNetSender::SetVirtualNetSenderFunction(DboVirtualNetSenderFunc); 

#endif

	// net sender callback  등록.
	CNtlNetSender::LinkNotifySendPacketFunc(CallbackNetSendPacket);

	// packet generator 생성.
	m_pLoginPacketGenerator = NTL_NEW CLoginPacketGenerator(m_pLoginNetSender);	
	m_pLobbyPacketGenerator = NTL_NEW CLobbyPacketGenerator(m_pLobbyNetSender);	
	m_pGamePacketGenerator = NTL_NEW CGamePacketGenerator(m_pGameNetSender);	
	m_pChatPacketGenerator = NTL_NEW CChatPacketGenerator(m_pChatNetSender);	

	// packet handler register
	RegisterPacketHandler();

    // DBO UI Config
    m_pDBOUIConfig = NTL_NEW CDBOUIConfig();

	NTL_RETURN(TRUE);
}

void CDboGlobal::Destroy(void)
{
	NTL_FUNCTION("CDboGlobal::Create");

	NTL_DELETE(m_pDBOUIConfig);
	
	if(m_pNetwork)
	{
		NTL_DELETE(m_pNetwork);
	}
	
	// network destroy
	UnRegisterPacketHandler();	
	CNtlNetSender::UnLinkNotifySendPacket();

	if(m_pLoginNetSender)
	{
		NTL_DELETE(m_pLoginNetSender);
	}

	if(m_pLobbyNetSender)
	{
		NTL_DELETE(m_pLobbyNetSender);
	}

	if(m_pGameNetSender)
	{
		NTL_DELETE(m_pGameNetSender);
	}

	if(m_pChatNetSender)
	{
		NTL_DELETE(m_pChatNetSender);
	}

	// packet handler destroy

	if(m_pLoginPacketGenerator)
	{
		NTL_DELETE(m_pLoginPacketGenerator);
	}

	if(m_pLobbyPacketGenerator)
	{
		NTL_DELETE(m_pLobbyPacketGenerator);
	}

	if(m_pGamePacketGenerator)
	{
		NTL_DELETE(m_pGamePacketGenerator);
	}

	if(m_pChatPacketGenerator)
	{
		NTL_DELETE(m_pChatPacketGenerator);
	}

	NTL_RETURNVOID();
}

void CDboGlobal::Reset(void)
{
	m_sGame.Init();
}


RwBool CDboGlobal::LoadConfigOptions(const RwChar *pFileName)
{
	NTL_FUNCTION("CDboGlobal::LoadConfigOptions");

	CNtlXMLDoc doc;
	doc.Create();

	// xml doc load
	if(doc.Load( (char*)pFileName ) == FALSE)
	{
		NTL_RETURN(FALSE);
	}

    char chBuffer[1024] = {0,};
    
	// config operation을 얻어온다.
	IXMLDOMNode* pNode = doc.SelectSingleNode((char*)"/config_options/op");
	
	if(!doc.GetTextWithAttributeName(pNode, "ver", chBuffer, 1024))
	{
		NTL_RETURN(FALSE);
	}

	if(!doc.GetTextWithAttributeName(pNode, "ip", chBuffer, 1024))
	{
		NTL_RETURN(FALSE);
	}

    m_sConfig.strAddr.reserve(256);
	m_sConfig.strAddr = chBuffer;

	if(!doc.GetTextWithAttributeName(pNode, "port", chBuffer, 1024))
	{
		NTL_RETURN(FALSE);
	}

	m_sConfig.dwPort = (DWORD)atoi(chBuffer);

	pNode->Release(); 

	// config local을 얻어온다.

	pNode = doc.SelectSingleNode((char*)"/config_options/local");
	
	if(!doc.GetTextWithAttributeName(pNode, "ver", chBuffer, 1024))
	{
		NTL_RETURN(FALSE);
	}

	if(!doc.GetTextWithAttributeName(pNode, "local_dsp", chBuffer, 1024))
	{
		NTL_RETURN(FALSE);
	}

	m_sConfig.strLocalDsp = chBuffer;

	if(!doc.GetTextWithAttributeName(pNode, "local_sync_dsp", chBuffer, 1024))
	{
		NTL_RETURN(FALSE);
	}

	m_sConfig.strLocalSyncDsp = chBuffer;

    pNode->Release();

    // Bug Trap
    pNode = doc.SelectSingleNode("/config_options/BUGTRAP");
    if(doc.GetTextWithAttributeName(pNode, "ip", chBuffer, 1024))
    {
        m_sConfig.strBugTrapServerIP.reserve(256);        
        m_sConfig.strBugTrapServerIP = chBuffer;
    }

    if(doc.GetTextWithAttributeName(pNode, "port", chBuffer, 1024))
    {
        m_sConfig.dwBugTrapServerPort = atoi(chBuffer);        
    }

    if(doc.GetTextWithAttributeName(pNode, "folder", chBuffer, 1024))
    {
        m_sConfig.strBugTrapFolder.reserve(256);
        m_sConfig.strBugTrapFolder = chBuffer;
    }

	pNode->Release(); 

	// config camera 
	
	NTL_RETURN(TRUE);
}

RwBool CDboGlobal::LoadUserOptions(const RwChar *pFileName)
{
	NTL_FUNCTION("CDboGlobal::LoadUserOptions");

	NTL_RETURN(TRUE);
}

RwBool CDboGlobal::HaveFunction(eDBO_ALLOWED_FUNC_FOR_DEV eAllowFunc)
{
	return Logic_IsExistFlag(m_sConnect.m_acAllowFuncForDev, eAllowFunc, DBO_ALLOWED_FUNC_FOR_DEV_COUNT);
}

void CDboGlobal::SetTargetSerial( SERIAL_HANDLE hSerial )
{
	CNtlSobStatusAnimSyncManager *pAnimSyncManager = GetNtlSobManager()->GetAnimSyncManager();

	if(m_sGame.hTargetSerial != INVALID_SERIAL_ID)
	{
		pAnimSyncManager->RemoveAnimSyncTarget(m_sGame.hTargetSerial);
	}

	if(hSerial != INVALID_SERIAL_ID)
	{
		pAnimSyncManager->AddAnimSyncTarget(hSerial);
	}

	m_sGame.hTargetSerial = hSerial;
}

void CDboGlobal::LoadingContractInfo()
{
	if( m_pContractInfo )
		return;

	RwInt32 iSize = 0;

	m_pContractInfo = NTL_NEW sContractInfo;
	ZeroMemory(m_pContractInfo, sizeof(m_pContractInfo));

	// 파일은 반드시 ANSI로 저장되어 있어야 한다	
	if( g_fnCallback_LoadContract )
	{
		RwChar* pcData = NULL;

		(*g_fnCallback_LoadContract)(dCONTRACT_FILE, (void**)&pcData, &iSize);

		if( !pcData )
		{
			DBO_FAIL("Can not contract file of name : " << dCONTRACT_FILE);
			return;
		}

		m_pContractInfo->pcContract = NTL_NEW RwChar[iSize + 1];
		memcpy(m_pContractInfo->pcContract, pcData, sizeof(RwChar)*iSize);
		m_pContractInfo->pcContract[iSize] = '\0';

		NTL_ARRAY_DELETE(pcData);
	}
	else
	{
		FILE* fp;
		fopen_s(&fp, dCONTRACT_FILE, "rb");
		if(!fp)
		{
			DBO_FAIL("Can not contract file of name : " << dCONTRACT_FILE);
			return;
		}


		fseek(fp, 0, SEEK_END);
		iSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);	

		m_pContractInfo->pcContract = NTL_NEW char[iSize + 1];
		ZeroMemory(m_pContractInfo->pcContract, iSize + 1);
		fread(m_pContractInfo->pcContract, iSize, 1, fp );

		fclose(fp);
	}

	m_pContractInfo->pwcContract = NTL_NEW wchar_t[iSize + 1];
	ZeroMemory(m_pContractInfo->pwcContract, sizeof(wchar_t) * (iSize + 1));
	::MultiByteToWideChar(GetACP(), 0, m_pContractInfo->pcContract, -1, m_pContractInfo->pwcContract, iSize);

	// 버전 정보는 4자리의 숫자
	WCHAR awcVersionBuffer[4];
	ZeroMemory(awcVersionBuffer, sizeof(awcVersionBuffer));
	memcpy(awcVersionBuffer, m_pContractInfo->pwcContract, 4 * sizeof(wchar_t));

	m_pContractInfo->uiContractVersion	= _wtoi(awcVersionBuffer);
	m_pContractInfo->pwcRealContract	= m_pContractInfo->pwcContract + 4;	
}

void CDboGlobal::SetGameContract(RwBool bAccept)
{
	if(m_pContractInfo)
		m_pContractInfo->bAcceptGameContract = bAccept;
}

RwBool CDboGlobal::IsAcceptGameContract()
{
	if( !m_pContractInfo )
		return FALSE;

	return m_pContractInfo->bAcceptGameContract;
}

RwUInt32 CDboGlobal::GetContractVerstion()
{
	if( !m_pContractInfo )
		return 0;

	return m_pContractInfo->uiContractVersion;
}

WCHAR* CDboGlobal::GetContract()
{
	if( !m_pContractInfo )
		return NULL;

	return m_pContractInfo->pwcRealContract;
}

void CDboGlobal::InitDiscord()
{
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	handlers.ready = handleDiscordReady;
	handlers.errored = handleDiscordError;
	handlers.disconnected = handleDiscordDisconnected;
	Discord_Initialize("580180321329479681", &handlers, 1, "");
}

void CDboGlobal::UpdateLoginStage(DiscordRichPresence presence)
{
	presence.state = "At the Login Screen";
	presence.startTimestamp = time(0);
	presence.instance = 1;
	m_pStartTime = presence.startTimestamp;
	Discord_UpdatePresence(&presence);
}

void CDboGlobal::UpdateCharStage(DiscordRichPresence presence)
{
	presence.state = "Selecting Character";
	presence.startTimestamp = m_pStartTime;
	presence.instance = 1;
	Discord_UpdatePresence(&presence);
}

void CDboGlobal::UpdateGameStage(DiscordRichPresence presence, WCHAR charName[17], ePC_CLASS charClass)
{
	char buf1[128] = {};
	char buf2[128] = {};
	
	char charbuf[17] = {};
	wcstombs(charbuf, charName, 17);
	sprintf(buf2, "Playing As %s", charbuf);
	presence.state = buf2;

	sprintf(CDboGlobal::GetInstance()->discordCharName, "%s", charbuf);
	presence.startTimestamp = m_pStartTime;
	presence.largeImageKey = "testimage";
	sprintf(CDboGlobal::GetInstance()->discordClassName, "%s", GetCharClassIcon(charClass).c_str());

	presence.smallImageKey = discordClassName;
	presence.instance = 1;
	Discord_UpdatePresence(&presence);
}

void CDboGlobal::UpdateGameStageLocation(DiscordRichPresence presence, std::wstring zoneName = L"", std::wstring sectorName = L"")
{
	char buf1[128] = {};
	char buf2[128] = {};
	char SectorName[64] = {};
	std::wstring detail = zoneName + L":" + sectorName;
	wcstombs(SectorName, detail.c_str(), detail.size());
		
	sprintf(buf2, "Playing As %s", CDboGlobal::discordCharName);
	sprintf(buf1, "%s", SectorName);
	presence.state = buf2;
	presence.details = buf1;
	presence.startTimestamp = m_pStartTime;
	presence.largeImageKey = "testimage";
	presence.smallImageKey = discordClassName;

	presence.instance = 1;
	Discord_UpdatePresence(&presence);
}
std::string CDboGlobal::GetCharClassIcon(ePC_CLASS charClass)
{
	std::string ClassName = "";

	switch (charClass)
	{
		case PC_CLASS_HUMAN_FIGHTER:
		{
			ClassName = "hfi";
			break;
		}
		case PC_CLASS_HUMAN_MYSTIC:
		{
			ClassName = "hmy";
			break;
		}
		case PC_CLASS_NAMEK_FIGHTER:
		{
			ClassName = "nfi";
			break;
		}
		case PC_CLASS_NAMEK_MYSTIC:
		{
			ClassName = "nmy";
			break;
		}
		case PC_CLASS_MIGHTY_MAJIN:
		{
			ClassName = "mfi";
			break;
		}
		case PC_CLASS_WONDER_MAJIN:
		{
			ClassName = "mmy";
			break;
		}
		case PC_CLASS_STREET_FIGHTER:
		{
			ClassName = "fighter";
			break;
		}
		case PC_CLASS_SWORD_MASTER:
		{
			ClassName = "swordsman";
			break;
		}
		case PC_CLASS_CRANE_ROSHI:
		{
			ClassName = "crane";
			break;
		}
		case PC_CLASS_TURTLE_ROSHI:
		{
			ClassName = "turtle";
			break;
		}
		case PC_CLASS_DARK_WARRIOR:
		{
			ClassName = "darkwarrior";
			break;
		}
		case PC_CLASS_SHADOW_KNIGHT:
		{
			ClassName = "shadowknight";
			break;
		}
		case PC_CLASS_DENDEN_HEALER:
		{
			ClassName = "dende";
			break;
		}
		case PC_CLASS_POCO_SUMMONER:
		{
			ClassName = "poko";
			break;
		}
		case PC_CLASS_ULTI_MA:
		{
			ClassName = "ultimate";
			break;
		}
		case PC_CLASS_GRAND_MA:
		{
			ClassName = "chef";
			break;
		}
		case PC_CLASS_PLAS_MA:
		{
			ClassName = "plasma";
			break;
		}
		case PC_CLASS_KAR_MA:
		{
			ClassName = "karma";
			break;
		}
		default:
		{
			ClassName = "testimage";
			break;
		}
	}


	return ClassName;
}


