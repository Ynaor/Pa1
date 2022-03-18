#include "channel.h"s


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


void InitAddresses(sockaddr_in *aReceiverAddr, sockaddr_in *aSenderAddr, sockaddr_in *aChannelAddr)
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

	std::cout << "Channel  -> Ip: " << inet_ntoa(aChannelAddr->sin_addr) << " port: " << ntohs(aChannelAddr->sin_port) << "\n";
	std::cout << "Sender   -> Ip: " << inet_ntoa(aSenderAddr->sin_addr)  << " port: " << ntohs(aSenderAddr->sin_port) << "\n";
	std::cout << "Reciever -> Ip: " << inet_ntoa(aReceiverAddr->sin_addr)<< " port: " << ntohs(aReceiverAddr->sin_port) << "\n";

	return;
}



// Will be generating random noise with given seed 
void RandomNoise(int aProbability, char *aBuffer, unsigned int aRandSeed)
{
	unsigned char mask; 
	std::default_random_engine generator(0); 
	std::uniform_int_distribution<int> distribution(0,TWO_POWER_SIXTEEN);
	long int randomNumber;

	for (int byte = 0; byte < PACKET_SIZE_BYTES; byte++) 
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
	long long int mask;
	
	for (int byte = 0; byte < PACKET_SIZE_BYTES; byte++)
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

void BindServer(SOCKET* aMainSocket, sockaddr_in* aServerAddr)
{
	auto bindResult = bind(*aMainSocket, (SOCKADDR*)aServerAddr, sizeof(*aServerAddr));

	if (bindResult == SOCKET_ERROR)
	{
		std::cerr << "Binding Failed. Exiting..\n" << WSAGetLastError();
		exit(1);
	}

	listen(*aMainSocket, SOMAXCONN );
	socklen_t len = sizeof(aServerAddr);
	int result = getsockname(*aMainSocket, (SOCKADDR*)aServerAddr,&len);
	std::cout<< "getsocket(): "<< result;
}


void RunChannel()
{
	
	SOCKET MainSocket = INVALID_SOCKET;
	sockaddr_in receiverAddr;
	sockaddr_in senderAddr;
	sockaddr_in channelAddr;
	WSADATA wsaData;                          // will contain the winsock data
	

	// Initizliaing Winsock
	WinsockInit(&wsaData);
	CreateSocket(&MainSocket);
	InitAddresses(&receiverAddr, &senderAddr, &channelAddr);
	BindServer(&MainSocket, &channelAddr);


	// main logic: Recieve a message, flip bits and send back to reciever
	while (TRUE)
	{
		continue;
	}


}
