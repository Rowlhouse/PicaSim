#include "SettingsMenu.h"
// #include "SettingsWidgets.h"
// #include "SelectMenu.h"
// #include "FileMenu.h"
// #include "Menu.h"
// #include "../GameSettings.h"
// #include "../PicaSim.h"
// #include "../Aeroplane.h"
// #include "../AeroplanePhysics.h"
// #include "../PicaJoystick.h"
// #include "../Scoreloop.h"
// #include "PicaDialog.h"

// #include <s3eOSExec.h>
// #include <s3eImagePicker.h>

#define LABEL(caption) CreateLabel(gameSettings, scrollArea, scrollLayout, caption, LABEL_TYPE_SETTING_BLOCK_TITLE)
#define CHECKBOX(caption, value) mCheckboxes.push_back(Checkbox(gameSettings, scrollArea, scrollLayout, caption, value))
#define SLIDER_INT(caption, units, value, minVal, maxVal) mIntSliders.push_back(IntSlider(gameSettings, scrollArea, scrollLayout, caption, units, value, minVal, maxVal))
#define SLIDER_FLOAT(caption, units, value, minVal, maxVal) mFloatSliders.push_back(FloatSlider(gameSettings, scrollArea, scrollLayout, caption, units, value, minVal, maxVal, 1.0f))
#define SLIDER_FLOAT_POWER(caption, units, value, minVal, maxVal, power) mFloatSliders.push_back(FloatSlider(gameSettings, scrollArea, scrollLayout, caption, units, value, minVal, maxVal, power))
#define SLIDER_ENUM(caption, value, texts, numTexts) mEnums.push_back(Enum(gameSettings, scrollArea, scrollLayout, caption, value, texts, numTexts))
#define SLIDER_STRING(caption, value, texts) mStrings.push_back(String(gameSettings, scrollArea, scrollLayout, caption, value, texts))

#define LABEL_BLOCK(caption) CreateLabel(gameSettings, blockArea, blockLayout, caption, LABEL_TYPE_SETTING)
#define CHECKBOX_BLOCK(caption, value) mCheckboxes.push_back(Checkbox(gameSettings, blockArea, blockLayout, caption, value))
#define SLIDER_INT_BLOCK(caption, units, value, minVal, maxVal) mIntSliders.push_back(IntSlider(gameSettings, blockArea, blockLayout, caption, units, value, minVal, maxVal))
#define SLIDER_FLOAT_BLOCK(caption, units, value, minVal, maxVal) mFloatSliders.push_back(FloatSlider(gameSettings, blockArea, blockLayout, caption, units, value, minVal, maxVal, 1.0f))
#define SLIDER_FLOAT_POWER_BLOCK(caption, units, value, minVal, maxVal, power) mFloatSliders.push_back(FloatSlider(gameSettings, blockArea, blockLayout, caption, units, value, minVal, maxVal, power))
#define SLIDER_ENUM_BLOCK(caption, value, texts, numTexts) mEnums.push_back(Enum(gameSettings, blockArea, blockLayout, caption, value, texts, numTexts))
#define SLIDER_STRING_BLOCK(caption, value, texts) mStrings.push_back(String(gameSettings, blockArea, blockLayout, caption, value, texts))
#define INFO_BLOCK(caption, text) CreateInfo(gameSettings, blockArea, blockLayout, caption, text)
#define BUTTON_BLOCK(caption, buttonText) CreateSettingsButton(gameSettings, blockArea, blockLayout, caption, buttonText, 0)
#define BUTTON_LABEL_BLOCK(caption, buttonText, label) CreateSettingsButton(gameSettings, blockArea, blockLayout, caption, buttonText, label)

static CIwUILabel* eventLabel = 0;

struct CheckForAIControlCallback : public IncludeCallback
{
  bool GetInclude(const char* filename) OVERRIDE
  {
    AIAeroplaneSettings aias;
    aias.LoadFromFile(filename);
    return aias.mAllowAIControl;
  }
};

struct CheckForTugCallback : public IncludeCallback
{
  bool GetInclude(const char* filename) OVERRIDE
  {
    AeroplaneSettings as;
    as.LoadFromFile(filename);
    return as.mAIAeroplaneSettings.mCanTow;
  }
};

//---------------------------------------------------------------------------------------------------------------------
enum SettingsStatus
{
  SETTINGS_UNSET,
  SETTINGS_BACK,
  SETTINGS_REFRESH,
  SETTINGS_LOAD_OPTIONS,
  SETTINGS_SAVE_OPTIONS,
  SETTINGS_DELETE_OPTIONS,
  SETTINGS_LOAD_ENVIRONMENT,
  SETTINGS_SAVE_ENVIRONMENT,
  SETTINGS_DELETE_ENVIRONMENT,
  SETTINGS_LOAD_OBJECTS,
  SETTINGS_SAVE_OBJECTS,
  SETTINGS_DELETE_OBJECTS,
  SETTINGS_LOAD_LIGHTING,
  SETTINGS_SAVE_LIGHTING,
  SETTINGS_DELETE_LIGHTING,
  SETTINGS_LOAD_CONTROLLER,
  SETTINGS_SAVE_CONTROLLER,
  SETTINGS_DELETE_CONTROLLER,
  SETTINGS_LOAD_JOYSTICK,
  SETTINGS_SAVE_JOYSTICK,
  SETTINGS_DELETE_JOYSTICK,
  SETTINGS_LOAD_AEROPLANE,
  SETTINGS_SAVE_AEROPLANE,
  SETTINGS_DELETE_AEROPLANE,
  SETTINGS_LOAD_AICONTROLLERS,
  SETTINGS_SAVE_AICONTROLLERS,
  SETTINGS_DELETE_AICONTROLLERS,
  SETTINGS_ADD_AICONTROLLER,
  SETTINGS_REMOVE_AICONTROLLERS,
  SETTINGS_LOAD_TUG,
  SETTINGS_SELECT_PANORAMA,
  SETTINGS_SELECT_FILE_TERRAIN,
  SETTINGS_SELECT_PREFERRED_CONTROLLER,
  SETTINGS_SELECT_OBJECTS_SETTINGS,
  SETTINGS_SELECT_LANGUAGE,
  SETTINGS_DELETE_LOCAL_HIGHSCORES,
  SETTINGS_RESET_OBJECTS,
  SETTINGS_CLEAR_JOYSTICK,
  SETTINGS_CALIBRATE_JOYSTICK,
  SETTINGS_CLEARALLSAVEDSETTINGSANDEXIT
};

extern float settingsWidthFraction;

//---------------------------------------------------------------------------------------------------------------------
class SettingsMenu : public Menu
{
public:
  SettingsMenu(GameSettings& gameSettings);
  ~SettingsMenu();

  int32 Update(bool keepAwake, bool throttle, bool& resourcesHaveChanged, const struct GameSettings& gameSettings) OVERRIDE;

  SettingsStatus mStatus;

  struct TabPanel
  {
    TabPanel();
    CIwUIElement* mContentArea;
    CIwUILayoutVertical* mContentLayout;
    CIwUIScrollableView* mScrollArea;
    CIwUIElement* mButtonsArea;
    CIwUILayoutGrid* mButtonsLayout;

    CIwUIButton* mLoadButton;
    CIwUIButton* mSaveButton;
    CIwUIButton* mDeleteButton;
    CIwUIButton* mToggleAdvancedButton;

    bool mAdvancedEnabled;
  };

  void GetImageButtonInfo(int& x, int& y, int& w, int& h);

private:
  void ZeroPointers();

  void CreateOptions1UI(GameSettings& gameSettings);
  void CreateOptions2UI(GameSettings& gameSettings);
  void CreateAeroplaneUI(GameSettings& gameSettings);
  void CreateSceneryUI(GameSettings& gameSettings);
  void CreateObjectsUI(GameSettings& gameSettings);
  void CreateLightingUI(GameSettings& gameSettings);
  void CreateControllerUI(GameSettings& gameSettings);
  void CreateJoystickUI(GameSettings& gameSettings);
  void CreateAIControllersUI(GameSettings& gameSettings);

  CIwUIButton* CreateSummaryAreaAndLayout(
    const GameSettings& gameSettings,
    CIwUIElement* scrollArea,
    CIwUILayout* scrollLayout,
    const std::string& title,
    const std::string& thumbnail,
    const std::string& info,
    bool createButton);

  virtual bool HandleEvent(CIwEvent* pEvent) OVERRIDE;

  CIwUILayoutGrid* mGridLayout;
  CIwUIImage* mGridArea;

  enum TabPanelEnum
  {
    TAB_OPTIONS1,
    TAB_OPTIONS2,
    TAB_AEROPLANE,
    TAB_SCENERY,
    TAB_OBJECTS,
    TAB_LIGHTING,
    TAB_AICONTROLLERS,
    TAB_CONTROLLER,
    TAB_JOYSTICK,
    TAB_NUM_TABS
  };
  static uint16   mSelectedTab;
  static TabPanel mTabPanels[TAB_NUM_TABS];

  GameSettings& mGameSettings;

  CIwUIButton* mBackButton;
  CIwUITabBar* mTabBar;
  CIwUIButton* mPanoramaButton;
  CIwUIButton* mImageButton;
  CIwUIButton* mAeroplanePreferredControllerButton;
  CIwUIButton* mSceneryObjectsButton;
  CIwUIButton* mAeroplaneWWWButton;
  CIwUIButton* mSceneryWWWButton;
  CIwUIButton* mDeleteLocalHighscoresButton;
  CIwUIButton* mClearAllSettingsButton;
  CIwUIButton* mResetObjectsButton;
  CIwUIButton* mAddAIControllerButton;
  CIwUIButton* mRemoveAIControllersButton;
  CIwUIButton* mClearJoystickButton;
  CIwUIButton* mCalibrateJoystickButton;
  CIwUIButton* mLanguageButton;

  CIwUIButton* mAeroplaneButton;
  CIwUIButton* mSceneryButton;
  CIwUIButton* mLightingButton;
  CIwUIButton* mTugButton;

  typedef std::vector<CIwUIButton*> Buttons;
  Buttons mDeleteAIControllerButtons;
  Buttons mCopyAIControllerButtons;

  CIwUIButton* mJoystickZeroButtons[JoystickSettings::JOYSTICK_NUM_CONTROLS];
  CIwUIButton* mJoystickPlusButtons[JoystickSettings::JOYSTICK_NUM_CONTROLS];
  CIwUIButton* mJoystickMinusButtons[JoystickSettings::JOYSTICK_NUM_CONTROLS];
  CIwUILabel*  mJoystickZeroLabels[JoystickSettings::JOYSTICK_NUM_CONTROLS];
  CIwUILabel*  mJoystickPlusLabels[JoystickSettings::JOYSTICK_NUM_CONTROLS];
  CIwUILabel*  mJoystickMinusLabels[JoystickSettings::JOYSTICK_NUM_CONTROLS];

  CIwUILabel* mJoystickNameLabel;
  CIwUILabel* mJoystickLabels[JoystickSettings::JOYSTICK_NUM_CONTROLS];
  CIwUILabel* mJoystickButtonLabels[JoystickSettings::JOYSTICK_NUM_BUTTONS];
  CIwUILabel* mJoystickAButtonLabels[JoystickSettings::JOYSTICK_NUM_CONTROLS];

  CIwUILabel* mMaxAILabel;

  CIwUILabel* mSmokeColourLabels[AeroplaneSettings::MAX_NUM_SMOKES_PER_PLANE];

  typedef std::vector<IntSlider> IntSliders;
  typedef std::vector<Enum> Enums;
  typedef std::vector<String> Strings;
  typedef std::vector<FloatSlider> FloatSliders;
  typedef std::vector<Checkbox> Checkboxes;

  IntSliders mIntSliders;
  Enums mEnums;
  Strings mStrings;
  FloatSliders mFloatSliders;
  Checkboxes mCheckboxes;

  Vector3 mSmokeHSVs[AeroplaneSettings::MAX_NUM_SMOKES_PER_PLANE];
};

uint16 SettingsMenu::mSelectedTab = 0;
SettingsMenu::TabPanel SettingsMenu::mTabPanels[SettingsMenu::TAB_NUM_TABS];

SettingsMenu::TabPanel::TabPanel() 
  : mContentArea(0), mContentLayout(0), mScrollArea(0), mButtonsArea(0), mButtonsLayout(0),
  mLoadButton(0), mSaveButton(0), mToggleAdvancedButton(0), mDeleteButton(0),
  mAdvancedEnabled(false)
{}

//---------------------------------------------------------------------------------------------------------------------
void SettingsMenu::ZeroPointers()
{
  // Options
  mDeleteLocalHighscoresButton = 0;
  mClearAllSettingsButton = 0;

  // Objects
  mResetObjectsButton = 0;

  // AI Controllers
  mAddAIControllerButton = 0;
  mRemoveAIControllersButton = 0;
  mMaxAILabel = 0;

  // Joystick
  mClearJoystickButton = 0;
  mCalibrateJoystickButton = 0;
  mJoystickNameLabel = 0;

  for (size_t iJoystickInput = 0 ; iJoystickInput != JoystickSettings::JOYSTICK_NUM_CONTROLS ; ++iJoystickInput)
  {
    mJoystickZeroButtons[iJoystickInput] = 0;
    mJoystickPlusButtons[iJoystickInput] = 0;
    mJoystickMinusButtons[iJoystickInput] = 0;
    mJoystickZeroLabels[iJoystickInput] = 0;
    mJoystickPlusLabels[iJoystickInput] = 0;
    mJoystickMinusLabels[iJoystickInput] = 0;
  }

  for (size_t i = 0 ; i != AeroplaneSettings::MAX_NUM_SMOKES_PER_PLANE ; ++i)
  {
    mSmokeColourLabels[i] = 0;
  }
}


//---------------------------------------------------------------------------------------------------------------------
void SettingsMenu::GetImageButtonInfo(int& x, int& y, int& w, int& h)
{
  if (mImageButton)
  {
    x = mImageButton->GetPos().x;
    y = mImageButton->GetPos().y;
    w = mImageButton->GetSize().x;
    h = mImageButton->GetSize().y;
  }
}


//---------------------------------------------------------------------------------------------------------------------
int32 SettingsMenu::Update(bool keepAwake, bool throttle, bool& resourcesHaveChanged, const struct GameSettings& gameSettings)
{
  mScrollArea = mTabPanels[mSelectedTab].mScrollArea;
  Language language = mGameSettings.mOptions.mLanguage;

  if (mTabPanels[TAB_JOYSTICK].mContentArea != 0 && mJoystickNameLabel)
  {
    const JoystickSettings& js = mGameSettings.mJoystickSettings;
    JoystickData joystick;
    if (S3E_RESULT_SUCCESS == GetJoystickStatus(joystick, mGameSettings.mOptions.mJoystickID))
    {
      mJoystickNameLabel->SetCaption(joystick.mName);
      for (size_t i = 0 ; i != JoystickSettings::JOYSTICK_NUM_CONTROLS ; ++i)
      {
        const JoystickSettings::JoystickAnalogueOverride& j = js.mJoystickAnalogueOverrides[i];
        float input = -1.0f + 2.0f * joystick.mAnalogueInputs[i] / 65535.0f;
        float output = input + j.mOffset;
        if (output > -j.mDeadZone && output < j.mDeadZone)
          output = 0.0f;
        else if (output > j.mDeadZone)
          output = (output - j.mDeadZone) / (1.0f - j.mDeadZone);
        else
          output = (output + j.mDeadZone) / (1.0f - j.mDeadZone);
        output *= output > 0.0f ? j.mScalePositive : j.mScaleNegative;

        char label[128];
        sprintf(label, TXT(PS_JOYSTICKLABEL), i, input, output);
        mJoystickLabels[i]->SetCaption(label);

        sprintf(label, "Input offset: %5.2f", j.mOffset);
        mJoystickZeroLabels[i]->SetCaption(label);
        sprintf(label, "Input -ve scale: %5.2f", j.mScaleNegative);
        mJoystickMinusLabels[i]->SetCaption(label);
        sprintf(label, "Input +ve scale: %5.2f", j.mScalePositive);
        mJoystickPlusLabels[i]->SetCaption(label);
      }

      for (size_t i = 0 ; i != JoystickSettings::JOYSTICK_NUM_BUTTONS ; ++i)
      {
        const JoystickSettings::JoystickButtonOverride& j = js.mJoystickButtonOverrides[i];
        float input = joystick.mButtons[i] / 128.0f;
        bool buttonDown = joystick.mButtons[i] > 64 ? 1 : 0;
        buttonDown = j.mInvert ? !buttonDown : buttonDown;
        char label[128];
        sprintf(label, TXT(PS_JOYSTICKBUTTONLABEL), i, input, buttonDown);
        mJoystickButtonLabels[i]->SetCaption(label);
      }

      for (size_t i = 0 ; i != JoystickSettings::JOYSTICK_NUM_CONTROLS ; ++i)
      {
        const JoystickSettings::JoystickButtonOverride& j = js.mJoystickAButtonOverrides[i];
        float input = -1.0f + 2.0f * joystick.mAnalogueInputs[i] / 65535.0f;
        bool buttonDown = input > 0.5f;
        buttonDown = j.mInvert ? !buttonDown : buttonDown;
        char label[128];
        sprintf(label, TXT(PS_JOYSTICKABUTTONLABEL), i, input, buttonDown);
        mJoystickAButtonLabels[i]->SetCaption(label);
      }
    }
    else
    {
      mJoystickNameLabel->SetCaption(TXT(PS_NOJOYSTICKWITHID));
    }
  }

  AeroplaneSettings& as = mGameSettings.mAeroplaneSettings;
  for (size_t i = 0 ; i != AeroplaneSettings::MAX_NUM_SMOKES_PER_PLANE ; ++i)
  {
    if (mSmokeColourLabels[i])
    {
      Vector3 rgb = HSV2RGB(mSmokeHSVs[i]);
      as.mSmokeSources[i].mColour = rgb;
      CIwColour col = ConvertToColour(rgb);
      mSmokeColourLabels[i]->SetColour(col);
      col.r += 128;
      col.g += 128;
      col.b += 128;
      mSmokeColourLabels[i]->SetTextColour(col);
    }
  }

  if (mMaxAILabel)
  {
    char txt[256];
    sprintf(txt, TXT(PS_MAXAI), mGameSettings.mOptions.mFreeFlightMaxAI);
    mMaxAILabel->SetCaption(txt);
  }
  return Menu::Update(keepAwake, throttle, resourcesHaveChanged, gameSettings);
}

//---------------------------------------------------------------------------------------------------------------------
SettingsMenu::SettingsMenu(GameSettings& gameSettings)
  : mGameSettings(gameSettings), mStatus(SETTINGS_UNSET)
{
  const Options& options = gameSettings.mOptions;
  const Language language = options.mLanguage;
  settingsControlFraction = 0.5f;

  mGridLayout = new CIwUILayoutGrid;
  mGridLayout->SetSizeToSpace(true); // Without this then large settings tabs push the tabs etc off screen
  mGridLayout->AddRow();
  mGridLayout->AddRow();
  mGridLayout->AddRow();
  mGridLayout->AddColumn();

  mGridArea = new CIwUIImage;
  mGridArea->SetSizeToContent(false); // If true the the whole screen is vertically centred. False places it at the top
  mGridArea->SetLayout(mGridLayout);
  CIwTexture* texture = (CIwTexture*)IwGetResManager()->GetResNamed("MenuBackground", "CIwTexture");
  mGridArea->SetTexture(texture);

  // Picks up all the events
  mGridArea->AddEventHandler(this); 

  CIwUIElement* spacer = 0;

  // Tab area and buttons
  {
    // Tab area
    CIwUILabel* navigationArea = new CIwUILabel;
    navigationArea->SetStyle("<label_bar_background>");
    navigationArea->SetSizeToContent(false); // If false the the area is aligned on the left. If true it centres
    mGridArea->AddChild(navigationArea);
    mGridLayout->AddElement(navigationArea, 0, 0);

    // Layout
    CIwUILayoutHorizontal* navigationLayout = new CIwUILayoutHorizontal;
    navigationLayout->SetSizeToSpace(true);
    navigationArea->SetLayout(navigationLayout);

    // Back button
    mBackButton = new CIwUIButton;
    mBackButton->SetStyle("<button_wide>");
    mBackButton->SetCaption(TXT(PS_BACK));
    navigationLayout->AddElement(mBackButton);
    navigationArea->AddChild(mBackButton);

    spacer = new CIwUIElement;
    navigationLayout->AddElement(spacer);
    navigationArea->AddChild(spacer);

    // Tab bar
    // Make a horizontal scroll area for it
    CIwUIScrollableView* tabBarArea = new CIwUIScrollableView;
    tabBarArea->SetSizeToContent(false);

    CIwUILayoutHorizontal* tabBarLayout = new CIwUILayoutHorizontal;
    tabBarLayout->SetSizeToSpace(false); // Needed for the buttons to go off screen and be scrollable
    tabBarArea->SetLayout(tabBarLayout);

    navigationLayout->AddElement(tabBarArea);
    navigationArea->AddChild(tabBarArea);

    // And the tab bar itself
    mTabBar = new CIwUITabBar;
    mTabBar->SetSizeToContent(false);
    mTabBar->SetRadioButtonStyle(0, "<radio_left>");
    mTabBar->SetRadioButtonCaption(0, TXT(PS_OPTIONS1));
    mTabBar->SetRadioButtonStyle(1, "<radio_centre>");
    mTabBar->SetRadioButtonCaption(1, TXT(PS_OPTIONS2));
    mTabBar->SetRadioButtonStyle(2, "<radio_centre>");
    mTabBar->SetRadioButtonCaption(2, TXT(PS_AEROPLANE));
    mTabBar->SetRadioButtonStyle(3, "<radio_centre>");
    mTabBar->SetRadioButtonCaption(3, TXT(PS_SCENERY));
    mTabBar->SetRadioButtonStyle(4, "<radio_centre>");
    mTabBar->SetRadioButtonCaption(4, TXT(PS_OBJECTS));
    mTabBar->SetRadioButtonStyle(5, "<radio_centre>");
    mTabBar->SetRadioButtonCaption(5, TXT(PS_LIGHTING));
    mTabBar->SetRadioButtonStyle(6, "<radio_centre>");
    mTabBar->SetRadioButtonCaption(6, TXT(PS_AICONTROLLERS));

    if (ShowJoystickInGame(mGameSettings))
    {
      mTabBar->SetRadioButtonStyle(7, "<radio_centre>");
      mTabBar->SetRadioButtonCaption(7, TXT(PS_CONTROLLER));
      mTabBar->SetRadioButtonStyle(8, "<radio_right>");
      mTabBar->SetRadioButtonCaption(8, TXT(PS_JOYSTICK));
    }
    else
    {
      mTabBar->SetRadioButtonStyle(7, "<radio_right>");
      mTabBar->SetRadioButtonCaption(7, TXT(PS_CONTROLLER));
    }

    mTabBar->SetSelected(mSelectedTab);

    tabBarArea->AddChild(mTabBar);
    tabBarLayout->AddElement(mTabBar);
  }

  ZeroPointers();
  CreateOptions1UI(gameSettings);
  CreateOptions2UI(gameSettings);
  CreateAeroplaneUI(gameSettings);
  CreateSceneryUI(gameSettings);
  CreateObjectsUI(gameSettings);
  CreateLightingUI(gameSettings);
  CreateAIControllersUI(gameSettings);
  CreateControllerUI(gameSettings);
  CreateJoystickUI(gameSettings);

  // Persist the selected tab
  mGridArea->AddChild(mTabPanels[mSelectedTab].mContentArea);
  mGridLayout->AddElement(mTabPanels[mSelectedTab].mContentArea, 0, 1);
  mGridArea->AddChild(mTabPanels[mSelectedTab].mButtonsArea);
  mGridLayout->AddElement(mTabPanels[mSelectedTab].mButtonsArea, 0, 2);

  IwGetUIView()->AddElement(mGridArea);
  IwGetUIView()->AddElementToLayout(mGridArea);

  // Add padding if necessary
  IwGetUIController()->Update();
  CIwVec2 tabBarSize = mTabBar->GetSize();
  CIwVec2 backSize = mBackButton->GetSize();
  int width = gameSettings.mOptions.mFrameworkSettings.mScreenWidth;
  int w = (width - tabBarSize.x)/2 - backSize.x;
  if (w > 1)
    spacer->SetSizeMin(CIwVec2(w,-1));
}

