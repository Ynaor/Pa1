#include "channel.h"


int main(int argc, char* argv[])
{

	// Global variables
	int gFlippedBits = 0;
	int senderPort = 0;
	int recieverPort = 0;

	// Local variables
	char messageBuffer[BUFFER_SIZE_BYTES];

	sockaddr_in ReceiverAddr;
	sockaddr_in SenderAddr;
	in_addr ChannelIpAddress;

	SOCKET SenderListenSock;
	SOCKET RecieverListenSock;


	getHostIp(&ChannelIpAddress); // getting the current host ip address -> will be set as server ip

	SenderListenSock   = newSocket(ChannelIpAddress, &SenderAddr, &senderPort, TRUE);
	RecieverListenSock = newSocket(ChannelIpAddress, &ReceiverAddr, &recieverPort, TRUE);

	std::cout << "sender socket: IP address = " << inet_ntoa(ChannelIpAddress);
	std::cout << " port = " << senderPort << "\n";
	std::cout << "Reciever socket: IP address = " << inet_ntoa(ChannelIpAddress);
	std::cout << " port = " << recieverPort << "\n";


	return 0;

}