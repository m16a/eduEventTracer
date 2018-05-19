#include "endpoint.h"

CEndPoint::CEndPoint()
{
	m_pSock = std::make_unique<CLinuxSocket>();
	m_pSock->SetListener(this);
}

CEndPoint::~CEndPoint()
{
	m_pSock->RemoveListener();
}

bool CEndPoint::ConnectSync(const char* ip, int port)
{
	m_pSock->ConnectSync(ip, port);	
}

void CEndPoint::Listen()
{
	m_pSock->Listen();
}

void CEndPoint::Update()
{
	m_pSock->Update();
}

void CEndPoint::OnMsg(TBuff& buff)
{
	EMsgType type;
	type = static_cast<EMsgType>(buff[0]);
	buff.erase(buff.begin());//TODO: memory shifting
	m_dispatcher.OnMsg(type, buff);
}