//---------------------------------------------------------------------------------------------------------------------
CIwUIButton* SettingsMenu::CreateSummaryAreaAndLayout(
  const GameSettings& gameSettings,
  CIwUIElement* scrollArea,
  CIwUILayout* scrollLayout,
  const std::string& title,
  const std::string& thumbnail,
  const std::string& info,
  bool createButton)
{
  int height = gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int width = (int) (settingsWidthFraction * gameSettings.mOptions.mFrameworkSettings.mScreenWidth);

  CIwUIButton* imageButton = 0;

  // Summary area
  CIwUIElement* summaryArea = new CIwUIElement;
  summaryArea->SetSizeToContent(true);

  // Summary layout
  CIwUILayoutHorizontal* summaryLayout = new CIwUILayoutHorizontal;
  summaryLayout->SetSizeToSpace(true);
  summaryArea->SetLayout(summaryLayout);

  // Now add the content

  // Add the image
  Texture* thumbnailTexture = GetCachedTexture(thumbnail, gameSettings.mOptions.m16BitTextures);
  uint32 w = thumbnailTexture->GetWidth();
  uint32 h = thumbnailTexture->GetHeight();
  int32 imageWidth = 0;
  if (w != 0 && h != 0)
  {
    CIwUIStyle style;
    style.InlinePropertySet();
    CIwUIPropertySet* pImageProps = style.GetInlinePropertySet();
    pImageProps->SetProperty("drawableType", IwHashString("image"));
    pImageProps->SetProperty("texture", thumbnailTexture);

    float ar = float(w)/float(h);
    float size = 0.4f;
    imageWidth = int32(ar * height * size);
    imageButton= new CIwUIButton;
    imageButton->SetProperty("aspectRatio", CIwVec2(w,h));
    imageButton->SetSizeMin(CIwVec2(imageWidth,(int32) (height*size)));
    imageButton->SetSizeMax(CIwVec2(imageWidth,(int32) (height*size)));
    imageButton->SetProperty("buttonUp", style);
    imageButton->SetProperty("buttonDown", style);

    summaryLayout->AddElement(imageButton, 0, IW_UI_ALIGN_CENTRE, IW_UI_ALIGN_TOP);
    summaryArea->AddChild(imageButton);
  }
  // Add the info

  // Summary area
  CIwUIElement* infoArea = new CIwUIElement;
  //infoArea->SetSizeMin(CIwVec2(width - imageWidth, -1));
  infoArea->SetSizeMax(CIwVec2(width - imageWidth, -1));

  // Summary layout
  CIwUILayoutVertical* infoLayout = new CIwUILayoutVertical;
  infoLayout->SetSizeToSpace(true);
  infoArea->SetLayout(infoLayout);

  summaryArea->AddChild(infoArea);
  summaryLayout->AddElement(infoArea, 0, IW_UI_ALIGN_CENTRE, IW_UI_ALIGN_MIDDLE);

  CIwUILabel* titleLabel = CreateLabel(gameSettings, infoArea, infoLayout, title.c_str(), LABEL_TYPE_HELP_TEXT);
  titleLabel->SetProperty("alignH", IW_UI_ALIGN_CENTRE);
  titleLabel->SetSizeToContent(false);
  CIwUILabel* infoLabel = CreateLabel(gameSettings, infoArea, infoLayout, info.c_str(), LABEL_TYPE_HELP_TEXT);
  infoLabel->SetProperty("alignH", IW_UI_ALIGN_CENTRE);
  infoLabel->SetSizeToContent(false);

  // attach to the parent
  scrollArea->AddChild(summaryArea);
  scrollLayout->AddElement(summaryArea);

  if (createButton)
    return imageButton;
  else
    return 0;
}


//---------------------------------------------------------------------------------------------------------------------
void CreateBlockAreaAndLayout(
  const GameSettings& gameSettings,
  CIwUILabel*& blockArea, 
  CIwUILayoutVertical*& blockLayout, 
  CIwUIScrollableView* scrollArea,
  CIwUILayoutVertical* scrollLayout)
{
  blockArea = new CIwUILabel;
  blockArea->SetStyle("<label_setting_background>");

  blockLayout = new CIwUILayoutVertical;
  blockLayout->SetSizeToSpace(false);
  blockArea->SetLayout(blockLayout);
  scrollArea->AddChild(blockArea);
  scrollLayout->AddElement(blockArea);

  blockArea->SetSizeMax(CIwVec2((int) (settingsWidthFraction * gameSettings.mOptions.mFrameworkSettings.mScreenWidth), -1));
}

//---------------------------------------------------------------------------------------------------------------------
void CreateContentAndButtonsAreaAndLayout(
  const GameSettings& gameSettings,
  SettingsMenu::TabPanel& tabPanel,
  CIwUIScrollableView*& scrollArea,
  CIwUILayoutVertical*& scrollLayout,
  const char* tabName,
  bool createButtons)
{
  const Language language = gameSettings.mOptions.mLanguage;
  int height = gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int width = gameSettings.mOptions.mFrameworkSettings.mScreenWidth;

  // Content area
  tabPanel.mContentArea = new CIwUIElement;
  tabPanel.mContentArea->SetSizeToContent(false);

  // Layout
  tabPanel.mContentLayout = new CIwUILayoutVertical;
  tabPanel.mContentLayout->SetSizeToSpace(true); // Stops the scroll view pushing everything else down
  tabPanel.mContentArea->SetLayout(tabPanel.mContentLayout);

  // Settings content area
  scrollArea = new CIwUIScrollableView;
  scrollArea->SetSizeToContent(false);
  scrollArea->SetSizeHint(CIwVec2(-1, height));
  tabPanel.mContentArea->AddChild(scrollArea);
  tabPanel.mContentLayout->AddElement(scrollArea);
  tabPanel.mScrollArea = scrollArea;

  // Layout
  scrollLayout = new CIwUILayoutVertical;
  scrollLayout->SetSizeToSpace(true);
  scrollArea->SetLayout(scrollLayout);

  // Buttons area
  if (createButtons)
  {
    tabPanel.mButtonsArea = new CIwUILabel;
    tabPanel.mButtonsArea->SetStyle("<label_bar_background>");
    tabPanel.mButtonsArea->SetSizeToContent(true);

    // Layout
    int numButtons = tabPanel.mAdvancedEnabled ? 4 : 3;
    tabPanel.mButtonsLayout = new CIwUILayoutGrid;
    tabPanel.mButtonsLayout->SetSizeToSpace(true);
    tabPanel.mButtonsLayout->AddRow();
    tabPanel.mButtonsArea->SetLayout(tabPanel.mButtonsLayout);

    // Buttons
    tabPanel.mLoadButton = new CIwUIButton;
    tabPanel.mLoadButton->SetStyle("<button_expanding>");
    tabPanel.mLoadButton->SetCaption(TXT(PS_LOAD));
    tabPanel.mButtonsLayout->AddColumn(0, width/numButtons);
    tabPanel.mButtonsLayout->AddElement(tabPanel.mLoadButton, 0, 0);
    tabPanel.mButtonsArea->AddChild(tabPanel.mLoadButton);

    // Save button
    tabPanel.mSaveButton = new CIwUIButton;
    tabPanel.mSaveButton->SetStyle("<button_expanding>");
    tabPanel.mSaveButton->SetCaption(TXT(PS_SAVE));
    tabPanel.mButtonsLayout->AddColumn(0, width/numButtons);
    tabPanel.mButtonsLayout->AddElement(tabPanel.mSaveButton, 1, 0);
    tabPanel.mButtonsArea->AddChild(tabPanel.mSaveButton);

    if (tabPanel.mAdvancedEnabled)
    {
      // Delete button
      tabPanel.mDeleteButton = new CIwUIButton;
      tabPanel.mDeleteButton->SetStyle("<button_expanding>");
      tabPanel.mDeleteButton->SetCaption(TXT(PS_DELETE));
      tabPanel.mButtonsLayout->AddColumn(0, width/numButtons);
      tabPanel.mButtonsLayout->AddElement(tabPanel.mDeleteButton, 2, 0);
      tabPanel.mButtonsArea->AddChild(tabPanel.mDeleteButton);
    }
    else
    {
      tabPanel.mDeleteButton = 0;
    }

    // Reset button
    tabPanel.mToggleAdvancedButton = new CIwUIButton;
    tabPanel.mToggleAdvancedButton->SetStyle("<button_expanding>");
    tabPanel.mButtonsLayout->AddColumn(0, width/numButtons);
    if (tabPanel.mAdvancedEnabled)
    {
      tabPanel.mToggleAdvancedButton->SetCaption(TXT(PS_SIMPLE));
      tabPanel.mButtonsLayout->AddElement(tabPanel.mToggleAdvancedButton, 3, 0);
    }
    else
    {
      tabPanel.mToggleAdvancedButton->SetCaption(TXT(PS_ADVANCED));
      tabPanel.mButtonsLayout->AddElement(tabPanel.mToggleAdvancedButton, 2, 0);
    }
    tabPanel.mButtonsArea->AddChild(tabPanel.mToggleAdvancedButton);
  }
  else
  {
    tabPanel.mButtonsArea = new CIwUIElement;
    tabPanel.mButtonsLayout = new CIwUILayoutGrid;
    tabPanel.mButtonsArea->SetLayout(tabPanel.mButtonsLayout);
  }
}

//---------------------------------------------------------------------------------------------------------------------
void SettingsMenu::CreateJoystickUI(GameSettings& gameSettings)
{
  CIwUIScrollableView* scrollArea;
  CIwUILayoutVertical* scrollLayout;

  bool joystickAvailable = JoystickAvailable() == S3E_TRUE;

  JoystickSettings& js = mGameSettings.mJoystickSettings;
  Language language = mGameSettings.mOptions.mLanguage;
  CreateContentAndButtonsAreaAndLayout(gameSettings, mTabPanels[TAB_JOYSTICK], scrollArea, scrollLayout, "joystick", joystickAvailable);

  CIwUILabel* blockArea = 0;
  CIwUILayoutVertical* blockLayout = 0;

  CIwUILabel* label;
  if (gameSettings.mOptions.mFrameworkSettings.mOS != S3E_OS_ID_WIN10)
  {
    label = LABEL(TXT(PS_JOYSTICKINFO));
    label->SetStyle("<label_centered>");
  }

  if (!joystickAvailable)
  {
    label = LABEL(TXT(PS_NOJOYSTICK));
    label->SetStyle("<label_centered>");
    // Push the text up a bit
    label = LABEL("  ");
    label->SetStyle("<label_centered>");

    mJoystickNameLabel = 0;
    mClearJoystickButton = 0;
    mCalibrateJoystickButton = 0;
    return;
  }

  LABEL(TXT(PS_EXTERNALJOYSTICKSETTINGS));
  CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);

  mJoystickNameLabel = LABEL_BLOCK(" ");
  SLIDER_INT_BLOCK(TXT(PS_JOYSTICKID), NULL, mGameSettings.mOptions.mJoystickID, 0, 4);
  CHECKBOX_BLOCK(TXT(PS_ENABLEJOYSTICK), js.mEnableJoystick);
  CHECKBOX_BLOCK(TXT(PS_ADJUSTFORCIRCULARSTICKMOVEMENT), js.mAdjustForCircularSticks);

  mClearJoystickButton = new CIwUIButton;
  mClearJoystickButton->SetStyle("<button_wide>");
  mClearJoystickButton->SetCaption(TXT(PS_CLEARJOYSTICKSETTINGS));
  blockLayout->AddElement(mClearJoystickButton);
  blockArea->AddChild(mClearJoystickButton);

  if (gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_WINDOWS)
  {
    mCalibrateJoystickButton = new CIwUIButton;
    mCalibrateJoystickButton->SetStyle("<button_wide>");
    mCalibrateJoystickButton->SetCaption(TXT(PS_CALIBRATEJOYSTICK));
    blockLayout->AddElement(mCalibrateJoystickButton);
    blockArea->AddChild(mCalibrateJoystickButton);
  }

  static const char* controlDescs[ControllerSettings::CONTROLLER_NUM_CONTROLS+1];
  controlDescs[ControllerSettings::CONTROLLER_STICK_ROLL] = TXT(PS_ROLLSTICK);
  controlDescs[ControllerSettings::CONTROLLER_STICK_PITCH] = TXT(PS_PITCHSTICK);
  controlDescs[ControllerSettings::CONTROLLER_STICK_YAW] = TXT(PS_YAWSTICK);
  controlDescs[ControllerSettings::CONTROLLER_STICK_SPEED] = TXT(PS_SPEEDSTICK);
  controlDescs[ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL] = TXT(PS_TILTHORIZONTAL);
  controlDescs[ControllerSettings::CONTROLLER_ACCEL_VERTICAL] = TXT(PS_TILTVERTICAL);
  controlDescs[ControllerSettings::CONTROLLER_ARROW_HORIZONTAL] = TXT(PS_ARROWSHORIZONTAL);
  controlDescs[ControllerSettings::CONTROLLER_ARROW_VERTICAL] = TXT(PS_ARROWSVERTICAL);
  controlDescs[ControllerSettings::CONTROLLER_CONSTANT] = TXT(PS_CONSTANT);
  controlDescs[ControllerSettings::CONTROLLER_BUTTON0] = TXT(PS_BUTTON0);
  controlDescs[ControllerSettings::CONTROLLER_BUTTON1] = TXT(PS_BUTTON1);
  controlDescs[ControllerSettings::CONTROLLER_BUTTON2] = TXT(PS_BUTTON2);
  controlDescs[ControllerSettings::CONTROLLER_NUM_CONTROLS] = TXT(PS_NONE);

  for (size_t iJoystickInput = 0 ; iJoystickInput != JoystickSettings::JOYSTICK_NUM_CONTROLS ; ++iJoystickInput)
  {
    char label[128];
    sprintf(label, TXT(PS_JOYSTICKLABEL), iJoystickInput, 0.0f, 0.0f);
    mJoystickLabels[iJoystickInput] = LABEL(label);
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);

    SLIDER_ENUM_BLOCK(TXT(PS_MAPTO), *((int*) &js.mJoystickAnalogueOverrides[iJoystickInput].mControl), controlDescs, sizeof(controlDescs)/sizeof(controlDescs[0]));
    mJoystickZeroButtons[iJoystickInput] = BUTTON_LABEL_BLOCK("Blah", TXT(PS_PRESSWHENCENTRED), &mJoystickZeroLabels[iJoystickInput]);
    mJoystickMinusButtons[iJoystickInput] = BUTTON_LABEL_BLOCK("Blah", TXT(PS_PRESSWHENLEFTORDOWN), &mJoystickMinusLabels[iJoystickInput]);
    mJoystickPlusButtons[iJoystickInput] = BUTTON_LABEL_BLOCK("Blah", TXT(PS_PRESSWHENRIGHTORUP), &mJoystickPlusLabels[iJoystickInput]);
    SLIDER_FLOAT_BLOCK(TXT(PS_DEADZONE), NULL, js.mJoystickAnalogueOverrides[iJoystickInput].mDeadZone, 0.0f, 0.5f);
  }

  static const char* buttonDescs[JoystickSettings::JoystickButtonOverride::NUM_BUTTON_CONTROLS];
  buttonDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_NONE] = TXT(PS_NONE);
  buttonDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_RATES] = TXT(PS_RATESBUTTON);
  buttonDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_RATESCYCLE] = TXT(PS_RATESCYCLEBUTTON);
  buttonDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_RELAUNCH] = TXT(PS_RELAUNCHBUTTON);
  buttonDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_CAMERA] = TXT(PS_CAMERABUTTON);
  buttonDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_PAUSEPLAY] = TXT(PS_PAUSEPLAYBUTTON);
  buttonDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON0] = TXT(PS_BUTTON0);
  buttonDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON1] = TXT(PS_BUTTON1);
  buttonDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON2] = TXT(PS_BUTTON2);
  buttonDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON0TOGGLE] = TXT(PS_BUTTON0TOGGLE);
  buttonDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON1TOGGLE] = TXT(PS_BUTTON1TOGGLE);
  buttonDescs[JoystickSettings::JoystickButtonOverride::CONTROL_BUTTON_BUTTON2TOGGLE] = TXT(PS_BUTTON2TOGGLE);

  for (size_t iJoystickButton = 0 ; iJoystickButton != JoystickSettings::JOYSTICK_NUM_BUTTONS ; ++iJoystickButton)
  {
    char label[128];
    sprintf(label, TXT(PS_JOYSTICKBUTTONLABEL), iJoystickButton, 0.0f, 0);
    mJoystickButtonLabels[iJoystickButton] = LABEL(label);
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);

    SLIDER_ENUM_BLOCK(TXT(PS_MAPTO), *((int*) &js.mJoystickButtonOverrides[iJoystickButton].mControl), buttonDescs, sizeof(buttonDescs)/sizeof(buttonDescs[0]));
    CHECKBOX_BLOCK(TXT(PS_INVERT), js.mJoystickButtonOverrides[iJoystickButton].mInvert);
  }

  for (size_t iJoystickAButton = 0 ; iJoystickAButton != JoystickSettings::JOYSTICK_NUM_CONTROLS ; ++iJoystickAButton)
  {
    char label[128];
    sprintf(label, TXT(PS_JOYSTICKBUTTONLABEL), iJoystickAButton, 0.0f, 0);
    mJoystickAButtonLabels[iJoystickAButton] = LABEL(label);
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);

    SLIDER_ENUM_BLOCK(TXT(PS_MAPTO), *((int*) &js.mJoystickAButtonOverrides[iJoystickAButton].mControl), buttonDescs, sizeof(buttonDescs)/sizeof(buttonDescs[0]));
    CHECKBOX_BLOCK(TXT(PS_INVERT), js.mJoystickButtonOverrides[iJoystickAButton].mInvert);
  }
}

