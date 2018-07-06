#include "event_provider.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main()
{
	CEventProvider ep;
	srand(time(NULL));
	while (true)	
	{
		struct timespec t, empty;
		t.tv_sec = 0; 
		int ran = rand() % 300 + 100;
		t.tv_nsec = ran * 1000000L;
		nanosleep(&t, &empty);

		ep.Update();	

		SSampleIntArg a;
		a.val = ran;
		ep.PostEvent(EMsgType::SampleEventInt, a);
	}


	return 0;
}
