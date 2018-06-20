#pragma once

#include "buffer.h"
#include <string.h>

enum EMsgType
{
	StartCapture,
	StopCapture,
	SampleEventInt,
	MsgCnt
};

struct SEmptyArg
{
	void Serialize(Ser& ser){}
};

bool Serialize(Ser& ser);


struct SSampleIntArg
{
	int val;
	void Serialize(Ser& ser)
	{
		if (ser.isReading)
		{
			int* pVal = reinterpret_cast<int*>(ser.buffer.data());
			val = *pVal;
		}
		else
		{
			ser.buffer.resize(4);
			memcpy(ser.buffer.data(), &val, 4);
		}
	}
};

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




