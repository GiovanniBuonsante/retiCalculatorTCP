/*
 * server.c
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
#include <unistd.h>
#include "operations.h"


void terminate(char err[20])
{
    printf("%s",err);
    exit(-1);
}

int main() {
    int status, accepted, sock, clientCount = 0;
    socklen_t lgclient;
    struct sockaddr_in server, client;
    char message[1024];
    char operation;
    int num1, num2, result = 0;

    ///port server set to 12345.
    int portserver = 12345;

    printf("TCP Server Calculator:\n");
    printf("Socket creation...\n");

    ///socket(communication domain,type,protocol).
    sock = socket(AF_INET, SOCK_STREAM, 6); ///Returns socket ID.
    if (sock == -1) {
        terminate("Error creating socket!\tThe program will terminate!\n");
    }

    server.sin_family = AF_INET; ///AF_INET means IPv4.
    server.sin_port = htons(portserver); ///convert from host to network.
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    ///Attaching the created socket to the specified IP.
    status = bind(sock, (const struct sockaddr *) &server, sizeof(server));

    if (status == -1) {
        terminate("Invalid bind!\tThe program will terminate!\n");
    }

    printf("Waiting for new connection on the port: %d\n", portserver);

    ///Waiting for a connection request from a client.
    status = listen(sock, 5);

    if (status == -1) {
        terminate("Invalid listen!\tThe program will terminate!\n");
    }

    while (1) {
        if (clientCount < 5) {
            ///Accepts connection and returns ID.
            accepted = accept(sock, (struct sockaddr *) &client, &lgclient);

            if (accepted == -1) {
                terminate("Invalid accept!\tThe program will terminate!\n");
            }

            pid_t pid = fork();
            if (pid == -1) {
                terminate("Fork failed!\tThe program will terminate!\n");
            } else if (pid == 0) {
                close(sock);
                ///inet_ntoa() converts from binary to IPv4.
                printf("Connection was accepted from client: %s\n", inet_ntoa(client.sin_addr));

                ///send() message to the client.
                strcpy(message, "Welcome!\nThis is calculator program.\nEnter '=' once you want to exit.\n\n");
                status = send(accepted, message, strlen(message), 0);
                if (status != strlen(message)) {
                    terminate("Error sending!\tThe program will terminate!\n");

                }

                while (1) {
                    usleep(50);

                    ///send() message to the client.
                    strcpy(message, "Enter the calculation + - * / (es. + 45 36): ");
                    status = send(accepted, message, strlen(message), 0);
                    if (status != strlen(message)) {
                        terminate("Error sending!\tThe program will terminate!\n\n");
                    }

                    printf("Waiting for the calculation string...\n");

                    /// recv returns the number of characters received by the client
                    status = recv(accepted, message, 1024, 0);
                    message[status] = '\0'; /// Terminating the string.
                    if (status != strlen(message)) {
                        terminate("Error receiving!\tThe program will terminate!\n\n");
                    }
                    ///if the message is the char '=' break the connection with that client.
                    if (strcmp(message, "=") == 0) {
                        break;
                    }

                    /// sscanf() to extract the numbers and operation from the string.
                    sscanf(message, "%c %d %d", &operation, &num1, &num2);

                    printf("Received calculation: %c %d %d\n\n", operation, num1, num2);

                    ///Calculation.
                    char err[20]; ///Error string.
                    if (operation == '+') {
                        result = add(num1, num2);
                    } else if (operation == '-') {
                        result = subtract(num1, num2);
                    } else if (operation == '*') {
                        result = multiply(num1, num2);
                    } else if (operation == '/') {
                        if (num2 != 0) {
                            result = divide(num1, num2);
                        } else {
                            strcpy(err, "Math Error!");
                        }
                    } else {
                        strcpy(err, "Unknown operation!");
                    }
                    char final_message[25];

                    if (strlen(err) > 0) {
                        strcpy(final_message, err);
                    } else {
                        strcpy(final_message, "The result is: ");

                        ///We used it here to convert double to string.
                        sprintf(message, "%d", result);
                        ///concatenate the final message whit the result.
                        strcat(final_message, message);
                    }

                    printf("%s\tSending to the client...\n", final_message);

                    ///send() message to the client.
                    /// concatenate final message with '\n'.
                    strcat(final_message, "\n\n");
                    status = send(accepted, final_message, strlen(final_message), 0);
                    if (status != strlen(final_message)) {
                        terminate("Error sending!\tThe program will terminate!\n");
                    }

                    printf("The result was sent successfully!\n\n");
                    ///Resetting the error string
                    strcpy(err, "\0");
                }

                /// Closing the connection
                close(accepted);
                printf("The program will terminate!\n\n");
                exit(0);
            } else {
                /// Parent process
                close(accepted);
                clientCount++;
            }
        } else {
            wait(NULL);
            clientCount--;
        }
    }
    ///Closing the connection
    close(sock);
    printf("The program will terminate!\n\n");
    exit(0);
}
///function of the operation
int add(int num1, int num2) {
    return num1 + num2;
}
int subtract(int num1, int num2) {
    return num1 - num2;
}
int multiply(int num1, int num2) {
    return num1 * num2;
}
int divide(int num1, int num2) {
    if (num2 != 0) {
        return num1 / num2;
    } else {
        return 0;
    }
}

