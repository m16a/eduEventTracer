#pragma once

#include "defines.h"
#include <netinet/in.h>
#include <sys/select.h>

constexpr size_t kMaxClients = 1;

struct ISocketListener
{
	virtual void OnMsg(TBuff& buff) = 0;
	virtual void OnNewListener() = 0;
};

class ISocket
{
	public:
	virtual bool ConnectSync(const char* ip, int port) = 0;
	virtual void Listen() = 0;
	virtual void Update() = 0;
	virtual void Send(TBuff& buff) = 0;

	virtual void SetListener(ISocketListener* p) {m_listener = p;};
	virtual void RemoveListener() {m_listener = nullptr;};
		
	protected:
	ISocketListener* m_listener {nullptr};
};

//TODO:  move to separate file
class CLinuxSocket : public ISocket
{
	public:
	CLinuxSocket();
	virtual bool ConnectSync(const char* ip, int port) override;
	virtual void Listen() override;
	virtual void Update() override;
	virtual void Send(TBuff& buff) override {};

	private:
	int m_sock { 0 };
	struct sockaddr_in m_address;
	int m_client_sockets[kMaxClients];

	fd_set m_readfds;
};
