#include "event_collector.h"


CEventCollector::CEventCollector(IServer& s) : m_sv(s)
{
	//TODO:subscribe to msgs
}

CEventCollector::~CEventCollector()
{
	//TODO:unsubscribe from msgs
}

void CEventCollector::OnMsgReceived(void* msg)
{

}

void CEventCollector::StartCapture()
{

}

void CEventCollector::StopCapture()
{

}

