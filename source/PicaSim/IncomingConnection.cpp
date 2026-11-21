#include "IncomingConnection.h"
#include "PicaSim.h"

#include <cctype>

static char s_terminator = '\n';

// If a peer sends this much data without a terminator, treat it as a protocol violation and close the connection
static const size_t s_maxReceiveBufferSize = 4096;

//======================================================================================================================
void IncomingConnection::ConvertToTokens(const char* txt, Tokens& tokens)
{
    size_t len = strlen(txt);
    Token token;
    for (size_t i = 0 ; i != len ; ++i)
    {
        if (txt[i] == ' ' || txt[i] == '\n' || txt[i] == '\r')
        {
            if (!token.empty())
            {
                tokens.push_back(token);
            }
            token.clear();
        }
        else
        {
            token.push_back(txt[i]);
        }
    }
    if (!token.empty())
    {
        tokens.push_back(token);
    }
}

//======================================================================================================================
void IncomingConnection::HandleAgent(Tokens& tokens)
{
    if (tokens.size() < 1)
    {
        TRACE("Agent requires 1 argument");
        return;
    }
    Token t1 = tokens.back();
    tokens.pop_back();

    mCurrentAgent = atoi(t1.c_str());
    TRACE("Agent = %d", mCurrentAgent);
}

//======================================================================================================================
void IncomingConnection::HandleTakeControl(Tokens& tokens)
{
    TRACE("Take control of agent %d", mCurrentAgent);
    Aeroplane* aeroplane = PicaSim::GetInstance().GetAeroplane((size_t) mCurrentAgent);
    if (aeroplane)
    {
        NetworkController& nc = mNetworkControllers[mCurrentAgent];
        nc.TakeControl(aeroplane);
    }
    else
    {
        TRACE("Can't find aeroplane %d", mCurrentAgent);
    }
}

//======================================================================================================================
void IncomingConnection::HandleCamera(Tokens& tokens)
{
    if (tokens.size() < 1)
    {
        TRACE("Camera requires 1 argument");
        return;
    }
    Token t1 = tokens.back();
    tokens.pop_back();

    int cameraMode = atoi(t1.c_str());
    TRACE("Camera mode = %d", cameraMode);
    PicaSim::GetInstance().SetMode((PicaSim::Mode) cameraMode);
}

//======================================================================================================================
void IncomingConnection::HandleReset(Tokens& tokens)
{
    TRACE("Reset agent %d", mCurrentAgent);
    Aeroplane* aeroplane = PicaSim::GetInstance().GetAeroplane((size_t) mCurrentAgent);
    if (aeroplane)
        aeroplane->Launch(aeroplane->GetLastLaunchPosition());
}

//======================================================================================================================
void IncomingConnection::HandleReleaseControl(Tokens& tokens)
{
    TRACE("Release control of agent %d", mCurrentAgent);
    NetworkController& nc = mNetworkControllers[mCurrentAgent];
    nc.ReleaseControl();
}

//======================================================================================================================
void IncomingConnection::HandleControl(Tokens& tokens)
{
    if (tokens.size() < 2)
    {
        TRACE("Control requires 2 arguments");
        return;
    }
    Token t1 = tokens.back();
    tokens.pop_back();
    Token t2 = tokens.back();
    tokens.pop_back();

    int channel = atoi(t1.c_str());
    float control = (float) atof(t2.c_str());
    TRACE("Control channel %d = %f", channel, control);

    NetworkController& nc = mNetworkControllers[mCurrentAgent];
    nc.SetControl((Controller::Channel) channel, control);
}

//======================================================================================================================
void IncomingConnection::HandleRequestTelemetry(Tokens& tokens)
{
    if (tokens.size() < 1)
    {
        TRACE("RequestTelemetry requires 1 arguments");
        return;
    }
    Token t1 = tokens.back();
    tokens.pop_back();

    Aeroplane* aeroplane = PicaSim::GetInstance().GetAeroplane((size_t) mCurrentAgent);
    if (!aeroplane)
    {
        TRACE("No current aeroplane");
        return;
    }

    float dt = (float) atof(t1.c_str());
    TRACE("RequestTelemetry %f", dt);

    aeroplane->SetIncomingConnection(this);
    TelemetryRequest& req = mTelemetryRequests[mCurrentAgent];
    req.mDt = dt;
}

//======================================================================================================================
void IncomingConnection::HandleMessage(Tokens& tokens)
{
    std::reverse(tokens.begin(), tokens.end());
    while (!tokens.empty())
    {
        const Token t = tokens.back();
        tokens.pop_back();

        if (t == "exit")
        {
            if (mIsLoopback)
            {
                TRACE("Exit");
                exit(0);
            }
            else
            {
                TRACE("Ignoring exit from remote peer");
            }
        }
        else if (t == "pause")
        {
            TRACE("Pause");
            PicaSim::GetInstance().SetStatus(PicaSim::STATUS_PAUSED);
        }
        else if (t == "unpause")
        {
            TRACE("Unpause");
            PicaSim::GetInstance().SetStatus(PicaSim::STATUS_FLYING);
        }
        else if (t == "agent")
        {
            HandleAgent(tokens);
        }
        else if (t == "takecontrol")
        {
            HandleTakeControl(tokens);
        }
        else if (t == "camera")
        {
            HandleCamera(tokens);
        }
        else if (t == "reset")
        {
            HandleReset(tokens);
        }
        else if (t == "releasecontrol")
        {
            HandleReleaseControl(tokens);
        }
        else if (t == "control")
        {
            HandleControl(tokens);
        }
        else if (t == "requesttelemetry")
        {
            HandleRequestTelemetry(tokens);
        }
        else
        {
            TRACE("Unhandled token %s", t.c_str());
        }
    }
}