//---------------------------------------------------------------------------------------------------------------------
void SettingsMenu::CreateOptions1UI(GameSettings& gameSettings)
{
  CIwUIScrollableView* scrollArea;
  CIwUILayoutVertical* scrollLayout;

  Options& options = mGameSettings.mOptions;
  Language language = options.mLanguage;
  CreateContentAndButtonsAreaAndLayout(gameSettings, mTabPanels[TAB_OPTIONS1], scrollArea, scrollLayout, "Options 1", true);

  mClearAllSettingsButton = 0;

  CIwUILabel* blockArea = 0;
  CIwUILayoutVertical* blockLayout = 0;

  LABEL(TXT(PS_LANGUAGESETTINGS));
  CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
  mLanguageButton = BUTTON_BLOCK(TXT(PS_CURRENTLANGUAGE), gLanguageStrings[language]);

  LABEL(TXT(PS_CAMERASETTINGS));
  CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);

  CHECKBOX_BLOCK(TXT(PS_ZOOMVIEW), options.mEnableZoomView);
  CHECKBOX_BLOCK(TXT(PS_PLANEONLYINZOOMVIEW), options.mOnlyPlaneInZoomView);
  CHECKBOX_BLOCK(TXT(PS_SMOKEONLYINMAINVIEW), options.mSmokeOnlyInMainView);
  if (mTabPanels[TAB_OPTIONS1].mAdvancedEnabled)
    SLIDER_FLOAT_BLOCK(TXT(PS_ZOOMVIEWSIZE), NULL, options.mZoomViewSize, 0.1f, 0.5f);
  SLIDER_FLOAT_BLOCK(TXT(PS_GROUNDAUTOZOOM), NULL, options.mGroundViewAutoZoom, 0.0f, 1.0f);
  SLIDER_FLOAT_BLOCK(TXT(PS_GROUNDHORIZONAMOUNT), NULL, options.mGroundViewHorizonAmount, 0.0f, 1.0f);
  SLIDER_FLOAT_BLOCK(TXT(PS_GROUNDLAG), "s", options.mGroundViewLag, 0.0f, 1.0f);
  if (mTabPanels[TAB_OPTIONS1].mAdvancedEnabled)
  {
    SLIDER_FLOAT_BLOCK(TXT(PS_GROUNDAUTOZOOMSCALE), NULL, options.mGroundViewAutoZoomScale, 0.1f, 2.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_GROUNDFIELDOFVIEW), "deg", options.mGroundViewFieldOfView, 10.0f, 120.0f);
    CHECKBOX_BLOCK(TXT(PS_GROUNDVIEWFOLLOW), options.mGroundViewFollow);
    SLIDER_FLOAT_BLOCK(TXT(PS_GROUNDVIEWYAWOFFSET), "deg", options.mGroundViewYawOffset, -180.0f, 180.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_GROUNDVIEWPITCHOFFSET), "deg", options.mGroundViewPitchOffset, -90.0f, 90.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_AEROPLANEFIELDOFVIEW), "deg", options.mAeroplaneViewFieldOfView, 10.0f, 120.0f);
  }

  if (mTabPanels[TAB_OPTIONS1].mAdvancedEnabled)
  {
    LABEL(TXT(PS_STEREOSCOPY));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    CHECKBOX_BLOCK(TXT(PS_ENABLE), options.mEnableStereoscopy);
    SLIDER_FLOAT_BLOCK(TXT(PS_STEREOSEPARATION), "m", options.mStereoSeparation, -1.0f, 1.0f);
    LABEL_BLOCK(TXT(PS_STEREOINFO));
  }

  if (
    gameSettings.mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_FREEFLY ||
    mTabPanels[TAB_OPTIONS1].mAdvancedEnabled
    )
  {
    LABEL(TXT(PS_SIMULATIONSETTINGS));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    if (gameSettings.mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_FREEFLY)
      SLIDER_FLOAT_BLOCK(TXT(PS_TIMESCALE), NULL, options.mTimeScale, 0.1f, 1.0f);
    SLIDER_INT_BLOCK(TXT(PS_PHYSICSACCURACY), NULL, options.mFrameworkSettings.mPhysicsSubsteps, 4, 12);
  }

  LABEL(TXT(PS_CONTROLLERSETTINGS));
  CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);

  static const char* modeDescs[4];
  modeDescs[0] = TXT(PS_MODE1DESCRIPTION);
  modeDescs[1] = TXT(PS_MODE2DESCRIPTION);
  modeDescs[2] = TXT(PS_MODE3DESCRIPTION);
  modeDescs[3] = TXT(PS_MODE4DESCRIPTION);

  SLIDER_ENUM_BLOCK(TXT(PS_CONTROLLERMODE), *((int*) &options.mControllerMode), modeDescs, 4);
  SLIDER_FLOAT_BLOCK(TXT(PS_CONTROLLERSIZE), NULL, options.mControllerSize, 0.2f, 1.0f);
  CHECKBOX_BLOCK(TXT(PS_BRAKESFORWARD), options.mControllerBrakesForward);
  CHECKBOX_BLOCK(TXT(PS_USEABSOLUTECONTROLLERTOUCHPOSITION), options.mControllerUseAbsolutePosition);
  if (gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_ANDROID)
  {
    CHECKBOX_BLOCK(TXT(PS_STAGGERCONTROLLER), options.mControllerStaggered);
  }
  CHECKBOX_BLOCK(TXT(PS_ENABLEELEVATORTRIM), options.mControllerEnableTrim);
  if (mTabPanels[TAB_OPTIONS1].mAdvancedEnabled)
  {
    SLIDER_FLOAT_BLOCK(TXT(PS_ELEVATORTRIMSIZE), NULL, options.mControllerTrimSize, 0.01f, 0.1f);
    CHECKBOX_BLOCK(TXT(PS_SQUARECONTROLLERS), options.mControllerSquare);
    SLIDER_FLOAT_BLOCK(TXT(PS_HORIZONTALOFFSETFROMEDGE), NULL, options.mControllerHorOffset, 0.0f, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_VERTICALOFFSETFROMEDGE), NULL, options.mControllerVerOffset, 0.0f, 1.0f);
    static const char* controllerStyleDescs[3];
    controllerStyleDescs[0] = TXT(PS_CROSS);
    controllerStyleDescs[1] = TXT(PS_BOX);
    controllerStyleDescs[2] = TXT(PS_CROSSANDBOX);
    SLIDER_ENUM_BLOCK(TXT(PS_STYLE), *((int*) &options.mControllerStyle), controllerStyleDescs, 3);

    SLIDER_INT_BLOCK(TXT(PS_SHAPEOPACITY), NULL, options.mControllerAlpha, 0, 255);
    SLIDER_INT_BLOCK(TXT(PS_STICKOPACITY), NULL, options.mControllerStickAlpha, 0, 255);
    CHECKBOX_BLOCK(TXT(PS_STICKCROSS), options.mControllerStickCross);
  }

  LABEL(TXT(PS_AUDIOSETTINGS));
  CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
  SLIDER_FLOAT_BLOCK(TXT(PS_OVERALLVOLUME), NULL, options.mVolumeScale, 0.0f, 2.0f);
  SLIDER_FLOAT_BLOCK(TXT(PS_VARIOMETERVOLUME), NULL, options.mVariometerVolume, 0.0f, 2.0f);
  if (mTabPanels[TAB_OPTIONS1].mAdvancedEnabled)
  {
    SLIDER_FLOAT_BLOCK(TXT(PS_WINDVOLUME), NULL, options.mWindVolume, 0.0f, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_OUTSIDEAEROPLANEVOLUME), NULL, options.mOutsideAeroplaneVolume, 0.0f, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_INSIDEAEROPLANEVOLUME), NULL, options.mInsideAeroplaneVolume, 0.0f, 1.0f);

    LABEL(TXT(PS_ONSCREENDISPLAYSETTINGS));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    SLIDER_FLOAT_BLOCK(TXT(PS_WINDARROWSIZE), NULL, options.mWindArrowSize, 0.05f, 0.25f);
    SLIDER_INT_BLOCK(TXT(PS_WINDARROWOPACITY), NULL, options.mWindsockOpacity, 0, 255);
    SLIDER_FLOAT_BLOCK(TXT(PS_PAUSEBUTTONSSIZE), NULL, options.mPauseButtonsSize, 0.25f, 0.75f);
    SLIDER_INT_BLOCK(TXT(PS_PAUSEBUTTONOPACITY), NULL, options.mPauseButtonOpacity, 0, 255);
  }

  LABEL(TXT(PS_INFORMATIONSETTINGS));
  CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
  CHECKBOX_BLOCK(TXT(PS_GRAPHFPS), options.mDisplayFPS);

  SLIDER_INT_BLOCK(TXT(PS_MAXMARKERSPERTHERMAL), NULL, options.mMaxMarkersPerThermal, 0, Options::MAX_MAX_MARKERS_PER_THERMAL);
  if (mTabPanels[TAB_OPTIONS1].mAdvancedEnabled)
  {
    CHECKBOX_BLOCK(TXT(PS_THERMALWINDFIELD), options.mDrawThermalWindField);
  }
  CHECKBOX_BLOCK(TXT(PS_DRAWGROUNDPOSITION), options.mDrawGroundPosition);

  static const char* skyGridDescs[3];
  skyGridDescs[0] = TXT(PS_SKYGRID_NONE);
  skyGridDescs[1] = TXT(PS_SKYGRID_SPHERE);
  skyGridDescs[2] = TXT(PS_SKYGRID_BOX);
  SLIDER_ENUM_BLOCK(TXT(PS_SKYGRIDOVERLAY), *((int*) &options.mSkyGridOverlay), skyGridDescs, 3);
  static const char* skyGridAlignDescs[4];
  skyGridAlignDescs[0] = TXT(PS_SKYGRIDALIGN_ALONGWIND);
  skyGridAlignDescs[1] = TXT(PS_SKYGRIDALIGN_CROSSWIND);
  skyGridAlignDescs[2] = TXT(PS_SKYGRIDALIGN_ALONGRUNWAY);
  skyGridAlignDescs[3] = TXT(PS_SKYGRIDALIGN_CROSSRUNWAY);
  SLIDER_ENUM_BLOCK(TXT(PS_SKYGRIDALIGNMENT), *((int*) &options.mSkyGridAlignment), skyGridAlignDescs, 4);
  if (mTabPanels[TAB_OPTIONS1].mAdvancedEnabled)
  {
    SLIDER_FLOAT_BLOCK(TXT(PS_SKYGRIDDISTANCE), "m", options.mSkyGridDistance, 10.0f, 200.0f);
  }

  if (mTabPanels[TAB_OPTIONS1].mAdvancedEnabled)
  {
    if (!PicaSim::IsCreated() || PicaSim::GetInstance().GetSettings().mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_FREEFLY)
    {
      SLIDER_FLOAT_BLOCK(TXT(PS_GRAPHDURATION), "s", options.mGraphDuration, 5.0f, 240.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_GRAPHALTITUDE), "m", options.mGraphAltitude, 0, 500.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_GRAPHAIRSPEED), "m/s", options.mGraphAirSpeed, 0, 200.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_GRAPHGROUNDSPEED), "m/s", options.mGraphGroundSpeed, 0, 200.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_GRAPHCLIMBRATE), "m/s", options.mGraphClimbRate, 0, 10.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_GRAPHWINDSPEED), "m/s", options.mGraphWindSpeed, 0, 30.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_GRAPHWINDVERTICALVELOCITY), "m/s", options.mGraphWindVerticalVelocity, 0, 15.0f);
    }
    SLIDER_FLOAT_BLOCK(TXT(PS_GRAPHFPS), NULL, options.mGraphFPS, 0, 60.0f);
    CHECKBOX_BLOCK(TXT(PS_STALLMARKERS), options.mStallMarkers);
    static const char* CoMDescs[3];
    CoMDescs[0] = TXT(PS_NONE);
    CoMDescs[1] = TXT(PS_CROSS);
    CoMDescs[2] = TXT(PS_BOX);
    SLIDER_ENUM_BLOCK(TXT(PS_DRAWAEROPLANECOM), *((int*) &options.mDrawAeroplaneCoM), CoMDescs, 3);
    SLIDER_INT_BLOCK(TXT(PS_NUMWINDSTREAMERS), NULL, options.mNumWindStreamers, 0, 64);
    SLIDER_FLOAT_BLOCK(TXT(PS_WINDSTREAMERTIME), "s", options.mWindStreamerTime, 0.1f, 60.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_WINDSTREAMERDELTAZ), "m", options.mWindStreamerDeltaZ, 0.1f, 10.0f);

    LABEL(TXT(PS_AEROFOILPLOT));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    SLIDER_INT_BLOCK(TXT(PS_AEROFOILINFO), NULL, options.mAerofoilInfo, -1, 32);
    SLIDER_FLOAT_BLOCK(TXT(PS_AEROFOILRANGE), NULL, options.mAerofoilPlotAngleRange, 10, 180);
    SLIDER_FLOAT_BLOCK(TXT(PS_AEROFOILCLMAX), NULL, options.mAerofoilPlotCLMax, 1.0f, 2.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_AEROFOILCDMAX), NULL, options.mAerofoilPlotCDMax, 0.1f, 2.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_AEROFOILCMMAX), NULL, options.mAerofoilPlotCMMax, 0.01f, 2.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_AEROFOILLDMAX), NULL, options.mAerofoilPlotLDMax, 10.0f, 200.0f);
    CHECKBOX_BLOCK(TXT(PS_AEROFOILPLOTREFERENCE), options.mAerofoilPlotReference);
    CHECKBOX_BLOCK(TXT(PS_AEROFOILPOLAR), options.mAerofoilPlotPolar);

    LABEL(TXT(PS_GRAPHICSSETTINGS));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    SLIDER_FLOAT_BLOCK(TXT(PS_GROUNDTERRAINLOD), NULL, options.mGroundViewTerrainLOD, 0.0f, 500.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_AEROPLANETERRAINLOD), NULL, options.mAeroplaneViewTerrainLOD, 0.0f, 500.0f);
    CHECKBOX_BLOCK(TXT(PS_UPDATETERRAINLOD), options.mGroundViewUpdateTerrainLOD);

    static const char* renderDescs[3];
    renderDescs[0] = TXT(PS_COMPONENTS);
    renderDescs[1] = TXT(PS_3DMODEL);
    renderDescs[2] = TXT(PS_BOTH);

    SLIDER_ENUM_BLOCK(TXT(PS_AEROPLANERENDER), *((int*) &options.mRenderPreference), renderDescs, 3);

    static const char* shadowDescs[3];
    shadowDescs[0] = TXT(PS_NONE);
    shadowDescs[1] = TXT(PS_BLOB);
    shadowDescs[2] = TXT(PS_PROJECTED);
    SLIDER_ENUM_BLOCK(TXT(PS_CONTROLLEDPLANESHADOWS), *((int*) &options.mControlledPlaneShadows), shadowDescs, 3);
    SLIDER_ENUM_BLOCK(TXT(PS_OTHERSHADOWS), *((int*) &options.mOtherShadows), shadowDescs, 3);
    SLIDER_INT_BLOCK(TXT(PS_PROJECTEDSHADOWDETAIL), NULL, options.mProjectedShadowDetail, 7, 10);
    CHECKBOX_BLOCK(TXT(PS_USE16BIT), options.m16BitTextures);
    CHECKBOX_BLOCK(TXT(PS_SEPARATESPECULAR), options.mSeparateSpecular);
    SLIDER_FLOAT_BLOCK(TXT(PS_AMBIENTLIGHTINGSCALE), NULL, options.mAmbientLightingScale, 0.0f, 5.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_DIFFUSELIGHTINGSCALE), NULL, options.mDiffuseLightingScale, 0.0f, 5.0f);
    SLIDER_INT_BLOCK(TXT(PS_TERRAINTEXTUREDETAIL), NULL, options.mBasicTextureDetail, 8, 10);
    SLIDER_INT_BLOCK(TXT(PS_MAXSKYBOXDETAIL), NULL, options.mMaxSkyboxDetail, 1, 2);
    SLIDER_FLOAT_BLOCK(TXT(PS_NEARCLIPDISTANCE), "m", options.mMaxNearClipDistance, 0.01f, 2.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_FARCLIPDISTANCE), "m", options.mFrameworkSettings.mFarClipPlaneDistance, 1000.0f, 100000.0f);
    CHECKBOX_BLOCK(TXT(PS_USEMULTILIGHTS), options.mFrameworkSettings.mUseMultiLights);
    CHECKBOX_BLOCK(TXT(PS_ENABLESMOKE), options.mEnableSmoke);
    SLIDER_FLOAT_BLOCK(TXT(PS_SMOKEQUALITY), NULL, options.mSmokeQuality, 0.4f, 5.0f);

    LABEL(TXT(PS_MISCSETTINGS));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    mClearAllSettingsButton = new CIwUIButton;
    mClearAllSettingsButton->SetStyle("<button_wide>");
    mClearAllSettingsButton->SetCaption(TXT(PS_CLEARALLSAVEDSETTINGSANDEXIT));
    blockLayout->AddElement(mClearAllSettingsButton);
    blockArea->AddChild(mClearAllSettingsButton);
  }
  else
  {
    LABEL(TXT(PS_GRAPHICSSETTINGS));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    static const char* shadowDescs[3];
    shadowDescs[0] = TXT(PS_NONE);
    shadowDescs[1] = TXT(PS_BLOB);
    shadowDescs[2] = TXT(PS_PROJECTED);
    SLIDER_ENUM_BLOCK(TXT(PS_CONTROLLEDPLANESHADOWS), *((int*) &options.mControlledPlaneShadows), shadowDescs, 3);
    SLIDER_ENUM_BLOCK(TXT(PS_OTHERSHADOWS), *((int*) &options.mOtherShadows), shadowDescs, 3);
    SLIDER_INT_BLOCK(TXT(PS_MAXSKYBOXDETAIL), NULL, options.mMaxSkyboxDetail, 1, 2);
    CHECKBOX_BLOCK(TXT(PS_SEPARATESPECULAR), options.mSeparateSpecular);
    CHECKBOX_BLOCK(TXT(PS_ENABLESMOKE), options.mEnableSmoke);
    SLIDER_FLOAT_BLOCK(TXT(PS_SMOKEQUALITY), NULL, options.mSmokeQuality, 0.4f, 3.0f);
  }
}

//---------------------------------------------------------------------------------------------------------------------
void SettingsMenu::CreateOptions2UI(GameSettings& gameSettings)
{
  CIwUIScrollableView* scrollArea;
  CIwUILayoutVertical* scrollLayout;

  Options& options = mGameSettings.mOptions;
  Language language = options.mLanguage;
  CreateContentAndButtonsAreaAndLayout(gameSettings, mTabPanels[TAB_OPTIONS2], scrollArea, scrollLayout, "Options 2", true);
  mDeleteLocalHighscoresButton = 0;

  CIwUILabel* blockArea = 0;
  CIwUILayoutVertical* blockLayout = 0;

  LABEL(TXT(PS_FREEFLYSETTINGS));
  CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
  CHECKBOX_BLOCK(TXT(PS_FREEFLYONSTARTUP), options.mFreeFlyOnStartup);
  LABEL_BLOCK(TXT(PS_TOOMANYAI));
  int maxAI = mTabPanels[TAB_OPTIONS2].mAdvancedEnabled ? 50 : 10;
  SLIDER_INT_BLOCK(TXT(PS_MAXNUMBERAI), NULL, options.mFreeFlightMaxAI, 0, maxAI);
  CHECKBOX_BLOCK(TXT(PS_DISPLAYFLIGHTTIME), options.mFreeFlightDisplayTime);
  CHECKBOX_BLOCK(TXT(PS_DISPLAYSPEED), options.mFreeFlightDisplaySpeed);
  CHECKBOX_BLOCK(TXT(PS_DISPLAYAIRSPEED), options.mFreeFlightDisplayAirSpeed);
  CHECKBOX_BLOCK(TXT(PS_DISPLAYMAXSPEED), options.mFreeFlightDisplayMaxSpeed);
  CHECKBOX_BLOCK(TXT(PS_DISPLAYASCENTRATE), options.mFreeFlightDisplayAscentRate);
  CHECKBOX_BLOCK(TXT(PS_DISPLAYALTITUDE), options.mFreeFlightDisplayAltitude);
  CHECKBOX_BLOCK(TXT(PS_DISPLAYDISTANCE), options.mFreeFlightDisplayDistance);
  CHECKBOX_BLOCK(TXT(PS_COLOURTEXT), options.mFreeFlightColourText);
  if (mTabPanels[TAB_OPTIONS2].mAdvancedEnabled)
  {
    CHECKBOX_BLOCK(TXT(PS_ENABLESOCKETCONTROLLER), options.mEnableSocketController);
    CHECKBOX_BLOCK(TXT(PS_TEXTATTOP), options.mFreeFlightTextAtTop);
    SLIDER_FLOAT_BLOCK(TXT(PS_TEXTBACKGROUNDOPACITY), NULL, options.mFreeFlightTextBackgroundOpacity, 0.0f, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_TEXTBACKGROUNDCOLOUR), NULL, options.mFreeFlightTextBackgroundColour, 0.0f, 1.0f);
  }
  static const char* unitDescs[3];
  unitDescs[0] = "m/s & m";
  unitDescs[1] = "km/h & m";
  unitDescs[2] = "mph & ft";
  SLIDER_ENUM_BLOCK(TXT(PS_UNITS), *((int*) &options.mFreeFlightUnits), unitDescs, Options::UNITS_MAX);

  if (gameSettings.mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_FREEFLY)
  {
    LABEL(TXT(PS_WALKABOUTSETTINGS));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    CHECKBOX_BLOCK(TXT(PS_ENABLEWALKABOUTBUTTON), options.mEnableWalkabout);
    if (mTabPanels[TAB_OPTIONS2].mAdvancedEnabled)
      CHECKBOX_BLOCK(TXT(PS_SETWINDDIRONWALKABOUT), options.mSetWindDirectionOnWalkabout);
  }

  LABEL(TXT(PS_RACESETTINGS));
  CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
  SLIDER_FLOAT_BLOCK(TXT(PS_LIMBODIFFICULTY), NULL, options.mLimboDifficultyMultiplier, 1, Options::LIMBO_MAX_DIFFICULTY_MULTIPLIER);
#ifdef ENABLE_SCORELOOP
#ifndef IW_DEBUG
  if (!options.mFrameworkSettings.isWindows())
#endif
  {
    CHECKBOX_BLOCK(TXT(PS_ENABLESCORELOOP), options.mEnableScoreloop);
  }
