#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace std;

// Don't forget to include "Ws2_32.lib" in the library list.

#include <iostream>
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include<windows.h> 

class Server {
private:
	// Initialize Winsock (Windows Sockets).
	// Create a WSADATA object called wsaData.
	// The WSADATA structure contains information about the Windows 
	// Sockets implementation.
	WSAData wsaData;

	// Create and bind a socket to an internet address.
	SOCKET m_socket;

	// Need to assemble the required data for connection in sockaddr structure.
	// Create a sockaddr_in object called serverService. 
	sockaddr_in serverService;

	int port;
	bool serverOn = false;

	// Send and receive data.
	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255] = { "\0" };
	int recvBuff[3];

	const char* massege[12] = {"\0",
		"Get Time", "Get Time Without Date", "Get Tim Since Epoch", "Get Client To Server Delay Estimation",
		"Measure RTT", "Get Time Without Date Or Seconds", "Get Year", "Get Month And Day",
		"Get Seconds Since Beginnig Of Month", "Get Day Of Year", "Get Daylight Savings" };
	enum eMassege {
		GET_TIME = 1, GET_TIME_WITOUT_DATE, GET_EPOCH_TIME, GET_DELAY_ESTIMATION,
		MEASURE_RTT, GET_TIME_WITHOUT_SEC, GET_YEAR, GET_MOUNTH_AND_DAY,
		GET_SEC_SINCE_BEG_Of_MOUNTH, GET_DAY_OF_YEAR, GET_DAYLIGHT_SAVINGS
	};

	time_t timer;
	struct tm * struct_time;


	void fetchReq();
	bool recv(bool printBuff);
	bool send(bool printBuff);
	void disconnect();



public:
	Server(const int port = 27015) : port(port) {};
	~Server() {
		// Closing connections and Winsock.
		if (this->serverOn) {
			disconnect();
		}
	}

	

	bool connect();
	void start();
	
	void getTime();
	void getTimeWithoutDate();
	void getTimeSinceEpoch();
	void getClientToServerDelayEstimation();
	void measureRTT();
	void getTimeWithoutDateOrSeconds();
	void getYear();
	void getMonthAndDay();
	void getSecondsSinceBeginnigOfMonth();
	void getDayOfYear();
	void getDaylightSavings();
};