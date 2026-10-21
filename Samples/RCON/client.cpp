// simple SA-MP UDP RCON client
// no RakNet library is required
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
typedef int SOCKET;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool SendCommand(SOCKET socket, const sockaddr_in& server, const char header[11], const char* password, const char* command)
{
    const unsigned int passwordLength = (unsigned int)strlen(password);
    const unsigned int commandLength = (unsigned int)strlen(command);
    char packet[4096];

    if (passwordLength > 65535 || commandLength > 65535 || 15 + passwordLength + commandLength > sizeof(packet))
        return false;

    memcpy(packet, header, 11);
    unsigned int pos = 11;
    packet[pos++] = (char)passwordLength;
    packet[pos++] = (char)(passwordLength >> 8);
    memcpy(packet + pos, password, passwordLength);
    pos += passwordLength;
    packet[pos++] = (char)commandLength;
    packet[pos++] = (char)(commandLength >> 8);
    memcpy(packet + pos, command, commandLength);
    pos += commandLength;

    return sendto(socket, packet, (int)pos, 0, (const sockaddr*)&server, sizeof(server)) == (int)pos;
}

static void ReceiveReplies(SOCKET socket, const sockaddr_in& server, const char header[11])
{
    bool received = false;
    for (;;)
    {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(socket, &readSet);

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;
        const int ready = select((int)socket + 1, &readSet, 0, 0, &timeout);
        if (ready <= 0)
            break;

        char buffer[4096];
        sockaddr_in sender;
#ifdef _WIN32
        int senderSize = sizeof(sender);
#else
        socklen_t senderSize = sizeof(sender);
#endif
        const int length = recvfrom(socket, buffer, sizeof(buffer), 0, (sockaddr*)&sender, &senderSize);
        if (length < 13 || sender.sin_addr.s_addr != server.sin_addr.s_addr || sender.sin_port != server.sin_port || memcmp(buffer, header, 11) != 0)
            continue;

        const unsigned int textLength = (unsigned char)buffer[11] | ((unsigned char)buffer[12] << 8);
        if (textLength != (unsigned int)length - 13)
            continue;

        fwrite(buffer + 13, 1, textLength, stdout);
        putchar('\n');
        received = true;
    }
    if (!received)
        puts("No RCON response.");
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        puts("Usage: client <ip/host> <port> <password>");
        return 1;
    }

    char* end = 0;
    const long portValue = strtol(argv[2], &end, 10);
    if (end == argv[2] || *end != 0 || portValue < 1 || portValue > 65535 ||
        !*argv[3])
    {
        puts("Invalid port or password.");
        return 1;
    }
    const unsigned short port = (unsigned short)portValue;

#ifdef _WIN32
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
        return 1;
#endif

    hostent* host = gethostbyname(argv[1]);
    if (!host || host->h_addrtype != AF_INET || !host->h_addr_list[0])
    {
        puts("Cannot resolve host.");
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    memcpy(&server.sin_addr, host->h_addr_list[0], 4);

    SOCKET socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket == INVALID_SOCKET)
    {
        puts("Cannot open UDP socket.");
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    // SA-MP query header: 'SAMP' + IPv4 address + little-endian port + 'x'
    char header[11];
    memcpy(header, "SAMP", 4);
    memcpy(header + 4, &server.sin_addr, 4);
    header[8] = (char)port;
    header[9] = (char)(port >> 8);
    header[10] = 'x';

    printf("SA-MP RCON: %s:%u\n", argv[1], (unsigned int)port);
    puts("Enter a command (quit to exit).");

    char command[1024];
    while (fgets(command, sizeof(command), stdin))
    {
        const unsigned int length = (unsigned int)strlen(command);
        if (length && command[length - 1] != '\n' && !feof(stdin))
        {
            // кeject truncated input rather than sending an incomplete command
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            puts("Command is too long.");
            continue;
        }
        command[strcspn(command, "\r\n")] = 0;
        if (strcmp(command, "quit") == 0)
            break;
        if (!*command)
            continue;

        if (!SendCommand(socket, server, header, argv[3], command))
            puts("Cannot send RCON command.");
        else
            ReceiveReplies(socket, server, header);
    }

    closesocket(socket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