#endif
  if (mTabPanels[TAB_OPTIONS2].mAdvancedEnabled)
  {
    SLIDER_FLOAT_BLOCK(TXT(PS_RACEVIBRATIONAMOUNT), NULL, options.mRaceVibrationAmount, 0, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_RACEBEEPVOLUME), NULL, options.mRaceBeepVolume, 0, 1.0f);

    mDeleteLocalHighscoresButton = new CIwUIButton;
    mDeleteLocalHighscoresButton->SetStyle("<button_wide>");
    mDeleteLocalHighscoresButton->SetCaption(TXT(PS_DELETELOCALHIGHSCORES));
    blockLayout->AddElement(mDeleteLocalHighscoresButton);
    blockArea->AddChild(mDeleteLocalHighscoresButton);

    LABEL(TXT(PS_MISCSETTINGS));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    if (gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_ANDROID)
    {
      CHECKBOX_BLOCK(TXT(PS_USEBACKBUTTON), options.mUseBackButtonToExit);
    }
    CHECKBOX_BLOCK(TXT(PS_DRAWLAUNCHMARKER), options.mDrawLaunchMarker);
    CHECKBOX_BLOCK(TXT(PS_USEAEROPLANEPREFERREDCONTROLLER), options.mUseAeroplanePreferredController);

    if (options.mFrameworkSettings.isWindows())
    {
      LABEL(TXT(PS_TESTINGDEVELOPERSETTINGS));
      CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
      if (gGLVersion == 1)
      {
        CHECKBOX_BLOCK(TXT(PS_WIREFRAMETERRAIN), options.mTerrainWireframe);
      }
      CHECKBOX_BLOCK(TXT(PS_DRAWSUNPOSITION), options.mDrawSunPosition);
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
void SettingsMenu::CreateAIControllersUI(GameSettings& gameSettings)
{
  CIwUIScrollableView* scrollArea;
  CIwUILayoutVertical* scrollLayout;
  CIwUILabel* blockArea = 0;
  CIwUILayoutVertical* blockLayout = 0;

  Options& options = mGameSettings.mOptions;
  AIControllersSettings& aics = mGameSettings.mAIControllersSettings;
  Language language = options.mLanguage;

  CreateContentAndButtonsAreaAndLayout(gameSettings, mTabPanels[TAB_AICONTROLLERS], scrollArea, scrollLayout, "AI Controllers", true);

  LABEL(TXT(PS_GENERALSETTINGS));
  CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);

  char txt[256];
  sprintf(txt, TXT(PS_MAXAI), options.mFreeFlightMaxAI);
  mMaxAILabel = LABEL_BLOCK(txt);

  if (aics.mAIControllers.size() > 0)
  {
    CHECKBOX_BLOCK(TXT(PS_INCLUDEINCAMERAVIEWS), aics.mIncludeInCameraViews);
    CHECKBOX_BLOCK(TXT(PS_CREATEMAXNUMCONTROLLERS), aics.mCreateMaxNumControllers);
    if (mTabPanels[TAB_AICONTROLLERS].mAdvancedEnabled)
    {
      SLIDER_FLOAT_BLOCK(TXT(PS_LAUNCHDIRECTION), NULL, aics.mLaunchDirection, -1.0f, 1.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_RANDOMCOLOUROFFSET), NULL, aics.mRandomColourOffset, 0.0f, 1.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_LAUNCHSEPARATIONDISTANCE), "m", aics.mLaunchSeparationDistance, 1.0f, 10.0f);
      CHECKBOX_BLOCK(TXT(PS_ENABLEDEBUGDRAW), aics.mEnableDebugDraw);
    }
  }

  mAddAIControllerButton = new CIwUIButton;
  mAddAIControllerButton->SetStyle("<button_wide>");
  mAddAIControllerButton->SetCaption(TXT(PS_ADDNEWAICONTROLLER));
  blockLayout->AddElement(mAddAIControllerButton);
  blockArea->AddChild(mAddAIControllerButton);

  if (aics.mAIControllers.size() > 0)
  {
    mRemoveAIControllersButton = new CIwUIButton;
    mRemoveAIControllersButton->SetStyle("<button_wide>");
    mRemoveAIControllersButton->SetCaption(TXT(PS_REMOVEAICONTROLLERS));
    blockLayout->AddElement(mRemoveAIControllersButton);
    blockArea->AddChild(mRemoveAIControllersButton);
  }

  for (size_t iAIController = 0 ; iAIController != aics.mAIControllers.size() ; ++iAIController)
  {
    AIControllersSettings::AIControllerSetting& aic = aics.mAIControllers[iAIController];

    LABEL(aic.mAeroplaneFile.c_str());
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);

    bool available = true;
    AeroplaneSettings as;
    if (!as.LoadFromFile(aic.mAeroplaneFile.c_str()))
    {
      available = false;
    }
    else
    {
      if (as.mAvailability > gameSettings.mStatistics.mAppType)
        available = false;
    }

    if (available)
    {
      SLIDER_FLOAT_BLOCK(TXT(PS_COLOUROFFSET), NULL, aic.mColourOffset, 0.0f, 1.0f);
      if (mTabPanels[TAB_AICONTROLLERS].mAdvancedEnabled)
      {
        CHECKBOX_BLOCK(TXT(PS_INCLUDEINCAMERAVIEWS), aic.mIncludeInCameraViews);
        CHECKBOX_BLOCK(TXT(PS_ENABLEDEBUGDRAW), aic.mEnableDebugDraw);

        CIwUIButton* copyButton = new CIwUIButton;
        copyButton->SetStyle("<button_wide>");
        copyButton->SetCaption(TXT(PS_COPY));
        blockLayout->AddElement(copyButton);
        blockArea->AddChild(copyButton);

        mCopyAIControllerButtons.push_back(copyButton);
      }
    }
    else
    {
      LABEL_BLOCK(TXT(PS_AVAILABLEINFULLVERSION));
    }

    CIwUIButton* deleteButton = new CIwUIButton;
    deleteButton->SetStyle("<button_wide>");
    deleteButton->SetCaption(TXT(PS_REMOVE));
    blockLayout->AddElement(deleteButton);
    blockArea->AddChild(deleteButton);

    mDeleteAIControllerButtons.push_back(deleteButton);
  }
}
//---------------------------------------------------------------------------------------------------------------------
void SettingsMenu::CreateAeroplaneUI(GameSettings& gameSettings)
{
  CIwUIScrollableView* scrollArea;
  CIwUILayoutVertical* scrollLayout;
  CIwUILabel* blockArea = 0;
  CIwUILayoutVertical* blockLayout = 0;

  Options& options = mGameSettings.mOptions;
  Language language = options.mLanguage;
  AeroplaneSettings& as = mGameSettings.mAeroplaneSettings;
  AIAeroplaneSettings& aias = as.mAIAeroplaneSettings;

  CreateContentAndButtonsAreaAndLayout(gameSettings, mTabPanels[TAB_AEROPLANE], scrollArea, scrollLayout, "aeroplane", gameSettings.mChallengeSettings.mAllowAeroplaneSettings);

  CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
  mAeroplaneButton = CreateSummaryAreaAndLayout(gameSettings, blockArea, blockLayout, as.mTitle, as.mThumbnail, as.mInfo, gameSettings.mChallengeSettings.mAllowAeroplaneSettings);

  if (!as.mWWW.empty())
  {
    mAeroplaneWWWButton = new CIwUIButton;
    mAeroplaneWWWButton->SetStyle("<button_expanding>");
    mAeroplaneWWWButton->SetCaption(as.mWWW.c_str());
    blockLayout->AddElement(mAeroplaneWWWButton);
    blockArea->AddChild(mAeroplaneWWWButton);
  }
  else
  {
    mAeroplaneWWWButton = 0;
  }

  mAeroplanePreferredControllerButton = 0;

  LABEL(TXT(PS_GENERALSETTINGS));
  CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
  {
    const int maxModels = 10;
    TiXmlDocument doc(as.mName + "/Aeroplane.xml");
    std::vector<std::string> modelNames;
    bool ok = doc.LoadFile();
    if (ok)
    {
      TiXmlHandle graphicsHandle = doc.FirstChild("Graphics");

      int iModel = 0;
      while (iModel < maxModels)
      {
        TiXmlElement* dataElement = graphicsHandle.Child("Model", iModel).ToElement();
        if (!dataElement)
          break;
        std::string name = readStringFromXML(dataElement, "name");
        modelNames.push_back(name);
        ++iModel;
      }

      if (iModel > 1)
      {
        SLIDER_STRING_BLOCK(TXT(PS_COLOURSCHEME), as.mColourScheme, modelNames);
      }
    }
  }
  SLIDER_FLOAT_BLOCK(TXT(PS_COLOUROFFSET), NULL, as.mColourOffset, 0.0f, 1.0f);
  SLIDER_FLOAT_BLOCK(TXT(PS_BALLAST), "%", as.mExtraMassPerCent, 0.0f, 100.0f);
  SLIDER_FLOAT_BLOCK(TXT(PS_BALLASTFWD),  "m", as.mExtraMassOffset.x, -1.0f, 1.0f);

  if (mTabPanels[TAB_AEROPLANE].mAdvancedEnabled && gameSettings.mChallengeSettings.mAllowAeroplaneSettings)
  {
    SLIDER_FLOAT_BLOCK(TXT(PS_BALLASTLEFT), "m", as.mExtraMassOffset.y, -1.0f, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_BALLASTUP),   "m", as.mExtraMassOffset.z, -1.0f, 1.0f);
    SLIDER_FLOAT_POWER_BLOCK(TXT(PS_DRAGMULTIPLIER), NULL, as.mDragScale, 0.2f, 4.0f, 2.0f);
    SLIDER_FLOAT_POWER_BLOCK(TXT(PS_SIZEMULTIPLIER), NULL, as.mSizeScale, 0.2f, 5.0f, 2.0f);
    SLIDER_FLOAT_POWER_BLOCK(TXT(PS_MASSMULTIPLIER), NULL, as.mMassScale, 0.2f, 5.0f, 2.0f);
    SLIDER_FLOAT_POWER_BLOCK(TXT(PS_ENGINEMULTIPLIER), NULL, as.mEngineScale, 0.2f, 5.0f, 2.0f);

    CHECKBOX_BLOCK(TXT(PS_SHOWBUTTON1), as.mShowButton[0]);
    CHECKBOX_BLOCK(TXT(PS_SHOWBUTTON2), as.mShowButton[1]);

    static const char* varioTypeDescs[3];
    varioTypeDescs[0] = TXT(PS_NO);
    varioTypeDescs[1] = TXT(PS_AUTO);
    varioTypeDescs[2] = TXT(PS_YES);
    SLIDER_ENUM_BLOCK(TXT(PS_HASVARIOMETER), *((int*) &as.mHasVariometer), varioTypeDescs, 3);
    char txt[256];
    strcpy(txt, as.mPreferredController.c_str());
    RemoveExtension(txt, ".xml");
    RemovePath(txt);
    mAeroplanePreferredControllerButton = BUTTON_BLOCK(TXT(PS_PREFERREDCONTROLLER), txt);

    LABEL(TXT(PS_LAUNCH));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    if (!(as.mType & 2)) 
    {
      static const char* flatLaunchMethodDescs[3];
      flatLaunchMethodDescs[0] = TXT(PS_HAND);
      flatLaunchMethodDescs[1] = TXT(PS_BUNGEE);
      flatLaunchMethodDescs[2] = TXT(PS_AEROTOW);
      SLIDER_ENUM_BLOCK(TXT(PS_FLATLAUNCHMETHOD), *((int*) &as.mFlatLaunchMethod), flatLaunchMethodDescs, 3);
    }
    SLIDER_FLOAT_BLOCK(TXT(PS_LAUNCHSPEED), "m/s", as.mLaunchSpeed, 0.0f, 40.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_LAUNCHANGLE), "deg", as.mLaunchAngleUp, -45.0f, 45.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_LAUNCHUP), "m", as.mLaunchUp, 0.0f, 10.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_LAUNCHFORWARD), "m", as.mLaunchForwards, -30.0f, 30.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_LAUNCHLEFT), "m", as.mLaunchLeft, -30.0f, 30.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_LAUNCHOFFSETUP), "m", as.mLaunchOffsetUp, -2.0f, 2.0f);
    CHECKBOX_BLOCK(TXT(PS_RELAUNCHWHENSTATIONARY), as.mRelaunchWhenStationary);

    LABEL(TXT(PS_HOOKS));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    if (!(as.mType & 2)) 
    {
      SLIDER_FLOAT_BLOCK(TXT(PS_BELLYHOOKOFFSETFORWARD), "m", as.mBellyHookOffset.x, -0.5f, 0.5f);
      //SLIDER_FLOAT_BLOCK(TXT(PS_BELLYHOOKOFFSETLEFT), "m", as.mBellyHookOffset.y, -0.5f, 0.5f);
      SLIDER_FLOAT_BLOCK(TXT(PS_BELLYHOOKOFFSETUP), "m", as.mBellyHookOffset.z, -1.0f, 0.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_NOSEHOOKOFFSETFORWARD), "m", as.mNoseHookOffset.x, -0.5f, 1.0f);
      //SLIDER_FLOAT_BLOCK(TXT(PS_NOSEHOOKOFFSETLEFT), "m", as.mNoseHookOffset.y, -0.5f, 0.5f);
      SLIDER_FLOAT_BLOCK(TXT(PS_NOSEHOOKOFFSETUP), "m", as.mNoseHookOffset.z, -1.0f, 1.0f);
    }
    else
    {
      SLIDER_FLOAT_BLOCK(TXT(PS_TAILHOOKOFFSETFORWARD), "m", as.mNoseHookOffset.x, -3.0f, 0.5f);
      //SLIDER_FLOAT_BLOCK(TXT(PS_TAILHOOKOFFSETLEFT), "m", as.mNoseHookOffset.y, -0.5f, 0.5f);
      SLIDER_FLOAT_BLOCK(TXT(PS_TAILHOOKOFFSETUP), "m", as.mNoseHookOffset.z, -1.0f, 1.0f);
    }

    // Only show bungee/aerotow for pure gliders
    if (!(as.mType & 2)) 
    {
      LABEL(TXT(PS_BUNGEELAUNCH));
      CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
      SLIDER_FLOAT_BLOCK(TXT(PS_MAXBUNGEELENGTH), "m", as.mMaxBungeeLength, 50.0f, 300.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_MAXBUNGEEACCEL), "m/s^2", as.mMaxBungeeAcceleration, 0.0f, 100.0f);

      LABEL(TXT(PS_AEROTOWLAUNCH));
      CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
      strcpy(txt, as.mTugName.c_str());
      RemoveExtension(txt, ".xml");
      RemovePath(txt);
      mTugButton = BUTTON_BLOCK(TXT(PS_TUGPLANE), txt);
      SLIDER_FLOAT_POWER_BLOCK(TXT(PS_TUGSIZESCALE), NULL, as.mTugSizeScale, 0.5f, 5.0f, 2.0f);
      SLIDER_FLOAT_POWER_BLOCK(TXT(PS_TUGMASSSCALE), NULL, as.mTugMassScale, 0.5f, 5.0f, 2.0f);
      SLIDER_FLOAT_POWER_BLOCK(TXT(PS_TUGENGINESCALE), NULL, as.mTugEngineScale, 0.5f, 5.0f, 2.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_TUGMAXCLIMBSLOPE), NULL, as.mTugMaxClimbSlope, 0.0f, 0.3f);
      SLIDER_FLOAT_BLOCK(TXT(PS_TUGTARGETSPEED), "m/s", as.mTugTargetSpeed, 2.0f, 40.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_AEROTOWROPELENGTH), "m", as.mAeroTowRopeLength, 2.0f, 50.0f);
      SLIDER_FLOAT_POWER_BLOCK(TXT(PS_AEROTOWROPESTRENGTH), NULL, as.mAeroTowRopeStrength, 1.0f, 100.0f, 2.0f);
      SLIDER_FLOAT_POWER_BLOCK(TXT(PS_AEROTOWROPEMASSSCALE), NULL, as.mAeroTowRopeMassScale, 0.2f, 5.0f, 2.0f);
      SLIDER_FLOAT_POWER_BLOCK(TXT(PS_AEROTOWROPEDRAGSCALE), NULL, as.mAeroTowRopeDragScale, 0.2f, 5.0f, 2.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_AEROTOWHEIGHT), "m", as.mAeroTowHeight, 2.0f, 400.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_AEROTOWCIRCUITSIZE), "m", as.mAeroTowCircuitSize, 10.0f, 500.0f);
    }

    LABEL(TXT(PS_CRASHDETECTION));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    SLIDER_FLOAT_POWER_BLOCK(TXT(PS_CRASHDELTAVELX), "m/s", as.mCrashDeltaVel.x, 0.0f, 100.0f, 4.0f);
    SLIDER_FLOAT_POWER_BLOCK(TXT(PS_CRASHDELTAVELY), "m/s", as.mCrashDeltaVel.y, 0.0f, 100.0f, 4.0f);
    SLIDER_FLOAT_POWER_BLOCK(TXT(PS_CRASHDELTAVELZ), "m/s", as.mCrashDeltaVel.z, 0.0f, 100.0f, 4.0f);
    SLIDER_FLOAT_POWER_BLOCK(TXT(PS_CRASHDELTAANGVELX), "deg/s", as.mCrashDeltaAngVel.x, 0.0f, 10000.0f, 4.0f);
    SLIDER_FLOAT_POWER_BLOCK(TXT(PS_CRASHDELTAANGVELY), "deg/s", as.mCrashDeltaAngVel.y, 0.0f, 10000.0f, 4.0f);
    SLIDER_FLOAT_POWER_BLOCK(TXT(PS_CRASHDELTAANGVELZ), "deg/s", as.mCrashDeltaAngVel.z, 0.0f, 10000.0f, 4.0f);
    SLIDER_FLOAT_POWER_BLOCK(TXT(PS_CRASHSUSPENSIONFORCESCALE), NULL, as.mCrashSuspensionForceScale, 0.1f, 100.0f, 4.0f);

    if (!(as.mType & 1)) 
    {
      LABEL(TXT(PS_TETHERING));
      CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
      SLIDER_INT_BLOCK(TXT(PS_TETHERLINES), NULL, as.mTetherLines, 0, 2);
      CHECKBOX_BLOCK(TXT(PS_TETHERREQUIRESTENSION), as.mTetherRequiresTension);
      SLIDER_FLOAT_BLOCK(TXT(PS_TETHERPHYSICSOFFSETFORWARD), "m", as.mTetherPhysicsOffset.x, -2.0f, 2.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_TETHERPHYSICSOFFSETLEFT),    "m", as.mTetherPhysicsOffset.y, -5.0f, 5.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_TETHERPHYSICSOFFSETUP),      "m", as.mTetherPhysicsOffset.z, -5.0f, 5.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_TETHERVISUALOFFSETFORWARD),  "m", as.mTetherVisualOffset.x, -2.0f, 2.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_TETHERVISUALOFFSETLEFT),     "m", as.mTetherVisualOffset.y, -5.0f, 5.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_TETHERVISUALOFFSETUP),       "m", as.mTetherVisualOffset.z, -5.0f, 5.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_TETHERDISTANCELEFT),         "m", as.mTetherDistanceLeft,   -30.0f, 30.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_TETHERCOLOURR), NULL, as.mTetherColour.x, 0.0f, 1.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_TETHERCOLOURG), NULL, as.mTetherColour.y, 0.0f, 1.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_TETHERCOLOURB), NULL, as.mTetherColour.z, 0.0f, 1.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_TETHERCOLOURA), NULL, as.mTetherColour.w, 0.0f, 1.0f);
    }

    LABEL(TXT(PS_CHASECAMERA));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    SLIDER_FLOAT_BLOCK(TXT(PS_CAMERATARGETPOSFWD), "m", as.mCameraTargetPosFwd, -2.0f, 2.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_CAMERATARGETPOSUP), "m", as.mCameraTargetPosUp, -2.0f, 2.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_DISTANCE), "m", as.mChaseCamDistance, 1.0f, 10.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_HEIGHT), "m", as.mChaseCamHeight, 0.0f, 3.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_VERTICALVELFRAC), NULL, as.mChaseCamVerticalVelMult, 0.0f, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_FLEXIBILITY), NULL, as.mChaseCamFlexibility, 0.0f, 1.0f);

    LABEL(TXT(PS_COCKPITCAMERA));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    SLIDER_FLOAT_BLOCK(TXT(PS_PITCH), NULL, as.mCockpitCamPitch, -90.0f, 90.0f);

    LABEL(TXT(PS_AICONTROLLER));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);

    static const char* planeTypeDescs[4];
    planeTypeDescs[0] = TXT(PS_GLIDER);
    planeTypeDescs[1] = TXT(PS_POWERED);
    planeTypeDescs[2] = TXT(PS_HELI);
    planeTypeDescs[3] = TXT(PS_CONTROLLINE);
    SLIDER_ENUM_BLOCK(TXT(PS_PLANETYPE), *((int*) &as.mAIType), planeTypeDescs, 4);
    CHECKBOX_BLOCK(TXT(PS_ALLOWAICONTROL), aias.mAllowAIControl);
    CHECKBOX_BLOCK(TXT(PS_CANTOW), aias.mCanTow);
    SLIDER_FLOAT_BLOCK(TXT(PS_WAYPOINTTOLERANCE),         "m",      aias.mWaypointTolerance,         0.1f, 50.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_MINSPEED),                  "m/s",    aias.mGliderControlMinSpeed,                  1.0f, 40.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_CRUISESPEED),               "m/s",    aias.mGliderControlCruiseSpeed,               1.0f, 50.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_MAXBANKANGLE),              "deg",    aias.mControlMaxBankAngle,              10.0f, 90.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_BANKANGLEPERHEADINGCHANGE), NULL,     aias.mControlBankAnglePerHeadingChange, 0.0f, 4.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_SPEEDPERALTITUDECHANGE),    "s^-1",   aias.mGliderControlSpeedPerAltitudeChange,    0.0f, 4.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_GLIDESLOPEPEREXCESSSPEED),  "s/m",    aias.mGliderControlSlopePerExcessSpeed,  0.0f, 0.2f);
    SLIDER_FLOAT_BLOCK(TXT(PS_PITCHPERROLLANGLE),         "deg^-1", aias.mControlPitchControlPerRollAngle,         0.0f, 0.02f);
    SLIDER_FLOAT_BLOCK(TXT(PS_HEADINGCHANGEFORNOSLOPE),   "deg",    aias.mGliderControlHeadingChangeForNoSlope,   0.0f, 360.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_MAXPITCHCONTROL),           NULL,     aias.mControlMaxPitchControl,           0.0f, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_MAXROLLCONTROL),            NULL,     aias.mControlMaxRollControl,            0.0f, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_CONTROLPERROLLANGLE),       "deg^-1", aias.mControlRollControlPerRollAngle,       0.0f, 0.05f);
    SLIDER_FLOAT_BLOCK(TXT(PS_PITCHCONTROLPERGLIDESLOPE), NULL,     aias.mGliderControlPitchControlPerGlideSlope, 0.0f, 10.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_ROLLTIMESCALE),             "s",      aias.mControlRollTimeScale,             0.0f, 0.5f);
    SLIDER_FLOAT_BLOCK(TXT(PS_PITCHTIMESCALE),            "s",      aias.mControlPitchTimeScale,            0.0f, 0.5f);

    LABEL(TXT(PS_AINAVIGATION));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    SLIDER_FLOAT_BLOCK(TXT(PS_MINALTITUDE),            "m", aias.mGliderControlMinAltitude,       -100.0f, 100.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_SLOPEMINUPWINDDISTANCE), "m", aias.mSlopeMinUpwindDistance, -500.0f, 500.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_SLOPEMAXUPWINDDISTANCE), "m", aias.mSlopeMaxUpwindDistance, -500.0f, 500.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_SLOPEMINLEFTDISTANCE),   "m", aias.mSlopeMinLeftDistance,   -500.0f, 500.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_SLOPEMAXLEFTDISTANCE),   "m", aias.mSlopeMaxLeftDistance,   -500.0f, 500.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_SLOPEMINUPDISTANCE),     "m", aias.mSlopeMinUpDistance,     -100.0f, 100.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_SLOPEMAXUPDISTANCE),     "m", aias.mSlopeMaxUpDistance,     -100.0f, 100.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_SLOPEMAXWAYPOINTTIME),   "s", aias.mSlopeMaxWaypointTime,     10.0f, 1000.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_FLATMAXDISTANCE),        "s", aias.mFlatMaxDistance,          10.0f, 1000.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_FLATMAXWAYPOINTTIME),    "s", aias.mFlatMaxWaypointTime,      10.0f, 1000.0f);
  }
  else
  {
    if (gameSettings.mChallengeSettings.mAllowAeroplaneSettings && !(as.mType & 2))
    {
      static const char* flatLaunchMethodDescs[3];
      flatLaunchMethodDescs[0] = TXT(PS_HAND);
      flatLaunchMethodDescs[1] = TXT(PS_BUNGEE);
      flatLaunchMethodDescs[2] = TXT(PS_AEROTOW);
      SLIDER_ENUM_BLOCK(TXT(PS_FLATLAUNCHMETHOD), *((int*) &as.mFlatLaunchMethod), flatLaunchMethodDescs, 3);
    }
    CHECKBOX_BLOCK(TXT(PS_SHOWBUTTON1), as.mShowButton[0]);
    CHECKBOX_BLOCK(TXT(PS_SHOWBUTTON2), as.mShowButton[1]);
    char txt[256];
    strcpy(txt, as.mPreferredController.c_str());
    RemoveExtension(txt, ".xml");
    RemovePath(txt);
    INFO_BLOCK(TXT(PS_PREFERREDCONTROLLER), txt);
  }


  for (int i = 0 ; i != AeroplaneSettings::MAX_NUM_SMOKES_PER_PLANE ; ++i)
  {
    char txt[128];
    sprintf(txt, "Smoke source %d", i);
    mSmokeColourLabels[i] = LABEL(txt);
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    CHECKBOX_BLOCK(TXT(PS_ENABLE), as.mSmokeSources[i].mEnable);
    mSmokeHSVs[i] = RGB2HSV(as.mSmokeSources[i].mColour);
    SLIDER_FLOAT_BLOCK(TXT(PS_COLOURH), NULL, mSmokeHSVs[i].x, 0, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_COLOURS), NULL, mSmokeHSVs[i].y, 0, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_COLOURV), NULL, mSmokeHSVs[i].z, 0, 1.0f);
    if (mTabPanels[TAB_AEROPLANE].mAdvancedEnabled)
    {
      static const char* channelDescs[Controller::MAX_CHANNELS+1];
      channelDescs[Controller::CHANNEL_AILERONS]  = TXT(PS_AILERONS);
      channelDescs[Controller::CHANNEL_ELEVATOR]  = TXT(PS_ELEVATOR);
      channelDescs[Controller::CHANNEL_RUDDER]    = TXT(PS_RUDDER);
      channelDescs[Controller::CHANNEL_THROTTLE]  = TXT(PS_THROTTLE);
      channelDescs[Controller::CHANNEL_LOOKYAW]   = TXT(PS_LOOKYAW);
      channelDescs[Controller::CHANNEL_LOOKPITCH] = TXT(PS_LOOKPITCH);
      channelDescs[Controller::CHANNEL_AUX1]      = TXT(PS_AUX1);
      channelDescs[Controller::CHANNEL_SMOKE1]     = TXT(PS_SMOKE1);
      channelDescs[Controller::CHANNEL_SMOKE2]     = TXT(PS_SMOKE2);
      channelDescs[Controller::CHANNEL_HOOK]       = TXT(PS_HOOK);
      channelDescs[Controller::MAX_CHANNELS]      = TXT(PS_CONSTANT);
      SLIDER_FLOAT_BLOCK(TXT(PS_POSITIONX), "m", as.mSmokeSources[i].mOffset.x, -5, 5);
      SLIDER_FLOAT_BLOCK(TXT(PS_POSITIONY), "m", as.mSmokeSources[i].mOffset.y, -5, 5);
      SLIDER_FLOAT_BLOCK(TXT(PS_POSITIONZ), "m", as.mSmokeSources[i].mOffset.z, -5, 5);
      SLIDER_FLOAT_BLOCK(TXT(PS_VELFWD), "m/s", as.mSmokeSources[i].mVel.x, -20, 20);
      SLIDER_FLOAT_BLOCK(TXT(PS_VELLEFT), "m/s", as.mSmokeSources[i].mVel.y, -20, 20);
      SLIDER_FLOAT_BLOCK(TXT(PS_VELUP), "m/s", as.mSmokeSources[i].mVel.z, -20, 20);
      SLIDER_INT_BLOCK(TXT(PS_MAXPARTICLES), NULL, as.mSmokeSources[i].mMaxParticles, 100, 10000);
      SLIDER_ENUM_BLOCK(TXT(PS_CHANNELFOROPACITY), *((int*) &as.mSmokeSources[i].mChannelForAlpha), channelDescs, Controller::MAX_CHANNELS+1);
      SLIDER_FLOAT_BLOCK(TXT(PS_MINOPACITY), NULL, as.mSmokeSources[i].mMinAlpha, -1, 1);
      SLIDER_FLOAT_BLOCK(TXT(PS_MAXOPACITY), NULL, as.mSmokeSources[i].mMaxAlpha, -1, 1);
      SLIDER_ENUM_BLOCK(TXT(PS_CHANNELFORRATE), *((int*) &as.mSmokeSources[i].mChannelForRate), channelDescs, Controller::MAX_CHANNELS+1);
      SLIDER_FLOAT_BLOCK(TXT(PS_MINRATE), "Hz", as.mSmokeSources[i].mMinRate, -90, 90.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_MAXRATE), "Hz", as.mSmokeSources[i].mMaxRate, -90, 90.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_INITIALSIZE), "m", as.mSmokeSources[i].mInitialSize, 0, 1.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_FINALSIZE), "m", as.mSmokeSources[i].mFinalSize, 0, 10.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_LIFESPAN), "sec", as.mSmokeSources[i].mLifetime, 1, 20.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_DAMPINGTIME), "sec", as.mSmokeSources[i].mDampingTime, 0, 2.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_JITTER), "m/s", as.mSmokeSources[i].mVelJitter, 0, 1.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_ENGINEWASH), NULL, as.mSmokeSources[i].mEngineWash, 0, 1.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_HUECYCLEFREQ), "Hz", as.mSmokeSources[i].mHueCycleFreq, 0, 2.0f);
    }
    else
    {
      SLIDER_FLOAT_BLOCK(TXT(PS_MAXOPACITY), NULL, as.mSmokeSources[i].mMaxAlpha, -1, 1);
    }
  }

  if (PicaSim::IsCreated())
  {
    const Aeroplane* aeroplane = PicaSim::GetInstance().GetPlayerAeroplane();
    if (aeroplane && aeroplane->GetPhysics())
    {
      LABEL(TXT(PS_INFO));
      CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
      char txt[256];
      float mass = aeroplane->GetPhysics()->GetMass();
      sprintf(txt, "%5.3f", mass);
      INFO_BLOCK(TXT(PS_MASS), txt);
      if (mTabPanels[TAB_AEROPLANE].mAdvancedEnabled)
      {
        Vector3 inertia = aeroplane->GetPhysics()->GetInertiaLocal();
        sprintf(txt, "%5.3f, %5.3f, %5.3f", inertia.x, inertia.y, inertia.z);
        INFO_BLOCK(TXT(PS_INERTIA), txt);

        float wingArea = aeroplane->GetPhysics()->GetTotalWingArea();
        if (wingArea > 0.0f)
        {
          sprintf(txt, "%5.2f", wingArea);
          INFO_BLOCK(TXT(PS_WINGAREA), txt);
        }

        Vector3 a, b;
        aeroplane->GetPhysics()->getLocalAABB(a, b);
        Vector3 extents = b - a;
        sprintf(txt, "%5.3f, %5.3f, %5.3f", extents.x, extents.y, extents.z);
        INFO_BLOCK(TXT(PS_EXTENTS), txt);

        Vector3 pos = aeroplane->GetTransform().GetTrans();
        sprintf(txt, "%5.3f, %5.3f, %5.3f", pos.x, pos.y, pos.z);
        INFO_BLOCK(TXT(PS_CURRENTPOSITION), txt);
      }
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
void SettingsMenu::CreateSceneryUI(GameSettings& gameSettings)
{
  EnvironmentSettings& es = mGameSettings.mEnvironmentSettings;
  AIEnvironmentSettings& aies = es.mAIEnvironmentSettings;
  TerrainSettings& ts = mGameSettings.mEnvironmentSettings.mTerrainSettings;
  Options& options = mGameSettings.mOptions;
  Language language = options.mLanguage;

  CIwUIScrollableView* scrollArea;
  CIwUILayoutVertical* scrollLayout;
  CIwUILabel* blockArea = 0;
  CIwUILayoutVertical* blockLayout = 0;
  CreateContentAndButtonsAreaAndLayout(gameSettings, mTabPanels[TAB_SCENERY], scrollArea, scrollLayout, "scenery", gameSettings.mChallengeSettings.mAllowEnvironmentSettings);

  CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
  mSceneryButton = CreateSummaryAreaAndLayout(gameSettings, blockArea, blockLayout, es.mTitle, es.mThumbnail, es.mInfo, gameSettings.mChallengeSettings.mAllowEnvironmentSettings);

  if (!es.mWWW.empty())
  {
    mSceneryWWWButton = new CIwUIButton;
    mSceneryWWWButton->SetStyle("<button_expanding>");
    mSceneryWWWButton->SetCaption(es.mWWW.c_str());
    blockLayout->AddElement(mSceneryWWWButton);
    blockArea->AddChild(mSceneryWWWButton);
  }
  else
  {
    mSceneryWWWButton = 0;
  }

  mPanoramaButton = 0;
  mImageButton = 0;  
  mSceneryObjectsButton = 0;

  if (gameSettings.mChallengeSettings.mAllowWindStrengthSetting)
  {
    LABEL(TXT(PS_WINDSETTINGS));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    SLIDER_FLOAT_BLOCK(TXT(PS_WINDSPEED), "m/s", es.mWindSpeed, 0.00001f, 20.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_WINDBEARING), "deg", es.mWindBearing, -180.0f, 180.0f);
  }
  else
  {
    LABEL(TXT(PS_WINDSETTINGS));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    char str[256];
    sprintf(str, "%5.2f m/s", es.mWindSpeed);
    INFO_BLOCK(TXT(PS_WINDSPEED), str);
    sprintf(str, "%5.0f deg", es.mWindBearing);
    INFO_BLOCK(TXT(PS_WINDBEARING), str);
  }

  if (!gameSettings.mChallengeSettings.mAllowEnvironmentSettings)
  {
    CIwUILabel* label = LABEL(TXT(PS_CANNOTMODIFYSCENERY));
    label->SetStyle("<label_centered>");
    return;
  }

  if (mTabPanels[TAB_SCENERY].mAdvancedEnabled)
  {
    CHECKBOX_BLOCK(TXT(PS_ALLOWBUNGEE), es.mAllowBungeeLaunch);
  }

  if (mTabPanels[TAB_SCENERY].mAdvancedEnabled)
  {
    SLIDER_FLOAT_BLOCK(TXT(PS_WINDGUSTTIME), "s", es.mWindGustTime, 15.0f, 300.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_WINDGUSTFRACTION), NULL, es.mWindGustAmplitudeFraction, 0.0f, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_WINDGUSTANGLE), "deg", es.mWindGustBearingAmplitude, 0.0f, 90.0f);
  }
  SLIDER_FLOAT_BLOCK(TXT(PS_TURBULENCEAMOUNT), NULL, es.mTurbulenceAmount, 0.0f, 5.0f);

  if (mTabPanels[TAB_SCENERY].mAdvancedEnabled)
  {
    SLIDER_FLOAT_BLOCK(TXT(PS_SURFACETURBULENCEAMOUNT), NULL, es.mSurfaceTurbulence, 0.0f, 2.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_SHEARTURBULENCEAMOUNT), NULL, es.mShearTurbulence, 0.0f, 2.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_DEADAIRTURBULENCE), NULL, es.mDeadAirTurbulence, 0.0f, 2.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_WINDLIFTSMOOTHING), NULL, es.mWindLiftSmoothing, 0.01f, 10.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_VERTICALWINDDECAYDISTANCE), NULL, es.mVerticalWindDecayDistance, 10.0f, 400.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_SEPARATIONTENDENCY), NULL, es.mSeparationTendency, 0.0f, 5.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_ROTORTENDENCY), NULL, es.mRotorTendency, 0.0f, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_BOUNDARYLAYERDEPTH), NULL, es.mBoundaryLayerDepth, 10.0f, 500.0f);
  }

  LABEL(TXT(PS_THERMALSETTINGS));
  CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
  SLIDER_FLOAT_BLOCK(TXT(PS_DENSITY), " per km^2", es.mThermalDensity, 0.0f, 50.0f);

  if (mTabPanels[TAB_SCENERY].mAdvancedEnabled)
  {
     SLIDER_FLOAT_BLOCK(TXT(PS_RANGE), NULL, es.mThermalRange, 100.0f, 10000.0f);
     SLIDER_FLOAT_BLOCK(TXT(PS_LIFESPAN), "s", es.mThermalAverageLifeSpan, 60.0f, 600.0f);
     SLIDER_FLOAT_BLOCK(TXT(PS_DEPTH), "m", es.mThermalAverageDepth, 10.0f, 500.0f);
     SLIDER_FLOAT_BLOCK(TXT(PS_CORERADIUS), "m", es.mThermalAverageCoreRadius, 20.0f, 200.0f);
     SLIDER_FLOAT_BLOCK(TXT(PS_DOWNDRAFTEXTENT), "m", es.mThermalAverageDowndraftRadius, 50.0f, 200.0f);
     SLIDER_FLOAT_BLOCK(TXT(PS_UPDRAFTSPEED), "m/s", es.mThermalAverageUpdraftSpeed, 0.0f, 10.0f);
     SLIDER_FLOAT_BLOCK(TXT(PS_ASCENTRATE), "m/s", es.mThermalAverageAscentRate, 0.0f, 5.0f);
     SLIDER_FLOAT_BLOCK(TXT(PS_THERMALEXPANSIONOVERLIFESPAN), NULL, es.mThermalExpansionOverLifespan, 1.0f, 5.0f);
  }

  LABEL(TXT(PS_RUNWAY));
  CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);

  static const char* runwayTypeDescs[3];
  runwayTypeDescs[0] = TXT(PS_NONE);
  runwayTypeDescs[1] = TXT(PS_RUNWAY);
  runwayTypeDescs[2] = TXT(PS_CIRCLE);
  SLIDER_ENUM_BLOCK(TXT(PS_RUNWAYTYPE), *((int*) &es.mRunwayType), runwayTypeDescs, 3);
  SLIDER_FLOAT_POWER_BLOCK(TXT(PS_RUNWAYLENGTH), "m", es.mRunwayLength, 10.0f, 200.0f, 4.0f);

  if (mTabPanels[TAB_SCENERY].mAdvancedEnabled)
  {
    SLIDER_FLOAT_BLOCK(TXT(PS_RUNWAYX), "m", es.mRunwayPosition.x, -1000.0f, 1000.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_RUNWAYY), "m", es.mRunwayPosition.y, -1000.0f, 1000.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_RUNWAYHEIGHT), "m",  es.mRunwayPosition.z, -1.0f, 5.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_RUNWAYANGLE), "deg", es.mRunwayAngle, 0.0f, 180.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_RUNWAYWIDTH), "m", es.mRunwayWidth, 1.0f, 20.0f);
  }

  if (mTabPanels[TAB_SCENERY].mAdvancedEnabled)
  {
    LABEL(TXT(PS_SURFACESETTINGS));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    SLIDER_FLOAT_BLOCK(TXT(PS_SURFACEROUGHNESS), "m", ts.mSurfaceRoughness, 0.0f, 0.05f);
    SLIDER_FLOAT_BLOCK(TXT(PS_SURFACEFRICTION), NULL, ts.mFriction, 0.0f, 3.0f);

    if (es.mTerrainSettings.mType == TerrainSettings::TYPE_MIDPOINT_DISPLACEMENT)
    {
      LABEL(TXT(PS_RANDOMTERRAINSETTINGS));
      CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);

      SLIDER_INT_BLOCK(TXT(PS_RANDOMSEED), NULL, ts.mMidpointDisplacementSeed, 0, 500);
      SLIDER_FLOAT_BLOCK(TXT(PS_DISPLACEMENTHEIGHT), "m", ts.mMidpointDisplacementHeight, 0.0f, 300.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_SMOOTHNESS), NULL, ts.mMidpointDisplacementRoughness, 0.9f, 1.3f);
      SLIDER_FLOAT_BLOCK(TXT(PS_EDGEHEIGHT), "m", ts.mMidpointDisplacementEdgeHeight, -200.0f, 0.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_UPWARDSBIAS), NULL, ts.mMidpointDisplacementUpwardsBias, 0.0f, 1.0f);
      SLIDER_INT_BLOCK(TXT(PS_FILTERITERATIONS), NULL, ts.mMidpointDisplacementFilterIterations, 0, 5);

      CHECKBOX_BLOCK(TXT(PS_DRAWPLAIN), ts.mRenderPlain);
      CHECKBOX_BLOCK(TXT(PS_COLLIDEWITHPLAIN), ts.mCollideWithPlain);

      SLIDER_FLOAT_BLOCK(TXT(PS_PLAININNERRADIUS), "m", ts.mPlainInnerRadius, 1000.0f, 50000.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_PLAINFOGDISTANCE), "m", ts.mPlainFogDistance, 1000.0f, 50000.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_PLAINHEIGHT), "m", ts.mPlainHeight, -200.0f, 200.0f);

      SLIDER_FLOAT_BLOCK(TXT(PS_TERRAINSIZE), "m", ts.mTerrainSize, 1000.0f, 10000.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_COASTENHANCEMENT), NULL, ts.mCoastEnhancement, 0.0f, 10.0f);
      SLIDER_INT_BLOCK(TXT(PS_TERRAINDETAIL), NULL, ts.mHeightmapDetail, 6, 9);
    }
    else if (es.mTerrainSettings.mType == TerrainSettings::TYPE_RIDGE)
    {
      LABEL(TXT(PS_RIDGETERRAINSETTINGS));
      CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);

      SLIDER_FLOAT_BLOCK(TXT(PS_HEIGHT), "m", ts.mRidgeHeight, 0.0f, 500.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_MAXHEIGHTFRACTION), NULL, ts.mRidgeMaxHeightFraction, 0.0f, 1.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_WIDTH), "m", ts.mRidgeWidth, 0.0f, 500.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_HEIGHTOFFSET), "m", ts.mRidgeEdgeHeight, -50.0f, 50.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_HORIZONTALVARIATION), "m", ts.mRidgeHorizontalVariation, -50.0f, 50.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_HORIZONTALWAVELENGTH), "m", ts.mRidgeHorizontalVariationWavelength, 50.0f, 2000.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_VERTICALVARIATION), NULL, ts.mRidgeVerticalVariationFraction, 0.0f, 1.0f);

      CHECKBOX_BLOCK(TXT(PS_DRAWPLAIN), ts.mRenderPlain);
      CHECKBOX_BLOCK(TXT(PS_COLLIDEWITHPLAIN), ts.mCollideWithPlain);

      SLIDER_FLOAT_BLOCK(TXT(PS_PLAININNERRADIUS), "m", ts.mPlainInnerRadius, 1000.0f, 50000.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_PLAINFOGDISTANCE), "m", ts.mPlainFogDistance, 1000.0f, 50000.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_PLAINHEIGHT), "m", ts.mPlainHeight, -200.0f, 200.0f);

      SLIDER_FLOAT_BLOCK(TXT(PS_TERRAINSIZE), "m", ts.mTerrainSize, 1000.0f, 10000.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_COASTENHANCEMENT), NULL, ts.mCoastEnhancement, 0.0f, 10.0f);
      SLIDER_INT_BLOCK(TXT(PS_TERRAINDETAIL), NULL, ts.mHeightmapDetail, 6, 9);
    }
    else if (es.mTerrainSettings.mType == TerrainSettings::TYPE_PANORAMA)
    {
#if 0
      LABEL(TXT(PS_PANORAMASETTINGS));
      CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
      mPanoramaButton = new CIwUIButton;
      mPanoramaButton->SetStyle("<button_wide>");
      mPanoramaButton->SetCaption(es.mTerrainSettings.mPanoramaName.c_str());
      blockLayout->AddElement(mPanoramaButton);
      blockArea->AddChild(mPanoramaButton);
#endif
    }
    else if (es.mTerrainSettings.mType == TerrainSettings::TYPE_PANORAMA_3D)
    {
      LABEL(TXT(PS_PANORAMA3DSETTINGS));
      CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
#if 0
      mPanoramaButton = new CIwUIButton;
      mPanoramaButton->SetStyle("<button_wide>");
      mPanoramaButton->SetCaption(es.mTerrainSettings.mPanoramaName.c_str());
      blockLayout->AddElement(mPanoramaButton);
      blockArea->AddChild(mPanoramaButton);
#endif
      CHECKBOX_BLOCK(TXT(PS_DRAWPLAIN), ts.mRenderPlain);
      CHECKBOX_BLOCK(TXT(PS_COLLIDEWITHPLAIN), ts.mCollideWithPlain);

      SLIDER_FLOAT_BLOCK(TXT(PS_PLAININNERRADIUS), "m", ts.mPlainInnerRadius, 1000.0f, 50000.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_PLAINFOGDISTANCE), "m", ts.mPlainFogDistance, 1000.0f, 50000.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_PLAINHEIGHT), "m", ts.mPlainHeight, -200.0f, 200.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_COASTENHANCEMENT), NULL, ts.mCoastEnhancement, 0.0f, 10.0f);
    }
    else if (es.mTerrainSettings.mType == TerrainSettings::TYPE_FILE_TERRAIN)
    {
      LABEL(TXT(PS_HEIGHTFIELDSETTINGS));
      CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
#if 0
      mImageButton = new CIwUIButton;
      mImageButton->SetStyle("<button_wide>");
      mImageButton->SetCaption(es.mTerrainSettings.mFileTerrainName.c_str());
      blockLayout->AddElement(mImageButton);
      blockArea->AddChild(mImageButton);
#endif
      SLIDER_FLOAT_BLOCK(TXT(PS_MINHEIGHT), "m", ts.mFileTerrainMinZ, -100.0f, 500.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_MAXHEIGHT), "m", ts.mFileTerrainMaxZ, -100.0f, 500.0f);

      CHECKBOX_BLOCK(TXT(PS_DRAWPLAIN), ts.mRenderPlain);
      CHECKBOX_BLOCK(TXT(PS_COLLIDEWITHPLAIN), ts.mCollideWithPlain);

      SLIDER_FLOAT_BLOCK(TXT(PS_PLAININNERRADIUS), "m", ts.mPlainInnerRadius, 1000.0f, 50000.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_PLAINFOGDISTANCE), "m", ts.mPlainFogDistance, 1000.0f, 50000.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_PLAINHEIGHT), "m", ts.mPlainHeight, -200.0f, 200.0f);
      SLIDER_FLOAT_BLOCK(TXT(PS_COASTENHANCEMENT), NULL, ts.mCoastEnhancement, 0.0f, 10.0f);
    }

    // AI
    LABEL(TXT(PS_AISCENERY));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    static const char* sceneTypeDescs[2];
    sceneTypeDescs[0] = TXT(PS_FLAT);
    sceneTypeDescs[1] = TXT(PS_SLOPE);
    SLIDER_ENUM_BLOCK(TXT(PS_SCENETYPE), *((int*) &aies.mSceneType), sceneTypeDescs, 2);

    LABEL(TXT(PS_OBJECTS));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    char txt[256];
    strcpy(txt, es.mObjectsSettingsFile.c_str());
    RemoveExtension(txt, ".xml");
    RemovePath(txt);
    mSceneryObjectsButton = BUTTON_BLOCK(TXT(PS_OBJECTSSETTINGS), txt);

    LABEL(TXT(PS_INFO));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    char posString[256];
    sprintf(posString, "%5.2f, %5.2f, %5.2f", es.mObserverPosition.x, es.mObserverPosition.y, es.mObserverPosition.z);
    INFO_BLOCK(TXT(PS_CURRENTVIEWPOSITION), posString);
    sprintf(posString, "%5.1f", es.mWindBearing);
    INFO_BLOCK("Current wind bearing", posString);
  }

}

