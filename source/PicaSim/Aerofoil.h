#ifndef AEROFOIL_H
#define AEROFOIL_H

#include "AerofoilParameters.h"

class Aeroplane;
class AerofoilDefinition;

/// The aerofoil will consist of a definition (graph of CL, CD etc and functions to calculate forces/torques), 
/// together with the parameters for this instance, which will include adjustments (transform, control settings etc)
class Aerofoil
{
public:
    void Init(
        const char* name, 
        const AerofoilConfiguration& configuration, 
        Aeroplane* aeroplane,
        uint32& checksum);

    void Terminate();

    AerofoilParameters& GetParameters() {return mParameters;}

    /// Opportunity to apply forces
    void UpdatePrePhysics(
        float deltaTime, 
        const AerofoilControl& aerofoilControl, 
        const struct TurbulenceData& turbulenceData,
        Vector3& wash, Vector3& force);

    /// Updates the aerofoils to update their position etc after the physics step
    void UpdatePostPhysics(float deltaTime);

private:
    AerofoilParameters mParameters;
    AerofoilConfiguration mConfiguration;
    AerofoilDefinition* mDefinition;
    Aeroplane* mAeroplane;
    float mPrevFlying;
    float mLastAoA;
};

#endif
