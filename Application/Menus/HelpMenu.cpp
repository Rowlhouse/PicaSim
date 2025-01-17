#include "HelpMenu.h"
// #include "Menu.h"
// #include "Helpers.h"
// #include "../GameSettings.h"
// #include "../PicaStrings.h"

// #include <s3eOSExec.h>
// #include "dpiInfo.h"
  
//---------------------------------------------------------------------------------------------------------------------
class HelpMenu : public Menu
{
public:
  enum TabPanelEnum
  {
    TAB_ABOUT,
    TAB_HOW_TO_FLY,
    TAB_HOW_TO_CONFIGURE,
    TAB_HOW_TO_RACE,
    TAB_TIPS,
    TAB_OBJECTEDITING,
    TAB_KEYBOARD,
    TAB_JOYSTICKS,
    TAB_CREDITS,
    TAB_LICENCE,
    TAB_VERSIONS,
    TAB_NUM_TABS
  };

  HelpMenu(const GameSettings& gameSettings, TabPanelEnum initialTab);
  ~HelpMenu();

  int32 Update(bool keepAwake, bool throttle, bool& resourcesHaveChanged, const struct GameSettings& gameSettings) OVERRIDE;

  bool GetFinished() const {return mFinished;}

private:
  CIwUIScrollableView* CreateText(const char* text, bool centre, int space);

  virtual bool HandleEvent(CIwEvent* pEvent) OVERRIDE;

  bool mFinished;

  CIwUILayoutGrid* mGridLayout;
  CIwUIImage* mGridArea;

  uint16   mSelectedTab;
  CIwUIScrollableView* mScrollAreas[TAB_NUM_TABS];

  CIwUIButton* mBackButton;
  CIwUITabBar* mTabBar;
  CIwUIButton* mWebsiteButton;

  const GameSettings& mGameSettings;
};

//---------------------------------------------------------------------------------------------------------------------
int32 HelpMenu::Update(bool keepAwake, bool throttle, bool& resourcesHaveChanged, const struct GameSettings& gameSettings)
{
  mScrollArea = mScrollAreas[mSelectedTab];
  if (mUpdateCounter > 0)
    mTabBar->SetEnabled(true);
  return Menu::Update(keepAwake, throttle, resourcesHaveChanged, gameSettings);
}

//---------------------------------------------------------------------------------------------------------------------
CIwUIScrollableView* HelpMenu::CreateText(const char* text, bool centre, int space)
{
  int width = mGameSettings.mOptions.mFrameworkSettings.mScreenWidth;
  int height = mGameSettings.mOptions.mFrameworkSettings.mScreenHeight;

  CIwUILabel* label = new CIwUILabel;
  label->SetStyle("<label_setting_background>");
  label->SetCaption(text);
  if (centre)
    label->SetProperty("alignH", IW_UI_ALIGN_CENTRE);
  else
    label->SetProperty("alignH", IW_UI_ALIGN_LEFT);
  label->SetProperty("alignV", IW_UI_ALIGN_TOP);
  label->SetSizeMin(CIwVec2(width*15/16, -1));
  label->SetSizeMax(CIwVec2(width*15/16, -1));
  label->SetSizeToContent(true);

  CIwUILayoutVertical* layout = new CIwUILayoutVertical;
  layout->AddElement(label);

  CIwUIScrollableView* scrollArea = new CIwUIScrollableView;
  scrollArea->SetSizeToContent(true);
  scrollArea->SetSizeMin(CIwVec2(-1, height - space));
  scrollArea->AddChild(label);

  scrollArea->SetLayout(layout);

  return scrollArea;
}

//---------------------------------------------------------------------------------------------------------------------
static float GetSurfaceDiagonalInches()
{
  int32 dpi = DPI::dpiGetScreenDPI();
  if (dpi > 0)
  {
    int w = s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
    int h = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);
    float d = hypotf((float) w, (float) h);
    return d / dpi;
  }
  else
  {
    return 0.0f;
  }
}

