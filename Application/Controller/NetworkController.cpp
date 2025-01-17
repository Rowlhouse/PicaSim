#include "NetworkController.h"


//---------------------------------------------------------------------------------------------------------------------
NetworkController::NetworkController()
{
  for (unsigned int i = 0 ; i != MAX_CHANNELS ; ++i)
    mOutputControls[i] = 0.0f;

  mOutputControls[Controller::CHANNEL_SMOKE1]   = -1.0f; 
  mOutputControls[Controller::CHANNEL_SMOKE2]   = -1.0f;
  mOutputControls[Controller::CHANNEL_HOOK]     = -1.0f;

  mAeroplane = 0;
  mOriginalController = 0;
}

//---------------------------------------------------------------------------------------------------------------------
void NetworkController::TakeControl(Aeroplane* aeroplane)
{
  if (mAeroplane == aeroplane)
    return;

  // Return the original controller
  if (mAeroplane)
  {
    IwAssert(ROWLHOUSE, mOriginalController);
    mAeroplane->SetController(mOriginalController);
  }

  mAeroplane = aeroplane;
  mOriginalController = &aeroplane->GetController();
  aeroplane->SetController(this);
}

//---------------------------------------------------------------------------------------------------------------------
void NetworkController::ReleaseControl()
{
  if (!mOriginalController)
    return;
  mAeroplane->SetController(mOriginalController);
  mOriginalController = 0;
  mAeroplane = 0;
}

//---------------------------------------------------------------------------------------------------------------------
void NetworkController::SetControl(Controller::Channel channel, float control)
{
  IwAssert(ROWLHOUSE, channel < MAX_CHANNELS);
  mOutputControls[channel] = control;
}

//---------------------------------------------------------------------------------------------------------------------
float NetworkController::GetControl(Controller::Channel channel) const
{
  IwAssert(ROWLHOUSE, channel < MAX_CHANNELS);
  return mOutputControls[channel];
}

