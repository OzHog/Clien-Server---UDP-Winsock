
#include "Server.h"
using namespace std;


#define TIME_PORT	27015

void main() {
	Server server;
	if (server.connect())
		server.start();
}