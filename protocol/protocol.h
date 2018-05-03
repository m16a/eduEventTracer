#pragma once

enum EMsgType
{
	StartCapture,
	StopCapture,
	Event
}


struct IMsg
{
	EMsgType type;
	size_t size;
	void* buffer;
}



/*
msgProvider->postEvent(timestamp, structA, structB, ...)
{
	void*	buff = malloc();
	f(buff, timestamp, structA, structB)

	send_to_sock(buff)


	free(buff);

}


Serialize(T ser, agrs)
{


}


onEvent(timestamp, structA, structB, ...)
*/
