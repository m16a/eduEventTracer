#include "event_provider.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <chrono>

void FakePayload()
{
	struct timespec t, empty;
	t.tv_sec = 0; 
	int ran = rand() % 300 + 100;
	t.tv_nsec = ran * 1000000L;
	nanosleep(&t, &empty);
}

int main()
{
	CEventProvider ep;
	srand(time(NULL));
	while (true)	
	{
		std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

		int startMs = std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch()).count();
		FakePayload();

		std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
		int endMs = std::chrono::duration_cast<std::chrono::milliseconds>(end.time_since_epoch()).count();
		
		//int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		ep.Update();	

		STimeIntervalArg a;
		a.startTime = startMs;
		a.endTime = endMs;

		ep.PostEvent(EMsgType::TimeInterval, a);
	}

	return 0;
}
