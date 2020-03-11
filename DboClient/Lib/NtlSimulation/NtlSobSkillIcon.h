/*****************************************************************************
 *
 * File			: NtlSobSkillIcon.h
 * Author		: HyungSuk, Jang
 * Copyright	: (��)NTL
 * Date			: 2006. 4. 24	
 * Abstract		: Simulation skill icon object
 *****************************************************************************
 * Desc         : 
 *
 *****************************************************************************/

#ifndef __NTL_SOB_SKILL_ICON_H__
#define __NTL_SOB_SKILL_ICON_H__

#include "NtlSobIcon.h"

class CNtlSobSkill;

class CNtlSobSkillIcon : public CNtlSobIcon
{
	DECLEAR_MEMORY_POOL(CNtlSobSkillIcon, NTL_DEFAULT_MEMORY_POOL)

public:

	static RwBool m_bUseReq;
	static RwReal m_fUseResTimeOut;

public:

	CNtlSobSkillIcon();
	virtual ~CNtlSobSkillIcon();

	virtual void HandleEvents(RWS::CMsg &pMsg);

	virtual RwBool IsDragAndDropPossible(void);
	virtual RwBool IsUsePossible(void);
	
	virtual RwBool Use(SERIAL_HANDLE hTargetSerialId, RwUInt32 uiRPBonusType=0xFF);	

	void SetSkillIconState(RwUInt8 byIconState, RwUInt8 byRpBonusType, RwBool bSpecialCoolTimeCheck = FALSE, RwReal fCoolTime = 0.0f );

protected:

	RwBool ApplyTargetCheck( RwUInt32 hTargetSerial );
    
};

#endif
