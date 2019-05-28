#include "Server.h"

bool Server::connect() {
	// Call WSAStartup and return its value as an integer and check for errors.
	// The WSAStartup function initiates the use of WS2_32.DLL by a process.
	// First parameter is the version number 2.2.
	// The WSACleanup function destructs the use of WS2_32.DLL by a process.
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &(this->wsaData)) ) {
		cout << "Time Server: Error at WSAStartup()\n";
		return false;
	}

	// Server side:
	// Create and bind a socket to an internet address.
	// After initialization, a SOCKET object is ready to be instantiated.
	// Create a SOCKET object called m_socket. 
	// For this application:	use the Internet address family (AF_INET), 
	//							datagram sockets (SOCK_DGRAM), 
	//							and the UDP/IP protocol (IPPROTO_UDP).
	this->m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// Check for errors to ensure that the socket is a valid socket.
	// Error detection is a key part of successful networking code. 
	// If the socket call fails, it returns INVALID_SOCKET. 
	// The "if" statement in the previous code is used to catch any errors that
	// may have occurred while creating the socket. WSAGetLastError returns an 
	// error number associated with the last error that occurred.
	if (INVALID_SOCKET == this->m_socket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}

	// Address family (must be AF_INET - Internet address family).
	this->serverService.sin_family = AF_INET;

	// IP address. The sin_addr is a union (s_addr is a unsigdned long (4 bytes) data type).
	// INADDR_ANY means to listen on all interfaces.
	// inet_addr (Internet address) is used to convert a string (char *) into unsigned int.
	// inet_ntoa (Internet address) is the reverse function (converts unsigned int to char *)
	// The IP address 127.0.0.1 is the host itself, it's actually a loop-back.
	this->serverService.sin_addr.s_addr = INADDR_ANY;	//inet_addr("127.0.0.1");

	// IP Port. The htons (host to network - short) function converts an
	// unsigned short from host to TCP/IP network byte order (which is big-endian).
	serverService.sin_port = htons(this->port);

	// Bind the socket for client's requests.
	// The bind function establishes a connection to a specified socket.
	// The function uses the socket handler, the sockaddr structure (which
	// defines properties of the desired connection) and the length of the
	// sockaddr structure (in bytes).
	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR *)&serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		disconnect();
		return false;
	}

	cout << "Server is On!" << endl;
	cout << "Press ESC to close the service." << endl;

	this->serverOn = true;
	return true;
}

void Server::disconnect() {
	cout << "Time Server: Closing Connection." << endl;
	closesocket(m_socket);
	WSACleanup();
}

bool Server::recv(bool printBuff) {
	bytesRecv = recvfrom(m_socket, (char*)recvBuff, 255, 0, &client_addr, &client_addr_len);
	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
		disconnect();
		return false;
	}

	recvBuff[0] = ntohl(recvBuff[0]);
	if(printBuff)
		cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << this->massege[recvBuff[0]] << "\" message.\n";

	return true;
}

bool Server::send(bool printBuff) {
	bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr *)&client_addr, client_addr_len);

	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
		disconnect();
		return false;
	}

	if(printBuff)
		cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	return true;
}

void Server::start() {

	if (this->serverOn) {

	// Get client's requests and answer them.
	// The recvfrom function receives a datagram and stores the source address.
	// The buffer for data to be received and its available size are 
	// returned by recvfrom. The fourth argument is an idicator 
	// specifying the way in which the call is made (0 for default).
	// The two last arguments are optional and will hold the details of the client for further communication. 
	// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).
	cout << "\nTime Server: Wait for clients' requests.\n";

		do {
			if (!recv(false))
				this->connect();
			
			fetchReq();

			if (!send(false))
				this->connect();
			

		} while (!GetAsyncKeyState(VK_ESCAPE));

		cout << "Server is off" << endl;
	}
}

void Server::fetchReq() {

	switch ((eMassege)recvBuff[0])
	{
	case Server::GET_TIME:
		getTime();
		break;

	case Server::GET_TIME_WITOUT_DATE:
		getTimeWithoutDate();
		break;

	case Server::GET_EPOCH_TIME:
		getTimeSinceEpoch();
		break;

	case Server::GET_DELAY_ESTIMATION:
		getClientToServerDelayEstimation();
		break;

	case Server::MEASURE_RTT:
		measureRTT();
		break;

	case Server::GET_TIME_WITHOUT_SEC:
		getTimeWithoutDateOrSeconds();
		break;

	case Server::GET_YEAR:
		getYear();
		break;

	case Server::GET_MOUNTH_AND_DAY:
		getMonthAndDay();
		break;

	case Server::GET_SEC_SINCE_BEG_Of_MOUNTH:
		getSecondsSinceBeginnigOfMonth();
		break;

	case Server::GET_DAY_OF_YEAR:
		getDayOfYear();
		break;

	case Server::GET_DAYLIGHT_SAVINGS:
		getDaylightSavings();
		break;

	default:
		break;
	}

}

void Server::getTime() {
	time(&timer);
	// Parse the current time to printable string.
	struct_time = localtime(&timer);
	strftime(sendBuff, 255, "Current local time: %c", struct_time);
}

void Server::getTimeWithoutDate() {

	time(&timer);
	struct_time = localtime(&timer);
	strftime(sendBuff, 255, "Current local time: %T", struct_time);
}

void Server::getTimeSinceEpoch(){

	time_t epoch = time(nullptr);
	sprintf(sendBuff, "Time Since Epoch: %d secondes", epoch);
}

void Server::getTimeWithoutDateOrSeconds() {
	time(&timer);
	struct_time = localtime(&timer);
	strftime(sendBuff, 255, "Current local time: %R", struct_time);
}

void Server::getYear() {
	time(&timer);
	struct_time = localtime(&timer);
	strftime(sendBuff, 255, "Current year: %Y", struct_time);
}

void Server::getMonthAndDay() {
	time(&timer);
	struct_time = localtime(&timer);
	strftime(sendBuff, 255, "Current local time: %A %B", struct_time);
}

void Server::getSecondsSinceBeginnigOfMonth() {

	struct tm begMonth;
	int seconds;

	time(&timer);
	/* get current time; same as: now = time(NULL)  */
	struct_time = localtime(&timer);
	begMonth = *localtime(&timer);

	begMonth.tm_mday = 1; begMonth.tm_sec = 0; begMonth.tm_min = 0; begMonth.tm_hour = 0;

	seconds = difftime(timer, mktime(&begMonth));
	strftime(sendBuff, 255, "Seconds since beginnig of %B: ", struct_time);
	sprintf(sendBuff + strlen(sendBuff), "%d", seconds);
	cout << "\njjj";
}

void Server::getDayOfYear() {

	time(&timer);
	struct_time = localtime(&timer);

	sprintf(sendBuff, "Day of year: %d", struct_time->tm_yday);
}

void Server::getDaylightSavings() {
	
	time(&timer);
	struct_time = localtime(&timer);

	sprintf(sendBuff, "Daylight Saving Time : %d", struct_time->tm_isdst);
}

void Server::getClientToServerDelayEstimation() {
	double tick = GetTickCount();
	for (int i = 0; i < 200; i++)
	{

	}

	sprintf(sendBuff, "%f", tick);
	//cout << "tick: " << tick << endl;
	cout << "sendBuff: " << sendBuff << endl;

}

void Server::measureRTT() {
	strcpy(sendBuff, "RTT");
}