//---------------------------------------------------------------------------------------------------------------------
HelpMenu::HelpMenu(const GameSettings& gameSettings, TabPanelEnum initialTab)
  : mSelectedTab(initialTab), mFinished(false), mGameSettings(gameSettings)
{
  Language language = gameSettings.mOptions.mLanguage;

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
  int width = mGameSettings.mOptions.mFrameworkSettings.mScreenWidth;

  int space = 0;

  // Tab area and buttons
  {
    // Tab area
    CIwUILabel* tabArea = new CIwUILabel;
    tabArea->SetStyle("<label_bar_background>");
    tabArea->SetSizeToContent(false); // If false the the area is aligned on the left. If true it centres
    mGridArea->AddChild(tabArea);
    mGridLayout->AddElement(tabArea, 0, 0);

    space += 2 * tabArea->GetSizeMin().y;

    // Layout
    CIwUILayoutHorizontal* horizontalLayout = new CIwUILayoutHorizontal;
    horizontalLayout->SetSizeToSpace(true);
    tabArea->SetLayout(horizontalLayout);

    // Back button
    mBackButton = new CIwUIButton;
    mBackButton->SetStyle("<button_wide>");
    mBackButton->SetCaption(GetPS(PS_BACK, language));
    horizontalLayout->AddElement(mBackButton);
    tabArea->AddChild(mBackButton);

    spacer = new CIwUIElement;
    horizontalLayout->AddElement(spacer);
    tabArea->AddChild(spacer);

    // Tab bar
    // Make a horizontal scroll area for it
    CIwUIScrollableView* tabBarArea = new CIwUIScrollableView;
    tabBarArea->SetSizeToContent(false);

    CIwUILayoutHorizontal* tabBarLayout = new CIwUILayoutHorizontal;
    tabBarLayout->SetSizeToSpace(false);
    tabBarArea->SetLayout(tabBarLayout);

    horizontalLayout->AddElement(tabBarArea);
    tabArea->AddChild(tabBarArea);

    // Tab bar itself
    mTabBar = new CIwUITabBar;
    mTabBar->SetEnabled(false); // Workaround for bogus events first frame
    mTabBar->SetSizeToContent(true);
    mTabBar->SetRadioButtonStyle(TAB_ABOUT, "<radio_left>");
    mTabBar->SetRadioButtonStyle(TAB_HOW_TO_FLY, "<radio_centre>");
    mTabBar->SetRadioButtonStyle(TAB_HOW_TO_CONFIGURE, "<radio_centre>");
    mTabBar->SetRadioButtonStyle(TAB_HOW_TO_RACE, "<radio_centre>");
    mTabBar->SetRadioButtonStyle(TAB_TIPS, "<radio_centre>");
    mTabBar->SetRadioButtonStyle(TAB_OBJECTEDITING, "<radio_centre>");
    mTabBar->SetRadioButtonStyle(TAB_KEYBOARD, "<radio_centre>");
    mTabBar->SetRadioButtonStyle(TAB_JOYSTICKS, "<radio_centre>");
    mTabBar->SetRadioButtonStyle(TAB_CREDITS, "<radio_centre>");
    mTabBar->SetRadioButtonStyle(TAB_LICENCE, "<radio_centre>");
    mTabBar->SetRadioButtonStyle(TAB_VERSIONS, "<radio_right>");
    mTabBar->SetRadioButtonCaption(TAB_ABOUT, GetPS(PS_ABOUT, language));
    mTabBar->SetRadioButtonCaption(TAB_HOW_TO_FLY, GetPS(PS_FLYING, language));
    mTabBar->SetRadioButtonCaption(TAB_HOW_TO_CONFIGURE, GetPS(PS_SETTINGS, language));
    mTabBar->SetRadioButtonCaption(TAB_HOW_TO_RACE, GetPS(PS_RACES, language));
    mTabBar->SetRadioButtonCaption(TAB_TIPS, GetPS(PS_TIPS, language));
    mTabBar->SetRadioButtonCaption(TAB_OBJECTEDITING, GetPS(PS_OBJECTEDITING, language));
    mTabBar->SetRadioButtonCaption(TAB_KEYBOARD, GetPS(PS_KEYBOARD, language));
    mTabBar->SetRadioButtonCaption(TAB_JOYSTICKS, GetPS(PS_JOYSTICK, language));
    mTabBar->SetRadioButtonCaption(TAB_CREDITS, GetPS(PS_CREDITS, language));
    mTabBar->SetRadioButtonCaption(TAB_LICENCE, GetPS(PS_LICENCE, language));
    mTabBar->SetRadioButtonCaption(TAB_VERSIONS, GetPS(PS_VERSIONS, language));
    mTabBar->SetSelected(mSelectedTab);

    tabBarArea->AddChild(mTabBar);
    tabBarLayout->AddElement(mTabBar);
  }

  // Website button
  {
    CIwUILayoutGrid* websiteLayout = new CIwUILayoutGrid;
    websiteLayout->SetSizeToSpace(true);
    websiteLayout->AddRow();
    websiteLayout->AddColumn();

    CIwUILabel* websiteBackground = new CIwUILabel;
    websiteBackground->SetStyle("<label_bar_background>");
    websiteBackground->SetSizeToContent(false);
    websiteBackground->SetLayout(websiteLayout);

    mGridArea->AddChild(websiteBackground);
    mGridLayout->AddElement(websiteBackground, 0, 2);

    mWebsiteButton = new CIwUIButton;
    mWebsiteButton->SetStyle("<button_wide>");
    mWebsiteButton->SetCaption(GetPS(PS_WEBSITE, language));
    mWebsiteButton->SetSizeToContent(false);
    websiteBackground->AddChild(mWebsiteButton);
    websiteLayout->AddElement(mWebsiteButton, 0, 0);
  }

  if (gameSettings.mStatistics.mAppType != Statistics::APP_FULL)
    mScrollAreas[TAB_ABOUT] = CreateText(GetPS(PS_ABOUTFREETEXT, language), false, space);
  else
    mScrollAreas[TAB_ABOUT] = CreateText(GetPS(PS_ABOUTPAIDTEXT, language), false, space);
  mScrollAreas[TAB_HOW_TO_FLY] = CreateText(GetPS(PS_HOWTOFLYTEXT, language), false, space);
  mScrollAreas[TAB_HOW_TO_CONFIGURE] = CreateText(GetPS(PS_HOWTOCONFIGURETEXT, language), false, space);
  mScrollAreas[TAB_HOW_TO_RACE] = CreateText(GetPS(PS_HOWTORACETEXT, language), false, space);
  mScrollAreas[TAB_TIPS] = CreateText(GetPS(PS_TIPSTEXT, language), false, space);
  mScrollAreas[TAB_OBJECTEDITING] = CreateText(GetPS(PS_OBJECTEDITINGTEXT, language), false, space);
  mScrollAreas[TAB_KEYBOARD] = CreateText(GetPS(PS_KEYBOARDTEXT, language), false, space);
  mScrollAreas[TAB_JOYSTICKS] = CreateText(GetPS(PS_JOYSTICKSETUPTEXT, language), false, space);
  mScrollAreas[TAB_CREDITS] = CreateText(GetPS(PS_CREDITSTEXT, language), true, space);
  mScrollAreas[TAB_LICENCE] = CreateText(GetPS(PS_LICENCETEXT, language), false, space);
  mScrollAreas[TAB_VERSIONS] = CreateText(GetPS(PS_VERSIONSTEXT, language), false, space);

  char labelTxt[256];
  sprintf(labelTxt, "Build info:\n%s\n%s", MARMALADE_VERSION_STRING_FULL, __TIMESTAMP__);
  CIwUILabel* versionLabel = new CIwUILabel;
  versionLabel->SetStyle("<label_setting_background>");
  versionLabel->SetCaption(labelTxt);
  versionLabel->SetProperty("alignH", IW_UI_ALIGN_LEFT);
  versionLabel->SetProperty("alignV", IW_UI_ALIGN_TOP);
  versionLabel->SetSizeMin(CIwVec2(width*15/16, -1));
  versionLabel->SetSizeMax(CIwVec2(width*15/16, -1));
  versionLabel->SetSizeToContent(true);
  mScrollAreas[TAB_VERSIONS]->AddChild(versionLabel);
  mScrollAreas[TAB_VERSIONS]->GetLayout()->AddElement(versionLabel);

  int32 dpi = DPI::dpiGetScreenDPI();
  float diagonal = GetSurfaceDiagonalInches();
  const char* deviceName = s3eDeviceGetString(S3E_DEVICE_ID);
  int32 numCores = s3eDeviceGetInt(S3E_DEVICE_NUM_CPU_CORES);

  sprintf(labelTxt, "Device info:\nName: %s\nCores: %d\nSurface: %d dpi %5.2f\"", 
    deviceName, numCores, dpi, diagonal);
  CIwUILabel* deviceLabel = new CIwUILabel;
  deviceLabel->SetStyle("<label_setting_background>");
  deviceLabel->SetCaption(labelTxt);
  deviceLabel->SetProperty("alignH", IW_UI_ALIGN_LEFT);
  deviceLabel->SetProperty("alignV", IW_UI_ALIGN_TOP);
  deviceLabel->SetSizeMin(CIwVec2(width*15/16, -1));
  deviceLabel->SetSizeMax(CIwVec2(width*15/16, -1));
  deviceLabel->SetSizeToContent(true);
  mScrollAreas[TAB_VERSIONS]->AddChild(deviceLabel);
  mScrollAreas[TAB_VERSIONS]->GetLayout()->AddElement(deviceLabel);

  // Persist the selected tab
  mGridArea->AddChild(mScrollAreas[mSelectedTab]);
  mGridLayout->AddElement(mScrollAreas[mSelectedTab], 0, 1);

  IwGetUIView()->AddElement(mGridArea);
  IwGetUIView()->AddElementToLayout(mGridArea);

  // Add padding if necessary
  IwGetUIController()->Update();
  CIwVec2 tabBarSize = mTabBar->GetSize();
  CIwVec2 backSize = mBackButton->GetSize();
  int w = (width - tabBarSize.x)/2 - backSize.x;
  if (w > 1)
    spacer->SetSizeMin(CIwVec2(w,-1));
}


