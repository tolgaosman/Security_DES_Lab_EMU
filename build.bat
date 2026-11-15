@echo off
setlocal

echo Building des_test.exe...
gcc -std=c11 -Wall -Wextra des_test.c des.c -o des_test.exe || exit /b 1

echo Building tcpServer.exe...
gcc -std=c11 -Wall -Wextra tcpServer.c des.c -o tcpServer.exe -lws2_32 || exit /b 1

echo Building tcpClient.exe...
gcc -std=c11 -Wall -Wextra tcpClient.c des.c -o tcpClient.exe -lws2_32 || exit /b 1

echo Done! All binaries built successfully.

