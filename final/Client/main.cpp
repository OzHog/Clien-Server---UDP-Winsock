

#include "Client.h"

using namespace std;

#define TIME_PORT	27015

int main()
{
	Client client;
	if (client.connect())
		client.start();
	return 0;
}