#include "event_provider.h"



int main(int argc, char const* argv[])
{
	CEventProvider ep;
	
	while (true)	
	{
		ep.Update();	
	}


	return 0;
}
