#include "SettingsWidgets.h"

#include <IwGL.h>
#include <limits>

// Overridden elsewhere
float settingsControlFraction = 0.0f;

static const int16 sliderMin = std::numeric_limits<int16>::min()/2;
static const int16 sliderMax = std::numeric_limits<int16>::max()/2;

float settingsWidthFraction = 0.95f;

//---------------------------------------------------------------------------------------------------------------------
CIwUILabel* CreateLabel(const GameSettings& gameSettings, CIwUIElement* parentArea, CIwUILayout* parentLayout, const char* caption, LabelType labelType)
{
  int width =  (int) (settingsWidthFraction * gameSettings.mOptions.mFrameworkSettings.mScreenWidth);
  CIwUIElement* spacer = 0;
  if (labelType == LABEL_TYPE_SETTING_BLOCK_TITLE)
  {
    spacer = new CIwUIElement;
    parentArea->AddChild(spacer);
    parentLayout->AddElement(spacer);
  }


  CIwUILabel* label = new CIwUILabel;
  switch (labelType)
  {
  case LABEL_TYPE_HELP_TEXT: label->SetStyle("<label_expanding_text>"); break;
  case LABEL_TYPE_SETTING: label->SetStyle("<label_centered>"); break;
  case LABEL_TYPE_SETTING_BLOCK_TITLE: label->SetStyle("<label_setting_block_title>"); break;
  case LABEL_TYPE_CENTERED_TITLE: label->SetStyle("<label_title_centered>"); break;
  case LABEL_TYPE_LEFT: label->SetStyle("<label_left>"); break;
  }
  label->SetCaption(caption);
  parentArea->AddChild(label);
  parentLayout->AddElement(label, label->GetStyle());

  if (spacer)
  {
    spacer->SetSizeMin(label->GetSizeMin());
    label->SetProperty("offset", CIwVec2((int) (0.25f * (1.0f - settingsWidthFraction) * width),0));
  }

  return label;
}

//---------------------------------------------------------------------------------------------------------------------
CIwUILabel* CreateInfo(const GameSettings& gameSettings, CIwUIElement* parentArea, CIwUILayout* parentLayout, const char* caption, const char* info)
{
  int width =  (int) (settingsWidthFraction * gameSettings.mOptions.mFrameworkSettings.mScreenWidth);
  int height =  gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int sliderWidth = (int) (width * settingsControlFraction);
  int captionWidth = width - sliderWidth;

  CIwUIElement* dataArea = new CIwUIElement;
  dataArea->SetSizeToContent(false);

  CIwUILayoutGrid* dataLayout = new CIwUILayoutGrid;
  dataLayout->AddColumn(captionWidth);
  dataLayout->AddColumn(sliderWidth);
  dataLayout->AddRow();

  dataArea->SetLayout(dataLayout);
  parentArea->AddChild(dataArea);
  parentLayout->AddElement(dataArea);

  // Label
  CIwUILabel* label = new CIwUILabel;
  label->SetStyle("<label_right>");
  label->SetCaption(caption);

  dataArea->AddChild(label);
  dataLayout->AddElement(label, 0, 0);

  // Info
  CIwUILabel* text = new CIwUILabel;
  text->SetStyle("<label_left_faint>");
  text->SetCaption(info);

  dataArea->AddChild(text);
  dataLayout->AddElement(text, 1, 0);

  return text;
}

