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
		std::cerr << "Winsock initialization failed\n";
		exit(1);
	}
}

// Check the port value is in bounds, will throw an erro and exit if failed
void ValidatePortNumber(int32_t aPortNumber)
{
	if ((aPortNumber < PORT_MIN_VALUE) || aPortNumber > PORT_MAX_VALUE)
	{
		std::cerr << "Port value is out of allowed bounds\n";
		exit(1);
	}
}


// Will be generating random noise with given seed 
void RandomNoise(double aProbability, char *aBuffer, unsigned int aRandSeed)
{
	int byte, bit;
	unsigned char mask; 

	srand(aRandSeed); // using the random seed given as input to our program
	for (byte = 0; byte < PACKET_SIZE_BYTES; byte++)
	{
		mask = SINGLE_BIT_MASK;
		for (bit = 0; bit < BITS_IN_BYTE; bit++)
		{
			//TODO: complete the random noise (fully undesrstand the instructions given) using srand
		}
	}
}