//---------------------------------------------------------------------------------------------------------------------
void SettingsMenu::CreateObjectsUI(GameSettings& gameSettings)
{
  CIwUIScrollableView* scrollArea;
  CIwUILayoutVertical* scrollLayout;

  Options& options = mGameSettings.mOptions;
  ObjectsSettings& os = mGameSettings.mObjectsSettings;
  EnvironmentSettings& es = mGameSettings.mEnvironmentSettings;

  mResetObjectsButton = 0;

  Language language = mGameSettings.mOptions.mLanguage;
  CreateContentAndButtonsAreaAndLayout(gameSettings, mTabPanels[TAB_OBJECTS], scrollArea, scrollLayout, "Objects", true);

  CIwUILabel* blockArea = 0;
  CIwUILayoutVertical* blockLayout = 0;

  if (!gameSettings.mChallengeSettings.mAllowEnvironmentSettings)
  {
    CIwUILabel* label = LABEL(TXT(PS_CANNOTMODIFYSCENERY));
    label->SetStyle("<label_centered>");
    return;
  }

  char txt[256];

  LABEL(TXT(PS_GENERALSETTINGS));
  CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
  CHECKBOX_BLOCK(TXT(PS_ENABLEOBJECTEDITING), options.mEnableObjectEditing);
  CHECKBOX_BLOCK(TXT(PS_FORCEALLVISIBLE), os.mForceAllVisible);
  if (!os.mBoxes.empty() && PicaSim::IsCreated())
  {
    mResetObjectsButton = new CIwUIButton;
    mResetObjectsButton->SetStyle("<button_wide>");
    mResetObjectsButton->SetCaption(TXT(PS_RESETOBJECTS));
    blockLayout->AddElement(mResetObjectsButton);
    blockArea->AddChild(mResetObjectsButton);
  }


  LABEL(TXT(PS_SUMMARY));
  CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
  if (os.mBoxes.empty())
  {
    LABEL_BLOCK(" ");
    LABEL_BLOCK(TXT(PS_NOOBJECTS));
    LABEL_BLOCK(" ");
  }
  else
  {
    size_t numObjects, numStaticVisible, numStaticInvisible, numDynamicVisible;
    os.GetStats(numObjects, numStaticVisible, numStaticInvisible, numDynamicVisible);
    sprintf(txt, "%lu", (long unsigned) numObjects);
    INFO_BLOCK(TXT(PS_OBJECTSTOTAL), txt);
    sprintf(txt, "%lu", (long unsigned) numStaticVisible);
    INFO_BLOCK(TXT(PS_OBJECTSSTATICVISIBLE), txt);
    sprintf(txt, "%lu", (long unsigned) numStaticInvisible);
    INFO_BLOCK(TXT(PS_OBJECTSSTATICINVISIBLE), txt);
    sprintf(txt, "%lu", (long unsigned) numDynamicVisible);
    INFO_BLOCK(TXT(PS_OBJECTSDYNAMICVISIBLE), txt);

    for (size_t iObject = 0 ; iObject != numObjects ; ++iObject)
    {
      ObjectsSettings::Box& box = os.mBoxes[iObject];
      sprintf(txt, TXT(PS_OBJECTNUMBER), iObject);
      LABEL(txt);
      CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
      if (mTabPanels[TAB_OBJECTS].mAdvancedEnabled)
      {
        SLIDER_FLOAT_POWER_BLOCK(TXT(PS_WIDTH),  "m", box.mExtents.x, 0.01f, 500.0f, 4.0f);
        SLIDER_FLOAT_POWER_BLOCK(TXT(PS_DEPTH),  "m", box.mExtents.y, 0.01f, 500.0f, 4.0f);
        SLIDER_FLOAT_POWER_BLOCK(TXT(PS_HEIGHT), "m", box.mExtents.z, 0.01f, 500.0f, 4.0f);

        SLIDER_FLOAT_POWER_BLOCK(TXT(PS_POSITIONX), "m", box.mTM.t.x, -1000.0f, 1000.0f, 4.0f);
        SLIDER_FLOAT_POWER_BLOCK(TXT(PS_POSITIONY), "m", box.mTM.t.y, -1000.0f, 1000.0f, 4.0f);
        SLIDER_FLOAT_POWER_BLOCK(TXT(PS_POSITIONZ), "m", box.mTM.t.z, -1000.0f, 1000.0f, 4.0f);

        SLIDER_FLOAT_BLOCK(TXT(PS_COLOURR), NULL, box.mColour.x, 0.0f, 1.0f);
        SLIDER_FLOAT_BLOCK(TXT(PS_COLOURG), NULL, box.mColour.y, 0.0f, 1.0f);
        SLIDER_FLOAT_BLOCK(TXT(PS_COLOURB), NULL, box.mColour.z, 0.0f, 1.0f);
      }
      else
      {
        sprintf(txt, "%5.2f, %5.2f, %5.2f", box.mTM.t.x, box.mTM.t.y, box.mTM.t.z);
        INFO_BLOCK(TXT(PS_POSITION), txt);
        sprintf(txt, "%5.2f, %5.2f, %5.2f", box.mExtents.x, box.mExtents.y, box.mExtents.z);
        INFO_BLOCK(TXT(PS_SIZE), txt);
        sprintf(txt, "%5.2f, %5.2f, %5.2f", box.mColour.x, box.mColour.y, box.mColour.z);
        INFO_BLOCK(TXT(PS_COLOUR), txt);
      }
      SLIDER_FLOAT_POWER_BLOCK(TXT(PS_MASS), "kg", box.mMass, 0.0f, 100.0f, 4.0f);
      CHECKBOX_BLOCK(TXT(PS_VISIBLE), box.mVisible);
      CHECKBOX_BLOCK(TXT(PS_SHADOW), box.mShadow);
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
void SettingsMenu::CreateLightingUI(GameSettings& gameSettings)
{
  LightingSettings& ls = mGameSettings.mLightingSettings;
  Options& options = mGameSettings.mOptions;
  Language language = options.mLanguage;

  bool isPanorama = mGameSettings.mEnvironmentSettings.mTerrainSettings.mType == TerrainSettings::TYPE_PANORAMA;

  CIwUIScrollableView* scrollArea;
  CIwUILayoutVertical* scrollLayout;
  CIwUILabel* blockArea = 0;
  CIwUILayoutVertical* blockLayout = 0;
  CreateContentAndButtonsAreaAndLayout(gameSettings, mTabPanels[TAB_LIGHTING], scrollArea, scrollLayout, "lighting", 
    !isPanorama && gameSettings.mChallengeSettings.mAllowEnvironmentSettings);

  if (isPanorama)
  {
    CIwUILabel* label = LABEL(TXT(PS_NOLIGHTINGSETTINGS));
    label->SetStyle("<label_centered>");
    // Push the text up a bit
    label = LABEL("  ");
    label->SetStyle("<label_centered>");
    mLightingButton = 0;
  }
  else 
  {
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    mLightingButton = CreateSummaryAreaAndLayout(gameSettings, blockArea, blockLayout, ls.mTitle, ls.mThumbnail, ls.mInfo, gameSettings.mChallengeSettings.mAllowEnvironmentSettings);

    LABEL(TXT(PS_SETTINGS));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);

    if (gameSettings.mChallengeSettings.mAllowEnvironmentSettings)
      SLIDER_INT_BLOCK(TXT(PS_SUNBEARING), "deg", ls.mSunBearingOffset, 0, 360);

    if (gameSettings.mChallengeSettings.mAllowEnvironmentSettings)
      SLIDER_FLOAT_BLOCK(TXT(PS_THERMALACTIVITY), NULL, ls.mThermalActivity, 0.0f, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_TERRAINDARKNESS), NULL, ls.mGamma, 0.3f, 1.0f);
  }
}

//---------------------------------------------------------------------------------------------------------------------
void SettingsMenu::CreateControllerUI(GameSettings& gameSettings)
{
  ControllerSettings& cs = mGameSettings.mControllerSettings;
  Options& options = mGameSettings.mOptions;
  Language language = options.mLanguage;

  CIwUIScrollableView* scrollArea;
  CIwUILayoutVertical* scrollLayout;
  CIwUILabel* blockArea = 0;
  CIwUILayoutVertical* blockLayout = 0;
  CreateContentAndButtonsAreaAndLayout(gameSettings, mTabPanels[TAB_CONTROLLER], scrollArea, scrollLayout, "controller", true);

  {
    LABEL(TXT(PS_GENERALSETTINGS));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    CHECKBOX_BLOCK(TXT(PS_RESETALTSETTINGONLAUNCH), cs.mResetAltSettingOnLaunch);
    CHECKBOX_BLOCK(TXT(PS_TREATTHROTTLEASBRAKES), cs.mTreatThrottleAsBrakes);
    SLIDER_INT_BLOCK(TXT(PS_NUMCONFIGURATIONS), NULL, cs.mNumAltSettings, 1, ControllerSettings::CONTROLLER_MAX_NUM_ALT_SETTINGS);

    const size_t numControlDescs = ControllerSettings::CONTROLLER_NUM_CONTROLS+1;
    static const char* controlDescs[numControlDescs];
    controlDescs[ControllerSettings::CONTROLLER_STICK_ROLL] = TXT(PS_ROLLSTICK);
    controlDescs[ControllerSettings::CONTROLLER_STICK_PITCH] = TXT(PS_PITCHSTICK);
    controlDescs[ControllerSettings::CONTROLLER_STICK_YAW] = TXT(PS_YAWSTICK);
    controlDescs[ControllerSettings::CONTROLLER_STICK_SPEED] = TXT(PS_SPEEDSTICK);
    controlDescs[ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL] = TXT(PS_TILTHORIZONTAL);
    controlDescs[ControllerSettings::CONTROLLER_ACCEL_VERTICAL] = TXT(PS_TILTVERTICAL);
    controlDescs[ControllerSettings::CONTROLLER_ARROW_HORIZONTAL] = TXT(PS_ARROWSHORIZONTAL);
    controlDescs[ControllerSettings::CONTROLLER_ARROW_VERTICAL] = TXT(PS_ARROWSVERTICAL);
    controlDescs[ControllerSettings::CONTROLLER_CONSTANT] = TXT(PS_CONSTANT);
    controlDescs[ControllerSettings::CONTROLLER_BUTTON0] = TXT(PS_BUTTON0);
    controlDescs[ControllerSettings::CONTROLLER_BUTTON1] = TXT(PS_BUTTON1);
    controlDescs[ControllerSettings::CONTROLLER_BUTTON2] = TXT(PS_BUTTON2);
    controlDescs[ControllerSettings::CONTROLLER_NUM_CONTROLS] = TXT(PS_NONE);

    LABEL(TXT(PS_CONTROLSOURCES));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    SLIDER_ENUM_BLOCK(TXT(PS_AILERONS),  *((int*) &cs.mControlPerChannel[Controller::CHANNEL_AILERONS ]), controlDescs, numControlDescs);
    SLIDER_ENUM_BLOCK(TXT(PS_ELEVATOR),  *((int*) &cs.mControlPerChannel[Controller::CHANNEL_ELEVATOR ]), controlDescs, numControlDescs);
    SLIDER_ENUM_BLOCK(TXT(PS_RUDDER),    *((int*) &cs.mControlPerChannel[Controller::CHANNEL_RUDDER   ]), controlDescs, numControlDescs);
    SLIDER_ENUM_BLOCK(TXT(PS_THROTTLE),  *((int*) &cs.mControlPerChannel[Controller::CHANNEL_THROTTLE ]), controlDescs, numControlDescs);
    SLIDER_ENUM_BLOCK(TXT(PS_LOOKYAW),   *((int*) &cs.mControlPerChannel[Controller::CHANNEL_LOOKYAW  ]), controlDescs, numControlDescs);
    SLIDER_ENUM_BLOCK(TXT(PS_LOOKPITCH), *((int*) &cs.mControlPerChannel[Controller::CHANNEL_LOOKPITCH]), controlDescs, numControlDescs);
    SLIDER_ENUM_BLOCK(TXT(PS_AUX1),      *((int*) &cs.mControlPerChannel[Controller::CHANNEL_AUX1     ]), controlDescs, numControlDescs);
    SLIDER_ENUM_BLOCK(TXT(PS_SMOKE1),    *((int*) &cs.mControlPerChannel[Controller::CHANNEL_SMOKE1   ]), controlDescs, numControlDescs);
    SLIDER_ENUM_BLOCK(TXT(PS_SMOKE2),    *((int*) &cs.mControlPerChannel[Controller::CHANNEL_SMOKE2   ]), controlDescs, numControlDescs);
    SLIDER_ENUM_BLOCK(TXT(PS_HOOK),      *((int*) &cs.mControlPerChannel[Controller::CHANNEL_HOOK     ]), controlDescs, numControlDescs);
  }

  bool haveController[ControllerSettings::CONTROLLER_NUM_CONTROLS];
  for (int i = 0 ; i != ControllerSettings::CONTROLLER_NUM_CONTROLS ; ++i)
    haveController[i] = false;
  for (int i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
  {
    if (!haveController[cs.mControlPerChannel[i]])
      haveController[cs.mControlPerChannel[i]] = true;
  }

  for (int iAlt = 0 ; iAlt != cs.mNumAltSettings ; ++iAlt)
  {
    if (cs.mNumAltSettings > 1)
    {
      if (iAlt == 0)
      {
        // Add padding
        CIwUILabel* label = LABEL(" ");
        CIwUIColour invisible;
        invisible.r = invisible.g = invisible.b = invisible.a = 0;
        label->SetColour(invisible);
      }

      char txt[256];
      sprintf(txt, TXT(PS_SETTINGSFORCONTROLLER), iAlt + 1, cs.GetName(iAlt).c_str());
      CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
      LABEL_BLOCK(txt);
    }
    
    if (!mTabPanels[TAB_CONTROLLER].mAdvancedEnabled)
    {
      if (
        haveController[ControllerSettings::CONTROLLER_STICK_ROLL] ||
        haveController[ControllerSettings::CONTROLLER_STICK_PITCH] ||
        haveController[ControllerSettings::CONTROLLER_STICK_YAW] ||
        haveController[ControllerSettings::CONTROLLER_STICK_SPEED] ||
        haveController[ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL] ||
        haveController[ControllerSettings::CONTROLLER_ACCEL_VERTICAL] ||
        haveController[ControllerSettings::CONTROLLER_CONSTANT] ||
        haveController[ControllerSettings::CONTROLLER_BUTTON0] ||
        haveController[ControllerSettings::CONTROLLER_BUTTON1] ||
        haveController[ControllerSettings::CONTROLLER_BUTTON2]
       )
      {
        LABEL(TXT(PS_TRIMSETTINGS));
        CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
      }
      else
      {
        CIwUILabel* label = LABEL(TXT(PS_NOSIMPLESETTINGS));
        label->SetStyle("<label_centered>");
      }
    }

    static const char* controlClampDescs[ControllerSettings::CONTROL_CLAMP_MAX];
    controlClampDescs[ControllerSettings::CONTROL_CLAMP_NONE] = TXT(PS_NONE);
    controlClampDescs[ControllerSettings::CONTROL_CLAMP_POSITIVE] = TXT(PS_POSITIVE);
    controlClampDescs[ControllerSettings::CONTROL_CLAMP_NEGATIVE] = TXT(PS_NEGATIVE);

    if (haveController[ControllerSettings::CONTROLLER_STICK_ROLL])
    {
      if (mTabPanels[TAB_CONTROLLER].mAdvancedEnabled)
      {
        LABEL(TXT(PS_ROLLSTICKMOVEMENT));
        CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
        SLIDER_ENUM_BLOCK(TXT(PS_CLAMPING),  *((int*) &cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_ROLL, iAlt).mClamp), controlClampDescs, sizeof(controlClampDescs)/sizeof(controlClampDescs[0]));
        SLIDER_FLOAT_BLOCK(TXT(PS_EXPONENTIAL), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_ROLL, iAlt).mExponential, 0.5f, 3.0f);
        SLIDER_FLOAT_BLOCK(TXT(PS_SCALE), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_ROLL, iAlt).mScale, -2.0f, 2.0f);
        SLIDER_FLOAT_BLOCK(TXT(PS_TRIM), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_ROLL, iAlt).mTrim, -1.0f, 1.0f);
        CHECKBOX_BLOCK(TXT(PS_SPRING), cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_ROLL, iAlt).mAutoCentre);
      }
      else
      {
        SLIDER_FLOAT_BLOCK(TXT(PS_ROLLSTICK), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_ROLL, iAlt).mTrim, -1.0f, 1.0f);
      }
    }

    if (haveController[ControllerSettings::CONTROLLER_STICK_PITCH])
    {
      if (mTabPanels[TAB_CONTROLLER].mAdvancedEnabled)
      {
        LABEL(TXT(PS_PITCHSTICKMOVEMENT));
        CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
        SLIDER_ENUM_BLOCK(TXT(PS_CLAMPING),  *((int*) &cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_PITCH, iAlt).mClamp), controlClampDescs, sizeof(controlClampDescs)/sizeof(controlClampDescs[0]));
        SLIDER_FLOAT_BLOCK(TXT(PS_EXPONENTIAL), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_PITCH, iAlt).mExponential, 0.5f, 3.0f);
        SLIDER_FLOAT_BLOCK(TXT(PS_SCALE), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_PITCH, iAlt).mScale, -2.0f, 2.0f);
        SLIDER_FLOAT_BLOCK(TXT(PS_TRIM), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_PITCH, iAlt).mTrim, -1.0f, 1.0f);
        CHECKBOX_BLOCK(TXT(PS_SPRING), cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_PITCH, iAlt).mAutoCentre);
      }
      else
      {
        SLIDER_FLOAT_BLOCK(TXT(PS_PITCHSTICK), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_PITCH, iAlt).mTrim, -1.0f, 1.0f);
      }
    }

    if (haveController[ControllerSettings::CONTROLLER_STICK_YAW])
    {
      if (mTabPanels[TAB_CONTROLLER].mAdvancedEnabled)
      {
        LABEL(TXT(PS_YAWSTICKMOVEMENT));
        CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
        SLIDER_ENUM_BLOCK(TXT(PS_CLAMPING),  *((int*) &cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_YAW, iAlt).mClamp), controlClampDescs, sizeof(controlClampDescs)/sizeof(controlClampDescs[0]));
        SLIDER_FLOAT_BLOCK(TXT(PS_EXPONENTIAL), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_YAW, iAlt).mExponential, 0.5f, 3.0f);
        SLIDER_FLOAT_BLOCK(TXT(PS_SCALE), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_YAW, iAlt).mScale, -2.0f, 2.0f);
        SLIDER_FLOAT_BLOCK(TXT(PS_TRIM), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_YAW, iAlt).mTrim, -1.0f, 1.0f);
        CHECKBOX_BLOCK(TXT(PS_SPRING), cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_YAW, iAlt).mAutoCentre);
      }
      else
      {
        SLIDER_FLOAT_BLOCK(TXT(PS_YAWSTICK), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_YAW, iAlt).mTrim, -1.0f, 1.0f);
      }
    }

    if (haveController[ControllerSettings::CONTROLLER_STICK_SPEED])
    {
      if (mTabPanels[TAB_CONTROLLER].mAdvancedEnabled)
      {
        LABEL(TXT(PS_SPEEDSTICKMOVEMENT));
        CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
        SLIDER_ENUM_BLOCK(TXT(PS_CLAMPING),  *((int*) &cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_SPEED, iAlt).mClamp), controlClampDescs, sizeof(controlClampDescs)/sizeof(controlClampDescs[0]));
        SLIDER_FLOAT_BLOCK(TXT(PS_EXPONENTIAL), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_SPEED, iAlt).mExponential, 0.5f, 3.0f);
        SLIDER_FLOAT_BLOCK(TXT(PS_SCALE), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_SPEED, iAlt).mScale, -2.0f, 2.0f);
        SLIDER_FLOAT_BLOCK(TXT(PS_TRIM), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_SPEED, iAlt).mTrim, -1.0f, 1.0f);
        CHECKBOX_BLOCK(TXT(PS_SPRING), cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_SPEED, iAlt).mAutoCentre);
      }
      else
      {
        SLIDER_FLOAT_BLOCK(TXT(PS_SPEEDSTICK), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_STICK_SPEED, iAlt).mTrim, -1.0f, 1.0f);
      }
    }

    if (haveController[ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL])
    {
      if (mTabPanels[TAB_CONTROLLER].mAdvancedEnabled)
      {
        LABEL(TXT(PS_ACCELEROMETERROLLMOVEMENT));
        CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
        SLIDER_ENUM_BLOCK(TXT(PS_CLAMPING),  *((int*) &cs.GetControlSetting(ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL, iAlt).mClamp), controlClampDescs, sizeof(controlClampDescs)/sizeof(controlClampDescs[0]));
        SLIDER_FLOAT_BLOCK(TXT(PS_EXPONENTIAL), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL, iAlt).mExponential, 0.5f, 3.0f);
        SLIDER_FLOAT_BLOCK(TXT(PS_SCALE), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL, iAlt).mScale, -2.0f, 2.0f);
        SLIDER_FLOAT_BLOCK(TXT(PS_TRIM), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL, iAlt).mTrim, -0.5f, 0.5f);
        SLIDER_FLOAT_BLOCK(TXT(PS_TILTROLLSENSITIVITY), NULL, cs.mControllerAccelerometerXSensitivity, 0.0f, 1.0f);
      }
      else
      {
        SLIDER_FLOAT_BLOCK(TXT(PS_ACCELEROMETERROLL), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_ACCEL_HORIZONTAL, iAlt).mTrim, -1.0f, 1.0f);
      }
    }
    if (haveController[ControllerSettings::CONTROLLER_ACCEL_VERTICAL])
    {
      if (mTabPanels[TAB_CONTROLLER].mAdvancedEnabled)
      {
        LABEL(TXT(PS_ACCELEROMETERPITCHMOVEMENT));
        CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
        SLIDER_ENUM_BLOCK(TXT(PS_CLAMPING),  *((int*) &cs.GetControlSetting(ControllerSettings::CONTROLLER_ACCEL_VERTICAL, iAlt).mClamp), controlClampDescs, sizeof(controlClampDescs)/sizeof(controlClampDescs[0]));
        SLIDER_FLOAT_BLOCK(TXT(PS_EXPONENTIAL), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_ACCEL_VERTICAL, iAlt).mExponential, 0.5f, 3.0f);
        SLIDER_FLOAT_BLOCK(TXT(PS_SCALE), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_ACCEL_VERTICAL, iAlt).mScale, -2.0f, 2.0f);
        SLIDER_FLOAT_BLOCK(TXT(PS_TRIM), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_ACCEL_VERTICAL, iAlt).mTrim, -1.0f, 1.0f);
        SLIDER_FLOAT_BLOCK(TXT(PS_TILTPITCHSENSITIVITY), NULL, cs.mControllerAccelerometerYSensitivity, 0.0f, 1.0f);
        SLIDER_FLOAT_BLOCK(TXT(PS_TILTNEUTRALANGLE), "deg", cs.mControllerAccelerometerOffsetAngle, 0.0f, 90.0f);
      }
      else
      {
        SLIDER_FLOAT_BLOCK(TXT(PS_ACCELEROMETERPITCH), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_ACCEL_VERTICAL, iAlt).mTrim, -1.0f, 1.0f);
      }
    }

    if (haveController[ControllerSettings::CONTROLLER_CONSTANT])
    {
      if (mTabPanels[TAB_CONTROLLER].mAdvancedEnabled)
      {
        LABEL(TXT(PS_CONSTANT));
        CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
        SLIDER_FLOAT_BLOCK(TXT(PS_TRIM), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_CONSTANT, iAlt).mTrim, -1.0f, 1.0f);
      }
      else
      {
        SLIDER_FLOAT_BLOCK(TXT(PS_CONSTANT), NULL, cs.GetControlSetting(ControllerSettings::CONTROLLER_CONSTANT, iAlt).mTrim, -1.0f, 1.0f);
      }
    }

    LABEL(TXT(PS_MIXES));
    CreateBlockAreaAndLayout(gameSettings, blockArea, blockLayout, scrollArea, scrollLayout);
    ControllerSettings::Mix& mix = cs.GetMix(iAlt);
    SLIDER_FLOAT_BLOCK(TXT(PS_ELEVATORTOFLAPS), NULL,  mix.mMixElevatorToFlaps, -3.0f, 3.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_AILERONTORUDDER), NULL,  mix.mMixAileronToRudder, -1.0f, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_FLAPSTOELEVATOR), NULL,  mix.mMixFlapsToElevator, -1.0f, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_BRAKESTOELEVATOR), NULL, mix.mMixBrakesToElevator, -1.0f, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_RUDDERTOELEVATOR), NULL, mix.mMixRudderToElevator, -1.0f, 1.0f);
    SLIDER_FLOAT_BLOCK(TXT(PS_RUDDERTOAILERON), NULL,  mix.mMixRudderToAileron, -1.0f, 1.0f);

    if (cs.mNumAltSettings > 1)
    {
      // Add padding
      CIwUILabel* label = LABEL(" ");
      CIwUIColour invisible;
      invisible.r = invisible.g = invisible.b = invisible.a = 0;
      label->SetColour(invisible);
    }
  }

  LABEL(TXT(PS_CONTROLLERSEESETTINGS))->SetStyle("<label_centered>");
}


