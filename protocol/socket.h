#pragma once

struct ISocketListener
{
	virtual void OnMsg(void* buff) = 0;
};

struct ISocket
{
	virtual void Connect() = 0;
	virtual void Listen() = 0;
	virtual void Poll() = 0;
	virtual void Sent(void* buff) = 0;

	virtual void SetListener(ISocketListener*) = 0;
	virtual void RemoveListener() = 0;
};
