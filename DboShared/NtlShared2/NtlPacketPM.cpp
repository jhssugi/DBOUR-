#include "StdAfx.h"
#include "NtlPacketPM.h"


const char * s_packetName_PM[] =
{
	DECLARE_PACKET_NAME( PM_NOTIFY_SERVER_BEGIN ),
	DECLARE_PACKET_NAME( PM_HEARTBEAT ),

	DECLARE_PACKET_NAME( PM_SERVERFARM_NAME_REQ ),

	DECLARE_PACKET_NAME( PM_SERVER_CONTROL_TURN_ON_REQ ),
	DECLARE_PACKET_NAME( PM_SERVER_CONTROL_TURN_OFF_REQ ),
	DECLARE_PACKET_NAME( PM_SERVER_LOCK_REQ ),
	DECLARE_PACKET_NAME( PM_SERVER_UNLOCK_REQ ),
	DECLARE_PACKET_NAME( PM_SERVER_USER_ALL_KICK_REQ ),
	DECLARE_PACKET_NAME( PM_SERVER_AGENT_PATCH_REQ ),
	DECLARE_PACKET_NAME( PM_SERVER_COMMAND_REQ ),
	DECLARE_PACKET_NAME( PM_SERVER_CONTROL_TURN_OFF_ALL_NFY ),
	DECLARE_PACKET_NAME( PM_REPORT_LOAD ),
	DECLARE_PACKET_NAME( PM_SERVER_STATUS_LOCALSETTING_INFO_REQ ),
	DECLARE_PACKET_NAME( PM_SERVER_STATUS_LOCALSETTING_EDIT_REQ ),
	DECLARE_PACKET_NAME( PM_PING_REQ ),

};


//------------------------------------------------------------------
//
//------------------------------------------------------------------
const char * NtlGetPacketName_PM(WORD wOpCode)
{
	if( wOpCode < PM_OPCODE_BEGIN )
	{
		return "NOT DEFINED PACKET NAME : OPCODE LOW";
	}

	if( wOpCode > PM_OPCODE_END )
	{
		return "NOT DEFINED PACKET NAME : OPCODE HIGH";
	}

	int nIndex = wOpCode - PM_OPCODE_BEGIN;
	if( nIndex >= _countof( s_packetName_PM) )
	{
		return "OPCODE BUFFER OVERFLOW";
	}

	return s_packetName_PM[ nIndex ];
}