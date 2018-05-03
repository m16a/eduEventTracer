#pragma once

struct IServerMsgListener
{
	virtual void OnMsgReceived(void* msg) = 0;
};

struct IServer
{
	virtual void SendMessage(void*) = 0;
	virtual void AddListener(IServerMsgListener*){};
	virtual void RemoveListener(IServerMsgListener*){};
};

class CServer : public IServer
{
	virtual void SendMessage(void*) override {};
	virtual void AddListener(IServerMsgListener*) override {};
	virtual void RemoveListener(IServerMsgListener*) override {};
	
	IServerMsgListener* m_delegate;
};







