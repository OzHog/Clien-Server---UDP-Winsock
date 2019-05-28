#include "Client.h"

bool Client::connect() {

	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Client: Error at WSAStartup()\n";
		return false;
	}

	// Client side:
	// Create a socket and connect to an internet address.
	connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket)
	{
		cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}

	// For a client to communicate on a network, it must connect to a server.
	// Need to assemble the required data for connection in sockaddr structure.
	// Create a sockaddr_in object called server. 
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(this->ip);
	server.sin_port = htons(port);

	return true;
}

void Client::disconnect() {
	closesocket(connSocket);
	WSACleanup();
}

bool Client::sendMsg(bool printBuff) {
	// Asks the server what's the currnet time.
	// The send function sends data on a connected socket.
	// The buffer to be sent and its size are needed.
	// The fourth argument is an idicator specifying the way in which the call is made (0 for default).
	// The two last arguments hold the details of the server to communicate with. 
	// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).
	sendBuff[0] = htonl(sendBuff[0]);
	bytesSent = sendto(connSocket, (char*)sendBuff, sizeof((char*)sendBuff), 0, (const sockaddr *)&server, sizeof(server));
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
		closesocket(connSocket);
		WSACleanup();
		return false;
	}

	sendBuff[0] = ntohl(sendBuff[0]);

	if(printBuff)
		cout << "Time Client: Sent: " << bytesSent << "/" << sizeof((char*)sendBuff) << 
		" bytes of \"" << massege[sendBuff[0]] << "\" message.\n";

	return true;
}

bool Client::recvMsg(bool printBuff) {
	bytesRecv = recv(connSocket, recvBuff, 255, 0);
	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(connSocket);
		WSACleanup();
		return false;
	}

	recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
	if (printBuff)
		cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";

	return true;
}

void Client::printMenu() {
	cout << "Client Menu:" << endl;
	for (int i = 1; i < 13; i++)
		cout << i << ". " << massege[i] << endl;

}

void Client::start() {
	printMenu();

	do {
		getInput();

		switch ((eMassege)sendBuff[0]) {
		case GET_DELAY_ESTIMATION:
			delayEstimation();
			break;

		case MEASURE_RTT:
			measureRTT();
			break;

		default:
			if (!sendMsg(true) || !recvMsg(true)) {
				disconnect();
				connect();
			}
			break;
		}
	} while (sendBuff[0] != EXIT);
}

void Client::getInput() {
	cout << ">> ";
	cin >> sendBuff[0];
}

void Client::delayEstimation() {
	double timeArr[100];
	double diff_time_arr[50];
	int i, j;
	double delay = 0.0;

	for (i = 0; i < 100; i++) {
		if (!sendMsg(false)) {
			cout << "Can't complite the calculation" << endl;
			disconnect();
			connect();
			return;
		}
	}	

	for (i = 0; i < 100; i++) {
		if (!recvMsg(false)) {
			cout << "Can't complite the calculation" << endl;
			disconnect();
			connect();
			return;
		}

		timeArr[i] = atof(recvBuff);
	}

	for (i = 0, j = 0; j < 50; i++, j++) {
		diff_time_arr[j] = timeArr[i + 1] - timeArr[i];
	}
	
	for (i = 0; i < 50; i++) {
		delay += diff_time_arr[i];
	}

	cout << "Delay Estimation:" << delay / 50.0 << " Milliseconds" << endl;


}

void Client::measureRTT() {
	int timeArr[100];
		
	double sendtick, recvtick;
	int i, j;
	double RTT = 0.0;

	for (i = 0; i < 100; i++) {
		if (!sendMsg(false)) {
			cout << "Can't complite the calculation" << endl;
			disconnect();
			connect();
			return;
		}
		sendtick = GetTickCount();

		if (!recvMsg(false)) {
			cout << "Can't complite the calculation" << endl;
			disconnect();
			connect();
			return;
		}

		recvtick = GetTickCount();
		RTT += recvtick - sendtick;
	}


	cout << "Measured RTT: " << RTT / 100 << " Milliseconds" << endl;


}


