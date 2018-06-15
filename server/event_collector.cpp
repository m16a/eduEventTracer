#include "event_collector.h"
#include <iostream>

#include <unistd.h>


CEventCollector::CEventCollector()
{
}

CEventCollector::~CEventCollector()
{
	//TODO:unsubscribe from msgs
}

void CEventCollector::StartCapture()
{

}

void CEventCollector::StopCapture()
{

}

void CEventCollector::Update()
{
	sleep(1);
	CServer::Update();

	switch (m_state)
	{
		case EState::Listening:
			break;
		case EState::ListenerExist:
			PostEvent(EMsgType::StartCapture, SEmptyArg());
			break;
		case EState::Capturing:
			break;
			
	}

}


void CEventCollector::GoToState(EState s)
{
	m_state = s;
	std::cout << "Entering state: " << static_cast<int>(s) << std::endl;
}

void CEventCollector::OnNewListener()
{
	GoToState(EState::ListenerExist);
}

void CEventCollector::OnListenerDisonnected()
{
	GoToState(EState::Listening);
}
