﻿#include "precomp_dboclient.h"
#include "CharSelecterGui.h"

// core
#include "NtlDebug.h"

// presentation
#include "NtlPLDef.h"
#include "NtlPLGuiManager.h"

// dbo
#include "DboEvent.h"
#include "DboEventGenerator.h"
#include "DialogDefine.h"
#include "DboGlobal.h"
#include "DisplayStringDef.h"
#include "DisplayStringManager.h"
#include "CharStageState.h"
#include "DboLogic.h"
#include "DialogDefine.h"
#include "AlarmManager.h"
#include "LobbyManager.h"

namespace
{
	#define dHEIGHT_GAP				58
};

CCharSelecterGui::CCharSelecterGui(const RwChar* pName)
:CNtlPLGui(pName)
,m_byFocusIndex(INVALID_BYTE)
{

}

CCharSelecterGui::~CCharSelecterGui()
{

}

RwBool CCharSelecterGui::Create()
{
	NTL_FUNCTION( "CCharSelecterGui::Create" );

	if(!CNtlPLGui::Create("", "gui\\CharSelecter.srf", "gui\\CharSelecter.frm"))
		NTL_RETURN(FALSE);

	CNtlPLGui::CreateComponents(CNtlPLGuiManager::GetInstance()->GetGuiManager());

	m_pThis = (gui::CDialog*)GetComponent("dlgMain");

	CRectangle rect;

	// 배경 상단
	// Background top
	m_Background_UP.SetType(CWindowby3::WT_VERTICAL);
	m_Background_UP.SetSurface(0, GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfBackgroundLeft_UP" ));
	m_Background_UP.SetSurface(1, GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfBackgroundCenter_UP" ));
	m_Background_UP.SetSurface(2, GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfBackgroundRight_UP" ));	
	m_Background_UP.SetSize(222, 512);

	// 배경 하단
	// Background bottom
	m_Background_DOWN.SetType(CWindowby3::WT_VERTICAL);
	m_Background_DOWN.SetSurface(0, GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfBackgroundLeft_Down" ));
	m_Background_DOWN.SetSurface(1, GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfBackgroundCenter_Down" ));
	m_Background_DOWN.SetSurface(2, GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharMakePart.srf", "srfBackgroundRight_Down" ));	
	m_Background_DOWN.SetSize(222, 94);

	// Title text surface
	//m_srfTitleText.SetSurface(GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharSelecter.srf", "srfTitle" ));
	rect.SetRectWH(50, 12, 100, 14);
	m_pTitle = NTL_NEW gui::CStaticBox(rect, m_pThis, GetNtlGuiManager()->GetSurfaceManager(), COMP_TEXT_LEFT);
	m_pTitle->CreateFontStd(DEFAULT_FONT, DEFAULT_FONT_SIZE, DEFAULT_FONT_ATTR);
	m_pTitle->SetText(GetDisplayStringManager()->GetString(DST_LOBBY_CHARACTER_LIST));
	m_pTitle->Enable(false);


	RwInt32 iStartButtonY = 50;
	RwInt32 iNameY = 65;
	for( RwUInt8 i = 0 ; i < NTL_MAX_COUNT_USER_CHAR_SLOT ; ++i )
	{
		rect.SetRectWH(13, iStartButtonY, 198, 54);

		m_CharSlot[i].pButton[CHAR_STATE_NORMAL] = (gui::CButton*) NTL_NEW gui::CButton(rect, "",
			GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharSelecter.srf", "srfCharSlotBtnUp" ),
			GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharSelecter.srf", "srfCharSlotBtnDown" ),
			GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharSelecter.srf", "srfCharSlotBtnFoc" ),
			GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharSelecter.srf", "srfCharSlotBtnFoc" ),
			NTL_BUTTON_UP_COLOR, NTL_BUTTON_DOWN_COLOR, NTL_BUTTON_FOCUS_COLOR, NTL_BUTTON_UP_COLOR,
			GUI_BUTTON_DOWN_COORD_X, GUI_BUTTON_DOWN_COORD_Y, m_pThis, GetNtlGuiManager()->GetSurfaceManager() );

		m_CharSlot[i].pButton[CHAR_STATE_RESERVATED_DEL] = (gui::CButton*) NTL_NEW gui::CButton(rect, "",
			GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharSelecter.srf", "srfCharReservatedDelSlotBtnUp" ),
			GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharSelecter.srf", "srfCharReservatedDelSlotBtnDown" ),
			GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharSelecter.srf", "srfCharReservatedDelSlotBtnFoc" ),
			GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharSelecter.srf", "srfCharReservatedDelSlotBtnFoc" ),
			NTL_BUTTON_UP_COLOR, NTL_BUTTON_DOWN_COLOR, NTL_BUTTON_FOCUS_COLOR, NTL_BUTTON_UP_COLOR,
			GUI_BUTTON_DOWN_COORD_X, GUI_BUTTON_DOWN_COORD_Y, m_pThis, GetNtlGuiManager()->GetSurfaceManager() );

		for( RwUInt8 j = 0 ; j < NUM_CHAR_STATE ; ++j )
		{		
			m_CharSlot[i].m_slotButton[j] = m_CharSlot[i].pButton[j]->SigClicked().Connect(this, &CCharSelecterGui::OnClickCharButton);
			m_CharSlot[i].m_slotButtonMove[j]	= m_CharSlot[i].pButton[j]->SigMouseMove().Connect( this, &CCharSelecterGui::OnMouseButtonMove );
			m_CharSlot[i].m_slotButtonLeave[j]	= m_CharSlot[i].pButton[j]->SigMouseLeave().Connect( this, &CCharSelecterGui::OnMouseButtonLeave );
		}		

		// 캐릭터 이름
		// Character name
		rect.SetRectWH(75, iNameY, 120, 14);
		m_CharSlot[i].pName = NTL_NEW gui::CStaticBox( rect, m_pThis, GetNtlGuiManager()->GetSurfaceManager(), COMP_TEXT_CENTER );
		m_CharSlot[i].pName->CreateFontStd(DEFAULT_FONT, 105, DEFAULT_FONT_ATTR);
		m_CharSlot[i].pName->SetTextColor(RGB(255, 255, 255));
		m_CharSlot[i].pName->Clear();
		m_CharSlot[i].pName->Enable(false);

		// 캐릭터 레벨
		// Character level
		rect.SetRectWH(0, iNameY + 17, 200, 14);
		m_CharSlot[i].pLevel = NTL_NEW gui::CStaticBox( rect, m_pThis, GetNtlGuiManager()->GetSurfaceManager(), COMP_TEXT_RIGHT );
		m_CharSlot[i].pLevel->CreateFontStd(DEFAULT_FONT, DEFAULT_FONT_SIZE, DEFAULT_FONT_ATTR);
		m_CharSlot[i].pLevel->SetTextColor(RGB(255, 255, 255));
		m_CharSlot[i].pLevel->Enable(false);		
		
		// New Character 서페이스
		// New Character Surface
		rect.SetRectWH(75, iNameY + 7, 120, 14);
		m_CharSlot[i].pNewCharacter = NTL_NEW gui::CStaticBox(rect, m_pThis, GetNtlGuiManager()->GetSurfaceManager(), COMP_TEXT_LEFT);
		m_CharSlot[i].pNewCharacter->CreateFontStd(DEFAULT_FONT, DEFAULT_FONT_SIZE, DEFAULT_FONT_ATTR);
		m_CharSlot[i].pNewCharacter->SetTextColor(RGB(255, 255, 255));
		m_CharSlot[i].pNewCharacter->SetText(GetDisplayStringManager()->GetString(DST_LOBBY_CHARACTER_MAKE));
		m_CharSlot[i].pNewCharacter->Enable(false);

		// New Character 서페이스 포커스
		// New Character surface focus
		/* NEW CHARACTER FOCUS - NEVER USED IN RELEASE */
		/*
		rect.SetRectWH(75, iNameY, 120, 14);
		m_CharSlot[i].pNewCharacterFoc = NTL_NEW gui::CStaticBox(rect, m_pThis, GetNtlGuiManager()->GetSurfaceManager(), COMP_TEXT_LEFT);
		m_CharSlot[i].pNewCharacterFoc->CreateFontStd(DEFAULT_FONT, DEFAULT_FONT_SIZE, DEFAULT_FONT_ATTR);
		m_CharSlot[i].pNewCharacterFoc->SetTextColor(RGB(255, 255, 255));
		m_CharSlot[i].pNewCharacterFoc->SetText(GetDisplayStringManager()->GetString(DST_LOBBY_CHARACTER_MAKE));
		m_CharSlot[i].pNewCharacterFoc->Enable(false);
		*/
		iNameY			+= dHEIGHT_GAP;
		iStartButtonY	+= dHEIGHT_GAP;
	}

	// 캐릭터 삭제 버튼
	m_pCharDelButton = (gui::CButton*)GetComponent("btnCharDel");
	m_pCharDelButton->SetText( GetDisplayStringManager()->GetString(DST_LOBBY_DEL_CHAR));
	m_slotCharDelButton = m_pCharDelButton->SigClicked().Connect(this, &CCharSelecterGui::OnClickDelCharButton);

	// 캐릭터 복구 버튼
	m_pCharRecoveryButton = (gui::CButton*)GetComponent("btnCharRecovery");
	m_pCharRecoveryButton->SetText( GetDisplayStringManager()->GetString(DST_LOBBY_RECOVERY_CHAR_INFO) );
	m_slotCharRecoveryButton = m_pCharRecoveryButton->SigClicked().Connect(this, &CCharSelecterGui::OnClickRecoveryCharButton);

	rect.SetRectWH(0, 0, 0, 0);
	m_pPostPaintDummy = NTL_NEW gui::CFrame(rect, m_pThis, GetNtlGuiManager()->GetSurfaceManager());


	GetNtlGuiManager()->AddUpdateFunc( this );	


	// sig	
	m_slotPaint			= m_pThis->SigPaint().Connect( this, &CCharSelecterGui::OnPaint );
	m_slotPostPaint		= m_pPostPaintDummy->SigPaint().Connect( this, &CCharSelecterGui::OnPostPaint );

	LinkMsg(g_EventLobbyMessage);
	LinkMsg(g_EventCharStageStateEnter);

	Show(false);

	NTL_RETURN(TRUE);
}

VOID CCharSelecterGui::Destroy()
{
	NTL_FUNCTION("CCharSelecterGui::Destroy");

	GetNtlGuiManager()->RemoveUpdateFuncWithoutRefCheck( this );

	UnLinkMsg(g_EventLobbyMessage);
	UnLinkMsg(g_EventCharStageStateEnter);

	CNtlPLGui::DestroyComponents();
	CNtlPLGui::Destroy();

	NTL_RETURNVOID();
}

VOID CCharSelecterGui::Update(RwReal fElapsed)
{
	SERVER_HANDLE	hServer			= GetLobbyManager()->GetSelectedServerHandle();
	CLobby*			pLobby			= NULL;

	for (RwUInt8 i = 0; i < NTL_MAX_COUNT_USER_CHAR_SLOT; ++i)
	{
		if (!m_CharSlot[i].m_bHaveChar)
		{
			if (!m_CharSlot[i].pNewCharacter->IsVisible())
			{
				m_CharSlot[i].pNewCharacter->Show(true);
			}
			continue;
		}

		if (m_CharSlot[i].pNewCharacter->IsVisible())
		{
			m_CharSlot[i].pNewCharacter->Show(false);
		}

		if (CHAR_STATE_RESERVATED_DEL != m_CharSlot[i].m_byState)
			continue;

		// 꼭 필요한 경우만 딱 한번 로비 정보를 가져오기 위해
		if( !pLobby )
			pLobby = GetLobbyManager()->GetLobby(hServer);
		
		if( !pLobby )
			continue;

		sLOBBY_CHARACTER* pLOBBY_CHARACTER = pLobby->GetCharacter(i);
		if( !pLOBBY_CHARACTER )
			continue;

		if( !pLOBBY_CHARACTER->bReservatedDelete )
			continue;

		RwReal fTime_util_Deletion = pLOBBY_CHARACTER->fTime_util_Deletion;

		if( fTime_util_Deletion < 60 )
		{
			m_CharSlot[i].pLevel->Format(L"%s %d %s",
										GetDisplayStringManager()->GetString(DST_LOBBY_LEFT_DELETE_TIME),
										(RwUInt32)fTime_util_Deletion,
										GetDisplayStringManager()->GetString(DST_TIME_SECOND) );
		}
		else if( fTime_util_Deletion < (60*60) )
		{
			fTime_util_Deletion /= 60;
			fTime_util_Deletion += 1;
			m_CharSlot[i].pLevel->Format(L"%s %d %s",
										GetDisplayStringManager()->GetString(DST_LOBBY_LEFT_DELETE_TIME),
										(RwUInt32)fTime_util_Deletion,
										GetDisplayStringManager()->GetString(DST_TIME_MINUTE) );
		}
		else if( fTime_util_Deletion < (60*60*24) )
		{
			fTime_util_Deletion /= 60;
			fTime_util_Deletion /= 60;
			m_CharSlot[i].pLevel->Format(L"%s %d %s",
										GetDisplayStringManager()->GetString(DST_LOBBY_LEFT_DELETE_TIME),
										(RwUInt32)fTime_util_Deletion,
										GetDisplayStringManager()->GetString(DST_TIME_AN_HOUR) );
		}
		else
		{
			fTime_util_Deletion /= 60;
			fTime_util_Deletion /= 60;
			fTime_util_Deletion /= 24;
			m_CharSlot[i].pLevel->Format(L"%s %d %s",
										GetDisplayStringManager()->GetString(DST_LOBBY_LEFT_DELETE_TIME),
										(RwUInt32)fTime_util_Deletion,
										GetDisplayStringManager()->GetString(DST_TIME_DAY) );
		}
	}
}

VOID CCharSelecterGui::SetPosition(RwInt32 iXPos, RwInt32 iYPos)
{
	m_pThis->SetPosition(iXPos, iYPos);

	RwInt32 iStartX = iXPos;
	RwInt32 iStartY = iYPos;

	m_Background_UP		.SetPosition(iStartX, iStartY);
	m_Background_DOWN	.SetPosition(iStartX, iStartY + 512);
	//m_srfTitleText		.SetPosition(iStartX + 47, iStartY + 9);

	RwInt32 iSlotY = 50;
	for( RwUInt8 i = 0 ; i < NTL_MAX_COUNT_USER_CHAR_SLOT ; ++i )
	{
		m_CharSlot[i].SetPosition(iStartX + 15, iStartY + iSlotY);
		iSlotY += dHEIGHT_GAP;
	}
}

VOID CCharSelecterGui::SelectSlot(RwUInt8 byIndex)
{
	for( RwInt8 i = 0 ; i < NTL_MAX_COUNT_USER_CHAR_SLOT ; ++i )
	{
		if( byIndex != INVALID_BYTE && byIndex == i )
		{
			for( RwUInt8 j = 0 ; j < NUM_CHAR_STATE ; ++j )
			{
				m_CharSlot[i].pButton[j]->ClickEnable(false);
				m_CharSlot[i].pButton[j]->ClickEnable(false);
			}
		}
		else
		{
			for( RwUInt8 j = 0 ; j < NUM_CHAR_STATE ; ++j )
			{
				m_CharSlot[i].pButton[j]->ClickEnable(true);
				m_CharSlot[i].pButton[j]->ClickEnable(true);
			}			
		}
	}
}

VOID CCharSelecterGui::SetDelButton()
{
	SERVER_HANDLE	hServer	= GetLobbyManager()->GetSelectedServerHandle();
	CLobby*			pLobby	= GetLobbyManager()->GetLobby(hServer);
	if( !pLobby )
	{
		DBO_FAIL("Not exist lobby infomation of server handle : " << hServer);
		return;
	}

	RwUInt8 byCharIndex = pLobby->GetSelectedCharacterIndex();

	if( Logic_IsUsableIndex(byCharIndex, NTL_MAX_COUNT_USER_CHAR_SLOT, INVALID_BYTE) &&
		pLobby->GetCharacterCount() > 0 )
	{
		sLOBBY_CHARACTER* pLOBBY_CHARACTER = pLobby->GetCharacter(byCharIndex);
		if( pLOBBY_CHARACTER )
		{
			if( pLOBBY_CHARACTER->bReservatedDelete )
			{
				m_pCharDelButton		->Show(false);
				m_pCharRecoveryButton	->Show(true);
				
				return;
			}
		}
	}

	m_pCharDelButton		->Show(true);
	m_pCharRecoveryButton	->Show(false);
}

VOID CCharSelecterGui::EnableButtons(bool bEnable)
{
	m_pCharDelButton		->ClickEnable(bEnable);
	m_pCharRecoveryButton	->ClickEnable(bEnable);
}

VOID CCharSelecterGui::OnClickCharButton(gui::CComponent* pComponent)
{
	for( RwUInt8 i = 0 ; i < NTL_MAX_COUNT_USER_CHAR_SLOT ; ++i )
	{
		if( FALSE == Logic_IsUsableIndex(m_CharSlot[i].m_byState, NUM_CHAR_STATE) )
			continue;

		if( m_CharSlot[i].pButton[ m_CharSlot[i].m_byState ] == pComponent )
		{			
			if( m_CharSlot[i].m_bHaveChar )
			{
				SERVER_HANDLE	hServer	= GetLobbyManager()->GetSelectedServerHandle();
				CLobby*			pLobby	= GetLobbyManager()->GetLobby(hServer);
				if( !pLobby )
				{
					DBO_FAIL("Not exist lobby infomation of server handle : " << hServer);
					return;
				}

				pLobby->SetSelectedCharacterIndex(i);

				// 캐릭터 모델 바꾸기
				CDboEventGenerator::LobbyEvent(LMT_SELECT_CHARACTER, i);
			}
			else
			{
				// 캐릭터 메이킹 화면으로 이동한다
				GetCharStageState()->ReservateState(CHAR_STATE_MAKE_ENTER);
				GetCharStageState()->ChangeState(CHAR_STATE_SELECT_EXIT);
			}

			break;
		}
	}
}

VOID CCharSelecterGui::OnClickDelCharButton(gui::CComponent* pComponent)
{
	if( GetCharStageState()->GetCurrentState() != CHAR_STATE_SELECT_IDLE )
		return;

	SERVER_HANDLE	hServer	= GetLobbyManager()->GetSelectedServerHandle();
	CLobby*			pLobby	= GetLobbyManager()->GetLobby(hServer);
	if( !pLobby )
	{
		DBO_FAIL("Not exist lobby infomation of server handle : " << hServer);
		return;
	}

	RwUInt8 byCharIndex = pLobby->GetSelectedCharacterIndex();

	if( FALSE == Logic_IsUsableIndex(byCharIndex, NTL_MAX_COUNT_USER_CHAR_SLOT, INVALID_BYTE) ||
		pLobby->GetCharacterCount() == 0 )
		return;

	// 캐릭터를 지우시겠습니까
	GetAlarmManager()->AlarmMessage( DST_LOBBY_DEL_CHARACTER );
}

VOID CCharSelecterGui::OnClickRecoveryCharButton(gui::CComponent* pComponent)
{
	if( GetCharStageState()->GetCurrentState() != CHAR_STATE_SELECT_IDLE )
		return;

	SERVER_HANDLE	hServer	= GetLobbyManager()->GetSelectedServerHandle();
	CLobby*			pLobby	= GetLobbyManager()->GetLobby(hServer);
	if( !pLobby )
	{
		DBO_FAIL("Not exist lobby infomation of server handle : " << hServer);
		return;
	}

	RwUInt8 byCharIndex = pLobby->GetSelectedCharacterIndex();

	if( FALSE == Logic_IsUsableIndex(byCharIndex, NTL_MAX_COUNT_USER_CHAR_SLOT, INVALID_BYTE) ||
		pLobby->GetCharacterCount() == 0 )
		return;

	// 캐릭터를 복구하시겠습니까?
	GetAlarmManager()->AlarmMessage( DST_LOBBY_RECOVERY_CHAR );
}

VOID CCharSelecterGui::OnMouseButtonMove(RwInt32 nFlags, RwInt32 nX, RwInt32 nY)
{
	for( RwUInt8 i = 0 ; i < NTL_MAX_COUNT_USER_CHAR_SLOT ; ++i )
	{
		if( FALSE == Logic_IsUsableIndex(m_CharSlot[i].m_byState, NUM_CHAR_STATE) )
			continue;

		if( m_CharSlot[i].pButton[ m_CharSlot[i].m_byState ]->HasMouseOver() )
		{
			m_byFocusIndex = i;
			break;
		}
		else
		{
			m_byFocusIndex = INVALID_BYTE;
		}
	}
}

VOID CCharSelecterGui::OnMouseButtonLeave(gui::CComponent* pComponent)
{
	m_byFocusIndex = INVALID_BYTE;
}

VOID CCharSelecterGui::OnPaint()
{
	m_Background_UP		.Render();
	m_Background_DOWN	.Render();
	//m_srfTitleText		.Render();
}

VOID CCharSelecterGui::OnPostPaint()
{
	for( RwUInt8 i = 0 ; i < NTL_MAX_COUNT_USER_CHAR_SLOT ; ++i )
	{
		// 아예 그리지 않는 것이 아니라 빈칸 이미지를 그릴지 말지를 결정한다
		// Instead of not drawing at all, decide whether to draw a blank image
		if( i == m_byFocusIndex )
			m_CharSlot[i].Render(true);
		else
			m_CharSlot[i].Render(false);
	}
}

VOID CCharSelecterGui::HandleEvents( RWS::CMsg &msg )
{
	NTL_FUNCTION("CCharCreateGuiGui::HandleEvents");

	if( msg.Id == g_EventLobbyMessage )
	{
		SDboEventLobbyMessage* pEvent = reinterpret_cast<SDboEventLobbyMessage*>( msg.pData );
		switch(pEvent->byMessage)
		{
		case LMT_UPDATE_CHARACTER_LIST:
		case LMT_RENAME_CHARACTER:
			{
				CRectangle		rect;
				wchar_t			acBuffer[20]	= L"";
				SERVER_HANDLE	hServer			= GetLobbyManager()->GetSelectedServerHandle();
				CLobby*			pLobby			= GetLobbyManager()->GetLobby(hServer);
				if( !pLobby )
				{
					DBO_FAIL("Not exist lobby infomation of server handle : " << hServer);
					NTL_RETURNVOID();
				}

				for(RwUInt8 i = 0 ; i < NTL_MAX_COUNT_USER_CHAR_SLOT ; ++i )
				{
					if( i < pLobby->GetCharacterCount() )
					{
						sLOBBY_CHARACTER* pLOBBY_CHARACTER = pLobby->GetCharacter(i);
						if( !pLOBBY_CHARACTER )
						{
							DBO_FAIL("Not exist character of index : " << i);
							continue;
						}

						// 캐릭터 이름
						// Character name
						m_CharSlot[i].pName->SetText(pLOBBY_CHARACTER->tSummary.awchName);

						// 캐릭터 레벨
						// Character level
						if( pLOBBY_CHARACTER->bReservatedDelete )
						{
							m_CharSlot[i].pLevel->SetTextColor(RGB(255, 79, 79));
							m_CharSlot[i].m_byState = CHAR_STATE_RESERVATED_DEL;

							m_CharSlot[i].pButton[CHAR_STATE_NORMAL]->Show(false);
							m_CharSlot[i].pButton[CHAR_STATE_RESERVATED_DEL]->Show(true);
						}
						else
						{
							//swprintf_s(acBuffer, 20, L"%d Lv", pLOBBY_CHARACTER->tSummary.byLevel);
							//m_CharSlot[i].pLevel->SetText(acBuffer);
							if (m_CharSlot[i].pNewCharacter->IsVisible())
								m_CharSlot[i].pNewCharacter->Show(false);
							m_CharSlot[i].pLevel->SetTextColor(RGB(255, 255, 255));
							m_CharSlot[i].pLevel->Format(GetDisplayStringManager()->GetString(DST_LOBBY_CHAR_LEVEL2), pLOBBY_CHARACTER->tSummary.byLevel);
							//
							m_CharSlot[i].m_byState = CHAR_STATE_NORMAL;

							m_CharSlot[i].pButton[CHAR_STATE_NORMAL]->Show(true);
							m_CharSlot[i].pButton[CHAR_STATE_RESERVATED_DEL]->Show(false);
						}

						// 캐릭터 종족
						// Character race
						m_CharSlot[i].srfRace.GetRect(rect);

						switch(pLOBBY_CHARACTER->tSummary.byRace)
						{
						case RACE_HUMAN:
							{
								if( pLOBBY_CHARACTER->bReservatedDelete )
	   	   	   		   				m_CharSlot[i].srfRace.SetSurface(GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharSelecter.srf", "srfHuman_Del" ));
								else
									m_CharSlot[i].srfRace.SetSurface(GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharSelecter.srf", "srfHuman" ));
								break;
							}
						case RACE_NAMEK:
							{
								if( pLOBBY_CHARACTER->bReservatedDelete )
									m_CharSlot[i].srfRace.SetSurface(GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharSelecter.srf", "srfNamek_Del" ));
								else
									m_CharSlot[i].srfRace.SetSurface(GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharSelecter.srf", "srfNamek" ));
								break;
							}
						case RACE_MAJIN:
							{
								if( pLOBBY_CHARACTER->bReservatedDelete )
	   	   	   		   				m_CharSlot[i].srfRace.SetSurface(GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharSelecter.srf", "srfMajin_Del" ));
								else
									m_CharSlot[i].srfRace.SetSurface(GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharSelecter.srf", "srfMajin" ));
								break;
							}
						default:
							{
								DBO_FAIL("LMT_UPDATE_CHARACTER_LIST, Invalid race type : " << pLOBBY_CHARACTER->tSummary.byRace);
								m_CharSlot[i].srfRace.SetSurface(GetNtlGuiManager()->GetSurfaceManager()->GetSurface( "CharSelecter.srf", "srfNamek" ));
								break;
							}
						}
						
						m_CharSlot[i].srfRace.SetRectWH(rect.left, rect.top, m_CharSlot[i].srfRace.GetWidth(), m_CharSlot[i].srfRace.GetHeight());
						m_CharSlot[i].m_bHaveChar = true;
					}
					else
					{
						m_CharSlot[i].pName->Clear();
						m_CharSlot[i].pLevel->Clear();
						m_CharSlot[i].m_bHaveChar = false;
						m_CharSlot[i].m_byState = CHAR_STATE_NORMAL;

						m_CharSlot[i].pButton[CHAR_STATE_NORMAL]->Show(true);
						m_CharSlot[i].pButton[CHAR_STATE_RESERVATED_DEL]->Show(false);

						if (!m_CharSlot[i].pNewCharacter->IsVisible())
							m_CharSlot[i].pNewCharacter->Show(true);
					}
				}

				SetDelButton();

				break;
			}
		case LMT_SELECT_CHARACTER:
			{
				SetDelButton();
				SelectSlot((RwUInt8)pEvent->fValue);
				break;
			}
		}
	}
	else if( msg.Id == g_EventCharStageStateEnter )
	{
		SDboEventCharStageStateEnter* pEvent = reinterpret_cast<SDboEventCharStageStateEnter*>( msg.pData );

		if( pEvent->byState == CHAR_STATE_SELECT_IDLE )
			EnableButtons(true);
		else
			EnableButtons(false);
	}

	NTL_RETURNVOID();
}