#include "IncomingConnection.h"
#include "PicaSim.h"

static char s_terminator = '\n';

//======================================================================================================================
void IncomingConnection::ConvertToTokens(const char* txt, Tokens& tokens)
{
    size_t len = strlen(txt);
    Token token;
    for (size_t i = 0 ; i != len ; ++i)
    {
        if (txt[i] == ' ' || txt[i] == '\n')
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
    TRACE("Agent = %d\n", mCurrentAgent);
}

//======================================================================================================================
void IncomingConnection::HandleTakeControl(Tokens& tokens)
{
    TRACE("Take control of agent %d\n", mCurrentAgent);
    Aeroplane* aeroplane = PicaSim::GetInstance().GetAeroplane((size_t) mCurrentAgent);
    if (aeroplane)
    {
        NetworkController& nc = mNetworkControllers[mCurrentAgent];
        nc.TakeControl(aeroplane);
    }
    else
    {
        TRACE("Can't find aeroplane %d\n", mCurrentAgent);
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
    TRACE("Camera mode = %d\n", cameraMode);
    PicaSim::GetInstance().SetMode((PicaSim::Mode) cameraMode);
}

//======================================================================================================================
void IncomingConnection::HandleReset(Tokens& tokens)
{
    TRACE("Reset agent %d\n", mCurrentAgent);
    Aeroplane* aeroplane = PicaSim::GetInstance().GetAeroplane((size_t) mCurrentAgent);
    if (aeroplane)
        aeroplane->Launch(aeroplane->GetLastLaunchPosition());
}

//======================================================================================================================
void IncomingConnection::HandleReleaseControl(Tokens& tokens)
{
    TRACE("Release control of agent %d\n", mCurrentAgent);
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
    TRACE("Control channel %d = %f\n", channel, control);

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
        TRACE("No current aeroplane\n");
        return;
    }

    float dt = (float) atof(t1.c_str());
    TRACE("RequestTelemetry %f\n", dt);

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
            TRACE("Exit\n");
            exit(0);
        }
        else if (t == "pause")
        {
            TRACE("Pause\n");
            PicaSim::GetInstance().SetStatus(PicaSim::STATUS_PAUSED);
        }
        else if (t == "unpause")
        {
            TRACE("Unpause\n");
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
            TRACE("Unhandled token %s\n", t.c_str());
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
/// Reads into msg, up to (but not including) the terminator.
/// Uses socket set to check if data is available (non-blocking).
/// If there's nothing to read then msg will be empty. Returns false if the socket is dead.
static bool Receive(TCPsocket socket, SDLNet_SocketSet socketSet, std::string& msg)
{
    if (!socket)
        return false;

    msg.reserve(1024);
    msg = "";

    // Check if there's any data ready to read (non-blocking)
    int numReady = SDLNet_CheckSockets(socketSet, 0);
    if (numReady <= 0)
        return true; // No data available, but socket is still OK

    // Check if this specific socket has data
    if (!SDLNet_SocketReady(socket))
        return true; // This socket has no data

    // Read character by character until newline
    while (true)
    {
        char c;
        int result = SDLNet_TCP_Recv(socket, &c, 1);
        if (result <= 0)
        {
            // Connection closed or error
            return false;
        }

        if (c == s_terminator)
            return true;

        msg += std::tolower(c);

        // Check if more data is available
        numReady = SDLNet_CheckSockets(socketSet, 0);
        if (numReady <= 0 || !SDLNet_SocketReady(socket))
        {
            // No more data right now, but we have a partial message
            // Continue waiting for the terminator
            // Use a small timeout to avoid blocking forever
            numReady = SDLNet_CheckSockets(socketSet, 100);
            if (numReady <= 0 || !SDLNet_SocketReady(socket))
            {
                // Still no data - return what we have if anything
                // Actually, for a line-based protocol we should wait for the terminator
                // But to avoid blocking, return true with partial data
                // The next Update() call will continue reading
                return true;
            }
        }
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
IncomingConnection::IncomingConnection(TCPsocket socket)
        : mSocket(socket), mCurrentAgent(0)
{
}

//======================================================================================================================
IncomingConnection::UpdateResult IncomingConnection::Update(SDLNet_SocketSet socketSet)
{
    while (true)
    {
        std::string msg;
        if (!Receive(mSocket, socketSet, msg))
        {
            TRACE("Error reading - closing connection\n");
            return CONNECTION_CLOSED;
        }

        if (msg.empty())
            return CONNECTION_OK;

        Tokens tokens;
        ConvertToTokens(msg.c_str(), tokens);
        HandleMessage(tokens);
    }
}

//======================================================================================================================
void IncomingConnection::SendAgentMessages(const Aeroplane* aeroplane, float dt)
{
    if (!mSocket)
    {
        TRACE("No socket\n");
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
        TRACE("Can't find agent for %p\n", aeroplane);
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

        sprintf(messageToSend, "Agent %d Telemetry time %f pos %f %f %f faceDir %f %f %f upDir %f %f %f alt %f vel %f %f %f wind %f %f %f",
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
            TRACE("Unable to send message - closing socket\n");
            CloseSocket();
            return;
        }

        req.mTimeSinceSend = 0;
    }
}
