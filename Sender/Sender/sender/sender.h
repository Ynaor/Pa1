#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "winsock2.h"
#include <stdbool.h>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")

#define BYTE_SIZE 8

#define HAMMING_INPUT 11
#define HAMMING_OUTPUT 15


#define PACKET_DATA_SIZE 385// = 11(bits)*35(data bytes per message)  in bytes
#define PACKET_TOTAL_SIZE 525// = 15(bits)*35(total bytes per message) in bytes


int iResult = 0;
WSADATA wsaData;
SOCKET s;

char readed_byte;
FILE *fp;
unsigned char reading_mask;
int sizeof_sockaddr = sizeof(struct sockaddr);
