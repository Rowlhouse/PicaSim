#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include "Controller.h"
#include "Aeroplane.h"

#include "Framework.h"

#include <vector>

//======================================================================================================================
class AIController : public Controller, public Entity
{
public:
    AIController() : mAeroplane(nullptr), mAIControllerIndex(0) {}
    virtual ~AIController() {}  // Virtual destructor ensures proper cleanup of derived classes

    virtual bool Init(LoadingScreenHelper* loadingScreen) = 0;
    virtual void Terminate() = 0;
    virtual void Reset() = 0;

    virtual void Relaunched() = 0;

    const Aeroplane* GetAeroplane() const {return mAeroplane;}
    Aeroplane* GetAeroplane() {return mAeroplane;}

    float GetControl(Channel channel) const
    {
        IwAssert(ROWLHOUSE, channel < MAX_CHANNELS);
        return mOutputControls[channel];
    }

protected:
    float mOutputControls[MAX_CHANNELS];
    Aeroplane* mAeroplane;
    size_t mAIControllerIndex;
};

typedef std::vector<AIController*> AIControllers;

#endif
