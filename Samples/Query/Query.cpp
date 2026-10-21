#include "Query.h"
#include "SimpleMutex.h"

#include <string.h>
#include <string>
#include <vector>

namespace
{
    enum { MAX_REPLY_SIZE = 16384, MAX_QUERY_PLAYERS = 100, MAX_RULES = 20 };

    class Reply
    {
    public:
        explicit Reply(const char* request) : size(11)
        {
            memcpy(data, request, 11);
        }

        bool Write(const void* bytes, unsigned int length)
        {
            if (length > sizeof(data) - size)
                return false;

            memcpy(data + size, bytes, length);
            size += length;
            return true;
        }

        bool Write8(unsigned char value)
        {
            return Write(&value, 1);
        }

        bool Write16(unsigned short value)
        {
            unsigned char bytes[2];
            bytes[0] = (unsigned char)value;
            bytes[1] = (unsigned char)(value >> 8);
            return Write(bytes, 2);
        }

        bool Write32(unsigned int value)
        {
            unsigned char bytes[4];
            bytes[0] = (unsigned char)value;
            bytes[1] = (unsigned char)(value >> 8);
            bytes[2] = (unsigned char)(value >> 16);
            bytes[3] = (unsigned char)(value >> 24);
            return Write(bytes, 4);
        }

        bool WriteString8(const std::string& value)
        {
            return value.size() <= 255 && Write8((unsigned char)value.size()) && Write(value.data(), (unsigned int)value.size());
        }

        bool WriteString32(const std::string& value)
        {
            return value.size() <= sizeof(data) - size - 4 && Write32((unsigned int)value.size()) && Write(value.data(), (unsigned int)value.size());
        }

        const char* Data() const { return (const char*)data; }
        int Size() const { return (int)size; }

    private:
        unsigned char data[MAX_REPLY_SIZE];
        unsigned int size;
    };

    struct Rule
    {
        std::string name;
        std::string value;
    };

    struct Player
    {
        unsigned short id;
        std::string name;
        int score;
        unsigned int ping;
    };
}

struct Query::State
{
    State() : hostname(""), gamemode(""), language(""), maxPlayers(16), playerCount(0), passworded(false), queryEnabled(false), rconHandler(0), rconUserData(0) {}

    SimpleMutex mutex;
    std::string hostname;
    std::string gamemode;
    std::string language;
    unsigned short maxPlayers;
    unsigned short playerCount;
    bool passworded;
    bool queryEnabled;
    std::vector<Rule> rules;
    std::vector<Player> players;
    std::string rconPassword;
    RconHandler rconHandler;
    void* rconUserData;
};

Query::RconReply::RconReply(SOCKET s, const char* request, const sockaddr_in& address) : socket(s), sender(address)
{
    memcpy(header, request, sizeof(header));
}

bool Query::RconReply::WriteLine(const char* text, unsigned int length)
{
    if (length > 1200)
        return false;

    char packet[11 + 2 + 1200];
    memcpy(packet, header, 11);
    packet[11] = (char)length;
    packet[12] = (char)(length >> 8);
    if (length)
        memcpy(packet + 13, text, length);

    return SocketLayer::Instance()->SendTo(socket, packet, (int)length + 13, sender.sin_addr.s_addr, ntohs(sender.sin_port)) == 0;
}

bool Query::RconReply::Write(const char* text)
{
    if (!text)
        return false;

    const char* p = text;
    do
    {
        const char* end = p;
        while (*end && *end != '\n' && *end != '\r')
            ++end;

        const unsigned int lineLength = (unsigned int)(end - p);
        unsigned int offset = 0;
        do
        {
            const unsigned int left = lineLength - offset;
            const unsigned int part = left > 1200 ? 1200 : left;

            if (!WriteLine(p + offset, part))
                return false;

            offset += part;
        } while (offset < lineLength);

        if (!*end)
            break;

        if (end[0] == '\r' && end[1] == '\n')
            ++end;

        p = end + 1;
    } while (*p);

    return true;
}

