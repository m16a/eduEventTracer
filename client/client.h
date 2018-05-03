#pragma once


struct IEndPoint
{
	virtual void SendMessage(void*) = 0;
	virtual void OnMsgreceived(void*) = 0;
};



struct IClient : public IEndPoint
{

};


class CClient : public IClient
{
	virtual void SendMessage(void*) override {};
	virtual void OnMsgreceived(void*) override {};
};



