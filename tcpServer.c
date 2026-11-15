/*
 * tcpServer.c - Receiver side. Waits for the client, decrypts every received DES block,
 * displays it, then lets the user type a reply which is encrypted and sent back.
 *
 * Build : gcc tcpServer.c des.c -o tcpServer.exe -lws2_32
 * Run   : tcpServer.exe [port] [-v]        (default port 9000, -v = trace DES internals)
 */
#include <stdlib.h>
#include "des_common.h"

int main(int argc, char **argv)
{
    WSADATA wsa;
    SOCKET listenSock, clientSock;
    struct sockaddr_in serverAddr, clientAddr;
    int addrLen = sizeof(clientAddr);
    int port = DEFAULT_PORT, i;
    unsigned char cipher[BLOCK];
    char plain[BLOCK + 1], reply[BLOCK + 1];

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) des_trace = 1;
        else port = atoi(argv[i]);
    }

    printf("[+] Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("[-] Failed. Error Code : %d\n", WSAGetLastError());
        return 1;
    }

    listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSock == INVALID_SOCKET) { printf("[-] Error in socket creation.\n"); return 1; }
    printf("[+] Server Socket is created.\n");

    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;          /* accept on every network interface */
    serverAddr.sin_port        = htons((unsigned short)port);

    if (bind(listenSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("[-] Bind failed (error %d).\n", WSAGetLastError());
        return 1;
    }
    printf("[+] Bind to port %d\n", port);

    if (listen(listenSock, 3) == SOCKET_ERROR) { printf("[-] Listen failed.\n"); return 1; }
    printf("[+] Listening....\n");

    clientSock = accept(listenSock, (struct sockaddr *)&clientAddr, &addrLen);
    if (clientSock == INVALID_SOCKET) { printf("[-] Accept failed.\n"); return 1; }
    printf("[+] Connection accepted from %s:%d\n",
           inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    while (1) {
        if (!recv_all(clientSock, cipher, BLOCK)) { printf("[*] Client disconnected.\n"); break; }
        print_hex("[RECEIVED] Ciphertext : ", cipher, BLOCK);

        decrypt_message(cipher, plain);
        printf("[DECRYPTED] Message from Client: %s\n", plain);
        if (strcmp(plain, "exit") == 0) break;

        printf("Enter reply message (max %d chars): ", BLOCK);
        read_message(reply);

        encrypt_message(reply, cipher);
        print_hex("[SENT] Ciphertext     : ", cipher, BLOCK);
        send(clientSock, (const char *)cipher, BLOCK, 0);
    }

    closesocket(clientSock);
    closesocket(listenSock);
    WSACleanup();
    return 0;
}
