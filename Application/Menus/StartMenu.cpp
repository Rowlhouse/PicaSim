#include "StartMenu.h"
#include "FonctionsMarmelade.h"
#include "FonctionsPointerKeyboardMarmelade.h"
#include "GameSettings.h"
#include "HelpMenu.h"
#include "Menu.h"
// #include "../Scoreloop.h"
#include "PicaJoystick.h"
#include "ScoreloopMenu.h"
#include "SettingsMenu.h"
#include "Trace.h"
#include "VersionChecker.h"
#include "WindowsBilling.h"

// #include <s3eOSExec.h>

// #include <vector>

StartMenuResult startMenuResult = STARTMENU_MAX;

//---------------------------------------------------------------------------------------------------------------------
bool ShowRateMe(const GameSettings& gameSettings)
{
  if (
    gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_IPHONE ||
    gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_ANDROID
    )
  {
    return gameSettings.mStatistics.mTotalFlightTime > 1800.0f;
  }
  else
  {
    return false;
  }
}

//---------------------------------------------------------------------------------------------------------------------
class StartMenu : public Menu
{
public:
  StartMenu(struct GameSettings& gameSettings);
  ~StartMenu();

  CIwUIButton* CreateButton(CIwUIElement* pContainer, CIwUILayoutGrid* layout, 
    const char* label, int column, int row, unsigned char alpha, unsigned char r=255, unsigned char g=255, unsigned char b=255);

  CIwUIButton* CreateImageButton(CIwUIElement* parentArea, CIwUILayoutGrid* parentLayout, const char* textureName, int column, int row, int buttonSize);

  bool HandleEvent(CIwEvent* pEvent) OVERRIDE;

  int32_t Update(bool keepAwake, bool throttle, bool& resourcesHaveChanged, const struct GameSettings& gameSettings) OVERRIDE;

private:
  CIwUIElement* mRow0Area;
  CIwUIElement* mRow1Area;
  CIwUIElement* mRow2Area;

  CIwUIButton* mQuitButton;
  CIwUIButton* mSettingsButton;
  CIwUIButton* mScoreloopButton;

  CIwUIButton* mFlyButton;
  CIwUIButton* mChallengeButton;
  CIwUIButton* mBuyButton;

  CIwUIButton* mRateMeButton;
  CIwUIButton* mNewVersionButton;

  CIwUIButton* mHelpButton;
  CIwUIButton* mFacebookButton;
  CIwUIButton* mGoogleButton;

  CIwUILabel*  mInfoLabel;
  CIwUILabel*  mTrialLabel;

  GameSettings& mGameSettings;
  Texture* mTexture;
};

//---------------------------------------------------------------------------------------------------------------------
StartMenu::~StartMenu()
{
  if (mFlyButton)
    mFlyButton->RemoveEventHandler(this);
  if (mQuitButton)
    mQuitButton->RemoveEventHandler(this);
  if (mSettingsButton)
    mSettingsButton->RemoveEventHandler(this);

  if (mScoreloopButton)
    mScoreloopButton->RemoveEventHandler(this);
  if (mBuyButton)
    mBuyButton->RemoveEventHandler(this);

  if (mChallengeButton)
    mChallengeButton->RemoveEventHandler(this);

  if (mRateMeButton)
    mRateMeButton->RemoveEventHandler(this);
  if (mNewVersionButton)
    mNewVersionButton->RemoveEventHandler(this);

  if (mHelpButton)
    mHelpButton->RemoveEventHandler(this);
  if (mGoogleButton)
    mGoogleButton->RemoveEventHandler(this);
  if (mFacebookButton)
    mFacebookButton->RemoveEventHandler(this);

  delete mTexture;
}

