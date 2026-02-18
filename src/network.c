#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "network.h"
#include "common.h"

int start_server(){

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1){
        perror("socket");
        return -1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1){
        perror("bind");
        return -1;
    }

    if (listen(serverSocket, BACKLOG) == -1){
        perror("listen");
        return -1;
    }

    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);

    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);
    if (clientSocket == -1){
        perror("accept");
    }

    return 0;
}