#pragma once

#include <netinet/in.h>

struct ISocketListener
{
	virtual void OnMsg(void* buff) = 0;
	virtual void OnNewListener() = 0;
};

class ISocket
{
	public:
	virtual bool ConnectSync(const char* ip, int port) = 0;
	virtual void Listen() = 0;
	virtual void Poll() = 0;
	virtual void Sent(void* buff) = 0;

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
	virtual void Poll() override;
	virtual void Sent(void* buff) override;

	private:
	int m_sock { 0 };
	struct sockaddr_in m_address;
};
