#include "StdAfx.h"
#include "NtlPacketQC.h"


const char * s_packetName_QC[] =
{
	DECLARE_PACKET_NAME( QC_CHECK_AUTH_KEY_RES ),
	DECLARE_PACKET_NAME( QC_NOTIFY_SERVER_BEGIN_ACK ),
	DECLARE_PACKET_NAME( QC_CHARACTER_INFO_RES ),
	DECLARE_PACKET_NAME( QC_CHARACTER_ADD_RES ),
	DECLARE_PACKET_NAME( QC_CHARACTER_DEL_RES ),
	DECLARE_PACKET_NAME( QC_CHARACTER_LOAD_RES ),
	DECLARE_PACKET_NAME( QC_CONNECT_WAIT_CHECK_RES ),
	DECLARE_PACKET_NAME( QC_CONNECT_WAIT_COUNT_NFY ),
	DECLARE_PACKET_NAME( QC_CONNECT_WAIT_CANCEL_RES ),
	DECLARE_PACKET_NAME( QC_CONNECT_WAIT_COMMUNITY_SERVER_DOWN_NFY ),
	DECLARE_PACKET_NAME( QC_CHARACTER_RENAME_RES ),
	
};


//------------------------------------------------------------------
//
//------------------------------------------------------------------
const char * NtlGetPacketName_QC(WORD wOpCode)
{
	if( wOpCode < QC_OPCODE_BEGIN )
	{
		return "NOT DEFINED PACKET NAME : OPCODE LOW";
	}

	if( wOpCode > QC_OPCODE_END )
	{
		return "NOT DEFINED PACKET NAME : OPCODE HIGH";
	}

	int nIndex = wOpCode - QC_OPCODE_BEGIN;
	if( nIndex >= _countof( s_packetName_QC) )
	{
		return "OPCODE BUFFER OVERFLOW";
	}

	return s_packetName_QC[ nIndex ];
}