#pragma once

#include "server.h"

class CEventCollector
{
	public:
	CEventCollector();
	~CEventCollector();
	void StartCapture();
	void StopCapture();

	//obtain data for drawing
	void* GetData() const {};

	private:
	CServer m_sv;	
};
