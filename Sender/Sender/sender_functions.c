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
#include <math.h>
#pragma comment(lib, "Ws2_32.lib")

#include "HardCodedData.h"
#include "sender_functions.h"


static int g_port;
static char* g_ip;
static unsigned char read_mask = 0;
static char read_byte;
static unsigned int total_bytes_read = 0;
static unsigned int total_bytes_sent = 0;

/// <summary>
/// Boot up the client
/// </summary>
/// <param name="port">server port number</param>
/// <returns>zero if successful, one otherwise</returns>
int boot_client(char* address, int port)
{
    SOCKET main_socket = INVALID_SOCKET;
    g_ip = address;
    g_port = port;
    char f_name[MAX_FN] = "";

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

    // start server communications
    while (TRUE) {

        total_bytes_read = 0;
        total_bytes_sent = 0;
        read_mask = 0;

        main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (main_socket == INVALID_SOCKET) {
            printf("ERROR: Failed to create a socket\n");
            return 1;
        }

        if (connect(main_socket, (SOCKADDR*)&client_service, sizeof(client_service)) == SOCKET_ERROR) {
            printf("Failed connecting to server on %s:%lu\n", address, port);
            return 1;
        }

        // Client connected succefully, get file name
        printf("enter file name:\n");
        if (fgets(f_name, MAX_FN, stdin) == NULL) {
            printf("Error: could not read file name\n");
            return 1;
        }
        /// NEED TO HANDLE TO LONG OF A FILE NAME *********************************************
        if (strcmp(f_name, "quit") == 0) {
            return 0;
        }

        if (send_file(f_name, &main_socket)) {
            return 1;
        }

        printf("file length: %u bytes\n", total_bytes_read);
        printf("sent: %u bytes\n", total_bytes_sent);

    }
}



int send_file(char* file_name, SOCKET *p_socket) {
    FILE* fp;
    if (fopen_s(&fp, file_name, "rb")) {
        fprintf(stderr, "Error: failed to read file");
        return 1;
    } 
    
    int end_of_file = 0;

    int packet_buffer[BYTES_IN_PACKET * BITS_IN_BYTE] = { 0 };
    int bits_in_packet_buffer = 0;

    int frame_data_buffer[DATA_BYTES_IN_FRAME] = { 0 };
    int frame_buffer[BYTES_IN_FRAME] = { 0 };
    
    int bits_read = 0;
    int bits_in_frame_buffer = 0;
    
    int hamming_check_bits = DEFAULT_HAMMING_BITS;

    char message[BYTES_IN_PACKET];


        while (TRUE) {

            while (bits_in_packet_buffer < BYTES_IN_PACKET * BITS_IN_BYTE) {

                // read data for single frame. If the read_file_bits returns -1 and bits_read is zero there is no frame to generate.
                if (read_file_bits(&fp, &frame_data_buffer, &bits_read) == -1) {
                    end_of_file = 1;
                    if (bits_read == 0)
                        break;
                }
                hamming_check_bits = ceil(log2(bits_read));
                bits_in_frame_buffer = bits_read + hamming_check_bits;
                create_hamming(&frame_data_buffer, bits_read, &frame_buffer, hamming_check_bits);
                concatenate_array(packet_buffer, bits_in_packet_buffer, frame_buffer, bits_in_frame_buffer);
                
                bits_in_packet_buffer += bits_in_frame_buffer;
                
                bits_read = 0;
                memset(frame_data_buffer, 0, sizeof(frame_data_buffer));
                memset(frame_buffer, 0, sizeof(frame_buffer));

                if (end_of_file)
                    break;
            }
            
            // empty packet
            if (bits_in_packet_buffer == 0)
                break;


            int_to_char(packet_buffer, message, bits_in_packet_buffer / BITS_IN_BYTE);
            
            if (send_packet(message, bits_in_packet_buffer / BITS_IN_BYTE, p_socket)) {
                if (!fclose(fp))
                    fprintf(stderr, "Error: failed to close file");
                return 1;
            }

            memset(packet_buffer, 0, sizeof(packet_buffer));
            bits_in_packet_buffer = 0;
            
            if (end_of_file)
                break;
        }

        if (!fclose(fp)) {
            fprintf(stderr, "Error: failed to close file");
            return 1;
        }

        if (closesocket(*p_socket) == INVALID_SOCKET){
            int error = WSAGetLastError();
            if (error == WSAENOTSOCK || error == WSAECONNABORTED)
                return 0;
            printf("ERROR: Failed to close socket.\n");
            return 1;
        } 
        return 0;
}


