#include "ConnectionListener.h"
#include "Trace.h"
#include "SocketMarmelade.h"
// #include "PicaSim.h"

#define LISTEN_PORT  7777

//---------------------------------------------------------------------------------------------------------------------
void ConnectionListener::HandleIncomingConnection(s3eSocket* newSocket, const s3eInetAddress& address)
{
  TRACE("IncommingConnection: %p %s",newSocket,s3eInetToString(&address).c_str());
  int on = 1;
  s3eSocketSetOpt(newSocket, S3E_SOCKET_NODELAY, &on, sizeof(on));
  IncomingConnection incomingConnection(newSocket, address);
  mIncomingConnections.push_back(incomingConnection);
}

//---------------------------------------------------------------------------------------------------------------------
ConnectionListener::ConnectionListener()
{
  mSocketListener = 0;
}

//---------------------------------------------------------------------------------------------------------------------
void ConnectionListener::Init()
{
  TRACE_METHOD_ONLY(1);

  mSocketListener = s3eSocketCreate(S3E_SOCKET_TCP,0);
  if (!mSocketListener)
  {
    TRACE("Failed to create the socket listener");
    return;
  }

  char hostname[255] = "127.0.0.1";

  // look up address
  memset(&mListenerAddr,0,sizeof(mListenerAddr));

  if(s3eInetLookup(hostname,&mListenerAddr,NULL,NULL) == S3E_RESULT_ERROR)
  {
    TRACE("`x666666s3eInetLookup failed: %s",hostname);
    s3eSocketClose(mSocketListener);
    mSocketListener = 0;
    return;
  }

  mListenerAddr.m_Port.sin_port = s3eInetHtons(LISTEN_PORT);

  if (s3eSocketBind(mSocketListener,&mListenerAddr,S3E_TRUE) != S3E_RESULT_SUCCESS)
  {
    TRACE("`x666666Bind failed");
    s3eSocketClose(mSocketListener);
    mSocketListener = 0;
    return;
  }

  if(s3eSocketListen(mSocketListener,10) != S3E_RESULT_SUCCESS)
  {
    TRACE("`x666666Listen failed");
    s3eSocketClose(mSocketListener);
    mSocketListener = 0;
    return;
  }
}

//---------------------------------------------------------------------------------------------------------------------
void ConnectionListener::Update()
{
  if (!mSocketListener)
    return;

  s3eInetAddress incomingAddress;
  s3eSocket* newSocket = s3eSocketAccept(mSocketListener, &incomingAddress, 0, this);
  if (newSocket)
    HandleIncomingConnection(newSocket, incomingAddress);

  for (IncomingConnections::iterator it = mIncomingConnections.begin() ; it != mIncomingConnections.end() ; )
  {
    if (IncomingConnection::CONNECTION_CLOSED == it->Update())
    {
      TRACE("Lost connection\n");
      it->CloseSocket();
      it = mIncomingConnections.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
void ConnectionListener::Terminate()
{
  TRACE_METHOD_ONLY(1);
  for (IncomingConnections::iterator it = mIncomingConnections.begin() ; it != mIncomingConnections.end() ; ++it)
  {
    it->CloseSocket();
  }
  mIncomingConnections.clear();

  if (mSocketListener)
  {
    TRACE("Closing listening socket");
    s3eSocketClose(mSocketListener);
    mSocketListener = 0;
  }
}

