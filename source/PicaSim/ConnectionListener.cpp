#include "ConnectionListener.h"
#include "PicaSim.h"

#include <cstdio>

#define LISTEN_PORT  7777
#define MAX_SOCKETS  16

//======================================================================================================================
// Formats an IPv4 address as a.b.c.d:port. SDLNet_ResolveIP is not used because it does a
// reverse DNS lookup, which can block the game for seconds.
static std::string FormatAddress(const IPaddress* addr)
{
    if (!addr)
        return "unknown";
    // host and port are both in network byte order
    const Uint8* b = (const Uint8*)&addr->host;
    char text[32];
    snprintf(text, sizeof(text), "%d.%d.%d.%d:%d", b[0], b[1], b[2], b[3], (int) SDLNet_Read16(&addr->port));
    return text;
}

//======================================================================================================================
void ConnectionListener::HandleIncomingConnection(TCPsocket newSocket)
{
    IPaddress* peerAddr = SDLNet_TCP_GetPeerAddress(newSocket);
    std::string peer = FormatAddress(peerAddr);
    TRACE("IncomingConnection: %p from %s", newSocket, peer.c_str());

    // IPaddress.host is in network byte order, so the first byte is the first octet of the address
    const Uint8* hostBytes = peerAddr ? (const Uint8*)&peerAddr->host : nullptr;
    bool isLoopback = hostBytes && hostBytes[0] == 127;

    if (!isLoopback && !mAllowRemoteConnections)
    {
        TRACE("Rejecting remote connection from %s - set mSocketControllerAllowRemote in settings.xml to allow remote control",
            peer.c_str());
        SDLNet_TCP_Close(newSocket);
        return;
    }

    // Add to socket set for non-blocking receive checks. If the set is full the connection
    // would never be read, so close it instead.
    if (SDLNet_TCP_AddSocket(mSocketSet, newSocket) < 0)
    {
        TRACE("Too many connections - closing connection from %s", peer.c_str());
        SDLNet_TCP_Close(newSocket);
        return;
    }

    mIncomingConnections.push_back(std::make_unique<IncomingConnection>(newSocket, isLoopback));
}

//======================================================================================================================
ConnectionListener::ConnectionListener()
{
    mSocketListener = nullptr;
    mSocketSet = nullptr;
    mAllowRemoteConnections = false;
}

//======================================================================================================================
void ConnectionListener::Init(bool allowRemoteConnections)
{
    TRACE_METHOD_ONLY(ONCE_2);

    mAllowRemoteConnections = allowRemoteConnections;

    // Initialize SDL2_net if not already done
    if (SDLNet_Init() < 0)
    {
        TRACE("Failed to initialize SDL2_net: %s", SDLNet_GetError());
        return;
    }

    // Create socket set for non-blocking I/O
    mSocketSet = SDLNet_AllocSocketSet(MAX_SOCKETS);
    if (!mSocketSet)
    {
        TRACE("Failed to allocate socket set: %s", SDLNet_GetError());
        return;
    }

    // Resolve the listening address
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, nullptr, LISTEN_PORT) < 0)
    {
        TRACE("Failed to resolve host: %s", SDLNet_GetError());
        SDLNet_FreeSocketSet(mSocketSet);
        mSocketSet = nullptr;
        return;
    }

    // Open the server socket
    mSocketListener = SDLNet_TCP_Open(&ip);
    if (!mSocketListener)
    {
        TRACE("Failed to open server socket on port %d: %s", LISTEN_PORT, SDLNet_GetError());
        SDLNet_FreeSocketSet(mSocketSet);
        mSocketSet = nullptr;
        return;
    }

    TRACE("Listening on port %d", LISTEN_PORT);
}

//======================================================================================================================
void ConnectionListener::Update()
{
    if (!mSocketListener)
        return;

    // Check for incoming connections (non-blocking)
    TCPsocket newSocket = SDLNet_TCP_Accept(mSocketListener);
    if (newSocket)
        HandleIncomingConnection(newSocket);

    // Update all active connections
    for (IncomingConnections::iterator it = mIncomingConnections.begin() ; it != mIncomingConnections.end() ; )
    {
        IncomingConnection& connection = **it;
        if (IncomingConnection::CONNECTION_CLOSED == connection.Update(mSocketSet))
        {
            TRACE("Lost connection");
            if (connection.GetSocket())
                SDLNet_TCP_DelSocket(mSocketSet, connection.GetSocket());
            connection.CloseSocket();
            it = mIncomingConnections.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

//======================================================================================================================
void ConnectionListener::Terminate()
{
    TRACE_METHOD_ONLY(ONCE_2);
    for (IncomingConnections::iterator it = mIncomingConnections.begin() ; it != mIncomingConnections.end() ; ++it)
    {
        IncomingConnection& connection = **it;
        if (mSocketSet && connection.GetSocket())
            SDLNet_TCP_DelSocket(mSocketSet, connection.GetSocket());
        connection.CloseSocket();
    }
    mIncomingConnections.clear();

    if (mSocketListener)
    {
        TRACE("Closing listening socket");
        SDLNet_TCP_Close(mSocketListener);
        mSocketListener = nullptr;
    }

    if (mSocketSet)
    {
        SDLNet_FreeSocketSet(mSocketSet);
        mSocketSet = nullptr;
    }

    SDLNet_Quit();
}
