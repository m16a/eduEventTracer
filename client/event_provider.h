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

	virtual void Update() override;

	bool OnStartCapture(SEmptyArg&);

private:
	void GoToState(EState s);
	virtual void OnHostDisconnect() override;

private:
	EState m_state{EState::Disconnected};
};