//---------------------------------------------------------------------------------------------------------------------
SettingsMenu::~SettingsMenu()
{
  for (uint16 i = 0 ; i != TAB_NUM_TABS ; ++i)
  {
    if (i != mSelectedTab)
    {
      delete mTabPanels[i].mContentArea;
      delete mTabPanels[i].mButtonsArea;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
bool SettingsMenu::HandleEvent(CIwEvent* pEvent)
{
  CIwManaged* sender = pEvent->GetSender();
  const uint32 eventID = pEvent->GetID();

  if (eventLabel)
  {
    TRACE("Event ID = %d", eventID);
    switch (eventID)
    {
    case IWUI_EVENT_KEY: eventLabel->SetCaption("KEY"); break;
    case IWUI_EVENT_CHAR: eventLabel->SetCaption("CHAR"); break;
    case IWUI_EVENT_CLICK: eventLabel->SetCaption("CLICK"); break;
    case IWUI_EVENT_FOCUS: eventLabel->SetCaption("FOCUS"); break;
    case IWUI_EVENT_BUTTON: eventLabel->SetCaption("BUTTON"); break;
    case IWUI_EVENT_SOFTKEY: eventLabel->SetCaption("SOFTKEY"); break;
    case IWUI_EVENT_TEXT_INPUT: eventLabel->SetCaption("TEXT_INPUT"); break;
    default: 
      {
        char txt[256];
        sprintf(txt, "event %d", eventID);
        eventLabel->SetCaption(txt); 
        break;
      }
    }
  }

  if (eventID == IWUI_EVENT_BUTTON)
  {
    if (sender == mBackButton)
    {
      mStatus = SETTINGS_BACK;
      return true;
    }
    else if (sender == mPanoramaButton)
    {
      mStatus = SETTINGS_SELECT_PANORAMA;
      return true;
    }
    else if (sender == mImageButton)
    {
      mStatus = SETTINGS_SELECT_FILE_TERRAIN;
      return true;
    }
    else if (sender == mAeroplanePreferredControllerButton)
    {
      mStatus = SETTINGS_SELECT_PREFERRED_CONTROLLER;
      return true;
    }
    else if (sender == mSceneryObjectsButton)
    {
      mStatus = SETTINGS_SELECT_OBJECTS_SETTINGS;
      return true;
    }
    else if (sender == mLanguageButton)
    {
      mStatus = SETTINGS_SELECT_LANGUAGE;
      return true;
    }
    else if (sender == mAeroplaneWWWButton)
    {
      std::string url("http://");
      url += mAeroplaneWWWButton->GetCaption();
      s3eOSExecExecute(url.c_str(), false);
      return true;
    }
    else if (sender == mSceneryWWWButton)
    {
      std::string url("http://");
      url += mSceneryWWWButton->GetCaption();
      s3eOSExecExecute(url.c_str(), false);
      return true;
    }
    else if (sender == mDeleteLocalHighscoresButton)
    {
      mStatus = SETTINGS_DELETE_LOCAL_HIGHSCORES;
      return true;
    }
    else if (sender == mClearAllSettingsButton)
    {
      mStatus = SETTINGS_CLEARALLSAVEDSETTINGSANDEXIT;
      return true;
    }
    else if (sender == mResetObjectsButton)
    {
      mStatus = SETTINGS_RESET_OBJECTS;
      return true;
    }
    else if (sender == mAddAIControllerButton)
    {
      mStatus = SETTINGS_ADD_AICONTROLLER;
      return true;
    }
    else if (sender == mRemoveAIControllersButton)
    {
      mStatus = SETTINGS_REMOVE_AICONTROLLERS;
      return true;
    }
    else if (sender == mTugButton)
    {
      mStatus = SETTINGS_LOAD_TUG;
      return true;
    }
    else if (sender == mClearJoystickButton)
    {
      mStatus = SETTINGS_CLEAR_JOYSTICK;
      return true;
    }
    else if (sender == mCalibrateJoystickButton)
    {
      mStatus = SETTINGS_CALIBRATE_JOYSTICK;
      return true;
    }
    else if (sender == mTabPanels[TAB_OPTIONS1].mLoadButton)
    {
      mStatus = SETTINGS_LOAD_OPTIONS;
      return true;
    }
    else if (sender == mTabPanels[TAB_OPTIONS1].mSaveButton)
    {
      mStatus = SETTINGS_SAVE_OPTIONS;
      return true;
    }
    else if (sender == mTabPanels[TAB_OPTIONS1].mDeleteButton)
    {
      mStatus = SETTINGS_DELETE_OPTIONS;
      return true;
    }
    else if (sender == mTabPanels[TAB_OPTIONS1].mToggleAdvancedButton)
    {
      mTabPanels[TAB_OPTIONS1].mAdvancedEnabled = !mTabPanels[TAB_OPTIONS1].mAdvancedEnabled;
      mStatus = SETTINGS_REFRESH;
      return true;
    }
    else if (sender == mTabPanels[TAB_OPTIONS2].mLoadButton)
    {
      mStatus = SETTINGS_LOAD_OPTIONS;
      return true;
    }
    else if (sender == mTabPanels[TAB_OPTIONS2].mSaveButton)
    {
      mStatus = SETTINGS_SAVE_OPTIONS;
      return true;
    }
    else if (sender == mTabPanels[TAB_OPTIONS2].mDeleteButton)
    {
      mStatus = SETTINGS_DELETE_OPTIONS;
      return true;
    }
    else if (sender == mTabPanels[TAB_OPTIONS2].mToggleAdvancedButton)
    {
      mTabPanels[TAB_OPTIONS2].mAdvancedEnabled = !mTabPanels[TAB_OPTIONS2].mAdvancedEnabled;
      mStatus = SETTINGS_REFRESH;
      return true;
    }
    else if (sender == mTabPanels[TAB_AEROPLANE].mLoadButton || sender == mAeroplaneButton)
    {
      mStatus = SETTINGS_LOAD_AEROPLANE;
      return true;
    }
    else if (sender == mTabPanels[TAB_AEROPLANE].mSaveButton)
    {
      mStatus = SETTINGS_SAVE_AEROPLANE;
      return true;
    }
    else if (sender == mTabPanels[TAB_AEROPLANE].mDeleteButton)
    {
      mStatus = SETTINGS_DELETE_AEROPLANE;
      return true;
    }
    else if (sender == mTabPanels[TAB_AEROPLANE].mToggleAdvancedButton)
    {
      mTabPanels[TAB_AEROPLANE].mAdvancedEnabled = !mTabPanels[TAB_AEROPLANE].mAdvancedEnabled;
      mStatus = SETTINGS_REFRESH;
      return true;
    }
    else if (sender == mTabPanels[TAB_AICONTROLLERS].mLoadButton)
    {
      mStatus = SETTINGS_LOAD_AICONTROLLERS;
      return true;
    }
    else if (sender == mTabPanels[TAB_AICONTROLLERS].mSaveButton)
    {
      mStatus = SETTINGS_SAVE_AICONTROLLERS;
      return true;
    }
    else if (sender == mTabPanels[TAB_AICONTROLLERS].mDeleteButton)
    {
      mStatus = SETTINGS_DELETE_AICONTROLLERS;
      return true;
    }
    else if (sender == mTabPanels[TAB_AICONTROLLERS].mToggleAdvancedButton)
    {
      mTabPanels[TAB_AICONTROLLERS].mAdvancedEnabled = !mTabPanels[TAB_AICONTROLLERS].mAdvancedEnabled;
      mStatus = SETTINGS_REFRESH;
      return true;
    }
    else if (sender == mTabPanels[TAB_SCENERY].mLoadButton || sender == mSceneryButton)
    {
      mStatus = SETTINGS_LOAD_ENVIRONMENT;
      return true;
    }
    else if (sender == mTabPanels[TAB_SCENERY].mSaveButton)
    {
      mStatus = SETTINGS_SAVE_ENVIRONMENT;
      return true;
    }
    else if (sender == mTabPanels[TAB_SCENERY].mDeleteButton)
    {
      mStatus = SETTINGS_DELETE_ENVIRONMENT;
      return true;
    }
    else if (sender == mTabPanels[TAB_SCENERY].mToggleAdvancedButton)
    {
      mTabPanels[TAB_SCENERY].mAdvancedEnabled = !mTabPanels[TAB_SCENERY].mAdvancedEnabled;
      mStatus = SETTINGS_REFRESH;
      return true;
    }
    else if (sender == mTabPanels[TAB_OBJECTS].mLoadButton)
    {
      mStatus = SETTINGS_LOAD_OBJECTS;
      return true;
    }
    else if (sender == mTabPanels[TAB_OBJECTS].mSaveButton)
    {
      mStatus = SETTINGS_SAVE_OBJECTS;
      return true;
    }
    else if (sender == mTabPanels[TAB_OBJECTS].mDeleteButton)
    {
      mStatus = SETTINGS_DELETE_OBJECTS;
      return true;
    }
    else if (sender == mTabPanels[TAB_OBJECTS].mToggleAdvancedButton)
    {
      mTabPanels[TAB_OBJECTS].mAdvancedEnabled = !mTabPanels[TAB_OBJECTS].mAdvancedEnabled;
      mStatus = SETTINGS_REFRESH;
      return true;
    }
    else if (sender == mTabPanels[TAB_LIGHTING].mLoadButton || sender == mLightingButton)
    {
      mStatus = SETTINGS_LOAD_LIGHTING;
      return true;
    }
    else if (sender == mTabPanels[TAB_LIGHTING].mSaveButton)
    {
      mStatus = SETTINGS_SAVE_LIGHTING;
      return true;
    }
    else if (sender == mTabPanels[TAB_LIGHTING].mDeleteButton)
    {
      mStatus = SETTINGS_DELETE_LIGHTING;
      return true;
    }
    else if (sender == mTabPanels[TAB_LIGHTING].mToggleAdvancedButton)
    {
      mTabPanels[TAB_LIGHTING].mAdvancedEnabled = !mTabPanels[TAB_LIGHTING].mAdvancedEnabled;
      mStatus = SETTINGS_REFRESH;
      return true;
    }
    else if (sender == mTabPanels[TAB_CONTROLLER].mLoadButton)
    {
      mStatus = SETTINGS_LOAD_CONTROLLER;
      return true;
    }
    else if (sender == mTabPanels[TAB_CONTROLLER].mSaveButton)
    {
      mStatus = SETTINGS_SAVE_CONTROLLER;
      return true;
    }
    else if (sender == mTabPanels[TAB_CONTROLLER].mDeleteButton)
    {
      mStatus = SETTINGS_DELETE_CONTROLLER;
      return true;
    }
    else if (sender == mTabPanels[TAB_CONTROLLER].mToggleAdvancedButton)
    {
      mTabPanels[TAB_CONTROLLER].mAdvancedEnabled = !mTabPanels[TAB_CONTROLLER].mAdvancedEnabled;
      mStatus = SETTINGS_REFRESH;
      return true;
    }
    else if (sender == mTabPanels[TAB_JOYSTICK].mLoadButton)
    {
      mStatus = SETTINGS_LOAD_JOYSTICK;
      return true;
    }
    else if (sender == mTabPanels[TAB_JOYSTICK].mSaveButton)
    {
      mStatus = SETTINGS_SAVE_JOYSTICK;
      return true;
    }
    else if (sender == mTabPanels[TAB_JOYSTICK].mDeleteButton)
    {
      mStatus = SETTINGS_DELETE_JOYSTICK;
      return true;
    }
    else if (sender == mTabPanels[TAB_JOYSTICK].mToggleAdvancedButton)
    {
      mTabPanels[TAB_JOYSTICK].mAdvancedEnabled = !mTabPanels[TAB_JOYSTICK].mAdvancedEnabled;
      mStatus = SETTINGS_REFRESH;
      return true;
    }
    else
    {
      for (size_t iDelete = 0 ; iDelete != mDeleteAIControllerButtons.size() ; ++iDelete)
      {
        if (sender == mDeleteAIControllerButtons[iDelete])
        {
          mGameSettings.mAIControllersSettings.mAIControllers.erase(mGameSettings.mAIControllersSettings.mAIControllers.begin() + iDelete);
          mStatus = SETTINGS_REFRESH;
          return true;
        }
      }

      for (size_t iCopy = 0 ; iCopy != mCopyAIControllerButtons.size() ; ++iCopy)
      {
        if (sender == mCopyAIControllerButtons[iCopy])
        {
          AIControllersSettings::AIControllerSetting copy = mGameSettings.mAIControllersSettings.mAIControllers[iCopy];
          mGameSettings.mAIControllersSettings.mAIControllers.insert(mGameSettings.mAIControllersSettings.mAIControllers.begin() + iCopy, copy);
          mStatus = SETTINGS_REFRESH;
          return true;
        }
      }

      JoystickData joystick;
      if (S3E_RESULT_SUCCESS == GetJoystickStatus(joystick, mGameSettings.mOptions.mJoystickID))
      {
        JoystickSettings& js = mGameSettings.mJoystickSettings;
        for (size_t i = 0 ; i != JoystickSettings::JOYSTICK_NUM_CONTROLS ; ++i)
        {
          JoystickSettings::JoystickAnalogueOverride& j = js.mJoystickAnalogueOverrides[i];
          float input = -1.0f + 2.0f * joystick.mAnalogueInputs[i] / 65535.0f;

          float output = input + j.mOffset;
          if (output > -j.mDeadZone && output < j.mDeadZone)
            output = 0.0f;
          else if (output > j.mDeadZone)
            output = (output - j.mDeadZone) / (1.0f - j.mDeadZone);
          else
            output = (output + j.mDeadZone) / (1.0f - j.mDeadZone);

          if (sender == mJoystickZeroButtons[i])
          {
            j.mOffset = -input;
          }
          else if (sender == mJoystickMinusButtons[i])
          {
            if (output < 0.0f)
              j.mScaleNegative = -1.0f / output;
            else if (output > 0.0f)
              j.mScalePositive = -1.0f / output;
          }
          else if (sender == mJoystickPlusButtons[i])
          {
            if (output > 0.0f)
              j.mScalePositive = 1.0f / output;
            else if (output < 0.0f)
              j.mScaleNegative = 1.0f / output;
          }
        }
      }
    }
  }
  else if (eventID == IWUI_EVENT_TABBAR)
  {
    if (sender == mTabBar)
    {
      if (mUpdateCounter > 10)
      {
        // remove
        mGridArea->RemoveChild(mTabPanels[mSelectedTab].mContentArea);
        mGridLayout->RemoveElement(mTabPanels[mSelectedTab].mContentArea);
        mGridArea->RemoveChild(mTabPanels[mSelectedTab].mButtonsArea);
        mGridLayout->RemoveElement(mTabPanels[mSelectedTab].mButtonsArea);

        mSelectedTab = mTabBar->GetSelected();

        mGridArea->AddChild(mTabPanels[mSelectedTab].mContentArea);
        mGridLayout->AddElement(mTabPanels[mSelectedTab].mContentArea, 0, 1);
        mGridArea->AddChild(mTabPanels[mSelectedTab].mButtonsArea);
        mGridLayout->AddElement(mTabPanels[mSelectedTab].mButtonsArea, 0, 2);

        IwGetUIView()->Update(1);
      }
      else
      {
        // Workaround for bogus events where the tab gets selected even though it hasn't previously had a down event
        mTabBar->SetSelected(mSelectedTab);
      }
      return true;
    }
  }
  else if (eventID == IWUI_EVENT_SLIDER)
  {
    for (size_t i = 0 ; i != mIntSliders.size() ; ++i)
    {
      IntSlider& slider = mIntSliders[i];
      if (sender == slider.mElements.mSlider)
      {
        slider.SetFromSlider();
        return true;
      }
    }
    for (size_t i = 0 ; i != mEnums.size() ; ++i)
    {
      Enum& slider = mEnums[i];
      if (sender == slider.mElements.mSlider)
      {
        slider.SetFromSlider();
        return true;
      }
    }
    for (size_t i = 0 ; i != mStrings.size() ; ++i)
    {
      String& slider = mStrings[i];
      if (sender == slider.mElements.mSlider)
      {
        slider.SetFromSlider();
        return true;
      }
    }
    for (size_t i = 0 ; i != mFloatSliders.size() ; ++i)
    {
      FloatSlider& slider = mFloatSliders[i];
      if (sender == slider.mElements.mSlider)
      {
        slider.SetFromSlider();
        return true;
      }
    }
  }
  else if (eventID == IWUI_EVENT_CHECKBOX)
  {
    for (size_t i = 0 ; i != mCheckboxes.size() ; ++i)
    {
      Checkbox& checkbox = mCheckboxes[i];
      if (sender == checkbox.mElements.mCheckbox)
      {
        checkbox.SetFromCheckbox();
        return true;
      }
    }
  }
  return false;
}


//---------------------------------------------------------------------------------------------------------------------
void DisplaySettingsMenu(GameSettings& gameSettings, SettingsChangeActions& actions)
{
  TRACE_FUNCTION_ONLY(1);

  AudioManager::GetInstance().SetAllChannelsToZeroVolume();
  FixStateForMenus fixStateForMenus(true, true);

  GameSettings origSettings = gameSettings;

  SettingsStatus settingsStatus = SETTINGS_UNSET;

  int imageButtonXPos = 0;
  int imageButtonYPos = 0;
  int imageButtonWidth = 1;
  int imageButtonHeight = 1;

  while (true)
  {
    {
      SettingsMenu* settingsMenu = new SettingsMenu(gameSettings); // Gets deleted on IwGetUIView()->DestroyElements()

      while (true)
      {
        int origControllerNumAltSettings = gameSettings.mControllerSettings.mNumAltSettings;
        bool resourcesHaveChanged;
        settingsMenu->Update(gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_ANDROID, true, resourcesHaveChanged, gameSettings);

        if (resourcesHaveChanged)
        {
          settingsStatus = SETTINGS_REFRESH;
          break;
        }

        settingsMenu->GetImageButtonInfo(imageButtonXPos, imageButtonYPos, imageButtonWidth, imageButtonHeight);

        if (s3eDeviceCheckQuitRequest()!=0)
        {
          IwGetUIView()->DestroyElements();
          return;
        }

        if (
          settingsMenu->mStatus == SETTINGS_BACK ||
          (s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED)
          )
        {
          actions = gameSettings.GetSettingsChangeActions(actions, origSettings);
          IwGetUIView()->DestroyElements();
          return;
        }

        if (settingsMenu->mStatus != SETTINGS_UNSET)
        {
          settingsStatus = settingsMenu->mStatus;
          break;
        }

        if (
          origControllerNumAltSettings != gameSettings.mControllerSettings.mNumAltSettings ||
          settingsMenu->mStatus == SETTINGS_REFRESH
          )
        {
          // Just reform the UI
          settingsStatus = SETTINGS_UNSET;
          break;
        }
        AudioManager::GetInstance().Update(1.0f/30.0f);
      }
      IwGetUIView()->DestroyElements();
    } // Scope for SettingsMenu

    Language language = gameSettings.mOptions.mLanguage;

    if (settingsStatus == SETTINGS_LOAD_OPTIONS)
    {
      std::string file;
      FileMenuLoad(file, gameSettings, "SystemSettings/Options", "UserSettings/Options", ".xml", TXT(PS_LOADOPTIONS), 0, 0, TXT(PS_BACK), NULL);
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Loading Options %s - ", file.c_str());
        bool result = gameSettings.mOptions.LoadFromFile(file);
        IwAssert(ROWLHOUSE, result);
        TRACE_FILE_IF(1) TRACE(" %s\n", result ? "success" : "failed");
        gameSettings.mStatistics.mLoadedOptions = true;
      }
    }
    else if (settingsStatus == SETTINGS_SAVE_OPTIONS)
    {
      std::string file;
      FileMenuSave(file, gameSettings, "UserSettings/Options", ".xml", TXT(PS_SAVEOPTIONS));
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Saving Options %s - ", file.c_str());
        gameSettings.mOptions.SaveToFile(file);
      }
    }
    else if (settingsStatus == SETTINGS_DELETE_OPTIONS)
    {
      FileMenuDelete(gameSettings, "UserSettings/Options", ".xml", TXT(PS_DELETEOPTIONS));
    }
    else if (settingsStatus == SETTINGS_LOAD_ENVIRONMENT)
    {
      SelectAndLoadEnvironment(gameSettings, TXT(PS_LOADSCENERY), TXT(PS_BACK), NULL);
    }
    else if (settingsStatus == SETTINGS_SAVE_ENVIRONMENT)
    {
      std::string file;
      FileMenuSave(file, gameSettings, "UserSettings/Environment", ".xml", TXT(PS_SAVESCENERY));
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Saving Environment %s - ", file.c_str());
        gameSettings.mEnvironmentSettings.SaveToFile(file);
      }
    }
    else if (settingsStatus == SETTINGS_DELETE_ENVIRONMENT)
    {
      FileMenuDelete(gameSettings, "UserSettings/Environment", ".xml", TXT(PS_DELETESCENERY));
    }
    else if (settingsStatus == SETTINGS_LOAD_OBJECTS)
    {
      std::string file;
      FileMenuLoad(file, gameSettings, "SystemSettings/Objects", "UserSettings/Objects", ".xml", TXT(PS_LOADOBJECTS), 0, 0, TXT(PS_BACK), NULL);
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Loading Objects %s - ", file.c_str());
        bool result = gameSettings.mObjectsSettings.LoadFromFile(file);
        IwAssert(ROWLHOUSE, result);
        TRACE_FILE_IF(1) TRACE(" %s\n", result ? "success" : "failed");
      }
    }
    else if (settingsStatus == SETTINGS_SAVE_OBJECTS)
    {
      std::string file;
      FileMenuSave(file, gameSettings, "UserSettings/Objects", ".xml", TXT(PS_SAVEOBJECTS));
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Saving Objects %s - ", file.c_str());
        gameSettings.mObjectsSettings.SaveToFile(file);
      }
    }
    else if (settingsStatus == SETTINGS_DELETE_OBJECTS)
    {
      FileMenuDelete(gameSettings, "UserSettings/Objects", ".xml", TXT(PS_DELETEOBJECTS));
    }
    else if (settingsStatus == SETTINGS_LOAD_LIGHTING)
    {
      std::string file;
      FileMenuLoad(file, gameSettings, "SystemSettings/Lighting", "UserSettings/Lighting", ".xml", TXT(PS_LOADLIGHTING), 0, 0, TXT(PS_BACK), NULL, FILEMENUTYPE_LIGHTING);
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Loading Lighting %s", file.c_str());
        bool result = gameSettings.mLightingSettings.LoadFromFile(file);
        IwAssert(ROWLHOUSE, result);
        TRACE_FILE_IF(1) TRACE(" %s\n", result ? "success" : "failed");
      }
    }
    else if (settingsStatus == SETTINGS_SAVE_LIGHTING)
    {
      std::string file;
      FileMenuSave(file, gameSettings, "UserSettings/Lighting", ".xml", TXT(PS_SAVELIGHTING));
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Saving Lighting %s - ", file.c_str());
        gameSettings.mLightingSettings.SaveToFile(file);
      }
    }
    else if (settingsStatus == SETTINGS_DELETE_LIGHTING)
    {
      FileMenuDelete(gameSettings, "UserSettings/Lighting", ".xml", TXT(PS_DELETELIGHTING));
    }
    else if (settingsStatus == SETTINGS_LOAD_CONTROLLER)
    {
      std::string file;
      FileMenuLoad(file, gameSettings, "SystemSettings/Controller", "UserSettings/Controller", ".xml", TXT(PS_LOADCONTROLLER), 0, 0, TXT(PS_BACK), NULL);
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Loading Controller %s", file.c_str());
        bool result = gameSettings.mControllerSettings.LoadFromFile(file);
        IwAssert(ROWLHOUSE, result);
        TRACE_FILE_IF(1) TRACE(" %s\n", result ? "success" : "failed");
      }
    }
    else if (settingsStatus == SETTINGS_SAVE_CONTROLLER)
    {
      std::string file;
      FileMenuSave(file, gameSettings, "UserSettings/Controller", ".xml", TXT(PS_SAVECONTROLLER));
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Saving Controller %s", file.c_str());
        gameSettings.mControllerSettings.SaveToFile(file);
      }
    }
    else if (settingsStatus == SETTINGS_DELETE_CONTROLLER)
    {
      FileMenuDelete(gameSettings, "UserSettings/Controller", ".xml", TXT(PS_DELETECONTROLLER));
    }
    else if (settingsStatus == SETTINGS_LOAD_JOYSTICK)
    {
      std::string file;
      FileMenuLoad(file, gameSettings, "SystemSettings/Joystick", "UserSettings/Joystick", ".xml", TXT(PS_LOADJOYSTICK), 0, 0, TXT(PS_BACK), NULL);
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Loading Joystick %s", file.c_str());
        bool result = gameSettings.mJoystickSettings.LoadFromFile(file);
        IwAssert(ROWLHOUSE, result);
        TRACE_FILE_IF(1) TRACE(" %s\n", result ? "success" : "failed");
      }
    }
    else if (settingsStatus == SETTINGS_SAVE_JOYSTICK)
    {
      std::string file;
      FileMenuSave(file, gameSettings, "UserSettings/Joystick", ".xml", TXT(PS_SAVEJOYSTICK));
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Saving Joystick %s", file.c_str());
        gameSettings.mJoystickSettings.SaveToFile(file);
      }
    }
    else if (settingsStatus == SETTINGS_DELETE_JOYSTICK)
    {
      FileMenuDelete(gameSettings, "UserSettings/Joystick", ".xml", TXT(PS_DELETEJOYSTICK));
    }
    else if (settingsStatus == SETTINGS_LOAD_AEROPLANE)
    {
      SelectAndLoadAeroplane(gameSettings, TXT(PS_LOADAEROPLANE), TXT(PS_BACK), NULL);
    }
    else if (settingsStatus == SETTINGS_SAVE_AEROPLANE)
    {
      std::string file;
      FileMenuSave(file, gameSettings, "UserSettings/Aeroplane", ".xml", TXT(PS_SAVEAEROPLANE));
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Saving Aeroplane %s", file.c_str());
        gameSettings.mAeroplaneSettings.SaveToFile(file);
      }
    }
    else if (settingsStatus == SETTINGS_DELETE_AEROPLANE)
    {
      FileMenuDelete(gameSettings, "UserSettings/Aeroplane", ".xml", TXT(PS_DELETEAEROPLANE));
    }
    else if (settingsStatus == SETTINGS_LOAD_AICONTROLLERS)
    {
      std::string file;
      FileMenuLoad(file, gameSettings, "SystemSettings/AIControllers", "UserSettings/AIControllers", ".xml", TXT(PS_LOADAICONTROLLERS), 0, 0, TXT(PS_BACK), NULL);
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Loading AI Controllers %s", file.c_str());
        bool result = gameSettings.mAIControllersSettings.LoadFromFile(file);
        IwAssert(ROWLHOUSE, result);
        TRACE_FILE_IF(1) TRACE(" %s\n", result ? "success" : "failed");
      }
    }
    else if (settingsStatus == SETTINGS_SAVE_AICONTROLLERS)
    {
      std::string file;
      FileMenuSave(file, gameSettings, "UserSettings/AIControllers", ".xml", TXT(PS_SAVEAICONTROLLERS));
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Saving AI Controllers %s", file.c_str());
        gameSettings.mAIControllersSettings.SaveToFile(file);
      }
    }
    else if (settingsStatus == SETTINGS_DELETE_AICONTROLLERS)
    {
      FileMenuDelete(gameSettings, "UserSettings/AIControllers", ".xml", TXT(PS_DELETEAICONTROLLERS));
    }
    else if (settingsStatus == SETTINGS_SELECT_PANORAMA)
    {
      std::string file;
      FileMenuLoad(file, gameSettings, "SystemData/Panoramas", "UserData/Panoramas", "", TXT(PS_SELECTPANORAMA), 0, 0, TXT(PS_BACK), NULL);
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Selected Panorama %s", file.c_str());
        gameSettings.mEnvironmentSettings.mTerrainSettings.mPanoramaName = file;
      }
    }
    else if (settingsStatus == SETTINGS_SELECT_FILE_TERRAIN)
    {
#if 0
      TRACE_FILE_IF(1) TRACE("Picking file terrain");
      s3eImagePickerFormat requestFormat = S3E_IMAGEPICKER_FORMAT_ANYIMAGE;
      int files = s3eImagePickerToFilePath(
        requestFormat, 1, imageButtonXPos, imageButtonYPos, imageButtonWidth, imageButtonHeight);
      if (files > 0 && s3eImagePickerGetCount() > 0)
      {
        const s3eImagePickerResult* p = s3eImagePickerGetIndex(0);
        const char* path = (const char*)p->m_Buffer;
        TRACE_FILE_IF(1) TRACE("Picked Path: %s", path);

        if (p->m_Format != S3E_IMAGEPICKER_FORMAT_ANYVIDEO)
        {
          gameSettings.mEnvironmentSettings.mTerrainSettings.mFileTerrainName = path;
        }
        s3eImagePickerReset();
      }

#else
      std::string file;
      FileMenuLoad(file, gameSettings, "SystemData/FileTerrains", "UserData/FileTerrains", "", TXT(PS_SELECTTERRAINFILE), 0, 0, TXT(PS_BACK), NULL);
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Selected File Terrain %s", file.c_str());
        gameSettings.mEnvironmentSettings.mTerrainSettings.mFileTerrainName = file;
      }
