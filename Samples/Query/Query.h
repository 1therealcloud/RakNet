#ifndef SAMP_QUERY_H
#define SAMP_QUERY_H

#include "SocketLayer.h"

// SA-MP UDP query handler
// install before starting the RakNet server and uninstall after stopping it
// only one instance can be installed at a time
class Query
{
public:
    Query();
    ~Query();

    class RconReply
    {
    public:
        // sends each line as a separate datagram
        bool Write(const char* text);

    private:
        friend class Query;
        RconReply(SOCKET socket, const char* header, const sockaddr_in& sender);
        bool WriteLine(const char* text, unsigned int length);

        SOCKET socket;
        char header[11];
        sockaddr_in sender;
    };

    // runs on the RakNet receive thread
    typedef void (*RconHandler)(const char* command, RconReply& reply, void* userData);

    bool Install();
    void Uninstall();

    // query replies are disabled until SetInfo is called
    // RCON is independent
    void SetInfo(const char* hostname, const char* gamemode, const char* language, unsigned short maxPlayers, bool passworded);
    void SetPlayerCount(unsigned short count);

    bool SetRule(const char* name, const char* value);
    void RemoveRule(const char* name);
    void ClearRules();

    // adds or updates a player
    // the count shown by 'i' is set separately
    bool SetPlayer(unsigned short id, const char* name, int score, unsigned int ping);
    void RemovePlayer(unsigned short id);
    void ClearPlayers();

    // an empty password disables RCON
    // no commands are built into this API
    void SetRconPassword(const char* password);
    void SetRconHandler(RconHandler handler, void* userData);

private:
    Query(const Query&);
    Query& operator=(const Query&);

    struct State;
    State* state;
    static Query* active;
    static bool OnQuery(SOCKET socket, const char* request, int length, const sockaddr_in& sender);
    bool Handle(SOCKET socket, const char* request, int length, const sockaddr_in& sender);
    bool HandleRcon(SOCKET socket, const char* request, int length, const sockaddr_in& sender);
};

#endif
