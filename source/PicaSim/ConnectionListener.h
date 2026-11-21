#ifndef CONNECTION_LISTENER_H
#define CONNECTION_LISTENER_H

#include "IncomingConnection.h"
#include <SDL_net.h>
#include <memory>
#include <string>
#include <vector>

//======================================================================================================================
/// Main interface for handling incoming connections that can control the simulation
class ConnectionListener
{
public:
    ConnectionListener();
    void Init(bool allowRemoteConnections);
    void Terminate();

    void Update();

private:
    void HandleIncomingConnection(TCPsocket newSocket);

    TCPsocket mSocketListener;
    SDLNet_SocketSet mSocketSet;
    bool mAllowRemoteConnections;

    // Held by pointer so each connection keeps its address when others are added or removed.
    // Aeroplanes keep a pointer to the connection that requested their telemetry.
    typedef std::vector<std::unique_ptr<IncomingConnection>> IncomingConnections;
    IncomingConnections mIncomingConnections;
};

#endif
