#include "channel.h"




int main(int argc, char* argv[])
{

	/*
	SOCKET socket;
	fd_set fd_reader_set;
	timeval time;

	unsigned short channelPort;          // 16 bits representation for a port
	unsigned short recieverPort;
	double errorProb;
	char* recieverIp;
	*/


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