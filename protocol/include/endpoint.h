#pragma once

#include "protocol.h"
#include "socket.h"
#include "dispatcher.h"
#include <memory>


struct CDispatcher;

class CEndPoint : public ISocketListener
{
	public:
	template <class TArg>
	void PostEvent(EMsgType t, TArg& data)
	{

	}

	template<class argType>
	void Bind(EMsgType type, bool(*callback)(argType))
	{
		m_dispatcher.Bind(type, callback);
	}

	void Connect(){};
	void Listen(){};
	void Poll(){};

	private:
	virtual void OnMsg(TBuff& buff) override{};

	private:
	
	std::unique_ptr<ISocket> pSock;	
	CDispatcher m_dispatcher;	

};