//======================================================================================================================
/// Sends msg, after adding the terminator. Blocks until sent. Returns false if socket error.
static bool Send(TCPsocket socket, std::string msg)
{
    if (!socket)
        return false;

    msg += s_terminator;
    int numChars = (int)msg.length();

    int result = SDLNet_TCP_Send(socket, msg.c_str(), numChars);
    if (result < numChars)
    {
        // SDL2_net: if result < length, there was an error
        TRACE("Send error: %s", SDLNet_GetError());
        return false;
    }

    return true;
}

//======================================================================================================================
void IncomingConnection::CloseSocket()
{
    size_t num = PicaSim::GetInstance().GetNumAeroplanes();
    for (size_t i = 0 ; i != num ; ++i)
    {
        Aeroplane* aeroplane = PicaSim::GetInstance().GetAeroplane(i);
        aeroplane->SetIncomingConnection(nullptr);
    }

    if (mSocket)
        SDLNet_TCP_Close(mSocket);
    mSocket = nullptr;

    for (NetworkControllers::iterator it = mNetworkControllers.begin() ; it != mNetworkControllers.end() ; ++it)
    {
        it->second.ReleaseControl();
    }
    mNetworkControllers.clear();
}

//======================================================================================================================
IncomingConnection::IncomingConnection(TCPsocket socket, bool isLoopback)
        : mSocket(socket), mIsLoopback(isLoopback), mCurrentAgent(0)
{
}

//======================================================================================================================
IncomingConnection::UpdateResult IncomingConnection::Update(SDLNet_SocketSet socketSet)
{
    if (!mSocket)
        return CONNECTION_CLOSED;

    // Read all available data into the persistent buffer (non-blocking)
    while (mReceiveBuffer.size() <= s_maxReceiveBufferSize &&
           SDLNet_CheckSockets(socketSet, 0) > 0 && SDLNet_SocketReady(mSocket))
    {
        char chunk[512];
        int result = SDLNet_TCP_Recv(mSocket, chunk, sizeof(chunk));
        if (result <= 0)
        {
            // Connection closed or error
            TRACE("Error reading - closing connection");
            return CONNECTION_CLOSED;
        }

        for (int i = 0 ; i != result ; ++i)
        {
            mReceiveBuffer.push_back((char) std::tolower((unsigned char) chunk[i]));
        }
    }

    // Handle every complete line - a trailing partial line stays in the buffer until the terminator arrives
    size_t terminatorPos;
    while ((terminatorPos = mReceiveBuffer.find(s_terminator)) != std::string::npos)
    {
        std::string msg = mReceiveBuffer.substr(0, terminatorPos);
        mReceiveBuffer.erase(0, terminatorPos + 1);

        if (msg.empty())
            continue;

        Tokens tokens;
        ConvertToTokens(msg.c_str(), tokens);
        HandleMessage(tokens);
    }

    // Guard against a peer streaming data without ever sending a terminator
    if (mReceiveBuffer.size() > s_maxReceiveBufferSize)
    {
        TRACE("Receive buffer overflow without terminator - closing connection");
        return CONNECTION_CLOSED;
    }

    return CONNECTION_OK;
}

//======================================================================================================================
void IncomingConnection::SendAgentMessages(const Aeroplane* aeroplane, float dt)
{
    if (!mSocket)
    {
        TRACE("No socket");
        return;
    }
    size_t num = PicaSim::GetInstance().GetNumAeroplanes();
    int agentID;
    for (agentID = 0 ; agentID != (int)num ; ++agentID)
    {
        if (PicaSim::GetInstance().GetAeroplane(agentID) == aeroplane)
            break;
    }
    if (agentID == (int)num)
    {
        TRACE("Can't find agent for %p", aeroplane);
        return;
    }

    auto& req = mTelemetryRequests[agentID];
    if (req.mDt <= 0)
        return;

    req.mTimeSinceSend += dt;

    if (req.mTimeSinceSend > req.mDt)
    {
        char messageToSend[1024];
        auto& tm = aeroplane->GetTransform();
        const Vector3 pos = tm.GetTrans();
        const Vector3 faceDir = tm.RowX();
        const Vector3 upDir = tm.RowZ();
        const Vector3 windVel = Environment::GetInstance().GetWindAtPosition(pos, Environment::WIND_TYPE_GUSTY);
        const Vector3 vel = aeroplane->GetVelocity();
        const Vector3 wind = windVel - vel;
        float altitude = fabsf(pos.z -  Environment::GetInstance().GetTerrain().GetTerrainHeightFast(pos.x, pos.y, true));
        float time = Environment::GetInstance().GetTime();

        snprintf(messageToSend, sizeof(messageToSend), "Agent %d Telemetry time %f pos %f %f %f faceDir %f %f %f upDir %f %f %f alt %f vel %f %f %f wind %f %f %f",
            agentID,
            time,
            pos.x, pos.y, pos.z,
            faceDir.x, faceDir.y, faceDir.z,
            upDir.x, upDir.y, upDir.z,
            altitude,
            vel.x, vel.y, vel.z,
            wind.x, wind.y, wind.z);

        if (!Send(mSocket, messageToSend))
        {
            TRACE("Unable to send message - closing socket");
            CloseSocket();
            return;
        }

        req.mTimeSinceSend = 0;
    }
}
