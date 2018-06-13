#pragma once

#include "server.h"

class CEventCollector : public CServer
{
	enum class EState
	{
		Listening,
		ListenerExist,
		Capturing
	};

	public:
	CEventCollector();
	~CEventCollector();
	void StartCapture();
	void StopCapture();
	virtual void Update() override;

	//obtain data for drawing
	void* GetData() const {};

	virtual void OnNewListener() override;

	private:
		void GoToState(EState s);
	private:
	EState m_state {EState::Listening};
};
