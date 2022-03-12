/*
Authors:				Shachar Cohen (313416521) & Yuval Naor (?????????)
Project:				Programming Assignment 1: Noisy Channel
Project description:	Sender-Receiver communication through a noisy channel
*/

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include "SharedHardCodedData.h"
#include "HardCodedData.h"


static int g_port;
static char* g_ip;
static int g_graceful_shutdown;

/// <summary>
/// Boot up the client
/// </summary>
/// <param name="port">server port number</param>
/// <param name="player_name">the player name</param>
/// <returns>zero if successful, one otherwise</returns>
int boot_client(char* address, int port)
{
    int ret_val = 0, g_graceful_shutdown = 0;
    SOCKET main_socket = INVALID_SOCKET;
    g_ip = address;
    g_port = port;

    // Initialize Winsock
    WSADATA wsa_data;
    int result;
    result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != 0) {
        printf("Error: WSAStartup failed: %d\n", result);
        return 1;
    }

    SOCKADDR_IN client_service;
    client_service.sin_family = AF_INET;
    client_service.sin_port = htons(port); //The htons function converts a u_short from host to TCP/IP network byte order (which is big-endian).
    client_service.sin_addr.s_addr = inet_addr(address);

    // create file and file handle for log files - NEED TO ADD 


    // start server communications
    main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (main_socket == INVALID_SOCKET){
        printf("ERROR: Failed to create a socket.\n");
        return 1;
    }

    if (connect(main_socket, (SOCKADDR*)&client_service, sizeof(client_service)) == SOCKET_ERROR)
    {
        printf("Failed connecting to server on %s:%lu.\nChoose what to do next:\n1. Try to reconnect\n2. Exit\n", address, port);
    }

    // let client know it connected succesfully.
    printf("Connected to server on %s:%lu\n", g_ip, g_port);

    // Need to add communication
}

/// <summary>
/// Starts communication with server
/// </summary>
int communicate_server()
{
}



/// <summary>
/// shuts down a socket if required
/// </summary>
/// <param name="p_connection">pointer to socket</param>
/// <param name="p_log_file">pointer to log file</param>
/// <returns>FUNCTION_SUCCESS or FUNCTION_ERROR</returns>
int handle_shutdown(SOCKET* p_connection, HANDLE* p_log_file)
{
    int error = 0;

    if (*p_connection != INVALID_SOCKET && g_graceful_shutdown == 1)
    {
        if (initialize_graceful_shutdown(p_connection, FALSE) == FUNCTION_ERROR)
        {
            printf("ERROR: Failed to close socket.\n");
            print_error_to_log(p_log_file, "ERROR: Failed to close socket.\n");
            return FUNCTION_ERROR;
        }
    }
    else if (*p_connection != INVALID_SOCKET)
    {
        if (closesocket(*p_connection) == INVALID_SOCKET)
        {
            error = WSAGetLastError();
            if (error == WSAENOTSOCK || error == WSAECONNABORTED)
                return FUNCTION_SUCCESS;
            printf("ERROR: Failed to close socket.\n");
            print_error_to_log(p_log_file, "ERROR: Failed to close socket.\n");
            return FUNCTION_ERROR;
        }
    }

    *p_connection = INVALID_SOCKET;

    return FUNCTION_SUCCESS;
}

int read_file_bits(FILE* p_file, int num_bits, int buffer) {

}

int hamming_code() {

}

int send_buffer() {

}