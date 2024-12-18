#ifndef HUMAN_CONTROLLER_H
#define HUMAN_CONTROLLER_H

#include "Controller.h"
#include "GameSettings.h"

#include "Framework.h"

class HumanController : public Controller, public Entity, public RenderOverlayObject
{
public:
  HumanController(GameSettings& gameSettings);
  ~HumanController();

  void SetAeroplane(const class Aeroplane* aeroplane) {mAeroplane = aeroplane;}

  void EntityUpdate(float deltaTime, int entityLevel) OVERRIDE;

  float GetControl(Channel channel) const OVERRIDE;

  void SetInputControl(int control, float value) OVERRIDE;
  float GetInputControl(int control) const OVERRIDE;

  float GetElevatorTrim() const OVERRIDE {return mElevatorTrim;}

  void RenderOverlayUpdate(int renderLevel, DisplayConfig& displayConfig) OVERRIDE;

private:
  void RenderStick(float xMid, float yMid, bool renderStickCross, 
    float controlX, float controlY, float controllerSizeX, float controllerSizeY, 
    float stickIndicatorSizeX, float stickIndicatorSizeY, const class ControllerShader* controllerShader);
  void RenderController(float xMid, float yMid, float controllerSizeX, float controllerSizeY, 
    Options::ControllerStyle style, const class ControllerShader* controllerShader);
  void UpdateKeyboard(float deltaTime);
  void UpdateAccelerometer(float deltaTime);
  void UpdateScreenSticks(float deltaTime);
  void UpdateJoystick(float deltaTime);

  GameSettings& mGameSettings;
  const class Aeroplane* mAeroplane;

  float mOutputControls[MAX_CHANNELS];
  float mInputControls[ControllerSettings::CONTROLLER_NUM_CONTROLS];
  float mProcessedInputControls[ControllerSettings::CONTROLLER_NUM_CONTROLS];

  enum {NUM_BUTTONS = 3};
  ButtonOverlay* mButtonOverlay[NUM_BUTTONS];

  float mRightStickX; // -1 to 1
  float mRightStickY;
  float mLeftStickX;
  float mLeftStickY;
  float mAccelX;
  float mAccelY;

  float mStickIndicatorSize;

  // The size of each controller range in pixels
  float mControllerSizeX, mControllerSizeY;

  // The position of each controller in fractions
  float mRightControllerPosX;
  float mRightControllerPosY;
  float mLeftControllerPosX;
  float mLeftControllerPosY;

  // For when using the relative touch
  bool  mRightWasTouched;
  float mRightInitialTouchPosX;
  float mRightInitialTouchPosY;
  bool  mLeftWasTouched;
  float mLeftInitialTouchPosX;
  float mLeftInitialTouchPosY;

  float mElevatorTrim;
};

#endif