/*
 * tcpClient.c - Sender side. Reads a message, encrypts it with DES, sends it to the server,
 * receives the server's encrypted reply, decrypts it and displays it.
 *
 * Build : gcc tcpClient.c des.c -o tcpClient.exe -lws2_32
 * Run   : tcpClient.exe [server_ip] [port] [-v]     (default 127.0.0.1 and 9000)
 */
#include <stdlib.h>
#include "des_common.h"

int main(int argc, char **argv)
{
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serverAddr;
    const char *ip = "127.0.0.1";
    int port = DEFAULT_PORT, i, positional = 0;
    char message[BLOCK + 1], decrypted[BLOCK + 1];
    unsigned char cipher[BLOCK];

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) des_trace = 1;
        else if (positional++ == 0) ip = argv[i];
        else port = atoi(argv[i]);
    }

    printf("[+] Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("[-] Failed. Error Code : %d\n", WSAGetLastError());
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) { printf("[-] Error in socket creation.\n"); return 1; }
    printf("[+] Client Socket is created.\n");

    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_port        = htons((unsigned short)port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("[-] Connection to %s:%d failed (error %d).\n", ip, port, WSAGetLastError());
        return 1;
    }
    printf("[+] Connected to Server %s:%d\n", ip, port);

    while (1) {
        printf("\nEnter your plaintext message (max %d chars, \"exit\" to quit): ", BLOCK);
        read_message(message);                                   /* 1. create message */

        encrypt_message(message, cipher);                        /* 2. encrypt        */
        printf("[DEBUG] Plaintext: \"%s\"\n", message);
        print_hex("[SENT] Ciphertext     : ", cipher, BLOCK);
        send(sock, (const char *)cipher, BLOCK, 0);              /* 3. send           */

        if (strcmp(message, "exit") == 0) break;

        if (!recv_all(sock, cipher, BLOCK)) {                    /* 4. receive        */
            printf("[*] Server disconnected.\n");
            break;
        }
        print_hex("[RECEIVED] Ciphertext : ", cipher, BLOCK);
        decrypt_message(cipher, decrypted);                      /* 5. decrypt        */
        printf("[DECRYPTED] Message from Server: %s\n", decrypted); /* 6. display     */
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
