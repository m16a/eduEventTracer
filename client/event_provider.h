#pragma once

#include "client.h"

class CEventProvider
{
public:
	CEventProvider(IClient& c);
	~CEventProvider();

	void PostEvent(void*);

private:
	IClient& m_cl;
};
