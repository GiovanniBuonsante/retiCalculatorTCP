/*
 * client.c
 *
 *  Created on: 2 dic 2023
 *      Author: giovannibuonsante
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "operations.h"

void terminate(char err[20])
{
    printf("%s",err);
    exit(-1);
}
int main() {
    int status, sock;
    struct sockaddr_in server;
    char *adrserver, message[1024];
    //struct sockaddr name;

    ///initialize port server.
    int portserver = 12345;

    /// reserved memory for IP address.
    adrserver = (char *)malloc(32);

    printf("TCP Client Calculator:\n");

    /// set the server IP address
    strcpy(adrserver, "127.0.0.1");

    printf("Socket creation...\n");

    ///socket(communication domain,type,protocol).
    sock=socket(AF_INET,SOCK_STREAM,6); ///Returns socket ID.

    if(sock == -1) {
        terminate("Error creating socket!\tThe program will terminate!\n");
    }

    server.sin_family = AF_INET; ///AF_INET means IPv4.
    server.sin_port=htons(portserver); ///convert from host to network.
    server.sin_addr.s_addr = inet_addr(adrserver);

    status=connect(sock,(const struct sockaddr*)&server,sizeof(server));

    if(status != 0) {
        terminate("Error Connection!\tThe program will terminate!\n");
    }

    printf("Connection was established successfully!\n\n");

    ///recv() returns the number of characters received.
    status=recv(sock,message,1024,0);
    message[status]='\0'; ///Terminating the string.
    if(status!=strlen(message))
        terminate("Error receiving!\tThe program will terminate!\n");

    ///print the message from the server.
    printf("%s",message);

    while(1){
        ///recv() receives the message from the server.
        status = recv(sock, message, 1024, 0);
        message[status] = '\0'; ///Terminating the string.
        if(status != strlen(message)) {
            terminate("Error receiving!\tThe program will terminate!\n");
        }

        ///print the message from the server
        printf("%s",message);
        ///get the input string by the client.
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0'; // remove the newline

        /// send() returns the number of characters send.
        status = send(sock, message, strlen(message), 0);

        if (status != strlen(message)) {
            terminate("Error sending!\tThe program will terminate!\n");
        }
        ///if the user input '=' break the loop.
        if (strcmp(message, "=") == 0) {
            break;
        }

        ///recv() receives the message from the server.
        status = recv(sock, message, 1024, 0);
        message[status] = '\0';
        if (status != strlen(message)) {
            terminate("Error receiving!\tThe program will terminate!\n");
        }
        ///print the message from the server with the final result.
        printf("%s",message);
    }
    ///terminate the connection with the server.
    printf("The program will terminate!\n\n");
    return 0;
}

