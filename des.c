/*
 * des.c - DES (FIPS 46-3) implemented step by step for CMSE456 Lab 2.
 * Bit numbering follows the standard: bit 1 is the most significant bit.
 * Each transformation is its own function so it can be stepped through
 * in the debugger and its variables watched.
 */
#include <stdio.h>
#include "des.h"

int des_trace = 0;

/* ------------------------------ tables ------------------------------ */
static const unsigned char IP[64] = {
    58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7
};
static const unsigned char FP[64] = {
    40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25
};
static const unsigned char E[48] = {
    32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1
};
static const unsigned char P[32] = {
    16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26, 5, 18, 31, 10,
    2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6, 22, 11, 4, 25
};
static const unsigned char PC1[56] = {
    57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4
};
static const unsigned char PC2[48] = {
    14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};
static const unsigned char SHIFTS[16] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};
static const unsigned char SBOX[8][64] = {
    {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13},
    {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9},
    {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12},
    {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14},
    {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3},
    {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13},
    {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12},
    {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
};

/* ---------------------------- primitives ---------------------------- */

/* Picks bits of `in` (an in_bits-wide value) as listed in `table` (1-based, MSB = 1). */
static uint64_t permute(uint64_t in, const unsigned char *table, int out_bits, int in_bits)
{
    uint64_t out = 0;
    int i;
    for (i = 0; i < out_bits; i++)
        out = (out << 1) | ((in >> (in_bits - table[i])) & 1ULL);
    return out;
}

/* ---------------------------- DES steps ----------------------------- */

uint64_t des_initial_permutation(uint64_t block)         { return permute(block, IP, 64, 64); }
uint64_t des_inverse_initial_permutation(uint64_t block) { return permute(block, FP, 64, 64); }
uint64_t des_expansion(uint32_t r)                       { return permute(r, E, 48, 32); }
uint64_t des_permuted_choice1(uint64_t key)              { return permute(key, PC1, 56, 64); }

/* Left circular shift of both 28-bit halves by the amount scheduled for `round` (0..15). */
void des_shift_halves(uint32_t *c, uint32_t *d, int round)
{
    int s = SHIFTS[round];
    const uint32_t mask = 0x0FFFFFFF;
    *c = ((*c << s) | (*c >> (28 - s))) & mask;
    *d = ((*d << s) | (*d >> (28 - s))) & mask;
}

uint64_t des_permuted_choice2(uint32_t c, uint32_t d)
{
    uint64_t cd = ((uint64_t)c << 28) | d;
    return permute(cd, PC2, 48, 56);
}

void des_key_schedule(uint64_t key, uint64_t subkey[16])
{
    uint64_t cd = des_permuted_choice1(key);
    uint32_t c = (uint32_t)(cd >> 28) & 0x0FFFFFFF;
    uint32_t d = (uint32_t)cd & 0x0FFFFFFF;
    int r;
    if (des_trace) printf("PC-1 : C0=%07X D0=%07X\n", (unsigned)c, (unsigned)d);
    for (r = 0; r < 16; r++) {
        des_shift_halves(&c, &d, r);
        subkey[r] = des_permuted_choice2(c, d);
        if (des_trace)
            printf("K%-2d  : shift=%d C=%07X D=%07X subkey=%012llX\n", r + 1, SHIFTS[r],
                   (unsigned)c, (unsigned)d, (unsigned long long)subkey[r]);
    }
}

uint64_t des_xor_round_key(uint64_t expanded, uint64_t subkey) { return expanded ^ subkey; }

/* 8 S-boxes: each takes 6 bits, outer bits = row, middle 4 bits = column, gives 4 bits. */
uint32_t des_sboxes(uint64_t x48)
{
    uint32_t out = 0;
    int i;
    for (i = 0; i < 8; i++) {
        unsigned six = (unsigned)(x48 >> (42 - 6 * i)) & 0x3F;
        unsigned row = ((six >> 4) & 2) | (six & 1);
        unsigned col = (six >> 1) & 0x0F;
        out = (out << 4) | SBOX[i][row * 16 + col];
    }
    return out;
}

uint32_t des_permutation_p(uint32_t s_out) { return (uint32_t)permute(s_out, P, 32, 32); }

uint32_t des_round_function(uint32_t r, uint64_t subkey)
{
    uint64_t expanded = des_expansion(r);
    uint64_t xored    = des_xor_round_key(expanded, subkey);
    uint32_t s_out    = des_sboxes(xored);
    uint32_t p_out    = des_permutation_p(s_out);
    if (des_trace)
        printf("       E(R)=%012llX  E^K=%012llX  S=%08X  P=%08X\n",
               (unsigned long long)expanded, (unsigned long long)xored,
               (unsigned)s_out, (unsigned)p_out);
    return p_out;
}

uint32_t des_xor_left(uint32_t left, uint32_t f_out) { return left ^ f_out; }

uint64_t des_swap(uint32_t left, uint32_t right) { return ((uint64_t)right << 32) | left; }

/* ------------------------------ cipher ------------------------------ */

static uint64_t des_core(uint64_t block, uint64_t key, int decrypt)
{
    uint64_t subkey[16];
    uint64_t ip;
    uint32_t left, right, new_right;
    int round;

    des_key_schedule(key, subkey);

    ip    = des_initial_permutation(block);
    left  = (uint32_t)(ip >> 32);
    right = (uint32_t)ip;
    if (des_trace) printf("IP   : L0=%08X R0=%08X\n", (unsigned)left, (unsigned)right);

    for (round = 0; round < 16; round++) {
        uint64_t k = decrypt ? subkey[15 - round] : subkey[round];
        uint32_t f = des_round_function(right, k);
        new_right  = des_xor_left(left, f);          /* R(i) = L(i-1) xor f(R(i-1), K(i)) */
        left       = right;                          /* L(i) = R(i-1)                     */
        right      = new_right;
        if (des_trace)
            printf("R%-2d  : L=%08X R=%08X  K=%012llX\n", round + 1,
                   (unsigned)left, (unsigned)right, (unsigned long long)k);
    }

    /* 32-bit swap of the halves, then the inverse initial permutation */
    {
        uint64_t preoutput = des_swap(left, right);  /* = R16 || L16 */
        uint64_t out = des_inverse_initial_permutation(preoutput);
        if (des_trace) printf("IP-1 : %016llX\n", (unsigned long long)out);
        return out;
    }
}

uint64_t des_encrypt_block(uint64_t plain,  uint64_t key) { return des_core(plain,  key, 0); }
uint64_t des_decrypt_block(uint64_t cipher, uint64_t key) { return des_core(cipher, key, 1); }

uint64_t des_bytes_to_u64(const unsigned char b[8])
{
    uint64_t v = 0;
    int i;
    for (i = 0; i < 8; i++) v = (v << 8) | b[i];
    return v;
}

void des_u64_to_bytes(uint64_t v, unsigned char b[8])
{
    int i;
    for (i = 7; i >= 0; i--) { b[i] = (unsigned char)(v & 0xFF); v >>= 8; }
}
