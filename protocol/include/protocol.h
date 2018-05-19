#pragma once

#include "ser.h"

enum EMsgType
{
	StartCapture,
	StopCapture,
	Event,
	MsgCnt
};


bool Serialize(Ser& ser);



/*

bool OnMessage(socket& s)
{
	
	type t = s.readType();

	hndlr = hndlrs[t];


	T args;

	deserialize(s.data(), args);

	listener->callback(t, args);


}


struct IMsg
{
	EMsgType type;
	size_t size;
	void* buffer;
}
*/



/*
 
User side

{
	msgProvider->postEvent(MsgType type, agrs, ...);
}



onEvent(MsgType type, agrs, ...)
{

}


---------------------------------------------------------------------------
msgProvider::postEvent(timestamp, structA, structB, ...)
{
	void*	buff = malloc();
	size_t sz = 0;
	Serialize(buff, sz, type, timestamp, structA, structB)

	send_to_sock(buff, sz);


	free(buff);

}




*/




