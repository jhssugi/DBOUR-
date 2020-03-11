#include "precomp_ntlsimulation.h"
#include "NtlSobPlayer.h"

// shared
#include "PcTable.h"

// core
#include "NtlDebug.h"
#include "NtlMath.h"

// presentation
#include "NtlPLEvent.h"
#include "NtlPLSceneManager.h"
#include "NtlPLCharacter.h"
#include "NtlPLPlayerName.h"
#include "NtlPLHelpers.h"

// simulation
#include "TableContainer.h"
#include "NtlSobManager.h"
#include "NtlFSMDef.h"
#include "NtlSLEvent.h"
#include "NtlSLEventFunc.h"
#include "NtlSLGlobal.h"
#include "NtlSobItem.h"
#include "NtlSobPetAttr.h"
#include "NtlSobPet.h"
#include "NtlSobPlayerAttr.h"
#include "NtlSobManager.h"
#include "NtlSobAttrFactory.h"
#include "NtlFSMCharActLayer.h"
#include "NtlBehaviorData.h"
#include "NtlSobCharProxy.h"
#include "NtlSLLogic.h"
#include "NtlSobCharProxyDecoration.h"


CNtlPetBuffer::CNtlPetBuffer()
{
	for(RwInt32 i = 0; i < NTL_MAX_HAVE_PET; i++)
		m_hPet[i] = INVALID_SERIAL_ID;
}

CNtlPetBuffer::~CNtlPetBuffer()
{
}

RwBool CNtlPetBuffer::Create(void)
{
	return TRUE;
}

void CNtlPetBuffer::Destroy(void)
{
}

void CNtlPetBuffer::SetSerialId(RwInt8 byIdx, SERIAL_HANDLE hSerialId)
{
	if(byIdx >= NTL_MAX_HAVE_PET)
		return;

	m_hPet[byIdx] = hSerialId;
}

SERIAL_HANDLE CNtlPetBuffer::GetSerialId(RwInt8 byIdx)
{
	if(byIdx >= NTL_MAX_HAVE_PET)
		return INVALID_SERIAL_ID;

	return m_hPet[byIdx];
}


void CNtlPetBuffer::AddPet(SERIAL_HANDLE hSerialId)
{
	for(RwInt32 i = 0; i < NTL_MAX_HAVE_PET; i++)
	{
		if(m_hPet[i] == INVALID_SERIAL_ID)
			m_hPet[i] = hSerialId;
	}
}

void CNtlPetBuffer::RemovePet(SERIAL_HANDLE hSerialId)
{
	for(RwInt32 i = 0; i < NTL_MAX_HAVE_PET; i++)
	{
		if(m_hPet[i] == hSerialId)
			m_hPet[i] = INVALID_SERIAL_ID;
	}
}

