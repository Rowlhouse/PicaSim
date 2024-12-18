#ifndef ENGINE_H
#define ENGINE_H

#include "Framework.h"
#include "Controller.h"
#include "GameSettings.h"

class Engine : public Entity
{
public:
  virtual void Init(class TiXmlElement* engineElement, class Aeroplane* aeroplane) {mLastWash = mLastWashAngVel = Vector3(0,0,0);}
  virtual void Terminate() = 0;
  virtual void Launched() {}


  //virtual const Transform& GetTM() const {return mTM;}

  virtual void UpdatePrePhysics(float deltaTime, const struct TurbulenceData& turbulenceData) = 0;

  /// Update position etc after the physics step
  virtual void UpdatePostPhysics(float deltaTime) = 0;

  const std::string& GetName() const {return mName;}

  /// Returns the last wash calculated (air flow), relative to this engine, and the angular velocity of it
  Vector3 GetLastWash(Vector3& washAngVel) const {washAngVel = mLastWashAngVel; return mLastWash;}

  /// Returns the rotation angle and speed fraction (can be -ve) - relevant for propellers
  virtual void GetRotation(float& angle, float& speedFrac) const {angle = speedFrac = 0.0f;}

  /// Returns the position and orientation of the engine - normally produces thrust along X axis
  virtual Transform GetTM() const = 0;

  virtual float GetRadius() const = 0;

  // Gets the angular velocity  - relevant for propellers
  virtual float GetPropSpeed() const {return 0.0f;}

protected:
  std::string mName;
  Vector3 mLastWash;
  Vector3 mLastWashAngVel;
};

#endif