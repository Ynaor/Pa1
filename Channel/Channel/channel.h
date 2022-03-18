#pragma once

#pragma comment(lib, "Ws2_32.lib")
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

//includes 
#include "winsock2.h"
#include <ws2def.h>
#include <cstdint>
#include <iostream>
#include <stdlib.h>
#include <random>
#include <ws2tcpip.h>

// defines
#define PORT_MIN_VALUE      0
#define PORT_MAX_VALUE      65535  //2^16 - 1
#define SINGLE_BIT_MASK		1
#define BITS_IN_BYTE        8
#define FRAME_SIZE_BYTES	31	   // "frame" is a collection of 20 31 byte collection (8 bits * 31 bits per chuck)
#define FRAME_SIZE_BITS     248    // 8 * 31
#define FRAMES_PER_PACKET   20     // we wish to send 20 frames in every packet, to increase the size of our packets
#define PACKET_SIZE_BYTES	620    // 31 bytes per "frame", 20 frames per packet
#define PACKET_SIZE_BITS    4960   // 620 * 8 bits per byte
#define TWO_POWER_FIFTEEN   32768  // 2^15
#define TWO_POWER_SIXTEEN   65536  // 2^16
 



// Socket Handling
// TODO: add description for each function
void InitAddresses(sockaddr_in* aReceiverAddr, sockaddr_in* aSenderAddr, sockaddr_in* aChannelAddr);
void WinsockInit(WSADATA* wsaData);
void CreateSocket(SOCKET* aSocket);
void InitAddresses(sockaddr_in* aReceiverAddr, sockaddr_in* aSenderAddr, sockaddr_in* aChannelAddr);
void BindServer(SOCKET* aMainSocket, sockaddr_in* aServerAddr);


// Channel functions
void RandomNoise(int aProbability, char* aBuffer, unsigned int aRandSeed);
void DeterministicNoise(int aCycle, char* aBuffer);
void RunChannel();



