#include "channel.h"


// ********************************************
// **********  Socket Handling   **************
// ********************************************

// Global variables
int gFlippedBits = 0;



// Initializing the winsock, will throw an error and exit if failed
void WinsockInit(WSADATA *wsaData)
{
	const auto api_ver = MAKEWORD(2, 2);
	if (WSAStartup(api_ver, wsaData) != NO_ERROR)
	{
		std::cerr << "Winsock initialization failed\n" << WSAGetLastError();
		exit(1);
	}
}


void CreateSocket(SOCKET* aSocket)
{
	*aSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (*aSocket == INVALID_SOCKET)
	{
		std::cerr << "Could not create a new socket\n" << WSAGetLastError();
		exit(1);
	}
}


void InitAddresses(sockaddr_in *aReceiverAddr, sockaddr_in *aSenderAddr, sockaddr_in *aChannelAddr, sockaddr_in *aUnknownAddr)
{

    // Channel
	memset(aChannelAddr, 0, sizeof(*aChannelAddr));
	aChannelAddr->sin_family = AF_INET;
	aChannelAddr->sin_addr.s_addr = INADDR_ANY;                       //  Auto assign IP
	aChannelAddr->sin_port = htons(0);                                // Auto assign port

	// Reciever
	memset(aReceiverAddr, 0, sizeof(*aReceiverAddr));
	aReceiverAddr->sin_family = AF_INET;
	aReceiverAddr->sin_addr.s_addr = aChannelAddr->sin_addr.s_addr;   // IP address of the server
	aReceiverAddr->sin_port = htons(0);                               // Auto assign port

	// Sender
	memset(aSenderAddr, 0, sizeof(*aSenderAddr));
	aSenderAddr->sin_family = AF_INET;
	aSenderAddr->sin_addr.s_addr =aChannelAddr->sin_addr.s_addr;      // IP address of the server
	aSenderAddr->sin_port = htons(0);                                 // Auto assign port

	// Unkown
	memset(aUnknownAddr, 0, sizeof(*aUnknownAddr));

	return;
}



// Will be generating random noise with given seed 
void RandomNoise(int aProbability, char *aBuffer, unsigned int aRandSeed)
{
	unsigned char mask; 
	std::default_random_engine generator(0); 
	std::uniform_int_distribution<int> distribution(0,TWO_POWER_SIXTEEN);
	long int randomNumber;

	for (int byte = 0; byte < BUFFER_SIZE_BYTES; byte++) 
	{
		mask = SINGLE_BIT_MASK;
		for (int bit = 0; bit < BITS_IN_BYTE; bit++)
		{
			randomNumber = distribution(generator);      // generating a random number in range [0, 2^16]
			if (randomNumber < aProbability)
			{
				aBuffer[byte] = aBuffer[byte] ^ mask;    // flip the desired bit
				gFlippedBits++;
			}

			mask <<= 1; // moving mask bit to the left
		}
	}
}

void DeterministicNoise(int aCycle, char* aBuffer)
{
	int counter = 0;
	unsigned char mask;
	
	for (int byte = 0; byte < BUFFER_SIZE_BYTES; byte++)
	{
		mask = SINGLE_BIT_MASK;
		for (int bit = 0; bit < BITS_IN_BYTE; bit++)
		{
			if (counter == aCycle)
			{
				aBuffer[byte] = aBuffer[byte] ^ mask;    // flip the desired bit when we counter == cycle
				gFlippedBits++;
				counter = 0;                             // reset counter
			}
			mask <<= 1;
			counter++;
		}
	}
}

void BindServer(SOCKET* aMainSocket, sockaddr_in* aChannelAddr)
{
	auto bindResult = bind(*aMainSocket, (SOCKADDR*)aChannelAddr, sizeof(*aChannelAddr));

	if (bindResult == SOCKET_ERROR)
	{
		std::cerr << "Binding Failed. Exiting..\n" << WSAGetLastError();
		exit(1);
	}

	/*
	listen(*aMainSocket, SOMAXCONN );
	SOCKET AcceptSocket = accept(*aMainSocket, NULL, NULL );
	
	int len = sizeof(aChannelAddr);
	getsockname(AcceptSocket, (SOCKADDR*)aChannelAddr,&len);
	*/
	
}


void RunChannel()
{
	
	SOCKET MainSocket = INVALID_SOCKET;
	sockaddr_in receiverAddr;
	sockaddr_in senderAddr;
	sockaddr_in channelAddr;
	sockaddr_in unkownAddr;                   // wil be used to determine who sent a message


	WSADATA wsaData;                          // will contain the winsock data
	fd_set fd_readerSet;
	timeval timeVal;
	
	char messageBuffer[BUFFER_SIZE_BYTES];

	// Initizliaing Winsock
	WinsockInit(&wsaData);
	CreateSocket(&MainSocket);
	InitAddresses(&receiverAddr, &senderAddr, &channelAddr, &unkownAddr);
	BindServer(&MainSocket, &channelAddr);



	// main logic: Recieve a message, flip bits and send back to reciever
	while (TRUE)
	{
		memset(messageBuffer, 0, sizeof(messageBuffer));
		FD_ZERO(&fd_readerSet);
		FD_SET(MainSocket, &fd_readerSet);

		// setting timing configuration - time waiting for messages
		timeVal.tv_sec = 2;  
		timeVal.tv_usec = 0;

		// select - ready to get new messages
		if (select(0, &fd_readerSet, NULL, NULL, &timeVal) == SOCKET_ERROR)
		{
			std::cerr << "Error in file descriptor select() \n";
			exit(1);
		}

		// get messages from unkown address - will check who sent later on
		if (recvfrom(MainSocket, messageBuffer, sizeof(messageBuffer), 0, (SOCKADDR*)&unkownAddr, &SIZE_OF_SOCKADDR) < 0)
		{
			std::cerr << "Encountered an error while recieving bytes \n";
			exit(1);
		}

		// TODO: complete this line and understand functionallity 
		if ((unkownAddr.sin_port == receiverAddr.sin_port) && unkownAddr.sin_addr.S_un.S_addr == receiverAddr.sin_addr.S_un.S_addr)
		{

		}


	}


}