//---------------------------------------------------------------------------------------------------------------------
CIwUIButton* CreateSettingsButton(const GameSettings& gameSettings, CIwUIElement* parentArea, CIwUILayout* parentLayout, const char* caption, const char* buttonText, CIwUILabel** _label)
{
  int width =  (int) (settingsWidthFraction * gameSettings.mOptions.mFrameworkSettings.mScreenWidth);
  int height =  gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int sliderWidth = (int) (width * settingsControlFraction);
  int captionWidth = width - sliderWidth;

  CIwUIElement* dataArea = new CIwUIElement;
  dataArea->SetSizeToContent(false);

  CIwUILayoutGrid* dataLayout = new CIwUILayoutGrid;
  dataLayout->AddColumn(captionWidth);
  dataLayout->AddColumn(sliderWidth);
  dataLayout->AddRow();

  dataArea->SetLayout(dataLayout);
  parentArea->AddChild(dataArea);
  parentLayout->AddElement(dataArea);

  // Label
  CIwUILabel* label = new CIwUILabel;
  label->SetStyle("<label_right>");
  label->SetCaption(caption);
  if (_label)
    *_label = label;

  dataArea->AddChild(label);
  dataLayout->AddElement(label, 0, 0);

  // Button
  CIwUIButton* button = new CIwUIButton;
  button->SetStyle("<button_wide>");
  button->SetCaption(buttonText);
  button->SetSizeMin(CIwVec2((int) (sliderWidth * 0.95f), button->GetSizeMin().y));
  button->SetSizeToContent(true);

  dataArea->AddChild(button);
  dataLayout->AddElement(button, 1, 0);

  return button;
}

//---------------------------------------------------------------------------------------------------------------------
IntSlider::IntSlider(
  const GameSettings& gameSettings,
  CIwUIElement* parentArea, CIwUILayout* parentLayout,
  const char* caption, const char* units, int& val,
  int minVal, int maxVal)
{
  mVal = &val;
  mUnits = units;

  int width =  (int) (settingsWidthFraction * gameSettings.mOptions.mFrameworkSettings.mScreenWidth);
  int height =  gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int sliderWidth = (int) (width * settingsControlFraction);
  int captionWidth = width - sliderWidth;

  mElements.mDataArea = new CIwUIElement;
  mElements.mDataArea->SetSizeToContent(false);

  mElements.mDataLayout = new CIwUILayoutGrid;
  mElements.mDataLayout->AddColumn(captionWidth);
  mElements.mDataLayout->AddColumn(sliderWidth);
  mElements.mDataLayout->AddRow();

  mElements.mDataArea->SetLayout(mElements.mDataLayout);
  parentArea->AddChild(mElements.mDataArea);
  parentLayout->AddElement(mElements.mDataArea);

  // Caption area/layout
  mElements.mCaptionLayout = new CIwUILayoutVertical;
  mElements.mCaptionLayout->SetSizeToSpace(true);

  mElements.mCaptionArea = new CIwUIElement;
  mElements.mCaptionArea->SetSizeToContent(true);
  mElements.mCaptionArea->SetLayout(mElements.mCaptionLayout);

  mElements.mDataArea->AddChild(mElements.mCaptionArea);
  mElements.mDataLayout->AddElement(mElements.mCaptionArea, 0, 0);

  // Label
  mElements.mLabel = new CIwUILabel;
  mElements.mLabel->SetStyle("<label_right_thin>");
  mElements.mLabel->SetCaption(caption);
  mElements.mLabelValue = new CIwUILabel;
  mElements.mLabelValue->SetStyle("<label_right_small_faint>");
  UpdateCaption();

  mElements.mCaptionArea->AddChild(mElements.mLabel);
  mElements.mCaptionLayout->AddElement(mElements.mLabel);
  mElements.mCaptionArea->AddChild(mElements.mLabelValue);
  mElements.mCaptionLayout->AddElement(mElements.mLabelValue);

  // Slider
  mElements.mSlider = new CIwUISlider;
  mElements.mSlider->SetStyle("<slider>");
  mElements.mSlider->SetProperty("sliderMin", int16(minVal));
  mElements.mSlider->SetProperty("sliderMax", int16(maxVal));
  mElements.mSlider->SetValue(val);

  mElements.mDataArea->AddChild(mElements.mSlider);
  mElements.mDataLayout->AddElement(mElements.mSlider, 1, 0);
}


//---------------------------------------------------------------------------------------------------------------------
void IntSlider::SetFromSlider()
{
  *mVal = mElements.mSlider->GetValue();
  UpdateCaption();
}

//---------------------------------------------------------------------------------------------------------------------
void IntSlider::UpdateCaption()
{
  char caption[256];
  if (mUnits)
    sprintf(caption, "%d%s", *mVal, mUnits);
  else
    sprintf(caption, "%d", *mVal);
  mElements.mLabelValue->SetCaption(caption);
}

