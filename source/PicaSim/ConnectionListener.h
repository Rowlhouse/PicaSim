#ifndef CONNECTION_LISTENER_H
#define CONNECTION_LISTENER_H

#include "IncomingConnection.h"
#include <SDL_net.h>
#include <vector>

//======================================================================================================================
/// Main interface for handling incoming connections that can control the simulation
class ConnectionListener
{
public:
    ConnectionListener();
    void Init();
    void Terminate();

    void Update();

private:
    void HandleIncomingConnection(TCPsocket newSocket);

    TCPsocket mSocketListener;
    SDLNet_SocketSet mSocketSet;

    typedef std::vector<IncomingConnection> IncomingConnections;
    IncomingConnections mIncomingConnections;
};

#endif
