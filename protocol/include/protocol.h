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