Query* Query::active = 0;

Query::Query() : state(new State) {}

Query::~Query()
{
    Uninstall();
    delete state;
}

bool Query::Install()
{
    if (active != 0)
        return active == this;

    active = this;
    SocketLayer::Instance()->SetQueryHandler(OnQuery);
    return true;
}

void Query::Uninstall()
{
    if (active == this)
    {
        SocketLayer::Instance()->SetQueryHandler(0);
        active = 0;
    }
}

void Query::SetInfo(const char* hostname, const char* gamemode, const char* language, unsigned short maxPlayers, bool passworded)
{
    state->mutex.Lock();
    state->hostname = hostname ? hostname : "";
    state->gamemode = gamemode ? gamemode : "";
    state->language = language ? language : "";
    state->maxPlayers = maxPlayers;
    state->passworded = passworded;
    state->queryEnabled = true;
    state->mutex.Unlock();
}

void Query::SetPlayerCount(unsigned short count)
{
    state->mutex.Lock();
    state->playerCount = count;
    state->mutex.Unlock();
}

bool Query::SetRule(const char* name, const char* value)
{
    if (!name || !value || !*name || strlen(name) > 255 || strlen(value) > 255)
        return false;

    state->mutex.Lock();
    for (unsigned int i = 0; i < state->rules.size(); ++i)
    {
        if (state->rules[i].name == name)
        {
            state->rules[i].value = value;
            state->mutex.Unlock();
            return true;
        }
    }
    if (state->rules.size() >= MAX_RULES)
    {
        state->mutex.Unlock();
        return false;
    }
    Rule rule;
    rule.name = name;
    rule.value = value;
    state->rules.push_back(rule);
    state->mutex.Unlock();
    return true;
}

void Query::RemoveRule(const char* name)
{
    if (!name)
        return;

    state->mutex.Lock();
    for (unsigned int i = 0; i < state->rules.size(); ++i)
    {
        if (state->rules[i].name == name)
        {
            state->rules.erase(state->rules.begin() + i);
            break;
        }
    }
    state->mutex.Unlock();
}

void Query::ClearRules()
{
    state->mutex.Lock();
    state->rules.clear();
    state->mutex.Unlock();
}

bool Query::SetPlayer(unsigned short id, const char* name, int score, unsigned int ping)
{
    if (!name || !*name || strlen(name) > 24)
        return false;

    state->mutex.Lock();
    for (unsigned int i = 0; i < state->players.size(); ++i)
    {
        if (state->players[i].id == id)
        {
            state->players[i].name = name;
            state->players[i].score = score;
            state->players[i].ping = ping;
            state->mutex.Unlock();
            return true;
        }
    }
    
    Player player;
    player.id = id;
    player.name = name;
    player.score = score;
    player.ping = ping;
    state->players.push_back(player);
    state->mutex.Unlock();
    return true;
}

void Query::RemovePlayer(unsigned short id)
{
    state->mutex.Lock();
    for (unsigned int i = 0; i < state->players.size(); ++i)
    {
        if (state->players[i].id == id)
        {
            state->players.erase(state->players.begin() + i);
            break;
        }
    }
    state->mutex.Unlock();
}

void Query::ClearPlayers()
{
    state->mutex.Lock();
    state->players.clear();
    state->mutex.Unlock();
}

void Query::SetRconPassword(const char* password)
{
    state->mutex.Lock();
    state->rconPassword = password ? password : "";
    state->mutex.Unlock();
}

void Query::SetRconHandler(RconHandler handler, void* userData)
{
    state->mutex.Lock();
    state->rconHandler = handler;
    state->rconUserData = userData;
    state->mutex.Unlock();
}

