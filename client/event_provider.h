#pragma once

#include "client.h"


class CEventProvider : public CClient
{
	enum class EState
	{
		Disconnected,
		Idle,
		Capturing
	};

public:
	CEventProvider();
	~CEventProvider();

	void PostEvent(void*);
	virtual void Update() override;


private:
	void GoToState(EState s);
	virtual void OnDisconnect() override;

private:
	EState m_state{EState::Disconnected};
};
