// Simple SA-MP UDP query client. No RakNet library is required.
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
typedef int SOCKET;
#define INVALID_SOCKET (-1)
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

class Reader
{
public:
    Reader(const char* data, int length) : data((const unsigned char*)data),
                                             length(length), pos(0) {}

    bool Read8(unsigned int& value)
    {
        if (length - pos < 1)
            return false;
        value = data[pos++];
        return true;
    }

    bool Read16(unsigned int& value)
    {
        if (length - pos < 2)
            return false;
        value = (unsigned int)data[pos] | ((unsigned int)data[pos + 1] << 8);
        pos += 2;
        return true;
    }

    bool Read32(unsigned int& value)
    {
        if (length - pos < 4)
            return false;
        value = (unsigned int)data[pos] |
                ((unsigned int)data[pos + 1] << 8) |
                ((unsigned int)data[pos + 2] << 16) |
                ((unsigned int)data[pos + 3] << 24);
        pos += 4;
        return true;
    }

    bool ReadString8(std::string& value)
    {
        unsigned int size;
        return Read8(size) && ReadString(value, size);
    }

    bool ReadString32(std::string& value)
    {
        unsigned int size;
        return Read32(size) && ReadString(value, size);
    }

    bool Finished() const { return pos == length; }

private:
    bool ReadString(std::string& value, unsigned int size)
    {
        if (size > (unsigned int)(length - pos))
            return false;
        value.assign((const char*)data + pos, size);
        pos += (int)size;
        return true;
    }

    const unsigned char* data;
    int length;
    int pos;
};

static unsigned long GetMilliseconds()
{
#ifdef _WIN32
    return GetTickCount();
#else
    timeval now;
    gettimeofday(&now, 0);
    return (unsigned long)(now.tv_sec * 1000UL + now.tv_usec / 1000);
#endif
}

static bool PrintReply(char type, const char* data, int length,
                       unsigned long elapsed)
{
    Reader reader(data, length);
    unsigned int count, value, id, score, ping;
    std::string name, game, language, detail;

    switch (type)
    {
    case 'i':
        if (!reader.Read8(value)) return false;
        printf("Password: %s\n", value ? "yes" : "no");
        if (!reader.Read16(count) || !reader.Read16(value) ||
            !reader.ReadString32(name) || !reader.ReadString32(game) ||
            !reader.ReadString32(language)) return false;
        printf("Hostname: %s\nPlayers: %u/%u\nGamemode: %s\nLanguage: %s\n",
               name.c_str(), count, value, game.c_str(), language.c_str());
        break;

    case 'r':
        if (!reader.Read16(count) || count > 1024) return false;
        printf("Rules (%u):\n", count);
        for (unsigned int i = 0; i < count; ++i)
        {
            if (!reader.ReadString8(name) || !reader.ReadString8(detail))
                return false;
            printf("  %s: %s\n", name.c_str(), detail.c_str());
        }
        break;

    case 'c':
    case 'd':
        if (!reader.Read16(count) || count > 1024) return false;
        printf("Players (%u):\n", count);
        for (unsigned int i = 0; i < count; ++i)
        {
            id = 0;
            if ((type == 'd' && !reader.Read8(id)) ||
                !reader.ReadString8(name) || !reader.Read32(score))
                return false;
            if (type == 'd')
            {
                if (!reader.Read32(ping)) return false;
                printf("  %u: %s (score: %d, ping: %u)\n", id,
                       name.c_str(), (int)score, ping);
            }
            else
                printf("  %s (score: %d)\n", name.c_str(), (int)score);
        }
        break;

    case 'p':
        if (length != 4) return false;
        printf("Ping: %lu ms\n", elapsed);
        break;

    default:
        return false;
    }
    return reader.Finished() || type == 'p';
}

static bool RunQuery(SOCKET socket, const sockaddr_in& server,
                     const char baseHeader[10], char type)
{
    char request[15];
    memcpy(request, baseHeader, 10);
    request[10] = type;
    int requestSize = 11;
    if (type == 'p')
    {
        request[11] = '\x53';
        request[12] = '\x41';
        request[13] = '\x4d';
        request[14] = '\x50';
        requestSize = 15;
    }

    printf("\n[%c] ", type);
    switch (type)
    {
    case 'i': puts("Server information"); break;
    case 'r': puts("Server rules"); break;
    case 'c': puts("Player list"); break;
    case 'd': puts("Detailed player list"); break;
    case 'p': puts("Ping"); break;
    }

    const unsigned long started = GetMilliseconds();
    if (sendto(socket, request, requestSize, 0,
               (const sockaddr*)&server, sizeof(server)) != requestSize)
    {
        puts("Cannot send query.");
        return false;
    }

    // Query is connectionless UDP. Discard unrelated datagrams.
    for (;;)
    {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(socket, &readSet);
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        if (select((int)socket + 1, &readSet, 0, 0, &timeout) <= 0)
        {
            puts("No response.");
            return false;
        }

        char buffer[16384];
        sockaddr_in sender;
#ifdef _WIN32
        int senderSize = sizeof(sender);
#else
        socklen_t senderSize = sizeof(sender);
#endif
        const int size = recvfrom(socket, buffer, sizeof(buffer), 0,
                                  (sockaddr*)&sender, &senderSize);
        if (size < 11 || sender.sin_addr.s_addr != server.sin_addr.s_addr ||
            sender.sin_port != server.sin_port ||
            memcmp(buffer, request, 11) != 0)
            continue;
        if (type == 'p' && (size != 15 || memcmp(buffer + 11, request + 11, 4)))
            continue;

        if (!PrintReply(type, buffer + 11, size - 11,
                        GetMilliseconds() - started))
        {
            puts("Invalid query response.");
            return false;
        }
        return true;
    }
}

int main(int argc, char** argv)
{
    if (argc < 3 || argc > 4)
    {
        puts("Usage: client <ip/host> <port> [i|r|c|d|p|all]");
        return 1;
    }

    char* end = 0;
    const long portValue = strtol(argv[2], &end, 10);
    if (end == argv[2] || *end || portValue < 1 || portValue > 65535)
    {
        puts("Invalid port.");
        return 1;
    }

    const char* mode = argc == 4 ? argv[3] : "all";
    if (strcmp(mode, "all") &&
        !(mode[0] && !mode[1] && strchr("ircdp", mode[0])))
    {
        puts("Invalid query type.");
        return 1;
    }

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
    server.sin_port = htons((unsigned short)portValue);
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

    char header[10];
    memcpy(header, "SAMP", 4);
    memcpy(header + 4, &server.sin_addr, 4);
    header[8] = (char)portValue;
    header[9] = (char)(portValue >> 8);

    printf("SA-MP Query: %s:%ld\n", argv[1], portValue);
    bool success = true;
    const char* types = strcmp(mode, "all") == 0 ? "ircdp" : mode;
    for (const char* type = types; *type; ++type)
        if (!RunQuery(socket, server, header, *type))
            success = false;

    closesocket(socket);
#ifdef _WIN32
    WSACleanup();
#endif
    return success ? 0 : 1;
}
