#ifndef SETTINGS_WIDGETS_H
#define SETTINGS_WIDGETS_H

#include "../GameSettings.h"

#include <IwUI.h>

#include <vector>
#include <string>

extern float settingsControlFraction;

enum LabelType
{
  LABEL_TYPE_HELP_TEXT,
  LABEL_TYPE_SETTING_BLOCK_TITLE,
  LABEL_TYPE_SETTING,
  LABEL_TYPE_CENTERED_TITLE,
  LABEL_TYPE_LEFT
};
CIwUILabel* CreateLabel(const GameSettings& gameSettings, CIwUIElement* parentArea, CIwUILayout* parentLayout, const char* caption, LabelType labelType);

// returns the text label
CIwUILabel* CreateInfo(const GameSettings& gameSettings, CIwUIElement* parentArea, CIwUILayout* parentLayout, const char* caption, const char* info);

CIwUIButton* CreateSettingsButton(const GameSettings& gameSettings, CIwUIElement* parentArea, CIwUILayout* parentLayout, const char* caption, const char* buttonText, CIwUILabel** label = 0);

struct SliderUIElements
{
  CIwUIElement* mDataArea;
  CIwUILayoutGrid* mDataLayout;
  CIwUISlider*  mSlider;
  CIwUILabel*  mLabel;
  CIwUILabel*  mLabelValue;

  CIwUILayoutVertical* mCaptionLayout;
  CIwUIElement* mCaptionArea;
};

struct CheckboxUIElements
{
  CIwUIElement* mDataArea;
  CIwUILayoutGrid* mDataLayout;
  CIwUICheckbox*  mCheckbox;
  CIwUILabel*  mLabel;
};

class IntSlider
{
public:
  IntSlider(
    const GameSettings& gameSettings,
    CIwUIElement* parentArea, CIwUILayout* parentLayout,
    const char* caption, const char* units, int& val,
    int minVal, int maxVal);

  void SetFromSlider();

  SliderUIElements mElements;
private:
  void UpdateCaption();

  int* mVal;

  const char* mCaption;
  const char* mUnits;
};

class FloatSlider
{
public:
  FloatSlider(
    const GameSettings& gameSettings,
    CIwUIElement* parentArea, CIwUILayout* parentLayout,
    const char* caption, const char* units, float& val,
    float minVal, float maxVal, float power);

  void SetFromSlider();

  SliderUIElements mElements;
private:
  void UpdateCaption();

  float* mVal;
  float mMinVal, mMaxVal;
  float mPower;

  const char* mUnits;
};

class Checkbox
{
public:
  Checkbox(
    const GameSettings& gameSettings,
    CIwUIElement* parentArea, CIwUILayout* parentLayout,
    const char* caption, bool& val);

  void SetFromCheckbox();

  CheckboxUIElements mElements;
private:
  bool* mVal;
};

class Enum
{
public:
  Enum(
    const GameSettings& gameSettings,
    CIwUIElement* parentArea, CIwUILayout* parentLayout,
    const char* caption, int& val, 
    const char** texts, int numTexts);

  void SetFromSlider();

  SliderUIElements mElements;
private:
  void UpdateCaption();

  int* mVal;

  const char* mCaption;
  const char** mTexts;
  int mNumTexts;
};

class String
{
public:
  String(
    const GameSettings& gameSettings,
    CIwUIElement* parentArea, CIwUILayout* parentLayout,
    const char* caption, int& val, 
    const std::vector<std::string>& texts);

  void SetFromSlider();

  SliderUIElements mElements;
private:
  void UpdateCaption();

  int* mVal;

  const char* mCaption;
  std::vector<std::string> mTexts;
};
#endif
