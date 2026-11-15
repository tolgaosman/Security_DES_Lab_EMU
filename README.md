# DES Block Cipher & Encrypted TCP Chat (C / Winsock)

A clean, from-scratch C11 implementation of the **Data Encryption Standard (DES, FIPS 46-3)** paired with a bidirectional **TCP Client/Server** application using Winsock2 to exchange DES-encrypted messages over a network.

Developed as part of the **Information & Network Security (CMSE456)** laboratory at Eastern Mediterranean University (EMU).

---

## Features

- **Modular DES Engine (`des.c` / `des.h`)** — Every stage of the 16-round Feistel cipher is implemented as an explicit, inspectable function:
  1. Initial Permutation (`IP`) and Inverse Initial Permutation (`IP^-1`)
  2. Key Schedule: Permuted Choice 1 (`PC-1`, 64 -> 56 bits), Left Circular Shifts (`C_i`, `D_i`), and Permuted Choice 2 (`PC-2`, 56 -> 48 bits)
  3. Feistel Round Function `f(R, K)`: Expansion (`E`, 32 -> 48 bits), Round Key XOR, 8 S-Box substitutions (`48 -> 32` bits), and Straight Permutation (`P`)
  4. Left-half XOR and final 32-bit swap
- **Verbose Trace Mode (`-v`)** — Prints intermediate values (`C0/D0`, all 16 subkeys `K1..K16`, `E(R)`, `E^K`, S-Box output, `P`, and per-round `L_i/R_i`) for step-by-step verification and debugging.
- **Automated Test Suite & Avalanche Analysis (`des_test.c`)** — Verifies intermediate permutations, subkeys, encryption, and decryption against standard textbook/FIPS test vectors, and measures ciphertext bit diffusion when flipping 1 bit in the plaintext or key.
- **DES-Encrypted TCP Client & Server (`tcpClient.c` / `tcpServer.c`)** — Encrypts 8-byte blocks using a pre-shared 64-bit key over TCP sockets, displaying both raw ciphertext (in hex) and decrypted plaintext on each side.

---

## Project Structure

| File | Description |
| :--- | :--- |
| `des.h` / `des.c` | Core DES (FIPS 46-3) block cipher implementation |
| `des_common.h` | Shared key, block padding/formatting, hex printer, and Winsock helpers |
| `des_test.c` | Known-answer test vectors and Avalanche Effect benchmark |
| `tcpServer.c` | TCP server that receives, decrypts, and replies with DES-encrypted blocks |
| `tcpClient.c` | TCP client that encrypts user messages with DES and decrypts server replies |
| `Makefile` / `build.bat` | Build scripts for MinGW-w64 (`gcc`) on Windows |

---

## Prerequisites

- **Windows** (uses `winsock2.h` / `ws2_32`)
- **GCC (MinGW-w64)** on your `PATH`

---

## Building

Using the batch script on Windows:

```bat
.\build.bat
```

Or with `make` / `mingw32-make`:

```bash
make
```

Or manually with `gcc`:

```bash
gcc -std=c11 -Wall -Wextra des_test.c des.c -o des_test.exe
gcc -std=c11 -Wall -Wextra tcpServer.c des.c -o tcpServer.exe -lws2_32
gcc -std=c11 -Wall -Wextra tcpClient.c des.c -o tcpClient.exe -lws2_32
```

---

## Usage

### 1. Run Unit Tests & Avalanche Check

```bash
.\des_test.exe
```

Pass `-v` to inspect every round's internal state:

```bash
.\des_test.exe -v
```

**Sample Output:**
```text
Initial permutation (IP)           got=5A005A003CF03C0F expected=5A005A003CF03C0F  OK
  L0 = 5A005A00  R0 = 3CF03C0F
Inverse IP (IP^-1 of IP)           got=02468ACEECA86420 expected=02468ACEECA86420  OK
Round key K1  (48 bit)             got=00007833C320DA70 expected=00007833C320DA70  OK
Round key K2  (48 bit)             got=00002B1A74CA48D8 expected=00002B1A74CA48D8  OK
Round key K16 (48 bit)             got=0000A6120B4D4C25 expected=0000A6120B4D4C25  OK
  K1 as 6-bit groups: 1E 03 0F 03 08 0D 29 30
Encryption (ciphertext)            got=DA02CE3A89ECAC3B expected=DA02CE3A89ECAC3B  OK
Decryption (plaintext)             got=02468ACEECA86420 expected=02468ACEECA86420  OK

Avalanche effect:
  1 bit changed in plaintext -> 32 bits differ in ciphertext
  1 bit changed in key       -> 30 bits differ in ciphertext

ALL TESTS PASSED
```

### 2. Run Encrypted TCP Server & Client

Open **two terminals**:

**Terminal 1 — Start the Server** (default port `9000`):
```bash
.\tcpServer.exe [port] [-v]
```

**Terminal 2 — Connect with the Client** (defaults to `127.0.0.1:9000`):
```bash
.\tcpClient.exe [server_ip] [port] [-v]
```

Type messages (up to 8 characters per DES block), or type `exit` from the client to close the session cleanly.

---

## Disclaimer

> **Educational Use Only:** Standard 56-bit DES is cryptographically obsolete and implemented here strictly for educational purposes to demonstrate Feistel network internals and symmetric block cipher mechanics. Do not use single DES in production systems.
