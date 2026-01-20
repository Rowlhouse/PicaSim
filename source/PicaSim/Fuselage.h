#ifndef FUSELAGE_H
#define FUSELAGE_H

#include "Framework.h"
#include "GameSettings.h"

class Fuselage
{
public:
    void Init(class TiXmlElement* fuselageElement, class Aeroplane* aeroplane);
    void Terminate();

    float GetMass() const {return mMass;}
    const Transform& GetTMLocal() const {return mTMLocal;}
    const Vector3& GetExtents() const {return mExtents;}
    bool GetCollide() const {return mCollide;}

    /// Opportunity to apply forces
    void UpdatePrePhysics(float deltaTime, const struct TurbulenceData& turbulenceData);

    /// Updates the fuselage to update their position etc after the physics step
    void UpdatePostPhysics(float deltaTime);

private:
    class Aeroplane* mAeroplane;

    std::string mName;
    float mMass;
    bool mCollide;
    Vector3 mExtents;
    Vector3 mCD;
    Transform mTMLocal;
};

#endif
