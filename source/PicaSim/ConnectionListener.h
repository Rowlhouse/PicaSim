#ifndef CONNECTION_LISTENER_H
#define CONNECTION_LISTENER_H

#include "IncomingConnection.h"

#include <s3eSocket.h>
#include <vector>

//---------------------------------------------------------------------------------------------------------------------
/// Main interface for handling incoming connections that can control the simulation
class ConnectionListener
{
public:
  ConnectionListener();
  void Init();
  void Terminate();

  void Update();

private:
  void HandleIncomingConnection(s3eSocket* newSocket, const s3eInetAddress& address);

  s3eSocket* mSocketListener;
  s3eInetAddress mListenerAddr;

  typedef std::vector<IncomingConnection> IncomingConnections;
  IncomingConnections mIncomingConnections;
};

#endif
