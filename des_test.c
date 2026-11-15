/*
 * des_test.c - verifies each DES step against standard FIPS 46-3 test vectors
 * and demonstrates the avalanche effect.  Build: gcc des_test.c des.c -o des_test
 * Run with "-v" to print all intermediate values (round keys, L/R per round).
 */
#include <stdio.h>
#include <string.h>
#include "des.h"

static int bits_diff(uint64_t a, uint64_t b)
{
    uint64_t x = a ^ b;
    int n = 0;
    while (x) { n += (int)(x & 1); x >>= 1; }
    return n;
}

static int check(const char *name, uint64_t got, uint64_t expected)
{
    int ok = (got == expected);
    printf("%-34s got=%016llX expected=%016llX  %s\n", name,
           (unsigned long long)got, (unsigned long long)expected, ok ? "OK" : "FAIL");
    return ok;
}

int main(int argc, char **argv)
{
    const uint64_t plain  = 0x02468aceeca86420ULL;
    const uint64_t key    = 0x0f1571c947d9e859ULL;
    const uint64_t cipher = 0xda02ce3a89ecac3bULL;   /* from the lab example, Table 4.2 */
    int fails = 0;
    uint64_t ip, enc, dec;
    uint64_t subkey[16];

    if (argc > 1 && strcmp(argv[1], "-v") == 0) des_trace = 1;

    /* Table 4.2: first row = halves after IP; K1 and K16 of the example */
    ip = des_initial_permutation(plain);
    fails += !check("Initial permutation (IP)", ip, 0x5a005a003cf03c0fULL);
    printf("  L0 = %08X  R0 = %08X\n", (unsigned)(ip >> 32), (unsigned)ip);
    fails += !check("Inverse IP (IP^-1 of IP)", des_inverse_initial_permutation(ip), plain);

    des_key_schedule(key, subkey);
    fails += !check("Round key K1  (48 bit)", subkey[0],  0x7833c320da70ULL);
    fails += !check("Round key K2  (48 bit)", subkey[1],  0x2b1a74ca48d8ULL);
    fails += !check("Round key K16 (48 bit)", subkey[15], 0xa6120b4d4c25ULL);
    /* Table 4.2 prints subkeys as eight 6-bit values: K1 = 1e 03 0f 03 08 0d 29 30 */
    printf("  K1 as 6-bit groups:");
    { int i; for (i = 0; i < 8; i++) printf(" %02X", (unsigned)(subkey[0] >> (42 - 6 * i)) & 0x3F); }
    printf("\n");

    enc = des_encrypt_block(plain, key);
    fails += !check("Encryption (ciphertext)", enc, cipher);
    dec = des_decrypt_block(enc, key);
    fails += !check("Decryption (plaintext)", dec, plain);

    printf("\nAvalanche effect:\n");
    printf("  1 bit changed in plaintext -> %d bits differ in ciphertext\n",
           bits_diff(enc, des_encrypt_block(0x12468aceeca86420ULL, key)));
    printf("  1 bit changed in key       -> %d bits differ in ciphertext\n",
           bits_diff(enc, des_encrypt_block(plain, 0x1f1571c947d9e859ULL)));

    printf("\n%s\n", fails ? "SOME TESTS FAILED" : "ALL TESTS PASSED");
    return fails != 0;
}