//---------------------------------------------------------------------------------------------------------------------
FloatSlider::FloatSlider(
  const GameSettings& gameSettings,
  CIwUIElement* parentArea, CIwUILayout* parentLayout,
  const char* caption, const char* units, float& val,
  float minVal, float maxVal, float power)
{
  mVal = &val;
  mMinVal = minVal;
  mMaxVal = maxVal;
  mPower = power;
  mUnits = units;

  int width =  (int) (settingsWidthFraction * gameSettings.mOptions.mFrameworkSettings.mScreenWidth);
  int height =  gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int sliderWidth = (int) (width * settingsControlFraction);
  int captionWidth = width - sliderWidth;

  mElements.mDataArea = new CIwUIElement;
  mElements.mDataArea->SetSizeToContent(false);

  mElements.mDataLayout = new CIwUILayoutGrid;
  mElements.mDataLayout->AddColumn(captionWidth);
  mElements.mDataLayout->AddColumn(sliderWidth);
  mElements.mDataLayout->AddRow();

  mElements.mDataArea->SetLayout(mElements.mDataLayout);
  parentArea->AddChild(mElements.mDataArea);
  parentLayout->AddElement(mElements.mDataArea);

  // Caption area/layout
  mElements.mCaptionLayout = new CIwUILayoutVertical;
  mElements.mCaptionLayout->SetSizeToSpace(true);

  mElements.mCaptionArea = new CIwUIElement;
  mElements.mCaptionArea->SetSizeToContent(true);
  mElements.mCaptionArea->SetLayout(mElements.mCaptionLayout);

  mElements.mDataArea->AddChild(mElements.mCaptionArea);
  mElements.mDataLayout->AddElement(mElements.mCaptionArea, 0, 0);

  // Label
  mElements.mLabel = new CIwUILabel;
  mElements.mLabel->SetStyle("<label_right_thin>");
  mElements.mLabel->SetCaption(caption);
  mElements.mLabelValue = new CIwUILabel;
  mElements.mLabelValue->SetStyle("<label_right_small_faint>");
  UpdateCaption();

  mElements.mCaptionArea->AddChild(mElements.mLabel);
  mElements.mCaptionLayout->AddElement(mElements.mLabel);
  mElements.mCaptionArea->AddChild(mElements.mLabelValue);
  mElements.mCaptionLayout->AddElement(mElements.mLabelValue);

  // Slider
  mElements.mSlider = new CIwUISlider;
  mElements.mSlider->SetStyle("<slider>");
  mElements.mSlider->SetProperty("sliderMin", sliderMin);
  mElements.mSlider->SetProperty("sliderMax", sliderMax);
  mElements.mSlider->SetProperty("sliderStep", (int16) ((sliderMax - sliderMin)/100));

  float frac = (val - minVal) / (maxVal - minVal);
  frac = powf(frac, 1.0f / mPower);

  int intVal = (int) (sliderMin + 
    frac * (sliderMax - sliderMin));
  int16 int16Val;
  if (intVal < sliderMin)
    int16Val = sliderMin;
  else if (intVal > sliderMax)
    int16Val = sliderMax;
  else
    int16Val = (int16) intVal;

  mElements.mSlider->SetValue(int16Val);

  mElements.mDataArea->AddChild(mElements.mSlider);
  mElements.mDataLayout->AddElement(mElements.mSlider, 1, 0);
}

//---------------------------------------------------------------------------------------------------------------------
void FloatSlider::SetFromSlider()
{
  int16 value = mElements.mSlider->GetValue();
  float frac = (float(value) - sliderMin) / (sliderMax - sliderMin);
  frac = powf(frac, mPower);
  *mVal = mMinVal + frac * (mMaxVal - mMinVal);
  UpdateCaption();
}


//---------------------------------------------------------------------------------------------------------------------
void FloatSlider::UpdateCaption()
{
  char caption[256];
  if (mUnits)
    sprintf(caption, "%.3f%s", *mVal, mUnits);
  else
    sprintf(caption, "%.3f", *mVal);
  mElements.mLabelValue->SetCaption(caption);
}

