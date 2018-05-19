#pragma once

#include "protocol.h"
#include "socket.h"
#include "dispatcher.h"
#include "ser.h"
#include <memory>


class CEndPoint : public ISocketListener
{
	public:
	CEndPoint();
	~CEndPoint();
	template <class TArg>
	void PostEvent(EMsgType t, TArg& data)
	{
		Ser ser;	
		ser.isReading = false;
		data.Serialize(ser);
		m_pSock->Send(ser.buffer);
	}

	template<class argType>
	void Bind(EMsgType type, bool(*callback)(argType))
	{
		m_dispatcher.Bind(type, callback);
	}

	bool ConnectSync(const char* ip, int port);

	void Listen();
	void Update();

	private:
	virtual void OnMsg(TBuff& buff) override;
	virtual void OnNewListener() override {};

	private:
	
	std::unique_ptr<ISocket> m_pSock;	
	CDispatcher m_dispatcher;	

};
