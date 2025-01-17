#ifndef INCOMING_CONNECTION_H
#define INCOMING_CONNECTION_H

#include "NetworkController.h"
#include "SocketMarmelade.h"

// #include <s3eSocket.h>
// #include <map>

//---------------------------------------------------------------------------------------------------------------------
#include "Aeroplane.h"
class IncomingConnection
{
public:
  IncomingConnection(s3eSocket* socket, const s3eInetAddress& address);

  /// Closes the socket
  void CloseSocket();

  enum UpdateResult {
    CONNECTION_OK,
    CONNECTION_CLOSED,
  };
  UpdateResult Update();

  // This can get called in the aeroplane's thread - and we can probe the aeroplane and
  // send a message, if there's anything to send, secure in the knowledge that the aeroplane exists.
  void SendAgentMessages(const Aeroplane* aeroplane, float dt);

private:
  typedef std::map<int, NetworkController> NetworkControllers;
  typedef std::string Token;
  typedef std::vector<Token> Tokens;

  static void ConvertToTokens(const char* txt,Tokens& tokens);

  void HandleMessage(Tokens& tokens);
  void HandleAgent(Tokens& tokens);
  void HandleTakeControl(Tokens& tokens);
  void HandleReleaseControl(Tokens& tokens);
  void HandleControl(Tokens& tokens);
  void HandleRequestTelemetry(Tokens& tokens);
  void HandleCamera(Tokens& tokens);
  void HandleReset(Tokens& tokens);

  s3eSocket*     mSocket;
  s3eInetAddress mAddress;

  NetworkControllers mNetworkControllers;
  int                mCurrentAgent;

  struct TelemetryRequest
  {
    TelemetryRequest() : mDt(0), mTimeSinceSend(0) {}
    float mDt;
    float mTimeSinceSend;
  };
  typedef std::map<int, TelemetryRequest> TelemetryRequests;
  TelemetryRequests mTelemetryRequests;

};


#endif
