#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

// This version contains updates which are compatible with newer c++ compiliers like clang 16 which gives some errors when using the old UDP code example posted on the Sierra Chart website. 

int main() 
{
    WSADATA wsaData;
    SOCKET SendSocket;
    sockaddr_in RecvAddr;

	// Port must be set the same in Sierra Chart Server Settings 
	// If the port was changed, it is recommended to restart SC 
    int Port = 22903;
	printf("Using UDP port: %d\n",Port);

	const char* SendBuf = "RELEASE_ALL_DLLS"; 

    // Initialize Winsock
    WSAStartup(MAKEWORD(2,2), &wsaData);

    // Create a socket for sending data
    SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // Set up the RecvAddr structure with the IP address of
    // the receiver (in this example case "127.0.0.1")
    // and the specified port number.
    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(Port);

	// Use inet_pton instead of inet_addr to avoid deprecated warning
    if (inet_pton(AF_INET, "127.0.0.1", &(RecvAddr.sin_addr)) != 1) 
	{
        perror("inet_pton");
        closesocket(SendSocket);
        WSACleanup();
        return 1; // Handle the error as needed
    }

    // Send a datagram to the receiver
	printf("Sending a datagram to the receiver...\n");
		sendto(SendSocket,
			SendBuf,
			strlen(SendBuf) + 1, // Include null terminator in the length
			0,
			(SOCKADDR*)&RecvAddr,
			sizeof(RecvAddr));

    // When the application is finished sending, close the socket.
    printf("Release All DLLs. Closing socket.\n");
    closesocket(SendSocket);

    // Clean up and quit.
    printf("Exiting.\n");
    WSACleanup();
    return 0 ;
}
