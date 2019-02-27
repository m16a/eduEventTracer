#include "client.h"
#define SERVER_PORT 60000
CClient::CClient() : CEndPoint(SERVER_PORT) { Listen(); }
