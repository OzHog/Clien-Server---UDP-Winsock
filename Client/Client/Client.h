#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
using namespace std;
// Don't forget to include "ws2_32.lib" in the library list.
#include <winsock2.h> 
#include <string.h>

class Client {
private:
	// Initialize Winsock (Windows Sockets).
	WSAData wsaData;
	SOCKET connSocket;
	sockaddr_in server;

	int bytesSent = 0;
	int bytesRecv = 0;
	char recvBuff[255] = { "\0" };
	int sendBuff[3] = { 0 };

	int port;
	char ip[16];

	const char* massege[13] = { "\0",
		"Get Time", "Get Time Without Date", "Get Tim Since Epoch", "Get Client To Server Delay Estimation",
		"Measure RTT", "Get Time Without Date Or Seconds", "Get Year", "Get Month And Day",
		"Get Seconds Since Beginnig Of Month", "Get Day Of Year", "Get Daylight Savings", "Exit" };

	enum eMassege {
		GET_TIME = 1, GET_TIME_WITOUT_DATE, GET_EPOCH_TIME, GET_DELAY_ESTIMATION,
		MEASURE_RTT, GET_TIME_WITHOUT_SEC, GET_YEAR, GET_MOUNTH_AND_DAY,
		GET_SEC_SINCE_BEG_Of_MOUNTH, GET_DAY_OF_YEAR, GET_DAYLIGHT_SAVINGS, EXIT
	};

	bool sendMsg(bool printBuff);
	bool recvMsg(bool printBuff);
	void disconnect();
	void printMenu();
	void getInput();
	void delayEstimation();
	void measureRTT();

public:
	Client(const char* ip = "127.0.0.1", const int port = 27015) : port(port) {
		strcpy(this->ip, ip);
	};
	~Client() {
		disconnect();
	}

	bool connect();
	void start();
};