//---------------------------------------------------------------------------------------------------------------------
Checkbox::Checkbox(
  const GameSettings& gameSettings,
  CIwUIElement* parentArea, CIwUILayout* parentLayout,
  const char* caption, bool& val)
{
  mVal = &val;

  int width =  (int) (settingsWidthFraction * gameSettings.mOptions.mFrameworkSettings.mScreenWidth);
  int height =  gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int checkboxWidth = (int) (width * settingsControlFraction);
  int captionWidth = width - checkboxWidth;

  mElements.mDataArea = new CIwUIElement;
  mElements.mDataArea->SetSizeToContent(false);

  mElements.mDataLayout = new CIwUILayoutGrid;
  mElements.mDataLayout->AddColumn(captionWidth);
  mElements.mDataLayout->AddColumn(checkboxWidth);
  mElements.mDataLayout->AddRow();

  mElements.mDataArea->SetLayout(mElements.mDataLayout);
  parentArea->AddChild(mElements.mDataArea);
  parentLayout->AddElement(mElements.mDataArea);

  // Caption
  mElements.mLabel = new CIwUILabel;
  mElements.mLabel->SetStyle("<label_right>");
  mElements.mLabel->SetCaption(caption);

  mElements.mDataArea->AddChild(mElements.mLabel);
  mElements.mDataLayout->AddElement(mElements.mLabel, 0, 0);

  // Checkbox
  mElements.mCheckbox = new CIwUICheckbox;
  mElements.mCheckbox->SetStyle("<checkbox>");
  mElements.mCheckbox->SetChecked(val);

  mElements.mDataArea->AddChild(mElements.mCheckbox);
  mElements.mDataLayout->AddElement(mElements.mCheckbox, 1, 0, 1, 1, IW_UI_ALIGN_LEFT);
}


//---------------------------------------------------------------------------------------------------------------------
void Checkbox::SetFromCheckbox()
{
  *mVal = mElements.mCheckbox->GetChecked();
}

//---------------------------------------------------------------------------------------------------------------------
Enum::Enum(
  const GameSettings& gameSettings,
  CIwUIElement* parentArea, CIwUILayout* parentLayout,
  const char* caption, int& val,
  const char** texts, int numTexts)
{
  mVal = &val;
  mTexts = texts;
  mNumTexts = numTexts;

  int width =  (int) (settingsWidthFraction * gameSettings.mOptions.mFrameworkSettings.mScreenWidth);
  int height =  gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int sliderWidth = (int) (width * settingsControlFraction);
  int captionWidth = width - sliderWidth;

  mElements.mDataArea = new CIwUIElement;
  mElements.mDataArea->SetSizeToContent(false);

  mElements.mDataLayout = new CIwUILayoutGrid;
  mElements.mDataLayout->AddColumn(captionWidth);
  mElements.mDataLayout->AddColumn(sliderWidth);
  mElements.mDataLayout->AddRow();

  mElements.mDataArea->SetLayout(mElements.mDataLayout);
  parentArea->AddChild(mElements.mDataArea);
  parentLayout->AddElement(mElements.mDataArea);

  // Caption area/layout
  mElements.mCaptionLayout = new CIwUILayoutVertical;
  mElements.mCaptionLayout->SetSizeToSpace(true);

  mElements.mCaptionArea = new CIwUIElement;
  mElements.mCaptionArea->SetSizeToContent(true);
  mElements.mCaptionArea->SetLayout(mElements.mCaptionLayout);

  mElements.mDataArea->AddChild(mElements.mCaptionArea);
  mElements.mDataLayout->AddElement(mElements.mCaptionArea, 0, 0);

  // Label
  mElements.mLabel = new CIwUILabel;
  mElements.mLabel->SetStyle("<label_right_thin>");
  mElements.mLabel->SetCaption(caption);
  mElements.mLabelValue = new CIwUILabel;
  mElements.mLabelValue->SetStyle("<label_right_small_faint>");
  UpdateCaption();

  mElements.mCaptionArea->AddChild(mElements.mLabel);
  mElements.mCaptionLayout->AddElement(mElements.mLabel);
  mElements.mCaptionArea->AddChild(mElements.mLabelValue);
  mElements.mCaptionLayout->AddElement(mElements.mLabelValue);

  // Slider
  mElements.mSlider = new CIwUISlider;
  mElements.mSlider->SetStyle("<slider>");
  mElements.mSlider->SetProperty("sliderMin", int16(0));
  mElements.mSlider->SetProperty("sliderMax", int16(numTexts-1));
  mElements.mSlider->SetValue(val);

  mElements.mDataArea->AddChild(mElements.mSlider);
  mElements.mDataLayout->AddElement(mElements.mSlider, 1, 0);
}


