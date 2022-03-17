/*
Authors:				Shachar Cohen (313416521) & Yuval Naor (?????????)
Project:				Programming Assignment 1: Noisy Channel
Project description:	Sender-Receiver communication through a noisy channel
*/

#ifndef __SEND_RECIEVE_FUNCTIONS_H__
#define __SEND_RECIEVE_FUNCTIONS_H__
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
#include "receiver_functions.h"


static int g_port;
static char* g_ip;
static unsigned char write_mask = 0;
static unsigned int total_bytes_written = 0;
static unsigned int total_bytes_received = 0;
static unsigned int next_bit_index = 0;         // next bit index in a byte, max val = 7;
static char write_byte;                         // next byte to write to file


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

        total_bytes_received = 0;
        total_bytes_written = 0;
        write_mask = 0;

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
        /// NEED TO HANDLE Too LONG FILE NAME *********************************************
        if (strcmp(f_name, "quit") == 0) {
            return 0;
        }

        if (write_file(f_name, &main_socket)) {
            return 1;
        }

        printf("received: %u bytes\n", total_bytes_received);
        printf("wrote: %u bytes\n", total_bytes_written);

    }
}


// assuming the number of DATA bits in a packet is a whole number of bytes!!!!!!!!!!!!!!!!!!!!!!!!!!!
int write_file(char* file_name, SOCKET* p_socket) {
    FILE* fp;
    if (fopen_s(&fp, file_name, "w")) {
        fprintf(stderr, "Error: failed to open file");
        return 1;
    }

    //int packet_data_buffer[DATA_BYTES_IN_PACKET * BITS_IN_BYTE] = { 0 };
    //int bits_in_packet_data_buffer = 0;

    //int frame_data_buffer[DATA_BYTES_IN_FRAME] = { 0 };
    //int frame_buffer[BYTES_IN_FRAME] = { 0 };

    //int bits_read = 0;
    //int bits_in_frame_buffer = 0;

    //int hamming_check_bits = DEFAULT_HAMMING_BITS;

    TransferResult_t recv_result;               // recv_packet result
    int recv_complete = 0;                      // 1 if all data received from server                 

    int packet_size = 0;                     // number of bytes in received packet
    int parsed_packet_bits = 0;                 // number of bits parsed from received packet

    int parsed_message[MAX_BITS_IN_PACKET] = { 0 };   // packet message parsed to bits


    // each iteration is per packet
    while (TRUE) {

        char message[MAX_BYTES_IN_PACKET];

        recv_result = recv_packet(message, MAX_BYTES_IN_PACKET, &p_socket, &packet_size);

        if (recv_result == TRNS_FAILED)
            return 1;

        if (recv_result == TRNS_DISCONNECTED)
            recv_complete = 1;

        get_bits(message, parsed_message, packet_size);
        write_bytes(parsed_message, packet_size, &);


    }
}

void write_bytes(FILE* p_file, int* source, int packet_size) {
    int total_bits = packet_size * BITS_IN_BYTE;
    int frames = total_bits / BITS_IN_FRAME;
    int parsed_frame[DATA_BITS_IN_FRAME];
    for (int i = 0; i < frames; i++) {
        decode_hamming(&parsed_frame, &source + i * BITS_IN_FRAME);

    }
}

void decode_hamming(int* encoded_data, int* data_buffer) {

}

