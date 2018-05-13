#pragma once

#include "protocol.h"
#include "socket.h"
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
	}

	void Connect(){};
	void Listen(){};
	void Poll(){};

	private:
	virtual void OnMsg(void* buff) override{};

	private:
	

	std::unique_ptr<ISocket> pSock;	
	std::unique_ptr<CDispatcher> pDispatcher;	


};
