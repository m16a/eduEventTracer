#pragma once

#include "client.h"


class CEventProvider
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
	void Update();


private:
	void GoToState(EState s);

private:

	CClient m_cl;
	EState m_state{EState::Disconnected};
};
