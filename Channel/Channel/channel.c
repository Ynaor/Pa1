

#include "channel.h"

//initialize Winsock
int winsock_init() {
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		printf("WSAStartup failed\n");
		exit(-1);
	}
	return 0;
}


int check_port_validation(int port_number) {
	if (port_number < 0 || port_number > 65535) {
		fprintf(stderr, "Port number is out of bound\n");
		exit(-1);
	}
	return 0;
}

//wating to get the full packet from sender
int recive_packet_from_sender(SOCKET s, struct sockaddr_in sender_addr) {
	// waiting for all the packet to arrive
	int bytes_left_to_receive = PACKET_TOTAL_SIZE - bytes_received;
	while (bytes_left_to_receive > 0) {
		bytes_received = recvfrom(s, received_bytes_buffer + PACKET_TOTAL_SIZE - bytes_left_to_receive, bytes_left_to_receive, 0, (struct sockaddr*)&sender_addr, &sizeof_sockaddr);
		if (bytes_received < 0)
		{
			fprintf(stderr, "failed receiving full packet from sender\n");
			exit(-1);
		}
		bytes_left_to_receive = bytes_left_to_receive - bytes_received;
	}
	total_num_of_bytes += PACKET_TOTAL_SIZE;
	return 0;
}


//add noise - changing bits in message 
void add_noise(double probability) {

	int letter, bit, limited_random_value, random_value;
	unsigned char mask;

	for (letter = 0; letter < PACKET_TOTAL_SIZE; letter++){
		mask = 1;   // Mask to flip a bit 

		for (bit = 0; bit < 8; bit++)
		{
		//because the value of RAND_MAX is unknown (min 2^15), we'll add 2^15 to make sure random_value can get all the values between 0-2^16
		random_value = (rand() + 32768 * (rand() % 2)); 
		limited_random_value = (random_value % 65536);    // limited_random_value = urandom (0, 2^16)

		if (limited_random_value < probability) {            // limited_random_value < probability  <=> flliping_porbability =  probability/2^16
			flipped_bits += 1;
			received_bytes_buffer[letter] = received_bytes_buffer[letter] ^ mask;
			}
			mask = mask << 1;   // Shift the mask bit
		}
	}
}


int send_packet_to_receiver(SOCKET s, struct sockaddr_in receiver_addr) {
	int bytes_sent;
	int bytes_left_to_send = PACKET_TOTAL_SIZE;
	while (bytes_left_to_send > 0) {
		bytes_sent = sendto(s, received_bytes_buffer + PACKET_TOTAL_SIZE - bytes_left_to_send, bytes_left_to_send, 0, (struct sockaddr*)&receiver_addr, sizeof_sockaddr);
		if (bytes_sent < 0) {
			fprintf(stderr, "failed sending full packet to receiver!\n");
			exit(-1);
		}
		bytes_left_to_send = bytes_left_to_send - bytes_sent;
	}
	return 0;
}


int main(int argc, char* argv[]) {

	SOCKET s;
	fd_set fd_reader_set;
	struct timeval time_val;
	int return_sockets;
	
	int channel_port;
	char *receiver_ip;
	int receiver_port;
	double error_probability;
	srand(atoi(argv[5])); //init random seed

	if (argc != 6) {
		fprintf(stderr, "6 arguments are needed\n");
		exit(-1);
	}
	   
	channel_port = atoi(argv[1]);   // Channel port
	receiver_ip = argv[2];			// Receiver IP
	receiver_port = atoi(argv[3]);  // Receiver socket's port number
	error_probability = atoi(argv[4]);     // error_probability in 1/2^16 resoulotion 

	if( winsock_init() !=0 )
		exit(-1);

	if (check_port_validation(receiver_port) != 0)
		exit(-1);

	// create socket
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		fprintf(stderr, "failed creating socket\n");
		exit(-1);
	}

	// addresses of receiver and sender
	struct sockaddr_in receiver_addr;
	struct sockaddr_in sender_addr;
	struct sockaddr_in channel_addr;
	struct sockaddr_in temp_addr; //need to understand who sent the message

	// initialize address structs
	memset(&receiver_addr, 0, sizeof(receiver_addr));
	receiver_addr.sin_family = AF_INET;
	receiver_addr.sin_port = htons(receiver_port);
	receiver_addr.sin_addr.s_addr = inet_addr(receiver_ip);

	memset(&channel_addr, 0, sizeof(channel_addr));
	channel_addr.sin_family = AF_INET;
	channel_addr.sin_port = htons(channel_port);
	channel_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	memset(&temp_addr, 0, sizeof(temp_addr));
	memset(&sender_addr, 0, sizeof(sender_addr));

	// bind socket
	if (bind(s, (struct sockaddr *) &channel_addr, sizeof(struct sockaddr_in)) < 0) {
		fprintf(stderr, "failed binding receiver socket\n");
		exit(-1);
	}

	time_val.tv_sec = 3;
	time_val.tv_usec = 0;

	// channel functionality: receiving messages -> adding noise -> sending messages
	while (1) {

		memset(received_bytes_buffer, 0, PACKET_TOTAL_SIZE);
		FD_ZERO(&fd_reader_set);
		FD_SET(s, &fd_reader_set);
		return_sockets = select(0, &fd_reader_set, NULL, NULL, &time_val);
		if (return_sockets == SOCKET_ERROR) {
			fprintf(stderr, "select function error\n");
			exit(-1);
		}
		if (return_sockets > 0) {
			//receiving message
			memset(&temp_addr, 0, sizeof(temp_addr));
			bytes_received = recvfrom(s, received_bytes_buffer, PACKET_TOTAL_SIZE, 0, (struct sockaddr *)&temp_addr, &sizeof_sockaddr);
			if (bytes_received < 0) {
				fprintf(stderr, "failed receiving bytes\n");
				exit(-1);
			}

			// message is end message from receiver - sending it to the sender
			if (temp_addr.sin_port == receiver_addr.sin_port && temp_addr.sin_addr.S_un.S_addr == receiver_addr.sin_addr.S_un.S_addr) {
				if (sendto(s, received_bytes_buffer, bytes_received, 0, (struct sockaddr *) &sender_addr, sizeof_sockaddr) <= 0) {
					fprintf(stderr, "failed sending data\n");
					exit(-1);
				}

				// Close socket
				if (closesocket(s) < 0) {
					fprintf(stderr, "failed closing socket\n");
					exit(-1);
				}
				break;
			}

			// message is data from sender to receiver - verifying that all tha packet has arrived
			else {
				sender_addr = temp_addr;
				if (recive_packet_from_sender(s, temp_addr) !=0 )
				return -1;

				add_noise(error_probability);

				if (send_packet_to_receiver(s, receiver_addr) != 0)
					return -1;
			}
		}
	}

	fprintf(stderr, "sender: %s\n", inet_ntoa(sender_addr.sin_addr));
	fprintf(stderr, "receiver: %s\n", receiver_ip);
	fprintf(stderr, "%d bytes, flipped %d bits\n", total_num_of_bytes, flipped_bits);

	return 0;
}