SERIAL_HANDLE CNtlPetBuffer::FindPetFromSkillTable(RwUInt32 uiTblIdx)
{
	for(RwInt32 i = 0; i < NTL_MAX_HAVE_PET; i++)
	{
		if(m_hPet[i] != INVALID_SERIAL_ID)
		{
			CNtlSobPet *pSobPet = reinterpret_cast<CNtlSobPet*>( GetNtlSobManager()->GetSobObject(m_hPet[i]) );
			if(pSobPet == NULL)
				continue;

			CNtlSobPetAttr *pSobPetAttr = reinterpret_cast<CNtlSobPetAttr*>( pSobPet->GetSobAttr() );
			if(pSobPetAttr->m_bySummonSourceType != DBO_OBJECT_SOURCE_SKILL)
				continue;

			if(pSobPetAttr->m_uiSourceTblId == uiTblIdx)
				return m_hPet[i];
		}
	}

	return INVALID_SERIAL_ID;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


DEFINITION_MEMORY_POOL(CNtlSobPlayer)

CNtlSobPlayer::CNtlSobPlayer()
{
	m_charID			= INVALID_SERIAL_ID;
	m_pEquipInventory	= NULL;
    ZeroMemory(&m_sAspsectState, sizeof(m_sAspsectState));

	SetFlags(SLFLAG_ADD_UPDATE | SLFLAG_CAN_SKILLUSE | SLFLAG_CAN_REVIVAL | SLFLAG_OBJECT_COLLISION | SLFLAG_LUA_INTERLOCKING | 
			SLFLAG_LUA_TRANSFORM_ENABLE | SLFLAG_TARGET_SELECT_GUI_UPDATE | SLFLAG_CAN_SWIMMING | SLFLAG_HURT_NOT_HEADING_CHANGE | 
			SLFLAG_FALLING_ENABLE | SLFLAG_CAN_AUTO_FOLLOW_TARGET);
	SetActorFlags(SLFLAG_ACTOR_HAVE_LIFE | SLFLAG_ACTOR_HAVE_MOVEABLE | SLFLAG_ACTOR_HAVE_BATTLE | SLFLAG_ACTOR_HAVE_SOCIAL);

	m_sDogiData.guildId			= INVALID_GUILDID;
	m_sDogiData.byGuildColor	= INVALID_BYTE;
	m_sDogiData.byDojoColor		= INVALID_BYTE;
}

CNtlSobPlayer::~CNtlSobPlayer()
{
}

void CNtlSobPlayer::ServerChangeOut(void)
{
	if(m_pEquipInventory)
	{
		NTL_DELETE(m_pEquipInventory);
		m_pEquipInventory = NULL;
	}

	CNtlSobFeelingActor::ServerChangeOut();
}

void CNtlSobPlayer::ServerChangeIn(void)
{
	m_pEquipInventory = NTL_NEW CNtlInventoryBase;

	CNtlSobFeelingActor::ServerChangeIn();
}

RwBool CNtlSobPlayer::Create(void)
{
	NTL_FUNCTION("CNtlSobPlayer::Create");

	m_pEquipInventory = NTL_NEW CNtlInventoryBase;

	// proxy 설정
	m_pSobProxy = NTL_NEW CNtlSobCharProxy;
	m_pSobProxy->Create(NTL_SLPROXY_COMP_EQUIP_ITEM);
	m_pSobProxy->SetSobObj(this);

	// behavior data
	m_pBeData = NTL_NEW CNtlBeCharData; 

	if(!CNtlSobFeelingActor::Create())
	{
		NTL_RETURN(FALSE);
	}

	// class name 설정.
	SetClassName(SLCLASS_NAME_PLAYER);

	NTL_RETURN(TRUE);
}

void CNtlSobPlayer::Destroy(void)
{
	NTL_FUNCTION("CNtlSobPlayer::Destroy");

	if(m_pEquipInventory)
	{
		NTL_DELETE(m_pEquipInventory);
		m_pEquipInventory = NULL;
	}

	// behavior data
	NTL_DELETE( m_pBeData );

	// actor destroy
	CNtlSobFeelingActor::Destroy();

	NTL_RETURNVOID();
}

void CNtlSobPlayer::Update(RwReal fElapsed)
{
	CNtlSobFeelingActor::Update(fElapsed);
}

void CNtlSobPlayer::SetGuildName(WCHAR* pwcGuildName)
{
	memset(m_awszGuildName, 0, sizeof(m_awszGuildName));

	if(pwcGuildName)
		memcpy(m_awszGuildName, pwcGuildName, sizeof(WCHAR)*NTL_MAX_SIZE_GUILD_NAME_IN_UNICODE);
}

CNtlFSMLayer* CNtlSobPlayer::CreateFSM(void)
{
	CNtlFSMCharActLayer *pFSMLayer = NTL_NEW CNtlFSMCharActLayer;
	pFSMLayer->SetActor(this); 
	pFSMLayer->Create(SLCONTROLLER_PLAYER_SYNC); 
	return pFSMLayer;
}

void CNtlSobPlayer::HandleEvents(RWS::CMsg &pMsg)
{
	NTL_FUNCTION("CNtlSobPlayer::HandleEvents");

	if(pMsg.Id == g_EventSobCreate)
	{
		SNtlEventSobPlayerCreate *pSobPlayerCreate = (SNtlEventSobPlayerCreate*)pMsg.pData; 

		// character ID
		m_charID = pSobPlayerCreate->pPcBrief->charId;

		// Guild name
		SetGuildName(pSobPlayerCreate->pPcBrief->wszGuildName);

		m_sDogiData = pSobPlayerCreate->pPcBrief->sDogi;

		// equip inventory setting
		m_pEquipInventory->HandleEvents(pMsg);

		// game attribute data setting.
		GetSobAttr()->HandleEvents(pMsg);

		// proxy setting
		GetSobProxy()->HandleEvents(pMsg);

		// 좌표와 방향 세팅.
		RwV3d vLoc, vDir;
		RwV3dAssign(&vLoc, &pSobPlayerCreate->vLoc); 
		RwV3dAssign(&vDir, &pSobPlayerCreate->vDir); 

		SWorldHeightStuff sHStuff;
		Logic_GetWorldHeight(this, &vLoc, sHStuff);
		vLoc.y = sHStuff.fFinialHeight;
		
		SetPosition(&vLoc);
		SetDirection(&vDir);
	}
	else if(pMsg.Id == g_EventSobConvertClass ||
            pMsg.Id == g_EventSobChangeAdult)
	{
		GetSobAttr()->HandleEvents(pMsg);
		GetSobProxy()->HandleEvents(pMsg);
		NTL_RETURNVOID();
	}    
	else if(pMsg.Id == g_EventSobEquipChange)
	{
		GetSobAttr()->HandleEvents(pMsg);
	}	
	else if(pMsg.Id == g_EventChangeGuildName)
	{
		SNtlEventChangleGuildName* pEvent = reinterpret_cast<SNtlEventChangleGuildName*>( pMsg.pData ) ;

		if( m_hSerialID == pEvent->hSerialId )
		{
			SetGuildName(pEvent->pwcGuildName);
			Logic_SetHeadNameColor(this);
		}
	}
	else if (pMsg.Id == g_EventMyNameplateChanged)
	{
		SNtlEventMyNameplateMode* pEvent = reinterpret_cast<SNtlEventMyNameplateMode*>(pMsg.pData);

		if (m_hSerialID == pEvent->hSerialId)
		{
			Logic_SetHeadNameColor(this);
		}
	}
	else if (pMsg.Id == g_EventOtherNameplateChanged)
	{
		SNtlEventOtherNameplateMode* pEvent = reinterpret_cast<SNtlEventOtherNameplateMode*>(pMsg.pData);
//		if(m_hSerialID != pEvent->hSerialId)
//			Logic_SetHeadNameColor(this);
	}

	//feeling actor로 Event도 보내준다.
	CNtlSobFeelingActor::HandleEvents(pMsg); 

	NTL_RETURNVOID();
}