/// <summary>
/// Read file to create Hamming interval data
/// </summary>
/// <param name="p_file">file pointer</param>
/// <param name="buffer">data buffer in which to save the read bits</param>
/// <returns>zero if MAX_DATA_BITS were read, one if reached end of file</returns>
int read_file_bits(FILE* p_file, int *data_buffer[], int *bits_read){
    while (bits_read < DATA_BYTES_IN_FRAME) {
        read_mask >>= 1;
        if (read_mask == 0) {
            if (fread(&read_byte, 1, 1, p_file) <= 0)
                return -1;
            total_bytes_read++; 
            read_mask = 1 << (BITS_IN_BYTE - 1);
        }
        data_buffer[*bits_read] = read_mask & read_byte;

        bits_read++; 
    }
    return 0; 
}


/// <summary>
/// Create Hamming interval by adding the hamming check bits to the data bits; this function does not need to have a specific file size
/// </summary>
/// <param name="data_buffer">Hamming interval data bits</param>
/// <param name="bits_read">The actual number of data bits in data_buffer</param>
void create_hamming(int *data_buffer, int bits_read, int *frame_buffer, int check_bits) {

    // place the check bits in the packet_buffer array and initialize to 1
    for (int i = 0; i < check_bits; i++) {
        frame_buffer[(int)pow(2, i)-1] = 1;
    }

    // place the data bits in the frame_buffer; start entering data into frame_buffer from index 1 - min check_bits = 1
    int frame_buffer_index = 1;
    for (int i = 0; i < bits_read; i++) {
        if (frame_buffer[frame_buffer_index] == 1)
            frame_buffer_index++;
        frame_buffer[frame_buffer_index] = data_buffer[i];
        frame_buffer_index++;
    }

    int valid_bits_in_frame = check_bits + bits_read;

    // set the values of the hamming bits:
    for (int i = 0; i < check_bits; i++) {
        int check_bit_index = (int)pow(2, i) - 1;
        int res = 0;
        for (int j = 0; j < valid_bits_in_frame; j++) {
            if ((j + 1) & (check_bit_index + 1))
                res = res ^ frame_buffer[j];
        }
        frame_buffer[check_bit_index] = res;
    }
}


/// <summary>
/// Sends a string of known size via a socket
/// </summary>
/// <param name="buffer">string buffer</param>
/// <param name="message_len">length of the string</param>
/// <param name="p_connection_socket">pointer to the socket</param>
/// <returns>send result</returns>
int send_packet(char* buffer, const int message_len, SOCKET* p_connection_socket) {
    char* p_current_place = buffer;
    int bytes_transferred, error_reason;
    int remaining_bytes = message_len;

    // send the string (not zero terminated)
    while (remaining_bytes > 0) {
        bytes_transferred = send((*p_connection_socket), p_current_place, remaining_bytes, 0);
        if (bytes_transferred == SOCKET_ERROR)
        {
            // check if send has been disabled
            error_reason = WSAGetLastError();
            if (error_reason == WSAESHUTDOWN || error_reason == WSAENOTSOCK || error_reason == WSAEINTR) {
                fprintf(stderr, "Error: send failed because %d.\n", error_reason);
                return 1;
            }

            // if not, error and hard close is done
            closesocket(*p_connection_socket);
            *p_connection_socket = INVALID_SOCKET;
            return 1;
        }

        total_bytes_sent += bytes_transferred;

        // check how many bytes left
        remaining_bytes -= bytes_transferred;
        p_current_place += bytes_transferred;
    }

    return 0;
}


/// <summary>
/// Merge two arrays from a given position in the first array
/// </summary>
/// <param name="basa_array">The array to have another array added to</param>
/// <param name="last_index">The index in the base_array that the second_array will be added to</param>
/// <param name="seconed_array">The array that is added</param>
/// <param name="size">The number of elements of second_array to be added to base_array</param>
void concatenate_array(int* basa_array, int last_index, int* seconed_array, int size) {
    for (int i = 0; i < size; i++) {
        basa_array[last_index + i] = seconed_array[i];
    }
}


/// <summary>
/// Converts an array of ints to chars by calculating the decimal value of every 8 consecutive bits
/// </summary>
/// <param name="source">source int array</param>
/// <param name="dest">destination char array</param>
/// <param name="num_of_bytes">size of char array</param>
/// <returns>send result</returns>
void int_to_char(int* source, char* dest, int num_of_bytes) {
    int temp = 0;
    int curr_val = 0;
    int ref_index = 0;

    for (int i = 0; i < (num_of_bytes); i++){
        curr_val = 0;
        ref_index = 8 * i;
        for (int j = 0; j < BITS_IN_BYTE; j++) {
            temp = source[j + ref_index];
            temp *= pow(2, (int)BITS_IN_BYTE - 1 - j);
            curr_val += temp;
        }
        dest[i] = curr_val;
    }
}
