#include "ConnectionListener.h"
#include "PicaSim.h"

#define LISTEN_PORT  7777
#define MAX_SOCKETS  16

//======================================================================================================================
void ConnectionListener::HandleIncomingConnection(TCPsocket newSocket)
{
    IPaddress* peerAddr = SDLNet_TCP_GetPeerAddress(newSocket);
    if (peerAddr)
    {
        const char* host = SDLNet_ResolveIP(peerAddr);
        TRACE("IncomingConnection: %p from %s:%d", newSocket, host ? host : "unknown", peerAddr->port);
    }
    else
    {
        TRACE("IncomingConnection: %p", newSocket);
    }

    IncomingConnection incomingConnection(newSocket);
    mIncomingConnections.push_back(incomingConnection);

    // Add to socket set for non-blocking receive checks
    SDLNet_TCP_AddSocket(mSocketSet, newSocket);
}

//======================================================================================================================
ConnectionListener::ConnectionListener()
{
    mSocketListener = nullptr;
    mSocketSet = nullptr;
}

//======================================================================================================================
void ConnectionListener::Init()
{
    TRACE_METHOD_ONLY(ONCE_2);

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
        if (IncomingConnection::CONNECTION_CLOSED == it->Update(mSocketSet))
        {
            TRACE("Lost connection");
            SDLNet_TCP_DelSocket(mSocketSet, it->GetSocket());
            it->CloseSocket();
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
        if (mSocketSet)
            SDLNet_TCP_DelSocket(mSocketSet, it->GetSocket());
        it->CloseSocket();
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