//---------------------------------------------------------------------------------------------------------------------
bool StartMenu::HandleEvent(CIwEvent* pEvent) 
{
  // I think we sometimes get bogus events on the first update... 
  if (mUpdateCounter < 2)
    return true;

  if (pEvent->GetID() == IWUI_EVENT_BUTTON)
  {
    CIwManaged* sender = pEvent->GetSender();
    if (sender == mFlyButton)
      startMenuResult = STARTMENU_FLY;
    else if (sender == mChallengeButton)
      startMenuResult = STARTMENU_CHALLENGE;
    else if (sender == mQuitButton)
      startMenuResult = STARTMENU_QUIT;
    else if (sender == mSettingsButton)
      startMenuResult = STARTMENU_SETTINGS;
    else if (sender == mScoreloopButton)
      startMenuResult = STARTMENU_SCORELOOP;
    else if (sender == mHelpButton)
      startMenuResult = STARTMENU_HELP;
    else if (sender == mFacebookButton)
      s3eOSExecExecute("http://www.facebook.com/233467446753191", false);
    else if (sender == mGoogleButton)
      s3eOSExecExecute("http://plus.google.com/100904112144799965510", false);
    else if (sender == mBuyButton)
      startMenuResult = STARTMENU_BUYFULLVERSION;
    else if (sender == mRateMeButton)
      RateMe(mGameSettings);
    else if (sender == mNewVersionButton)
      NewVersion();
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------------------------------------------------
int32_t StartMenu::Update(bool keepAwake, bool throttle, bool& resourcesHaveChanged, const struct GameSettings& gameSettings)
{
  int width =  gameSettings.mOptions.mFrameworkSettings.mScreenWidth;
  int height =  gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  Language language = gameSettings.mOptions.mLanguage;

  if (mRateMeButton)
  {
    CIwVec2 pos(width - mRateMeButton->GetSize().x, 0);
    mRateMeButton->SetPos(pos);
  }
  if (mNewVersionButton)
  {
    bool newVersion = IsNewVersionAvailable();
    if (newVersion)
    {
      mNewVersionButton->SetVisible(true);
    }
    CIwVec2 pos(width - mNewVersionButton->GetSize().x, 0);
    mNewVersionButton->SetPos(pos);
  }
  if (gameSettings.mStatistics.mIsInTrialMode)
  {
    mTrialLabel->SetCaption(TXT(PS_INTRIALMODE));
    mBuyButton->SetVisible(true);
    mBuyButton->SetCaption(TXT(PS_DONATEMSG));
  }
  else
  {
    mTrialLabel->SetCaption("");
    mBuyButton->SetVisible(false);
    mBuyButton->SetCaption("");
  }

  if (gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_WIN10)
  {
    UpdateJoystick(0);
    if (!gameSettings.mJoystickSettings.mEnableJoystick && JoystickAvailable())
      mInfoLabel->SetCaption(TXT(PS_GAMEPADDETECTED));
    else
      mInfoLabel->SetCaption("");
  }

  mRow1Area->SetSizeMin(CIwVec2(-1, height/3));

  int32_t time = Menu::Update(keepAwake, throttle, resourcesHaveChanged, gameSettings);
  return time;
}


//---------------------------------------------------------------------------------------------------------------------
CIwUIButton* StartMenu::CreateButton(
  CIwUIElement* pContainer, CIwUILayoutGrid* layout, 
  const char* label, int column, int row, 
  unsigned char alpha, unsigned char r, unsigned char g, unsigned char b)
{
  CIwUIButton* button = new CIwUIButton;
  button->SetStyle("<button_start>");
  button->SetCaption(label);
  Colour col;
  col.Set(r, g, b, alpha);
  button->SetColour(col);
  pContainer->AddChild(button);
  button->AddEventHandler(this);
  if (layout)
    layout->AddElement(button, column, row);
  return button;
}

//---------------------------------------------------------------------------------------------------------------------
CIwUIButton* StartMenu::CreateImageButton(CIwUIElement* parentArea, CIwUILayoutGrid* parentLayout, const char* textureName, int column, int row, int buttonSize)
{
  s3eDeviceYield();
  CIwUIButton* button = CreateButton(parentArea, parentLayout, "", column, row, 255);
  button->SetStyle("<button_start_image>");
  CIwTexture* texture = (CIwTexture*)IwGetResManager()->GetResNamed(textureName, "CIwTexture");
  uint32_t w = texture->GetWidth();
  uint32_t h = texture->GetHeight();
  // style for the image button
  CIwUIStyle style;
  style.InlinePropertySet();
  CIwUIPropertySet* pImageProps = style.GetInlinePropertySet();
  pImageProps->SetProperty("drawableType", IwHashString("image"));
  pImageProps->SetProperty("texture", texture);
  button->SetProperty("buttonUp", style);
  button->SetProperty("buttonDown", style);
  button->SetProperty("aspectRatio", CIwVec2(w,h));
  button->SetSizeMax(CIwVec2(-1, buttonSize));
  button->SetSizeMin(CIwVec2(-1, buttonSize));

  s3eDeviceYield();
  return button;
}

//---------------------------------------------------------------------------------------------------------------------
StartMenu::StartMenu(GameSettings& gameSettings)
  : mGameSettings(gameSettings)
{
  TRACE_METHOD_ONLY(1);

  int width =  gameSettings.mOptions.mFrameworkSettings.mScreenWidth;
  int height =  gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  Language language = gameSettings.mOptions.mLanguage;

  CIwUIElement* pContainer = new CIwUIElement;
  pContainer->SetSizeToContent(true);

  s3eDeviceYield();
  mTexture = AddCentredImage("Menus/StartBackground.jpg", gameSettings.mOptions.m16BitTextures);
  s3eDeviceYield();

  // Set the layout
  CIwUILayoutGrid* layout = new CIwUILayoutGrid;
  layout->SetSizeToSpace(true);
  layout->AddColumn();
  layout->AddRow();
  layout->AddRow();
  layout->AddRow();

  mFlyButton = mChallengeButton = mQuitButton = mSettingsButton = mScoreloopButton = 
    mRateMeButton = mNewVersionButton = mHelpButton = mFacebookButton = mGoogleButton = mBuyButton = 0;

  pContainer->SetLayout(layout);

  IwGetUIView()->AddElement(pContainer);
  IwGetUIView()->AddElementToLayout(pContainer);

  int rowHeight = (int) (0.35f * height);
  int bottomRowHeight = height - 2*rowHeight;

  // First row for fly, donate and race
  CIwUILayoutGrid* row0Layout = new CIwUILayoutGrid;
  row0Layout->SetSizeToSpace(true);
  row0Layout->AddColumn();
  row0Layout->AddColumn();
  row0Layout->AddColumn();
  row0Layout->AddRow(rowHeight);

  // Middle row for spacing
  CIwUILayoutGrid* row1Layout = new CIwUILayoutGrid;
  row1Layout->SetSizeToSpace(true);
  row1Layout->AddColumn();
  row1Layout->AddRow();
  row1Layout->AddRow();

  // Bottom row for social, scoreloop and settings/help
  CIwUILayoutGrid* row2Layout = new CIwUILayoutGrid;
  row2Layout->SetSizeToSpace(true);
  row2Layout->AddColumn();
  row2Layout->AddColumn();
  row2Layout->AddColumn();
  row2Layout->AddColumn();
  row2Layout->AddColumn();
  row2Layout->AddRow();

  mRow0Area = new CIwUIElement;
  mRow1Area = new CIwUIElement;
  mRow2Area = new CIwUIElement;

  mRow0Area->SetSizeToContent(false);
  mRow1Area->SetSizeToContent(false);
  mRow2Area->SetSizeToContent(false);

  mRow0Area->SetLayout(row0Layout);
  mRow1Area->SetLayout(row1Layout);
  mRow2Area->SetLayout(row2Layout);

  pContainer->AddChild(mRow0Area);
  pContainer->AddChild(mRow1Area);
  pContainer->AddChild(mRow2Area);

  layout->AddElement(mRow0Area, 0, 0);
  layout->AddElement(mRow1Area, 0, 1);
  layout->AddElement(mRow2Area, 0, 2);

  unsigned char alpha = 128;
  unsigned char r = 255;
  unsigned char g = 255;
  unsigned char b = 255;

  // Top row for fly etc
  mFlyButton = CreateButton(mRow0Area, row0Layout, TXT(PS_FREEFLY), 0, 0, alpha, r, g, b);
  mBuyButton = CreateButton(mRow0Area, row0Layout, TXT(PS_DONATEMSG), 1, 0, alpha, r, g, b);
  mChallengeButton = CreateButton(mRow0Area, row0Layout, TXT(PS_RACE), 2, 0, alpha, r, g, b);
  if (mGameSettings.mStatistics.mAppType == Statistics::APP_FULL)
  {
    mBuyButton->SetVisible(false);
  }

  // Middle row is just a spacer - it will get sized in the update. Avoid an assert by adding a dummy label
  mInfoLabel = new CIwUILabel;
  mInfoLabel->SetStyle("<label_centered>");
  Colour col;
  col.Set(255, 255, 0);
  mInfoLabel->SetTextColour(col);
  row1Layout->AddElement(mInfoLabel, 0, 0);

  mTrialLabel = new CIwUILabel;
  mTrialLabel->SetStyle("<label_centered>");
  col.Set(255, 255, 255);
  mTrialLabel->SetTextColour(col);
  row1Layout->AddElement(mTrialLabel, 0, 1);

  int imageButtonSize = (int) (bottomRowHeight * 0.4f);

  // Rate button only after suitable conditions
  if (ShowRateMe(gameSettings))
  {
    mRateMeButton = CreateImageButton(mRow0Area, 0, "RateMe", 0, 0, (imageButtonSize * 3) / 4);
  }
  else if (gameSettings.mOptions.mFrameworkSettings.isWindowsDesktop())
  {
    mNewVersionButton = CreateImageButton(mRow0Area, 0, "NewVersion", 0, 0, (imageButtonSize * 3) / 4);
    mNewVersionButton->SetVisible(false);
  }

  // images
  mFacebookButton = CreateImageButton(mRow2Area, row2Layout, "Facebook", 0, 0, imageButtonSize);
  if (mGameSettings.mOptions.mFrameworkSettings.mOS != S3E_OS_ID_IPHONE)
    mGoogleButton = CreateImageButton(mRow2Area, row2Layout, "GooglePlus", 1, 0, imageButtonSize);
  if (gameSettings.mOptions.mEnableScoreloop)
  {
    mScoreloopButton = CreateImageButton(mRow2Area, row2Layout, "Leaderboards", 2, 0, imageButtonSize);
  }
  mHelpButton = CreateImageButton(mRow2Area, row2Layout, "Help", 3, 0, imageButtonSize);
  mSettingsButton = CreateImageButton(mRow2Area, row2Layout, "Utilities", 4, 0, imageButtonSize);

  if (
    mGameSettings.mOptions.mFrameworkSettings.mOS != S3E_OS_ID_IPHONE &&
    mGameSettings.mOptions.mFrameworkSettings.mOS != S3E_OS_ID_WIN10
    )
  {
    mQuitButton = CreateImageButton(mRow0Area, 0, "Stop", 0, 0, (int) (0.75f * imageButtonSize));
  }

  bool resourcesHaveChanged;
  Update(false, false, resourcesHaveChanged, gameSettings);
}



//---------------------------------------------------------------------------------------------------------------------
StartMenuResult DisplayStartMenu(struct GameSettings& gameSettings)
{
  TRACE_FUNCTION_ONLY(1);

  startMenuResult = STARTMENU_MAX;
  while (startMenuResult == STARTMENU_MAX)
  {
    WindowsCheckForTrialVersion();

    PrepareForIwGx(false);
    StartMenu* startMenu = new StartMenu(gameSettings); // Gets deleted on IwGetUIView()->DestroyElements()

    uint32_t menuTime = 0;

    while (startMenuResult == STARTMENU_MAX)
    {
      bool resourcesHaveChanged;
      menuTime += startMenu->Update(gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_ANDROID, true, resourcesHaveChanged, gameSettings);

      if (resourcesHaveChanged)
      {
        startMenuResult = STARTMENU_REFRESH;
      }
      if (
        s3eDeviceCheckQuitRequest() ||
        ((s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED) && menuTime > 500)
        )
      {
        startMenuResult = STARTMENU_QUIT;
      }
      if (startMenuResult == STARTMENU_BUYFULLVERSION)
      {
        BuyFullVersion(gameSettings);
        startMenuResult = STARTMENU_MAX;
      }
    }

    IwGetUIView()->DestroyElements();
    RecoverFromIwGx(false);

    if (startMenuResult == STARTMENU_SETTINGS)
    {
      SettingsChangeActions actions; // not needed
      DisplaySettingsMenu(gameSettings, actions);
      startMenuResult = STARTMENU_MAX;
    }
    else if (startMenuResult == STARTMENU_HELP)
    {
      DisplayHelpMenu(gameSettings, false);
      startMenuResult = STARTMENU_MAX;
    }
    else if (startMenuResult == STARTMENU_SCORELOOP)
    {
      DisplayScoreloopMenu(gameSettings);
      startMenuResult = STARTMENU_MAX;
    }
    else if (startMenuResult == STARTMENU_REFRESH)
    {
      startMenuResult = STARTMENU_MAX;
    }
  }
  RecoverFromIwGx(false);
  return startMenuResult;
}
