#ifndef NETWORK_CONTROLLER_H
#define NETWORK_CONTROLLER_H

#include "Controller.h"
#include "Aeroplane.h"

#include "Framework.h"

class NetworkController : public Controller
{
public:
    NetworkController();

    void TakeControl(Aeroplane* aeroplane);
    void ReleaseControl();
    float GetControl(Channel channel) const OVERRIDE;
    void  SetControl(Channel channel, float control);
private:
    float        mOutputControls[MAX_CHANNELS];
    Controller*  mOriginalController;
    Aeroplane*   mAeroplane;
};


#endif
