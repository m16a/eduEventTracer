#include "event_collector.h"
#include <iostream>

#include <unistd.h>


CEventCollector::CEventCollector()
{
	CEndPoint::Bind(EMsgType::SampleEventInt, this, &CEventCollector::OnSampleEventInt);
}

CEventCollector::~CEventCollector()
{
	//TODO:unsubscribe from msgs
}

void CEventCollector::StartCapture()
{
	PostEvent(EMsgType::StartCapture, SEmptyArg());
	GoToState(EState::Capturing);
}

void CEventCollector::StopCapture()
{
	PostEvent(EMsgType::StopCapture, SEmptyArg());
	GoToState(EState::ListenerExist);
}

void CEventCollector::Update()
{
	CServer::Update();

	switch (m_state)
	{
		case EState::Listening:
			break;
		case EState::ListenerExist:
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

bool CEventCollector::OnSampleEventInt(SSampleIntArg& arg)
{
	std::cout << "Recived sample: " << arg.val << std::endl;
	return true;
}
