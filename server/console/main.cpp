#include "event_collector.h"

int main()
{
	CEventCollector	ec;
	while(true)
	{
		ec.Update();
	}

	return 0;
}
