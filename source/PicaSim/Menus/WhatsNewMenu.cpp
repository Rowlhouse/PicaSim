#include "WhatsNewMenu.h"
#include "Menu.h"
#include "Helpers.h"
#include "SettingsWidgets.h"
#include "../GameSettings.h"
#include "../PicaStrings.h"

#include <s3eOSExec.h>
  
//---------------------------------------------------------------------------------------------------------------------
class WhatsNewMenu : public Menu
{
public:
  WhatsNewMenu(const GameSettings& gameSettings);
  ~WhatsNewMenu();

  bool GetFinished() const {return mFinished;}

private:
  CIwUIScrollableView* CreateText(const char* text, bool centre, int space);

  virtual bool HandleEvent(CIwEvent* pEvent) OVERRIDE;

  bool mFinished;

  CIwUILayoutGrid* mGridLayout;
  CIwUIImage* mGridArea;
  CIwUIScrollableView* mScrollArea;

  CIwUITabBar* mTabBar;
  CIwUIButton* mOKButton;

  const GameSettings& mGameSettings;
};

//---------------------------------------------------------------------------------------------------------------------
CIwUIScrollableView* WhatsNewMenu::CreateText(const char* text, bool centre, int space)
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
WhatsNewMenu::WhatsNewMenu(const GameSettings& gameSettings)
  : mFinished(false), mGameSettings(gameSettings)
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
  int space = 0;

  // Tab area and buttons
  {
    // Navigation bar area
    CIwUILabel* navigationArea = new CIwUILabel;
    navigationArea->SetStyle("<label_bar_background>");
    navigationArea->SetSizeToContent(false);
    mGridArea->AddChild(navigationArea);
    mGridLayout->AddElement(navigationArea, 0, 0);

    space += 2 * navigationArea->GetSizeMin().y;

    // Layout
    CIwUILayoutHorizontal* navigationLayout = new CIwUILayoutHorizontal;
    navigationLayout->SetSizeToSpace(true);
    navigationArea->SetLayout(navigationLayout);

    CIwUILabel* titleLabel = CreateLabel(gameSettings, navigationArea, navigationLayout, GetPS(PS_WHATSNEW, language), LABEL_TYPE_CENTERED_TITLE);
    int width = mGameSettings.mOptions.mFrameworkSettings.mScreenWidth;
    titleLabel->SetSizeHint(CIwVec2(width, -1));
  }

  // The main text area
  {
    mScrollArea = CreateText(GetPS(PS_WHATSNEWTEXT, language), false, space);
    mGridArea->AddChild(mScrollArea);
    mGridLayout->AddElement(mScrollArea, 0, 1);
  }

  // OK button
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

    mOKButton = new CIwUIButton;
    mOKButton->SetStyle("<button_wide>");
    mOKButton->SetCaption(GetPS(PS_OK, language));
    mOKButton->SetSizeToContent(false);
    websiteBackground->AddChild(mOKButton);
    websiteLayout->AddElement(mOKButton, 0, 0);
  }

  IwGetUIView()->AddElement(mGridArea);
  IwGetUIView()->AddElementToLayout(mGridArea);
}


//---------------------------------------------------------------------------------------------------------------------
WhatsNewMenu::~WhatsNewMenu()
{
}

//---------------------------------------------------------------------------------------------------------------------
bool WhatsNewMenu::HandleEvent(CIwEvent* pEvent)
{
  CIwManaged* sender = pEvent->GetSender();
  if (pEvent->GetID() == IWUI_EVENT_BUTTON)
  {
    if (sender == mOKButton)
    {
      mFinished = true;
      return true;
    }
  }
  return false;
}


//---------------------------------------------------------------------------------------------------------------------
void DisplayWhatsNewMenu(GameSettings& gameSettings)
{
  AudioManager::GetInstance().SetAllChannelsToZeroVolume();
  FixStateForMenus fixStateForMenus(true, true);
  WhatsNewMenu* settingsMenu = new WhatsNewMenu(gameSettings); // Gets deleted on IwGetUIView()->DestroyElements()

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

    if (resourcesHaveChanged)
    {
      IwGetUIView()->DestroyElements();
      settingsMenu = new WhatsNewMenu(gameSettings); // Gets deleted on IwGetUIView()->DestroyElements()
    }
  }
  IwGetUIView()->DestroyElements();
}
