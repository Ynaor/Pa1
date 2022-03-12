/*
Authors:				Shachar Cohen (313416521) & Yuval Naor (?????????)
Project:				Programming Assignment 1: Noisy Channel
Project description:	Sender-Receiver communication through a noisy channel
*/

#ifndef __SharedHardCodedData_H__
#define __SharedHardCodedData_H__

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

# define BRUTAL_TERMINATION_CODE 0x55
# define FUNCTION_SUCCESS 0
# define FUNCTION_ERROR 1


// String length upper bounds (when possible)
#define MAX_NAME_LEN 21			// maximum name length = 20 characters (+\0)
#define MAX_NUM_LEN 11			// max number with 32 bits = 10 digits (+\0)
#define MAX_MESSAGE_TYPE_LEN 30	// maximum message type is SERVER_MOVE_REQUEST
#define MAX_FN 50				// "Client_log_"+ MAX_NAME_LEN + ".txt\0"	
#define MAX_ERROR_MESSAGE 200	// Custom errors

// communication settings
#define SERVER_WAIT_MS 15000
#define HUMAN_WAIT_MS 600000
#define SHUTOWN_WAIT_MS 15000
#define PEEK_WAIT 100

// communication messages
typedef enum {
	CLIENT_REQUEST, SERVER_APPROVED, SERVER_DENIED,
	CLIENT_DISCONNECT
} message_type;

typedef struct packet 
{
} packet;

typedef enum { TRNS_FAILED, TRNS_DISCONNECTED, TRNS_SUCCEEDED } TransferResult_t;

#endif
