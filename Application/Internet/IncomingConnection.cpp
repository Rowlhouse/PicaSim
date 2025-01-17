#include "IncomingConnection.h"
#include "FonctionsMarmelade.h"
#include "PicaSim.h"
#include "SocketMarmelade.h"
#include "Environment.h"

#define SOCKET_TIMEOUT (30000)
// #include "s3e.h"

// #include <sys/types.h>
// #include <sys/socket.h>

static char s_terminator = '\n';

//---------------------------------------------------------------------------------------------------------------------
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
#if 0
  for (size_t i = 0 ; i != tokens.size() ; ++i)
  {
    const Token& token = tokens[i];
    TRACE("%s", token.c_str());
  }
#endif
}

//---------------------------------------------------------------------------------------------------------------------
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

//---------------------------------------------------------------------------------------------------------------------
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

//---------------------------------------------------------------------------------------------------------------------
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

//---------------------------------------------------------------------------------------------------------------------
void IncomingConnection::HandleReset(Tokens& tokens)
{
  TRACE("Reset agent %d\n", mCurrentAgent);
  Aeroplane* aeroplane = PicaSim::GetInstance().GetAeroplane((size_t) mCurrentAgent);
  if (aeroplane)
    aeroplane->Launch(aeroplane->GetLastLaunchPosition());
}

//---------------------------------------------------------------------------------------------------------------------
void IncomingConnection::HandleReleaseControl(Tokens& tokens)
{
  TRACE("Release control of agent %d\n", mCurrentAgent);
  NetworkController& nc = mNetworkControllers[mCurrentAgent];
  nc.ReleaseControl();
}

//---------------------------------------------------------------------------------------------------------------------
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

//---------------------------------------------------------------------------------------------------------------------
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

//---------------------------------------------------------------------------------------------------------------------
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

//---------------------------------------------------------------------------------------------------------------------
/// Sends msg, after adding the terminator. Blocks, but returns false if the socket is dead.
static bool Send(s3eSocket* socket, std::string msg)
{
  if (!socket)
    return false;

  msg += s_terminator;
  size_t numChars = msg.length();

  size_t numSent = 0;
  // Loop until the full message has been sent
  do
  {
    // returns the number of bytes sent, or -1 on failure.
    int retValue = s3eSocketSend(socket, msg.begin()+numSent, numChars - numSent, 0);

    // sending has been successfull, see how many bytes are left to send
    if (retValue > 0)
      numSent += retValue;

    // an error occurred while sending
    if(retValue < 0)
    {
      // This error is OK, since S3E_SOCKET_ERR_AGAIN means, that
      // a function is in process right now
      if(s3eSocketGetError() == S3E_SOCKET_ERR_AGAIN)
      {
        s3eDeviceYield(50);
        continue;
      }

      // Any other error should be treated as "sending failed"
      return false;
    }

  } while(numSent < numChars);
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
/// Reads into msg, up to (but not including) the terminator. Blocking only if there is something to read.
/// If there's nothing to read then msg will be empty. Returns false if the socket is dead.
static bool Receive(s3eSocket* socket, std::string& msg)
{
  if (!socket)
    return false;

  msg.reserve(1024);
  msg = "";

  while (true)
  {
    char c;
    int32_t retValue = s3eSocketRecv(socket, &c, 1, 0);
    if (retValue > 0)
    {
      if (c == s_terminator)
        return true;
      msg += std::tolower(c);
    }
    else if (retValue == 0)
    {
      return false;
    }
    else
    {
      s3eSocketErrors errors = s3eSocketGetError();
      if (errors != S3E_SOCKET_ERR_WOULDBLOCK)
        return false;
      // Give up
      if (msg.empty())
        return true;
      // Go round again
    }
  }

  return true;
}


//---------------------------------------------------------------------------------------------------------------------
void IncomingConnection::CloseSocket()
{
  size_t num = PicaSim::GetInstance().GetNumAeroplanes();
  for (size_t i = 0 ; i != num ; ++i)
  {
    Aeroplane* aeroplane = PicaSim::GetInstance().GetAeroplane(i);
    aeroplane->SetIncomingConnection(0);
  }

  if (mSocket)
    s3eSocketClose(mSocket);
  mSocket = 0;

  for (NetworkControllers::iterator it = mNetworkControllers.begin() ; it != mNetworkControllers.end() ; ++it)
  {
    it->second.ReleaseControl();
  }
  mNetworkControllers.clear();
}

//---------------------------------------------------------------------------------------------------------------------
IncomingConnection::IncomingConnection(s3eSocket* socket, const s3eInetAddress& address) 
    : mSocket(socket), mAddress(address), mCurrentAgent(0)
{
}

//---------------------------------------------------------------------------------------------------------------------
IncomingConnection::UpdateResult IncomingConnection::Update()
{
  while (true)
  {
    std::string msg;
    if (!Receive(mSocket, msg))
    {
      TRACE("Error reading - closing connection\n");
      return CONNECTION_CLOSED;
    }

    if (msg.empty())
      return CONNECTION_OK;

    Tokens tokens;
    ///////////////////////////////////////////////////////////ConvertToTokens(msg.begin(), tokens);
    ConvertToTokens(msg.c_str(), tokens);
    HandleMessage(tokens);
  }
}

//---------------------------------------------------------------------------------------------------------------------
void IncomingConnection::SendAgentMessages(const Aeroplane* aeroplane, float dt)
{
  if (!mSocket)
  {
    TRACE("No socket\n");
    return;
  }
  size_t num = PicaSim::GetInstance().GetNumAeroplanes();
  int agentID;
  for (agentID = 0 ; agentID != num ; ++agentID)
  {
    if (PicaSim::GetInstance().GetAeroplane(agentID) == aeroplane)
      break;
  }
  if (agentID == num)
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
