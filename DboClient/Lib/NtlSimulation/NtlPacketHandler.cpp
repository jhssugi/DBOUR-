#include "precomp_ntlsimulation.h"
#include "NtlPacketHandler.h"

// shared
#include "NtlPacketCommon.h"

// core
#include "NtlDebug.h"

// simulation
#include "NtlSLEvent.h"
#include "NtlSLEventFunc.h"


CNtlPacketHandler::MapHandler CNtlPacketHandler::m_mapHandler;

void CNtlPacketHandler::Register(WORD wProtocolOp, NotifyPacketHander fp)
{
	m_mapHandler[wProtocolOp] = fp;
}

void CNtlPacketHandler::UnRegister(WORD wProtocolOp)
{
	MapHandler::iterator it;
	it = m_mapHandler.find(wProtocolOp);
	if(it != m_mapHandler.end())
		m_mapHandler.erase(it); 
}

void CNtlPacketHandler::DetectPacketHeader(void *pPacket)
{
	sNTLPACKETHEADER *pHeader = (sNTLPACKETHEADER*)pPacket;
	WORD wOpCode = pHeader->wOpCode;

	MapHandler::iterator it;
	it = m_mapHandler.find(wOpCode);
	if(it != m_mapHandler.end())
	{
		NotifyPacketHander fp = (*it).second;
		(*fp)(pPacket);
	}

	CNtlSLEventGenerator::NotifyReceivePacket(pPacket); 
}

