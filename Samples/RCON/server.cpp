#include "../Query/Query.h"
#include "RakNetworkFactory.h"
#include "RakServerInterface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void OnRcon(const char* command, Query::RconReply& reply, void*)
{
    if (strcmp(command, "status") == 0)
    {
        reply.Write("RCON test server is running.");
    }
    else if (strcmp(command, "help") == 0)
    {
        reply.Write("Commands: status, echo <text>, help");
    }
    else if (strncmp(command, "echo ", 5) == 0)
    {
        reply.Write(command + 5);
    }
    else
    {
        reply.Write("Unknown command.");
    }
}

int main(int argc, char** argv)
{
    if (argc > 3)
    {
        printf("Usage: server [port] [password]\n");
        return 1;
    }

    unsigned short port = 7777;
    if (argc > 1)
    {
        char* end = 0;
        const long value = strtol(argv[1], &end, 10);
        if (end == argv[1] || *end != 0 || value < 1 || value > 65535)
        {
            printf("Invalid port.\n");
            return 1;
        }
        port = (unsigned short)value;
    }

    const char* password = argc > 2 ? argv[2] : getenv("SAMP_RCON_PASSWORD");
    if (!password || !*password)
    {
        printf("Provide a password or set SAMP_RCON_PASSWORD.\n");
        return 1;
    }

    Query query;
    query.SetRconPassword(password);
    query.SetRconHandler(OnRcon, 0);

    RakServerInterface* server = RakNetworkFactory::GetRakServerInterface();
    if (!server)
        return 1;

    if (!query.Install())
    {
        printf("Cannot install RCON handler.\n");
        RakNetworkFactory::DestroyRakServerInterface(server);
        return 1;
    }

    if (!server->Start(32, 0, 10, port))
    {
        printf("Cannot start RCON server on port %u.\n", (unsigned int)port);
        query.Uninstall();
        RakNetworkFactory::DestroyRakServerInterface(server);
        return 1;
    }

    printf("RCON test server listening on UDP port %u.\n", (unsigned int)port);
    printf("Press Enter to stop.\n");
    getchar();

    server->Disconnect(0);
    RakNetworkFactory::DestroyRakServerInterface(server);
    query.Uninstall();
    return 0;
}
