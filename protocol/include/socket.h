#pragma once

struct ISocketListener
{
	virtual void OnMsg(void* buff) = 0;
};

class ISocket
{
	public:
	virtual void Connect() = 0;
	virtual void Listen() = 0;
	virtual void Poll() = 0;
	virtual void Sent(void* buff) = 0;

	virtual void SetListener(ISocketListener* p) {m_listener = p;};
	virtual void RemoveListener() {m_listener = nullptr;};
		
	protected:
	ISocketListener* m_listener {nullptr};
};


class CLinuxSocket : public ISocket
{


};
