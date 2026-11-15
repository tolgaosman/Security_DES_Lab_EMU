CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -O2
LDLIBS  = -lws2_32

TARGETS = des_test.exe tcpServer.exe tcpClient.exe

all: $(TARGETS)

des_test.exe: des_test.c des.c des.h
	$(CC) $(CFLAGS) des_test.c des.c -o $@

tcpServer.exe: tcpServer.c des.c des.h des_common.h
	$(CC) $(CFLAGS) tcpServer.c des.c -o $@ $(LDLIBS)

tcpClient.exe: tcpClient.c des.c des.h des_common.h
	$(CC) $(CFLAGS) tcpClient.c des.c -o $@ $(LDLIBS)

test: des_test.exe
	./des_test.exe

clean:
	rm -f $(TARGETS) *.o

.PHONY: all test clean

