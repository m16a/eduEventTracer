#include "buffer.h"
#include <stdio.h>
#include <iostream>

void Dump(TBuff& buff)
{
	for (int i=0; i < buff.size(); ++i)
	{
		char c = buff[i];

		char tmpStr[10];
		snprintf(tmpStr, sizeof(buff), "%02x ", c);
		std::cout << tmpStr;

		if (i % 20 == 0 && i > 0)
			std::cout << std::endl;
	}
	std::cout << std::endl;
}
