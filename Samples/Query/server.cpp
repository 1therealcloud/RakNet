#include "Query.h"
#include "RakNetworkFactory.h"
#include "RakServerInterface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    unsigned short port = 7777;
    if (argc > 1)
    {
        char* end = 0;
        const long value = strtol(argv[1], &end, 10);
        if (end == argv[1] || *end || value < 1 || value > 65535)
        {
            printf("Usage: Query [port]\n");
            return 1;
        }
        port = (unsigned short)value;
    }

    Query query;
    query.SetInfo("RakNet Query Sample", "Example", "English", 32, false);

    query.SetRule("mapname", "San Andreas");
    query.SetRule("lagcomp", "On");

    query.SetPlayer(0, "PlayerOne", 100, 45);
    query.SetPlayer(1, "PlayerTwo", 200, 70);
    
    query.SetPlayerCount(2);

    RakServerInterface* server = RakNetworkFactory::GetRakServerInterface();
    if (!server || !query.Install())
    {
        printf("Cannot install query handler\n");
        if (server)
            RakNetworkFactory::DestroyRakServerInterface(server);
        return 1;
    }

    if (!server->Start(32, 0, 10, port))
    {
        printf("Cannot start server on UDP port %u\n", (unsigned int)port);
        query.Uninstall();
        RakNetworkFactory::DestroyRakServerInterface(server);
        return 1;
    }

    printf("SA-MP query listening on UDP port %u\n", (unsigned int)port);
    printf("Press Enter to stop.\n");
    getchar();

    server->Disconnect(0);
    RakNetworkFactory::DestroyRakServerInterface(server);
    query.Uninstall();
    return 0;
}