#endif
    }
    else if (settingsStatus == SETTINGS_SELECT_PREFERRED_CONTROLLER)
    {
      std::string file;
      FileMenuLoad(file, gameSettings, "SystemSettings/Controller", "UserSettings/Controller", ".xml", TXT(PS_SELECTPREFERREDCONTROLLER), 0, 0, TXT(PS_BACK), NULL);
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Selected controller %s", file.c_str());
        gameSettings.mAeroplaneSettings.mPreferredController = file;
      }
    }
    else if (settingsStatus == SETTINGS_SELECT_OBJECTS_SETTINGS)
    {
      std::string file;
      FileMenuLoad(file, gameSettings, "SystemSettings/Objects", "UserSettings/Objects", ".xml", TXT(PS_SELECTOBJECTSSETTINGS), 0, 0, TXT(PS_BACK), NULL);
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Selected objects settings file %s", file.c_str());
        gameSettings.mEnvironmentSettings.mObjectsSettingsFile = file;
      }
    }
    else if (settingsStatus == SETTINGS_SELECT_LANGUAGE)
    {
      Language language = gameSettings.mOptions.mLanguage;
      int newLanguage = SelectItemMenu(gameSettings, gLanguageStrings, LANG_NUM_ACTIVE, TXT(PS_SELECTLANGUAGE));
      if (newLanguage >= 0 && newLanguage < LANG_NUM_ACTIVE)
        gameSettings.mOptions.mLanguage = (Language) newLanguage;
      TRACE_FILE_IF(1) TRACE("New language = %d", gameSettings.mOptions.mLanguage);
    }
    else if (settingsStatus == SETTINGS_DELETE_LOCAL_HIGHSCORES)
    {
      gameSettings.mStatistics.mHighScores.clear();
    }
    else if (settingsStatus == SETTINGS_ADD_AICONTROLLER)
    {
      CheckForAIControlCallback includeCallback;

      std::string file;
      SelectAeroplane(file, gameSettings, TXT(PS_LOADAEROPLANE), TXT(PS_BACK), NULL, &includeCallback);
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Selected AI Controller %s", file.c_str());
        AIControllersSettings::AIControllerSetting aic(file);
        gameSettings.mAIControllersSettings.mAIControllers.push_back(aic);
      }
    }
    else if (settingsStatus == SETTINGS_REMOVE_AICONTROLLERS)
    {
      TRACE_FILE_IF(1) TRACE("Removing all AI controllers");
      gameSettings.mAIControllersSettings.mAIControllers.clear();
    }
    else if (settingsStatus == SETTINGS_LOAD_TUG)
    {
      CheckForTugCallback includeCallback;

      std::string file;
      SelectAeroplane(file, gameSettings, TXT(PS_LOADAEROPLANE), TXT(PS_BACK), NULL, &includeCallback);
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Selected tug %s", file.c_str());
        gameSettings.mAeroplaneSettings.mTugName = file;
      }
    }
    else if (settingsStatus == SETTINGS_CLEAR_JOYSTICK)
    {
      gameSettings.mJoystickSettings = JoystickSettings();
    }
    else if (settingsStatus == SETTINGS_CALIBRATE_JOYSTICK)
    {
      ShowDialog("PicaSim", TXT(PS_RESTARTAFTERCALIBRATION), TXT(PS_OK));
      CalibrateJoystick();
    }
    else if (settingsStatus == SETTINGS_RESET_OBJECTS)
    {
      gameSettings.mObjectsSettings.mResetCounter++;
      actions = gameSettings.GetSettingsChangeActions(actions, origSettings);
      IwGetUIView()->DestroyElements();
      return;
    }
    else if (settingsStatus == SETTINGS_CLEARALLSAVEDSETTINGSANDEXIT)
    {
      IwGetUIView()->DestroyElements();
      int button = ShowDialog("PicaSim", TXT(PS_CONFIRMCLEARALLSETTINGS), TXT(PS_CANCEL), TXT(PS_OK));
      if (button == 1)
      {
        TRACE_FILE_IF(1) TRACE("Deleting game settings");
        s3eFileDelete("UserSettings/settings.xml");
        exit(0);
      }
      return;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
ScenarioResult SelectScenario(GameSettings& gameSettings, const char* title, const char* cancelButtonText, const char* altButtonText)
{
  TRACE_FUNCTION_ONLY(1);
  std::string file;
  FileMenuResult result = FileMenuLoad(file, gameSettings, "SystemSettings/Scenario", NULL, ".xml", title, 0, 0, 
    cancelButtonText, altButtonText, FILEMENUTYPE_AEROPLANE, 3.7f);
  if (!file.empty() && result == FILEMENURESULT_SELECTED)
  {
    TRACE_FILE_IF(1) TRACE("Selected scenario settings %s", file.c_str());

    if (file.find("lider.xml") != std::string::npos)
      return SCENARIO_TRAINERGLIDER;
    else if (file.find("owered.xml") != std::string::npos)
      return SCENARIO_TRAINERPOWERED;
    else if (file.find("hoose.xml") != std::string::npos)
      return SCENARIO_CHOOSE;
  }
  else if (result == FILEMENURESULT_ALT)
  {
    return SCENARIO_DEFAULT;
  }
  return SCENARIO_CANCELLED;
}

//---------------------------------------------------------------------------------------------------------------------
SelectResult SelectAeroplane(std::string& file, GameSettings& gameSettings, const char* title, const char* cancelButtonText, const char* altButtonText, IncludeCallback* includeCallback = 0)
{
  TRACE_FUNCTION_ONLY(1);
  Language language = gameSettings.mOptions.mLanguage;
  const char* aeroplaneTabTitles[] = {TXT(PS_ALL), TXT(PS_GLIDERS), TXT(PS_POWERED), TXT(PS_USER)};
  FileMenuResult result = FileMenuLoad(
    file, gameSettings, 
    "SystemSettings/Aeroplane", "UserSettings/Aeroplane", ".xml", 
    title, aeroplaneTabTitles, 4, 
    cancelButtonText, altButtonText, 
    FILEMENUTYPE_AEROPLANE, -1.0f, includeCallback);
  if (!file.empty() && result == FILEMENURESULT_SELECTED)
  {
    return SELECTRESULT_SELECTED;
  }
  else if (result == FILEMENURESULT_ALT)
  {
    return SELECTRESULT_SELECTED;
  }
  else
  {
    return SELECTRESULT_CANCELLED;
  }
}

//---------------------------------------------------------------------------------------------------------------------
SelectResult SelectAndLoadAeroplane(GameSettings& gameSettings, const char* title, const char* cancelButtonText, const char* altButtonText)
{
  TRACE_FUNCTION_ONLY(1);
  std::string file;
  Language language = gameSettings.mOptions.mLanguage;
  const char* aeroplaneTabTitles[] = {TXT(PS_ALL), TXT(PS_GLIDERS), TXT(PS_POWERED), TXT(PS_USER)};
  FileMenuResult result = FileMenuLoad(
    file, gameSettings, 
    "SystemSettings/Aeroplane", "UserSettings/Aeroplane", ".xml", 
    title, aeroplaneTabTitles, 4, 
    cancelButtonText, altButtonText, FILEMENUTYPE_AEROPLANE);
  if (!file.empty() && result == FILEMENURESULT_SELECTED)
  {
    TRACE_FILE_IF(1) TRACE("Loading Aeroplane settings %s", file.c_str());
    bool result = gameSettings.mAeroplaneSettings.LoadFromFile(file);
    IwAssert(ROWLHOUSE, result);
    TRACE_FILE_IF(1) TRACE(" %s\n", result ? "success" : "failed");
    gameSettings.mStatistics.mLoadedAeroplane = true;

    if (
      result &&
      gameSettings.mOptions.mUseAeroplanePreferredController && 
      !gameSettings.mAeroplaneSettings.mPreferredController.empty()
      )
    {
      TRACE_FILE_IF(1) TRACE("Loading Controller %s", gameSettings.mAeroplaneSettings.mPreferredController.c_str());
      bool controllerResult = gameSettings.mControllerSettings.LoadFromFile(gameSettings.mAeroplaneSettings.mPreferredController);
      TRACE_FILE_IF(1) TRACE(" %s\n", controllerResult ? "success" : "failed");
    }
    return SELECTRESULT_SELECTED;
  }
  else if (result == FILEMENURESULT_ALT)
  {
    return SELECTRESULT_SELECTED;
  }
  else
  {
    return SELECTRESULT_CANCELLED;
  }
}

//---------------------------------------------------------------------------------------------------------------------
SelectResult SelectAndLoadEnvironment(GameSettings& gameSettings, const char* title, const char* cancelButtonText, const char* altButtonText)
{
  TRACE_FUNCTION_ONLY(1);
  std::string file;
  Language language = gameSettings.mOptions.mLanguage;
  const char* environmentTabTitles[] = {TXT(PS_ALL), TXT(PS_SLOPE), TXT(PS_FLAT), TXT(PS_PANORAMIC), TXT(PS_3D), TXT(PS_USER)};
  FileMenuResult result = FileMenuLoad(
    file, gameSettings, 
    "SystemSettings/Environment", "UserSettings/Environment", ".xml", 
    title, environmentTabTitles, 6, cancelButtonText, altButtonText, FILEMENUTYPE_SCENERY);
  if (!file.empty() && result == FILEMENURESULT_SELECTED)
  {
    TRACE_FILE_IF(1) TRACE("Loading Environment %s - ", file.c_str());
    bool result = gameSettings.mEnvironmentSettings.LoadFromFile(file);
    IwAssert(ROWLHOUSE, result);
    TRACE_FILE_IF(1) TRACE(" %s\n", result ? "success" : "failed");
    bool objectsResult = gameSettings.mObjectsSettings.LoadFromFile(gameSettings.mEnvironmentSettings.mObjectsSettingsFile);
    IwAssert(ROWLHOUSE, objectsResult);
    gameSettings.mStatistics.mLoadedTerrain = true;
    return SELECTRESULT_SELECTED;
  }
  else if (result == FILEMENURESULT_ALT)
  {
    return SELECTRESULT_SELECTED;
  }
  else
  {
    return SELECTRESULT_CANCELLED;
  }
}