//---------------------------------------------------------------------------------------------------------------------
HelpMenu::~HelpMenu()
{
  for (uint16 i = 0 ; i != TAB_NUM_TABS ; ++i)
  {
    if (i != mSelectedTab)
    {
      delete mScrollAreas[i];
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
bool HelpMenu::HandleEvent(CIwEvent* pEvent)
{
  CIwManaged* sender = pEvent->GetSender();
  if (pEvent->GetID() == IWUI_EVENT_BUTTON)
  {
    if (sender == mBackButton)
    {
      mFinished = true;
      return true;
    }
    else if (sender == mWebsiteButton)
    {
      OpenWebsite(mGameSettings);
    }
  }
  else if (pEvent->GetID() == IWUI_EVENT_TABBAR)
  {
    if (sender == mTabBar)
    {
      if (mUpdateCounter > 10)
      {
        // remove
        mGridArea->RemoveChild(mScrollAreas[mSelectedTab]);
        mGridLayout->RemoveElement(mScrollAreas[mSelectedTab]);

        mSelectedTab = mTabBar->GetSelected();

        mGridArea->AddChild(mScrollAreas[mSelectedTab]);
        mGridLayout->AddElement(mScrollAreas[mSelectedTab], 0, 1);

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
  return false;
}


//---------------------------------------------------------------------------------------------------------------------
void DisplayHelpMenu(GameSettings& gameSettings, bool showHowToFly)
{
  AudioManager::GetInstance().SetAllChannelsToZeroVolume();
  FixStateForMenus fixStateForMenus(true, true);

  HelpMenu* settingsMenu = new HelpMenu(gameSettings, showHowToFly ? HelpMenu::TAB_HOW_TO_FLY : HelpMenu::TAB_ABOUT); // Gets deleted on IwGetUIView()->DestroyElements()

  while (!settingsMenu->GetFinished())
  {
    bool resourcesHaveChanged;
    settingsMenu->Update(gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_ANDROID, true, resourcesHaveChanged, gameSettings);

    if (
      s3eDeviceCheckQuitRequest() ||
      (s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED)
      )
    {
      break;
    }
    AudioManager::GetInstance().Update(1.0f/30.0f);

    if (resourcesHaveChanged)
    {
      IwGetUIView()->DestroyElements();
      settingsMenu = new HelpMenu(gameSettings, showHowToFly ? HelpMenu::TAB_HOW_TO_FLY : HelpMenu::TAB_ABOUT); // Gets deleted on IwGetUIView()->DestroyElements()
    }
  }
  IwGetUIView()->DestroyElements();
}
