#include "event_provider.h"
#include <iostream>

CEventProvider::CEventProvider()
{

}

CEventProvider::~CEventProvider()
{


}

void CEventProvider::PostEvent(void*)
{

}

void CEventProvider::Update()
{

	switch(m_state)
	{
		case EState::Disconnected:
			if (ConnectSync("127.0.0.1", 60000))
				GoToState(EState::Idle);
			else
				std::cout << "Connection failed" << std::endl;
			break;
		case EState::Idle:
			break;
		case EState::Capturing:
			break;
	}

	if (m_state != EState::Disconnected)
		CClient::Update();

}

void CEventProvider::GoToState(EState s)
{
	m_state = s;
	std::cout << "Entering state: " << static_cast<int>(s) << std::endl;
}

void CEventProvider::OnDisconnect()
{
	GoToState(EState::Disconnected);
}