/*
void get_bits(char* read_target, int* data_buffer[], int* packet_size) {

    int read_mask = 0;
    int temp;
    int ref_index = 0;
    
    for (int i = 0; i < packet_size; i++) {
        
        read_mask = 1 << (BITS_IN_BYTE - 1);
        ref_index = i * BITS_IN_BYTE;

        for (int j = 0; j < BITS_IN_BYTE; j++) {
            temp = read_mask & read_target[i];
            data_buffer[ref_index + j] = temp;
        }
    }
}


/*

        memset(packet_data_buffer, 0, sizeof(packet_data_buffer));
        bits_in_packet_data_buffer = 0;

        while (bits_in_packet_data_buffer < DATA_BITS_IN_PACKET) {

            // read data of 1 frame
            if (read_file_bits(&message, &frame_buffer, &bits_read) == -1) {
                end_of_file = 1;
                if (bits_read == 0)
                    break;
            }
            hamming_check_bits = ceil(log2(bits_read));
            bits_in_frame_buffer = bits_read + hamming_check_bits;
            create_hamming(&frame_data_buffer, bits_read, &frame_buffer, hamming_check_bits);
            concatenate_array(packet_data_buffer, bits_in_packet_data_buffer, frame_buffer, bits_in_frame_buffer);

            bits_in_packet_data_buffer += bits_in_frame_buffer;

            bits_read = 0;
            memset(frame_data_buffer, 0, sizeof(frame_data_buffer));
            memset(frame_buffer, 0, sizeof(frame_buffer));

            if (end_of_file)
                break;
        }

        // empty packet
        if (bits_in_packet_data_buffer == 0)
            break;

        int_to_char(packet_data_buffer, message, BYTES_IN_PACKET);

        if (send_packet(message, BYTES_IN_PACKET, p_socket)) {
            if (!fclose(fp))
                fprintf(stderr, "Error: failed to close file");
            return 1;
        }

}


/// <summary>
/// Read file to create Hamming interval data
/// </summary>
/// <param name="read_target">char pointer to read bits from</param>
/// <param name="buffer">data buffer in which to save the read bits</param>
/// <returns>zero if MAX_DATA_BITS were read, one if reached end of file</returns>
int read_bits(char **read_target, int* data_buffer[], int* bits_read) {
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
/// Decode Hamming
/// </summary>
/// <param name="data_buffer">Hamming interval data bits</param>
/// <param name="bits_read">The actual number of data bits in data_buffer</param>
void decode_hamming(int* data_buffer, int bits_read, int* frame_buffer, int check_bits) {

    // place the check bits in the packet_buffer array and initialize to 1
    for (int i = 0; i < check_bits; i++) {
        frame_buffer[(int)pow(2, i) - 1] = 1;
    }

    // place the data bits in the frame_buffer; start entering data into frame_buffer from index 1 - min check_bits = 1
    int frame_buffer_index = 1;
    for (int i = 0; i < bits_read - 1; i++) {
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
                res = res ^ frame_buffer[i];
        }
        frame_buffer[check_bit_index] = res;
    }
}


/// <summary>
/// Recieve packet via socket
/// </summary>
/// <param name="buffer">output string</param>
/// <param name="packet_length">packet length</param>
/// <param name="p_connection_socket">pointer to the connection socket</param>
/// <param name="bytes_received">total bytes received in this function</param>
/// <returns>TRNS_SUCCEDED if a full packet was received, TRNS_DISCONNECTED if while receiving a packet server finished transmition, TRNS_FAILED if there was an error </returns>
TransferResult_t recv_packet(char* buffer, const int packet_length, SOCKET* p_connection_socket, int* bytes_received) {
    char* p_current_place = buffer;
    int bytes_transferred, bytes_left = packet_length, ret_val = 0, error = 0;

    // recieve bytes of data until done
    while (bytes_left > 0) {

        bytes_transferred = recv(*p_connection_socket, p_current_place, bytes_left, 0);
        
        if (bytes_transferred == 0 || bytes_transferred == SOCKET_ERROR) {
            
            if (bytes_transferred == SOCKET_ERROR) {
                error = WSAGetLastError();
                // Assuming connection termination is only because the server completed sending the data 
                if (error == WSAENOTSOCK || error == WSAEINTR)
                    return TRNS_DISCONNECTED;
                printf("Error: receive failed because of %d.\n", error);
                ret_val = TRNS_FAILED;
            }
            
            else
                ret_val = TRNS_DISCONNECTED;
            
            // close socket
            if (*p_connection_socket != INVALID_SOCKET) {

                if (closesocket(*p_connection_socket) == INVALID_SOCKET) {
                    error = WSAGetLastError();
                    if (error != WSAENOTSOCK && error != WSAEINTR)
                        printf("ERROR: Failed to close socket. Error number %d.\n", WSAGetLastError());
                }
                else
                    *p_connection_socket = INVALID_SOCKET;
            
            }
            
            return ret_val;
        
        }
        
        bytes_received += bytes_transferred;
        total_bytes_received += bytes_transferred;
        bytes_left -= bytes_transferred;
        p_current_place += bytes_transferred;
    }

    return TRNS_SUCCEEDED;
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

    for (int i = 0; i < (num_of_bytes); i++) {
        curr_val = 0;
        ref_index = 8 * i;
        for (int j = 0; j < BITS_IN_BYTE; j++) {
            temp = source[j + ref_index];
            temp *= pow(2, j + (int)BITS_IN_BYTE - 1);
            curr_val += temp;
        }
        dest[i] = curr_val;
    }
}#pragma once
*/