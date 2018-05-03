#pragma once

#include "server.h"

class CEventCollector : public IServerMsgListener
{
	public:
	CEventCollector(IServer& s);
	~CEventCollector();
	void StartCapture();
	void StopCapture();

	//obtain data for drawing
	void* GetData() const {};

	//IServerMsgListener
	virtual void OnMsgReceived(void* msg) override;

	private:
	IServer& m_sv;	
};
