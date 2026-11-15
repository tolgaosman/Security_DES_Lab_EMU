#ifndef DES_H
#define DES_H

#include <stdint.h>

/* Set to 1 to print every intermediate DES value (IP, round keys, L/R per round, ...). */
extern int des_trace;

/* ---- the 12 DES steps required by the lab sheet (each one is a separate function) ---- */
uint64_t des_initial_permutation(uint64_t block);               /* 1  IP                        */
uint64_t des_inverse_initial_permutation(uint64_t block);       /* 2  IP^-1                     */
uint64_t des_expansion(uint32_t r);                             /* 3  E: 32 -> 48 bits          */
uint64_t des_permuted_choice1(uint64_t key);                    /* 4  PC-1: 64 -> 56 bits       */
void     des_shift_halves(uint32_t *c, uint32_t *d, int round); /* 5  left circular shifts      */
uint64_t des_permuted_choice2(uint32_t c, uint32_t d);          /* 6  PC-2: 56 -> 48 bits       */
void     des_key_schedule(uint64_t key, uint64_t subkey[16]);   /*    PC-1 + shifts + PC-2 x16  */
uint64_t des_xor_round_key(uint64_t expanded, uint64_t subkey); /* 7  XOR with round key        */
uint32_t des_sboxes(uint64_t x48);                              /* 8  S-boxes: 48 -> 32 bits    */
uint32_t des_permutation_p(uint32_t s_out);                     /* 9  P                         */
uint32_t des_round_function(uint32_t r, uint64_t subkey);       /*    E, XOR, S, P              */
uint32_t des_xor_left(uint32_t left, uint32_t f_out);           /* 10 XOR with left half        */
uint64_t des_swap(uint32_t left, uint32_t right);               /* 11 swap of the halves        */

/* ---- full cipher on one 64-bit block ---- */
uint64_t des_encrypt_block(uint64_t plain,  uint64_t key);
uint64_t des_decrypt_block(uint64_t cipher, uint64_t key);

/* ---- helpers: 8 bytes <-> 64-bit value (big endian) ---- */
uint64_t des_bytes_to_u64(const unsigned char b[8]);
void     des_u64_to_bytes(uint64_t v, unsigned char b[8]);

#endif
