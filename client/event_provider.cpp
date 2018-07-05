#include "event_provider.h"
#include <iostream>
#include <unistd.h>

CEventProvider::CEventProvider()
{
	CEndPoint::Bind(EMsgType::StartCapture, this, &CEventProvider::OnStartCapture);
	CEndPoint::Bind(EMsgType::StopCapture, this, &CEventProvider::OnStopCapture);
}

CEventProvider::~CEventProvider()
{


}

void CEventProvider::Update()
{
	sleep(1);
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
			SSampleIntArg a;
			a.val = 42;
			PostEvent(EMsgType::SampleEventInt, a);
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

void CEventProvider::OnHostDisconnect()
{
	GoToState(EState::Disconnected);
}

//callbacks
bool CEventProvider::OnStartCapture(SEmptyArg&)
{
	GoToState(EState::Capturing);
	return true;
}

bool CEventProvider::OnStopCapture(SEmptyArg&)
{
	GoToState(EState::Idle);
	return true;
}
