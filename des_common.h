#ifndef DES_COMMON_H
#define DES_COMMON_H

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "des.h"

#define DEFAULT_PORT 9000
#define BLOCK 8                                   /* one DES block = 8 bytes = max 8 characters */

/* Pre-shared 64-bit DES key (both machines must use the same key). */
static const uint64_t SHARED_KEY = 0x0f1571c947d9e859ULL;

static void print_hex(const char *label, const unsigned char *b, int n)
{
    int i;
    printf("%s", label);
    for (i = 0; i < n; i++) printf("%02X ", b[i]);
    printf("\n");
}

/* Plaintext (<= 8 chars) -> padded block -> DES -> 8 ciphertext bytes. */
static void encrypt_message(const char *text, unsigned char out[BLOCK])
{
    unsigned char block[BLOCK] = {0};              /* zero padding */
    size_t len = strlen(text);
    if (len > BLOCK) len = BLOCK;
    memcpy(block, text, len);
    des_u64_to_bytes(des_encrypt_block(des_bytes_to_u64(block), SHARED_KEY), out);
}

/* 8 ciphertext bytes -> DES decrypt -> NUL-terminated text. */
static void decrypt_message(const unsigned char in[BLOCK], char text[BLOCK + 1])
{
    unsigned char block[BLOCK];
    des_u64_to_bytes(des_decrypt_block(des_bytes_to_u64(in), SHARED_KEY), block);
    memcpy(text, block, BLOCK);
    text[BLOCK] = '\0';
}

/* TCP is a byte stream: keep calling recv() until exactly n bytes have arrived. */
static int recv_all(SOCKET s, unsigned char *buf, int n)
{
    int got = 0;
    while (got < n) {
        int r = recv(s, (char *)buf + got, n - got, 0);
        if (r <= 0) return 0;                      /* closed or error */
        got += r;
    }
    return 1;
}

/* Reads one line from stdin, strips the newline, keeps at most 8 characters. */
static void read_message(char msg[BLOCK + 1])
{
    char line[256];
    if (!fgets(line, sizeof(line), stdin)) { strcpy(msg, "exit"); return; }
    line[strcspn(line, "\r\n")] = '\0';
    strncpy(msg, line, BLOCK);
    msg[BLOCK] = '\0';
    if (strlen(line) > BLOCK) printf("[!] Only the first %d characters are used: \"%s\"\n", BLOCK, msg);
}

#endif
