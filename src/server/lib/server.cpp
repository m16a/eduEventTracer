#include "server.h"
#define SERVER_PORT 60000
CServer::CServer() : CEndPoint(SERVER_PORT) { Listen(); }