//---------------------------------------------------------------------------------------------------------------------
void Enum::SetFromSlider()
{
  *mVal = mElements.mSlider->GetValue();
  UpdateCaption();
}

//---------------------------------------------------------------------------------------------------------------------
void Enum::UpdateCaption()
{
  mElements.mLabelValue->SetCaption(mTexts[*mVal]);
}

//---------------------------------------------------------------------------------------------------------------------
String::String(
  const GameSettings& gameSettings,
  CIwUIElement* parentArea, CIwUILayout* parentLayout,
  const char* caption, int& val,
  const std::vector<std::string>& texts)
{
  mVal = &val;
  mTexts = texts;

  int width =  (int) (settingsWidthFraction * gameSettings.mOptions.mFrameworkSettings.mScreenWidth);
  int height =  gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int sliderWidth = (int) (width * settingsControlFraction);
  int captionWidth = width - sliderWidth;

  mElements.mDataArea = new CIwUIElement;
  mElements.mDataArea->SetSizeToContent(false);

  mElements.mDataLayout = new CIwUILayoutGrid;
  mElements.mDataLayout->AddColumn(captionWidth);
  mElements.mDataLayout->AddColumn(sliderWidth);
  mElements.mDataLayout->AddRow();

  mElements.mDataArea->SetLayout(mElements.mDataLayout);
  parentArea->AddChild(mElements.mDataArea);
  parentLayout->AddElement(mElements.mDataArea);

  // Caption area/layout
  mElements.mCaptionLayout = new CIwUILayoutVertical;
  mElements.mCaptionLayout->SetSizeToSpace(true);

  mElements.mCaptionArea = new CIwUIElement;
  mElements.mCaptionArea->SetSizeToContent(true);
  mElements.mCaptionArea->SetLayout(mElements.mCaptionLayout);

  mElements.mDataArea->AddChild(mElements.mCaptionArea);
  mElements.mDataLayout->AddElement(mElements.mCaptionArea, 0, 0);

  // Label
  mElements.mLabel = new CIwUILabel;
  mElements.mLabel->SetStyle("<label_right_thin>");
  mElements.mLabel->SetCaption(caption);
  mElements.mLabelValue = new CIwUILabel;
  mElements.mLabelValue->SetStyle("<label_right_small_faint>");
  UpdateCaption();

  mElements.mCaptionArea->AddChild(mElements.mLabel);
  mElements.mCaptionLayout->AddElement(mElements.mLabel);
  mElements.mCaptionArea->AddChild(mElements.mLabelValue);
  mElements.mCaptionLayout->AddElement(mElements.mLabelValue);

  // Slider
  mElements.mSlider = new CIwUISlider;
  mElements.mSlider->SetStyle("<slider>");
  mElements.mSlider->SetProperty("sliderMin", int16(0));
  mElements.mSlider->SetProperty("sliderMax", int16(mTexts.size()-1));
  mElements.mSlider->SetValue(val);

  mElements.mDataArea->AddChild(mElements.mSlider);
  mElements.mDataLayout->AddElement(mElements.mSlider, 1, 0);
}


//---------------------------------------------------------------------------------------------------------------------
void String::SetFromSlider()
{
  *mVal = mElements.mSlider->GetValue();
  UpdateCaption();
}

//---------------------------------------------------------------------------------------------------------------------
void String::UpdateCaption()
{
  char caption[256];
  int index = *mVal % mTexts.size();
  sprintf(caption, "%s", mTexts[index].c_str());
  mElements.mLabelValue->SetCaption(caption);
}

