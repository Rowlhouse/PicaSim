#ifndef OBJECTEDITINGOVERLAY_H
#define OBJECTEDITINGOVERLAY_H

#include "ButtonOverlay.h"
#include "ClassesMarmelade.h"
#include "Entity.h"
#include "Helpers.h"


struct GameSettings;
class Environment;

class ObjectEditingOverlay : public Entity
{
public:
  ObjectEditingOverlay(const GameSettings& gameSettings, Texture* texture, Environment& environment);
  ~ObjectEditingOverlay();

  void EntityUpdate(float deltaTime, int entityLevel) OVERRIDE;

  enum Button
  {
    NEXT, 
    PREV,
    MOVE,
    ROTATE,
    SCALE,
    COLOUR,
    VISIBLITY,
    SHADOW,
    WORLD,
    CAMERA,
    OBJECT,
    FASTER,
    SLOWER,
    CREATE_DYNAMIC,
    CREATE_STATIC,
    DELETE,
    PLUS_X,
    MINUS_X,
    PLUS_Y,
    MINUS_Y,
    PLUS_Z,
    MINUS_Z,
    RESET,
    NUM_BUTTONS
  };

private:
  enum EditMode {EDITMODE_MOVE, EDITMODE_SCALE, EDITMODE_ROTATE, EDITMODE_COLOUR, EDITMODE_MAX};
  enum EditSpace {EDITSPACE_WORLD, EDITSPACE_CAMERA, EDITSPACE_OBJECT};

  bool WasClicked(Button button) const;
  bool WasPressed(Button button) const;

  void UpdateButtons();

  const GameSettings& mGameSettings;

  ButtonOverlay* mButtons[NUM_BUTTONS];

  Texture* mTexture;

  Environment& mEnvironment;
  int     mSelectedObject;

  static EditMode mEditMode;
  static EditSpace mEditSpace;
  static float mMoveSpeed;
  static float mRotateSpeed;
  static float mScaleSpeed;
  static float mColourSpeed;
};

#endif

