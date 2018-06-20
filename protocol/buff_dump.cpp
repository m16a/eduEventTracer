#include "buffer.h"
#include <stdio.h>

void Dump(TBuff& buff)
{
	for (int i=0; i < buff.size(); ++i)
	{
		char c = buff[i];

		printf("%02x ", buff[i]);

		if (i % 20 == 0 && i > 0)
			printf("\n");
	}
		printf("\n");
}
