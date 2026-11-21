#ifndef INCOMING_CONNECTION_H
#define INCOMING_CONNECTION_H

#include "NetworkController.h"
#include <SDL_net.h>
#include <map>

//======================================================================================================================
class IncomingConnection
{
public:
    IncomingConnection(TCPsocket socket, bool isLoopback);

    // Aeroplanes keep a pointer to the connection that requested their telemetry, so a
    // connection must stay at one address for its whole life
    IncomingConnection(const IncomingConnection&) = delete;
    IncomingConnection& operator=(const IncomingConnection&) = delete;

    /// Closes the socket. Call SDLNet_TCP_DelSocket first if it is in a socket set.
    void CloseSocket();

    /// Returns the socket for socket set management
    TCPsocket GetSocket() const { return mSocket; }

    enum UpdateResult {
        CONNECTION_OK,
        CONNECTION_CLOSED,
    };
    UpdateResult Update(SDLNet_SocketSet socketSet);

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

    TCPsocket mSocket;
    bool mIsLoopback;
    // Set when sending fails. The socket is left open so that ConnectionListener can take it
    // out of its socket set before closing it.
    bool mSendFailed;
    std::string mReceiveBuffer;

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
