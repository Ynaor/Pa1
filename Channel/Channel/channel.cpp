#include "channel.h"



// ********************************************
// **********  Socket Handling   **************
// ********************************************

// Initializing the winsock, will throw an error and exit if failed
void WinsockInit()
{
	const auto api_ver = MAKEWORD(2, 2);
	if (WSAStartup(api_ver, &wsaData) != NO_ERROR)
	{
		fprintf(stderr, "Winsock initialization failed\n");
		exit(1);
	}
}

// Check the port value is in bounds, will throw an erro and exit if failed
void ValidatePortNumber(int32_t aPortNumber)
{
	if ((aPortNumber < PORT_MIN_VALUE) || aPortNumber > PORT_MAX_VALUE)
	{
		fprintf(stderr, "Port value is out of allowed bounds");
		exit(1);
	}
}


// Will be generating random noise with given seed 
void RandomNoise(double aProbability, char *aBuffer, unsigned int aRandSeed)
{
	int byte, bit;
	unsigned char mask; 
	for (byte = 0; byte < PACKET_SIZE_BYTES; byte++)
	{
		mask = SINGLE_BIT_MASK;
		for (bit = 0; bit < BITS_IN_BYTE; bit++)
		{
			//TODO: complete the random noise (fully undesrstand the instructions given) using srand
		}
	}
}




















int main(int argc, char* argv[])
{


	SOCKET socket;
	fd_set fd_reader_set;
	timeval time;

	unsigned short channelPort;          // 16 bits representation for a port
	unsigned short recieverPort;
	double errorProb;
	char* recieverIp;



	// TODO: finish input validation
	if ((argc > 4) | (argc < 2)) // Number of arguments iw wrong
	{
		std::cerr << "Number of arguments is wrong when calling channel.exe";
	}
	else
	{
		for (int i = 0; i < argc; i++) {

		}
	}
}