bool Query::HandleRcon(SOCKET socket, const char* request, int length, const sockaddr_in& sender)
{
    // header | u16 password length | password | u16 command length | command
    if (length < 15)
        return false;

    unsigned int pos = 11;
    const unsigned int passwordLength = (unsigned char)request[pos] | ((unsigned char)request[pos + 1] << 8);
    pos += 2;
    if (passwordLength == 0 || passwordLength > (unsigned int)length - pos ||
        (unsigned int)length - pos - passwordLength < 2)
        return false;

    const char* password = request + pos;
    pos += passwordLength;
    const unsigned int commandLength = (unsigned char)request[pos] | ((unsigned char)request[pos + 1] << 8);
    pos += 2;
    if (commandLength == 0 || commandLength != (unsigned int)length - pos ||
        memchr(password, 0, passwordLength) ||
        memchr(request + pos, 0, commandLength))
        return false;

    state->mutex.Lock();
    const bool enabled = !state->rconPassword.empty();
    const bool authorized = enabled &&
        state->rconPassword.size() == passwordLength &&
        memcmp(state->rconPassword.data(), password, passwordLength) == 0;
    const RconHandler handler = state->rconHandler;
    void* userData = state->rconUserData;
    state->mutex.Unlock();

    if (!enabled)
        return false;

    RconReply reply(socket, request, sender);
    if (!authorized)
        return reply.Write("Invalid RCON password.");
    if (!handler)
        return reply.Write("RCON handler is not configured.");

    const std::string command(request + pos, commandLength);
    handler(command.c_str(), reply, userData);
    return true;
}

bool Query::OnQuery(SOCKET socket, const char* request, int length, const sockaddr_in& sender)
{
    return active && active->Handle(socket, request, length, sender);
}

bool Query::Handle(SOCKET socket, const char* request, int length, const sockaddr_in& sender)
{
    if (length < 11 || memcmp(request, "SAMP", 4) != 0)
        return false;

    if (request[10] == 'x')
        return HandleRcon(socket, request, length, sender);

    Reply reply(request);
    bool valid = true;
    state->mutex.Lock();
    if (!state->queryEnabled)
    {
        state->mutex.Unlock();
        return false;
    }

    switch (request[10])
    {
    case 'i':
        valid = reply.Write8(state->passworded ? 1 : 0) && reply.Write16(state->playerCount) && reply.Write16(state->maxPlayers) && reply.WriteString32(state->hostname) && reply.WriteString32(state->gamemode) && reply.WriteString32(state->language);
        break;

    case 'r':
        valid = reply.Write16((unsigned short)state->rules.size());
        for (unsigned int i = 0; valid && i < state->rules.size(); ++i)
            valid = reply.WriteString8(state->rules[i].name) && reply.WriteString8(state->rules[i].value);
        break;

    case 'c':
    case 'd':
    {
        const bool detailed = request[10] == 'd';
        unsigned int count = 0;

        for (unsigned int i = 0; i < state->players.size(); ++i)
            if (!detailed || state->players[i].id <= 255)
                ++count;

        if (count > MAX_QUERY_PLAYERS)
            count = MAX_QUERY_PLAYERS;

        valid = reply.Write16((unsigned short)count);
        unsigned int written = 0;

        for (unsigned int i = 0; valid && i < state->players.size() && written < count; ++i)
        {
            const Player& p = state->players[i];

            if (detailed && p.id > 255)
                continue; // detailed query uses 8-bit player IDs

            valid = (!detailed || reply.Write8((unsigned char)p.id)) && reply.WriteString8(p.name) && reply.Write32((unsigned int)p.score) && (!detailed || reply.Write32(p.ping));
            ++written;
        }
        break;
    }

    case 'p':
        valid = length >= 15 && reply.Write(request + 11, 4);
        break;

    default:
        valid = false;
        break;
    }

    state->mutex.Unlock();
    if (!valid)
        return false;

    // кaw UDP reply
    // only use with RakNet's server datagram mode
    return SocketLayer::Instance()->SendTo(socket, reply.Data(), reply.Size(),
        sender.sin_addr.s_addr, ntohs(sender.sin_port)) == 0;
}
