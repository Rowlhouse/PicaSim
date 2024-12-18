#include "ScoreloopMenu.h"
#if ENABLE_SCORELOOP

#include "SettingsWidgets.h"
#include "SelectMenu.h"
#include "FileMenu.h"
#include "Menu.h"
#include "ScoreloopSetData.h"
#include "../GameSettings.h"
#include "../Scoreloop.h"
#include "PicaDialog.h"

#include <s3eOSReadString.h>

#define LABEL(caption) CreateLabel(mGameSettings, scrollArea, scrollLayout, caption, LABEL_TYPE_HELP_TEXT)
#define LABEL_CENTRE(caption) CreateLabel(mGameSettings, scrollArea, scrollLayout, caption, LABEL_TYPE_SETTING)
#define CHECKBOX(caption, value) mCheckboxes.push_back(Checkbox(gameSettings, scrollArea, scrollLayout, caption, value))
#define SLIDER_INT(caption, units, value, minVal, maxVal) mIntSliders.push_back(IntSlider(gameSettings, scrollArea, scrollLayout, caption, units, value, minVal, maxVal))
#define SLIDER_FLOAT(caption, units, value, minVal, maxVal) mFloatSliders.push_back(FloatSlider(gameSettings, scrollArea, scrollLayout, caption, units, value, minVal, maxVal))
#define SLIDER_ENUM(caption, value, texts, numTexts) mEnums.push_back(Enum(gameSettings, scrollArea, scrollLayout, caption, value, texts, numTexts))
#define SLIDER_STRING(caption, value, texts) mStrings.push_back(String(gameSettings, scrollArea, scrollLayout, caption, value, texts))

static int buttonSizeFraction = 10;

static unsigned int SCORE_RANGE = 20;

enum LeaderboardType
{
  LEADERBOARDTYPE_GLOBAL,
  LEADERBOARDTYPE_24H,
  LEADERBOARDTYPE_7DAYS,
  LEADERBOARDTYPE_30DAYS,
  LEADERBOARDTYPE_MAX,
};

//---------------------------------------------------------------------------------------------------------------------
enum ScoreloopStatus
{
  SCORELOOP_UNSET,
  SCORELOOP_BACK,
  SCORELOOP_LOAD_CHALLENGE,
  SCORELOOP_SELECT_LEADERBOARD,
  SCORELOOP_SET_USERNAME,
  SCORELOOP_SET_EMAIL,
};


//---------------------------------------------------------------------------------------------------------------------
class ScoreloopMenu : public Menu
{
public:
  enum TabPanelEnum
  {
    TAB_PROFILE,
    TAB_LEADERBOARD,
    TAB_NUM_TABS
  };

  ScoreloopMenu(
    GameSettings& gameSettings, 
    TabPanelEnum initialTab, 
    LeaderboardType leaderboardType, 
    const Score* score, 
    bool uploadScore,
    bool allowChallengeChange,
    const ChallengeSettings& challengeSettings);

  ~ScoreloopMenu();

  int32 Update(bool keepAwake, bool throttle, const struct GameSettings& gameSettings) OVERRIDE;

  ScoreloopStatus mStatus;

  struct TabPanel
  {
    TabPanel();
    CIwUIElement* mContentArea;
    CIwUILayout* mContentLayout;
    CIwUIElement* mStatusArea;
    CIwUILayout* mStatusLayout;

    CIwUILabel* mStatusLabel;
  };

  void OnLeaderboardCreated(SC_Error_t completionStatus);
  void OnScoreSubmitted(SC_Error_t completionStatus);
  void OnUserRequested(SC_Error_t completionStatus);

  SC_User_h GetCurrentUser() const {return mCurrentUser;}

private:
  void CreateProfileUI();
  void CreateLeaderboardUI(const char* initialText, bool allowChallengeChange);
  void ReadAvailableChallenges();

  virtual bool HandleEvent(CIwEvent* pEvent) OVERRIDE;

  void GetLeaderboard();
  void UploadScore(const Score& score);

  CIwUILayoutGrid* mGridLayout;
  CIwUIImage* mGridArea;
  bool mHaveDisplayedNetworkFailure;

  uint16   mSelectedTab;
  TabPanel mTabPanels[TAB_NUM_TABS];

  GameSettings& mGameSettings;
  const ChallengeSettings& mChallengeSettings;

  CIwUIButton* mBackButton;
  CIwUITabBar* mTabBar;

  SC_UserController_h mUserController;
  SC_ScoresController_h mScoresController;
  SC_ScoreController_h mScoreController;
  SC_Score_h mScoreloopScore;

  SC_User_h mCurrentUser;

  // Leaderboard items
  CIwUIButton* mChallengeButton;
  CIwUIButton* mLeaderboardTypeButton;
  CIwUIScrollableView* mLeaderboardScrollArea;
  CIwUILayoutVertical* mLeaderboardScrollLayout;

  CIwUIButton* mLeaderboardPrevButton;
  CIwUIButton* mLeaderboardTopButton;
  CIwUIButton* mLeaderboardMeButton;
  CIwUIButton* mLeaderboardNextButton;

  const Score* mScore;

  LeaderboardType mLeaderboardType;

  // Profile items
  CIwUIButton* mUsernameImageButton;
  CIwUIButton* mEmailImageButton;
  CIwUILabel* mUsernameLabel;
  CIwUILabel* mEmailLabel;
};

//---------------------------------------------------------------------------------------------------------------------
ScoreloopMenu::TabPanel::TabPanel() 
  : mContentArea(0), mContentLayout(0), mStatusArea(0), mStatusLayout(0)
{}

//---------------------------------------------------------------------------------------------------------------------
int32 ScoreloopMenu::Update(bool keepAwake, bool throttle, const struct GameSettings& gameSettings)
{
  if (mSelectedTab == TAB_LEADERBOARD)
    mScrollArea = mLeaderboardScrollArea;
  else
    mScrollArea = 0;
  return Menu::Update(keepAwake, throttle, gameSettings);
}

//---------------------------------------------------------------------------------------------------------------------
void CreateLeaderboardEntry(
  const GameSettings& gameSettings, const ChallengeSettings& challengeSettings,
  CIwUIElement* parentArea, CIwUILayout* parentLayout, 
  unsigned int rank, const char* name, double result, double minorResult, bool highlight)
{
  const Language language = gameSettings.mOptions.mLanguage;
  int width = gameSettings.mOptions.mFrameworkSettings.mScreenWidth;
  int height = gameSettings.mOptions.mFrameworkSettings.mScreenHeight;

  // Horizontal area
  CIwUIElement* horizontalArea = new CIwUIElement;
  horizontalArea->SetSizeToContent(false);
  parentArea->AddChild(horizontalArea);
  parentLayout->AddElement(horizontalArea);

  float rankSize = 0.09f * width;
  float nameSize = 0.3f * (width - rankSize);
  float scoreSize = 0.7f * (width - rankSize);

  // Horizontal Layout
  CIwUILayoutGrid* gridLayout = new CIwUILayoutGrid;
  gridLayout->SetSizeToSpace(true);
  gridLayout->AddColumn((int32) rankSize);
  gridLayout->AddColumn((int32) nameSize);
  gridLayout->AddColumn((int32) scoreSize);
  gridLayout->AddRow();//(int32) rankSize);
  horizontalArea->SetLayout(gridLayout);

  char rankString[16];
  sprintf(rankString, "%d", rank);
  CIwUILabel* rankLabel = new CIwUILabel;
  rankLabel->SetStyle("<label_centered>");
  rankLabel->SetCaption(rankString);
  rankLabel->SetSizeToContent(false);
  rankLabel->SetProperty("oneLine", true);
  rankLabel->SetProperty("aspectRatio", CIwVec2(1,1));

  horizontalArea->AddChild(rankLabel);
  gridLayout->AddElement(rankLabel, 0, 0);

  // username
  CIwUILabel* nameLabel = new CIwUILabel;
  nameLabel->SetStyle("<label_left>");
  nameLabel->SetCaption(name);
  nameLabel->SetSizeToContent(false);
  if (highlight)
  {
    Colour col;
    col.Set(255, 0, 0);
    nameLabel->SetTextColour(col);
  }

  horizontalArea->AddChild(nameLabel);
  gridLayout->AddElement(nameLabel, 1, 0);

  // Score/time
  char scoreString[128];
  if (challengeSettings.mChallengeMode != ChallengeSettings::CHALLENGE_LIMBO)
    sprintf(scoreString, TXT(PS_SCORELINERACE), result, minorResult);
  else
    sprintf(scoreString, TXT(PS_SCORELINELIMBO), result, (int) (0.5f + minorResult));
  CIwUILabel* scoreLabel = new CIwUILabel;
  scoreLabel->SetStyle("<label_left>");
  scoreLabel->SetCaption(scoreString);
  scoreLabel->SetSizeToContent(false);

  horizontalArea->AddChild(scoreLabel);
  gridLayout->AddElement(scoreLabel, 2, 0);
}

//---------------------------------------------------------------------------------------------------------------------
void ScoreloopMenu::OnLeaderboardCreated(SC_Error_t completionStatus)
{
  const Language language = mGameSettings.mOptions.mLanguage;
  if (completionStatus == SC_OK)
  {
    SC_ScoreList_h scoreList = SC_ScoresController_GetScores(mScoresController);
    if (scoreList)
    {
      CIwUIElement* scrollArea = mLeaderboardScrollArea;
      CIwUILayoutVertical* scrollLayout = mLeaderboardScrollLayout;

      // Clear out anything already there
      while (scrollArea->GetNumChildren())
      {
        CIwUIElement* child = scrollArea->GetChild(0);
        scrollArea->RemoveChild(child);
        scrollLayout->RemoveElement(child);
        delete child;
      }

      unsigned int numScores = SC_ScoreList_GetCount(scoreList);
      for (unsigned int iScore = 0 ; iScore != numScores ; ++iScore)
      {
        SC_Score_h score = SC_ScoreList_GetAt(scoreList, iScore);
        double scoreResult = SC_Score_GetResult(score);
        double scoreMinorResult = SC_Score_GetMinorResult(score);
        unsigned int scoreMode = SC_Score_GetMode(score);
        unsigned int scoreRank = SC_Score_GetRank(score);
        SC_User_h user = SC_Score_GetUser(score);
        SC_String_h userLogin = SC_User_GetLogin(user);
        const char* userString = SC_String_GetData(userLogin);

        bool highlight = false;
        if (mCurrentUser)
        {
          SC_String_h currentUserLogin = SC_User_GetLogin(mCurrentUser);
          if (currentUserLogin)
          {
            const char* currentUserString = SC_String_GetData(currentUserLogin);
            highlight = (userString && currentUserString && strcmp(userString, currentUserString) == 0);
          }
        }

        TabPanel& tabPanel = mTabPanels[TAB_LEADERBOARD];
        CreateLeaderboardEntry(mGameSettings, mChallengeSettings,
          scrollArea, scrollLayout, scoreRank, userString, scoreResult, scoreMinorResult, highlight);
      }
      if (numScores == 0)
      {
        LABEL_CENTRE(TXT(PS_NOSCORES));
      }
      else
      {
        mLeaderboardPrevButton->SetEnabled(true);
        mLeaderboardNextButton->SetEnabled(true);
        mLeaderboardTopButton->SetEnabled(true);
        mLeaderboardMeButton->SetEnabled(true);
      }

      if (mScore)
      {
        char text[512];
        sprintf(text, TXT(PS_CURRENTSCORE), mScore->mResult, mScore->mMinorResult);
        mTabPanels[TAB_LEADERBOARD].mStatusLabel->SetCaption(text);
      }
      else
      {
        mTabPanels[TAB_LEADERBOARD].mStatusLabel->SetCaption("");
      }
    }
    else
    {
      // Got a successful completion, but no scores
      mTabPanels[TAB_LEADERBOARD].mStatusLabel->SetCaption(TXT(PS_NOSCORESINTIME));
    }
  }
  else
  {
    if (!mHaveDisplayedNetworkFailure)
    {
      ShowDialog("PicaSim", TXT(PS_LEADERBOARDFAILED), TXT(PS_OK));

      mHaveDisplayedNetworkFailure = true;
    }

    mTabPanels[TAB_LEADERBOARD].mStatusLabel->SetCaption(TXT(PS_LEADERBOARDFAILEDRETRYING));
    SC_Range_t range = {0, SCORE_RANGE};
    int errorCode = SC_ScoresController_LoadScores(mScoresController,range);
    TRACE_FILE_IF(1) TRACE("SC_ScoresController_LoadScores - %d", errorCode);
  }
}

//---------------------------------------------------------------------------------------------------------------------
void LeaderboardCreationCallback(void* userData, SC_Error_t completionStatus)
{
  TRACE_FILE_IF(1) TRACE("LeaderboardCreationCallback %d", completionStatus);
  IwAssert(ROWLHOUSE, userData);
  if (!userData)
    return;
  ((ScoreloopMenu*) userData)->OnLeaderboardCreated(completionStatus);
}

//---------------------------------------------------------------------------------------------------------------------
void ScoreloopMenu::OnScoreSubmitted(SC_Error_t completionStatus)
{
  const Language language = mGameSettings.mOptions.mLanguage;
  if (completionStatus == SC_OK)
  {
    mTabPanels[TAB_LEADERBOARD].mStatusLabel->SetCaption(TXT(PS_RETRIEVINGLEADERBOARD));
    GetLeaderboard();
  }
  else
  {
    if (!mHaveDisplayedNetworkFailure)
    {
      ShowDialog("PicaSim", TXT(PS_SCORESUBMISSIONFAILED), TXT(PS_OK));
      mHaveDisplayedNetworkFailure = true;
    }
    mTabPanels[TAB_LEADERBOARD].mStatusLabel->SetCaption(TXT(PS_SCORESUBMISSIONFAILEDRETRYING));
    IwAssert(ROWLHOUSE, mScoreloopScore);
    SC_ScoreController_SubmitScore(mScoreController, mScoreloopScore);
  }
}

//---------------------------------------------------------------------------------------------------------------------
static void ScoreSubmissionCallback(void* userData, SC_Error_t completionStatus)
{
  TRACE_FILE_IF(1) TRACE("ScoreSubmissionCallback %d", completionStatus);
  IwAssert(ROWLHOUSE, userData);
  if (!userData)
    return;
  ((ScoreloopMenu*) userData)->OnScoreSubmitted(completionStatus);
}

//---------------------------------------------------------------------------------------------------------------------
void ScoreloopMenu::OnUserRequested(SC_Error_t completionStatus)
{
  const Language language = mGameSettings.mOptions.mLanguage;
  if (completionStatus == SC_OK)
  {
    mCurrentUser = SC_UserController_GetUser(mUserController);
    mTabPanels[TAB_PROFILE].mStatusLabel->SetCaption("");

    SC_String_h userLogin = SC_User_GetLogin(mCurrentUser);
    SC_String_h userEmail = SC_User_GetEmail(mCurrentUser);

    char txt[512];

    if (userLogin)
    {
      sprintf(txt, "%s", SC_String_GetData(userLogin));
      mGameSettings.mStatistics.mLastScoreloopUsername = txt;
    }
    else
    {
      sprintf(txt, "%s", TXT(PS_SETUSERNAME));
    }
    mUsernameLabel->SetCaption(txt);

    if (userEmail)
      sprintf(txt, "%s", SC_String_GetData(userEmail));
    else
      sprintf(txt, "%s", TXT(PS_SETEMAIL));
    mEmailLabel->SetCaption(txt);
  }
  else
  {
    if (!mHaveDisplayedNetworkFailure)
    {
      ShowDialog("PicaSim", TXT(PS_USERFAILED), TXT(PS_OK));
      mHaveDisplayedNetworkFailure = true;
    }
    mTabPanels[TAB_PROFILE].mStatusLabel->SetCaption(TXT(PS_USERFAILEDRETRYING));
    SC_UserController_GetUser(mUserController);
  }
}

//---------------------------------------------------------------------------------------------------------------------
void UserRequestCallback(void* userData, SC_Error_t completionStatus)
{
  TRACE_FILE_IF(1) TRACE("UserRequestCallback %d", completionStatus);
  IwAssert(ROWLHOUSE, userData);
  if (!userData)
    return;
  ((ScoreloopMenu*) userData)->OnUserRequested(completionStatus);
}

//---------------------------------------------------------------------------------------------------------------------
void ScoreloopMenu::GetLeaderboard()
{
  SC_ScoresSearchList_t searchList;
  
  switch (mLeaderboardType)
  {
  case LEADERBOARDTYPE_24H: searchList.timeInterval = SC_TIME_INTERVAL_24H; break;
  case LEADERBOARDTYPE_7DAYS: searchList.timeInterval = SC_TIME_INTERVAL_7DAYS; break;
  case LEADERBOARDTYPE_30DAYS: searchList.timeInterval = SC_TIME_INTERVAL_30DAYS; break;
  case LEADERBOARDTYPE_GLOBAL: 
  default: searchList.timeInterval = SC_TIME_INTERVAL_ALL; break;
  }
  searchList.countrySelector = SC_COUNTRY_SELECTOR_ALL;
  searchList.country = 0;
  searchList.usersSelector = SC_USERS_SELECTOR_ALL;
  searchList.buddyhoodUser = 0;

  int errorCode = SC_ScoresController_SetSearchList(mScoresController, searchList);
  errorCode = SC_ScoresController_SetMode(mScoresController, mChallengeSettings.mChallengeID);
  SC_Range_t range = {0, SCORE_RANGE};
  errorCode = SC_ScoresController_LoadScores(mScoresController, range);
}

//---------------------------------------------------------------------------------------------------------------------
void ScoreloopMenu::UploadScore(const Score& score)
{
  TRACE_FILE_IF(1) TRACE("Uploading Scoreloop score");
  SC_Error_t errorCode;
  if (!mScoreloopScore)
    SC_Client_CreateScore(gScoreloopClient, &mScoreloopScore);
  SC_Score_SetResult(mScoreloopScore, score.mResult);
  SC_Score_SetMinorResult(mScoreloopScore, score.mMinorResult);
  SC_Score_SetMode(mScoreloopScore, score.mMode);

  errorCode = SC_ScoreController_SubmitScore(mScoreController, mScoreloopScore);
  TRACE_FILE_IF(1) TRACE("Uploading Scoreloop score - result = %d", errorCode);
}

//---------------------------------------------------------------------------------------------------------------------
ScoreloopMenu::ScoreloopMenu(
  GameSettings& gameSettings, 
  TabPanelEnum initialTab, 
  LeaderboardType leaderboardType, 
  const Score* score, 
  bool uploadScore, 
  bool allowChallengeChange,
  const ChallengeSettings& challengeSettings)
  : mGameSettings(gameSettings), 
  mChallengeSettings(challengeSettings), 
  mSelectedTab(initialTab), 
  mLeaderboardType(leaderboardType),
  mStatus(SCORELOOP_UNSET), mScore(score)
{
  const Language language = gameSettings.mOptions.mLanguage;

  mHaveDisplayedNetworkFailure = false;

  mScoresController = 0;
  mScoreController = 0;
  mUserController = 0;

  mScoreloopScore = 0;
  mCurrentUser = 0;

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

  // navigation/Tab area and buttons
  {
    // Tab area
    CIwUILabel* tabArea = new CIwUILabel;
    tabArea->SetStyle("<label_bar_background>");
    tabArea->SetSizeToContent(false);
    mGridArea->AddChild(tabArea);
    mGridLayout->AddElement(tabArea, 0, 0);

    // Layout
    CIwUILayoutHorizontal* horizontalLayout = new CIwUILayoutHorizontal;
    horizontalLayout->SetSizeToSpace(true);
    tabArea->SetLayout(horizontalLayout);

    // Back button
    mBackButton = new CIwUIButton;
    mBackButton->SetStyle("<button_wide>");
    mBackButton->SetCaption(TXT(PS_BACK));
    horizontalLayout->AddElement(mBackButton);
    tabArea->AddChild(mBackButton);

    spacer = new CIwUIElement;
    horizontalLayout->AddElement(spacer);
    tabArea->AddChild(spacer);

    // Tab bar
    // Make a horizontal scroll area for it
    CIwUIScrollableView* tabBarArea = new CIwUIScrollableView;
    tabBarArea->SetSizeToContent(true);

    CIwUILayoutHorizontal* tabBarLayout = new CIwUILayoutHorizontal;
    tabBarLayout->SetSizeToSpace(false);
    tabBarArea->SetLayout(tabBarLayout);

    horizontalLayout->AddElement(tabBarArea);
    tabArea->AddChild(tabBarArea);

    // And the tab bar itself
    mTabBar = new CIwUITabBar;
    mTabBar->SetSizeToContent(true);
    mTabBar->SetRadioButtonStyle(0, "<radio_left>");
    mTabBar->SetRadioButtonStyle(1, "<radio_right>");
    mTabBar->SetRadioButtonCaption(0, TXT(PS_PROFILE));
    mTabBar->SetRadioButtonCaption(1, TXT(PS_LEADERBOARDS));
    mTabBar->SetSelected(mSelectedTab);

    tabBarArea->AddChild(mTabBar);
    tabBarLayout->AddElement(mTabBar);
  }

  CreateProfileUI();
  if (uploadScore)
    CreateLeaderboardUI(TXT(PS_UPLOADINGSCORE), allowChallengeChange);
  else
    CreateLeaderboardUI(TXT(PS_RETRIEVINGLEADERBOARD), allowChallengeChange);

  // Persist the selected tab
  mGridArea->AddChild(mTabPanels[mSelectedTab].mContentArea);
  mGridLayout->AddElement(mTabPanels[mSelectedTab].mContentArea, 0, 1);
  mGridArea->AddChild(mTabPanels[mSelectedTab].mStatusArea);
  mGridLayout->AddElement(mTabPanels[mSelectedTab].mStatusArea, 0, 2);

  IwGetUIView()->AddElement(mGridArea);
  IwGetUIView()->AddElementToLayout(mGridArea);

  // Get the leaderboard
  int errorCode = SC_Client_CreateScoresController(gScoreloopClient, &mScoresController, &LeaderboardCreationCallback, this);
  errorCode = SC_Client_CreateScoreController(gScoreloopClient, &mScoreController, &ScoreSubmissionCallback, this);
  errorCode = SC_Client_CreateUserController(gScoreloopClient, &mUserController, &UserRequestCallback, this);

  // Set the user request going
  errorCode = SC_UserController_LoadUser(mUserController);
  mTabPanels[TAB_PROFILE].mStatusLabel->SetCaption(TXT(PS_REQUESTINGUSER));

  // Set the leaderboard request going
  if (score && uploadScore)
    UploadScore(*score);
  else if (mChallengeSettings.mChallengeID != 0)
    GetLeaderboard();
  else
    mTabPanels[TAB_LEADERBOARD].mStatusLabel->SetCaption(TXT(PS_SELECTRACE));

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
void CreateContentAndStatusAreaAndLayout(
  const GameSettings& gameSettings,
  ScoreloopMenu::TabPanel& tabPanel,
  CIwUIScrollableView*& scrollArea,
  CIwUILayoutVertical*& scrollLayout,
  const char* tabName,
  const char* statusText)
{
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

  // Layout
  scrollLayout = new CIwUILayoutVertical;
  scrollLayout->SetSizeToSpace(true);
  scrollArea->SetLayout(scrollLayout);

  // Buttons area
  tabPanel.mStatusArea = new CIwUILabel;
  tabPanel.mStatusArea->SetStyle("<label_bar_background>");
  tabPanel.mStatusArea->SetSizeToContent(false);

  // Layout
  tabPanel.mStatusLayout = new CIwUILayout;
  tabPanel.mStatusLayout->SetSizeToSpace(true);
  tabPanel.mStatusArea->SetLayout(tabPanel.mStatusLayout);

  // Label
  tabPanel.mStatusLabel =  CreateLabel(gameSettings, tabPanel.mStatusArea, tabPanel.mStatusLayout, statusText, LABEL_TYPE_CENTERED_TITLE);
  tabPanel.mStatusLabel->SetSizeToContent(false);
}

//---------------------------------------------------------------------------------------------------------------------
void CreateProfileRow(
  const GameSettings& gameSettings,
  CIwUIScrollableView* scrollArea, CIwUILayoutVertical* scrollLayout,
  CIwUILabel*& label, const char* captionText, const char* subCaptionText, CIwUIButton*& imageButton,
  const char* image, const char* labelText)
{
  const Language language = gameSettings.mOptions.mLanguage;

  int height = gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int width = gameSettings.mOptions.mFrameworkSettings.mScreenWidth;

  extern float settingsWidthFraction;
  int areaWidth = (int) (width * settingsWidthFraction);

  CIwUIElement* rowArea = new CIwUIElement;
  rowArea->SetSizeToContent(false);
  rowArea->SetSizeMax(CIwVec2(areaWidth, -1));

  // Layout
  CIwUILayoutGrid* rowLayout = new CIwUILayoutGrid;
  rowLayout->SetSizeToSpace(true);
  int buttonSize = areaWidth/5;
  rowLayout->AddColumn(areaWidth - buttonSize);
  rowLayout->AddColumn(buttonSize);
  rowLayout->AddRow();
  rowLayout->AddRow();
  rowLayout->AddRow();
  rowLayout->AddRow();
  rowArea->SetLayout(rowLayout);

  // Add the caption
  CIwUILabel* captionLabel = new CIwUILabel;
  captionLabel->SetStyle("<label_setting_block_title>");
  captionLabel->SetCaption(captionText);
  rowLayout->AddElement(captionLabel, 0, 0, 2, 1, IW_UI_ALIGN_LEFT);
  rowArea->AddChild(captionLabel);

  // Add the text button
  label = new CIwUILabel;
  label->SetCaption(labelText);
  label->SetStyle("<label_setting_background>");

  rowLayout->AddElement(label, 0, 1);
  rowArea->AddChild(label);

  // Add the image
  imageButton = new CIwUIButton;
  imageButton->SetStyle("<button_wide>");
  imageButton->SetCaption(TXT(PS_EDIT));
  imageButton->SetSizeToContent(false);

  rowLayout->AddElement(imageButton, 1, 1);
  rowArea->AddChild(imageButton);

  if (subCaptionText)
  {
    CIwUILabel* subCaption = new CIwUILabel;
    subCaption->SetStyle("<label_right_small_faint>");
    subCaption->SetCaption(subCaptionText);
    subCaption->SetProperty("alignH", IW_UI_ALIGN_LEFT);
    rowLayout->AddElement(subCaption, 0, 2, 2, 1);
    rowArea->AddChild(subCaption);
  }

  // Add it all
  scrollLayout->AddElement(rowArea);
  scrollArea->AddChild(rowArea);

}

//---------------------------------------------------------------------------------------------------------------------
void ScoreloopMenu::CreateProfileUI()
{
  const Language language = mGameSettings.mOptions.mLanguage;
  CIwUIScrollableView* scrollArea;
  CIwUILayoutVertical* scrollLayout;
  CreateContentAndStatusAreaAndLayout(mGameSettings, mTabPanels[TAB_PROFILE], scrollArea, scrollLayout, TXT(PS_PROFILE), TXT(PS_PROFILESTATUS));

  CreateProfileRow(mGameSettings, scrollArea, scrollLayout, mUsernameLabel, TXT(PS_SCORELOOPUSERNAME), NULL, mUsernameImageButton, TXT(PS_USERNAME), mGameSettings.mStatistics.mLastScoreloopUsername.c_str());
  CreateProfileRow(mGameSettings, scrollArea, scrollLayout, mEmailLabel, TXT(PS_EMAIL), TXT(PS_OPTIONALEMAIL), mEmailImageButton, TXT(PS_EMAIL), TXT(PS_FETCHINGEMAIL));
}

//---------------------------------------------------------------------------------------------------------------------
void ScoreloopMenu::CreateLeaderboardUI(const char* initialText, bool allowChallengeChange)
{
  const Language language = mGameSettings.mOptions.mLanguage;
  int height = mGameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int width = mGameSettings.mOptions.mFrameworkSettings.mScreenWidth;

  // Content area
  mTabPanels[TAB_LEADERBOARD].mContentArea = new CIwUIElement;
  mTabPanels[TAB_LEADERBOARD].mContentArea->SetSizeToContent(false);

  // Layout
  mTabPanels[TAB_LEADERBOARD].mContentLayout = new CIwUILayoutVertical;
  mTabPanels[TAB_LEADERBOARD].mContentLayout->SetSizeToSpace(true); // Stops the scroll view pushing everything else down
  mTabPanels[TAB_LEADERBOARD].mContentArea->SetLayout(mTabPanels[TAB_LEADERBOARD].mContentLayout);

  // Leaderboard title area
  // Pair of buttons to select the leaderboard etc
  CIwUIElement* leaderboardTitleArea = new CIwUIElement;
  leaderboardTitleArea->SetSizeToContent(false);

  // Layout
  CIwUILayoutGrid* leaderboardTitleLayout = new CIwUILayoutGrid;
  leaderboardTitleLayout->SetSizeToSpace(true);
  leaderboardTitleLayout->AddColumn();
  leaderboardTitleLayout->AddColumn();
  leaderboardTitleLayout->AddRow();
  leaderboardTitleArea->SetLayout(leaderboardTitleLayout);

  mTabPanels[TAB_LEADERBOARD].mContentLayout->AddElement(leaderboardTitleArea);
  mTabPanels[TAB_LEADERBOARD].mContentArea->AddChild(leaderboardTitleArea);

  // Now the title buttons
  if (allowChallengeChange)
  {
    mChallengeButton = new CIwUIButton;
    mChallengeButton->SetStyle("<button_expanding>");
    mChallengeButton->SetCaption(mChallengeSettings.mTitle.c_str());
    leaderboardTitleLayout->AddElement(mChallengeButton, 0, 0);
    leaderboardTitleArea->AddChild(mChallengeButton);
  }
  else
  {
    mChallengeButton = 0;
    CIwUILabel* label = new CIwUILabel;
    label->SetStyle("<label_title_centered>");
    label->SetCaption(mChallengeSettings.mTitle.c_str());
    leaderboardTitleLayout->AddElement(label, 0, 0);
    leaderboardTitleArea->AddChild(label);
  }
  mLeaderboardTypeButton = new CIwUIButton;
  mLeaderboardTypeButton->SetStyle("<button_expanding>");
  const char* leaderboardTypeStrings[] = 
  {
    TXT(PS_ALLRESULTS),
    TXT(PS_LAST24HOURS),
    TXT(PS_LAST7DAYS),
    TXT(PS_LAST30DAYS)
  };
  mLeaderboardTypeButton->SetCaption(leaderboardTypeStrings[mLeaderboardType]);

  leaderboardTitleLayout->AddElement(mLeaderboardTypeButton, 1, 0);
  leaderboardTitleArea->AddChild(mLeaderboardTypeButton);

  // Leaderboard content area and layout
  CIwUILayout* leaderboardBackgroundLayout = new CIwUILayout;
  leaderboardBackgroundLayout->SetSizeToSpace(true);

  CIwUILabel* leaderboardBackgroundArea = new CIwUILabel;
  leaderboardBackgroundArea->SetStyle("<label_setting_background>");
  leaderboardBackgroundArea->SetSizeMin(CIwVec2(width*15/16, -1));
  leaderboardBackgroundArea->SetSizeMax(CIwVec2(width*15/16, -1));
  leaderboardBackgroundArea->SetSizeToContent(true);
  leaderboardBackgroundArea->SetLayout(leaderboardBackgroundLayout);

  mTabPanels[TAB_LEADERBOARD].mContentArea->AddChild(leaderboardBackgroundArea);
  mTabPanels[TAB_LEADERBOARD].mContentLayout->AddElement(leaderboardBackgroundArea);


  mLeaderboardScrollArea = new CIwUIScrollableView;
  mLeaderboardScrollArea->SetSizeToContent(false);
  mLeaderboardScrollArea->SetSizeHint(CIwVec2(-1, height));
  leaderboardBackgroundArea->AddChild(mLeaderboardScrollArea);
  leaderboardBackgroundLayout->AddElement(mLeaderboardScrollArea);

  // Layout
  mLeaderboardScrollLayout = new CIwUILayoutVertical;
  mLeaderboardScrollLayout->SetSizeToSpace(true);
  mLeaderboardScrollArea->SetLayout(mLeaderboardScrollLayout);

  // Status area
  mTabPanels[TAB_LEADERBOARD].mStatusArea = new CIwUIElement;
  mTabPanels[TAB_LEADERBOARD].mStatusArea->SetSizeToContent(true);

  // Layout
  mTabPanels[TAB_LEADERBOARD].mStatusLayout = new CIwUILayoutVertical;
  mTabPanels[TAB_LEADERBOARD].mStatusLayout->SetSizeToSpace(false);
  mTabPanels[TAB_LEADERBOARD].mStatusArea->SetLayout(mTabPanels[TAB_LEADERBOARD].mStatusLayout);

  // Leaderboard navigation buttons
  CIwUIElement* navArea = new CIwUIElement;
  navArea->SetSizeToContent(true);

  CIwUILayoutGrid* navLayout = new CIwUILayoutGrid;
  navLayout->SetSizeToSpace(true);
  navLayout->AddRow();
  navLayout->AddColumn(width/4);
  navLayout->AddColumn(width/4);
  navLayout->AddColumn(width/4);
  navLayout->AddColumn(width/4);
  navArea->SetLayout(navLayout);

  mTabPanels[TAB_LEADERBOARD].mStatusLayout->AddElement(navArea);
  mTabPanels[TAB_LEADERBOARD].mStatusArea->AddChild(navArea);

  mLeaderboardPrevButton = new CIwUIButton;
  mLeaderboardPrevButton->SetStyle("<button_expanding>");
  mLeaderboardPrevButton->SetCaption(TXT(PS_PREV));
  navLayout->AddElement(mLeaderboardPrevButton, 0, 0);
  navArea->AddChild(mLeaderboardPrevButton);

  mLeaderboardTopButton = new CIwUIButton;
  mLeaderboardTopButton->SetStyle("<button_expanding>");
  mLeaderboardTopButton->SetCaption(TXT(PS_TOP));
  navLayout->AddElement(mLeaderboardTopButton, 1, 0);
  navArea->AddChild(mLeaderboardTopButton);

  mLeaderboardMeButton = new CIwUIButton;
  mLeaderboardMeButton->SetStyle("<button_expanding>");
  mLeaderboardMeButton->SetCaption(TXT(PS_ME));
  navLayout->AddElement(mLeaderboardMeButton, 2, 0);
  navArea->AddChild(mLeaderboardMeButton);

  mLeaderboardNextButton = new CIwUIButton;
  mLeaderboardNextButton->SetStyle("<button_expanding>");
  mLeaderboardNextButton->SetCaption(TXT(PS_NEXT));
  navLayout->AddElement(mLeaderboardNextButton, 3, 0);
  navArea->AddChild(mLeaderboardNextButton);

  mLeaderboardPrevButton->SetEnabled(false);
  mLeaderboardNextButton->SetEnabled(false);
  mLeaderboardTopButton->SetEnabled(false);
  mLeaderboardMeButton->SetEnabled(false);

  // Label
  CIwUILabel* statusArea = new CIwUILabel;
  statusArea->SetStyle("<label_bar_background>");
  statusArea->SetSizeToContent(true);

  CIwUILayout* statusLayout = new CIwUILayout;
  statusLayout->SetSizeToSpace(true);
  statusArea->SetLayout(statusLayout);

  mTabPanels[TAB_LEADERBOARD].mStatusArea->AddChild(statusArea);
  mTabPanels[TAB_LEADERBOARD].mStatusLayout->AddElement(statusArea);

  mTabPanels[TAB_LEADERBOARD].mStatusLabel = CreateLabel(
    mGameSettings, statusArea, statusLayout, initialText, LABEL_TYPE_CENTERED_TITLE);
  mTabPanels[TAB_LEADERBOARD].mStatusLabel->SetSizeToContent(false);
  //mTabPanels[TAB_LEADERBOARD].mStatusLabel->SetStyle("<label_bar_background>");
}


//---------------------------------------------------------------------------------------------------------------------
ScoreloopMenu::~ScoreloopMenu()
{
  for (uint16 i = 0 ; i != TAB_NUM_TABS ; ++i)
  {
    if (i != mSelectedTab)
    {
      delete mTabPanels[i].mContentArea;
      delete mTabPanels[i].mStatusArea;
    }
  }

  if (mScoresController)
    SC_ScoresController_Release(mScoresController);
  if (mScoreController)
    SC_ScoreController_Release(mScoreController);
  if (mScoreloopScore)
    SC_Score_Release(mScoreloopScore);
  if (mUserController)
    SC_UserController_Release(mUserController);
}

//---------------------------------------------------------------------------------------------------------------------
bool ScoreloopMenu::HandleEvent(CIwEvent* pEvent)
{
  const Language language = mGameSettings.mOptions.mLanguage;
  CIwManaged* sender = pEvent->GetSender();
  if (pEvent->GetID() == IWUI_EVENT_BUTTON)
  {
    if (sender == mLeaderboardPrevButton)
    {
      SC_Bool_t result = SC_ScoresController_HasPreviousRange(mScoresController);
      if (result && result)
      {
        int errorCode = SC_ScoresController_LoadPreviousRange(mScoresController);
        mTabPanels[TAB_LEADERBOARD].mStatusLabel->SetCaption(TXT(PS_UPDATINGLEADERBOARD));
      }
      else
      {
        mTabPanels[TAB_LEADERBOARD].mStatusLabel->SetCaption(TXT(PS_NOMORESCORES));
      }
    }
    else if (sender == mLeaderboardNextButton)
    {
      SC_Bool_t result = SC_ScoresController_HasNextRange(mScoresController);
      if (result && result)
      {
        int errorCode = SC_ScoresController_LoadNextRange(mScoresController);
        mTabPanels[TAB_LEADERBOARD].mStatusLabel->SetCaption(TXT(PS_UPDATINGLEADERBOARD));
      }
      else
      {
        mTabPanels[TAB_LEADERBOARD].mStatusLabel->SetCaption(TXT(PS_NOMORESCORES));
      }
    }
    else if (sender == mLeaderboardTopButton)
    {
      SC_Range_t range = {0, SCORE_RANGE};
      int errorCode = SC_ScoresController_LoadScores(mScoresController, range);
      mTabPanels[TAB_LEADERBOARD].mStatusLabel->SetCaption(TXT(PS_UPDATINGLEADERBOARD));
    }
    else if (sender == mLeaderboardMeButton)
    {
      SC_Range_t range = {0, SCORE_RANGE};
      int errorCode = SC_ScoresController_LoadScoresAroundUser(mScoresController, mCurrentUser, SCORE_RANGE);
      mTabPanels[TAB_LEADERBOARD].mStatusLabel->SetCaption(TXT(PS_UPDATINGLEADERBOARD));
    }
    else if (sender == mBackButton)
    {
      mStatus = SCORELOOP_BACK;
      return true;
    }
    else if (sender == mChallengeButton)
    {
      mStatus = SCORELOOP_LOAD_CHALLENGE;
      return true;
    }
    else if (sender == mLeaderboardTypeButton)
    {
      mStatus = SCORELOOP_SELECT_LEADERBOARD;
      return true;
    }
    else if (sender == mUsernameImageButton)
    {
      {
        mStatus = SCORELOOP_SET_USERNAME;
      }
      return true;
    }
    else if (sender == mEmailImageButton)
    {
      {
        mStatus = SCORELOOP_SET_EMAIL;
      }
      return true;
    }
  }
  else if (pEvent->GetID() == IWUI_EVENT_TABBAR)
  {
    if (sender == mTabBar)
    {
      if (mUpdateCounter > 5)
      {
        // remove
        mGridArea->RemoveChild(mTabPanels[mSelectedTab].mContentArea);
        mGridLayout->RemoveElement(mTabPanels[mSelectedTab].mContentArea);
        mGridArea->RemoveChild(mTabPanels[mSelectedTab].mStatusArea);
        mGridLayout->RemoveElement(mTabPanels[mSelectedTab].mStatusArea);

        mSelectedTab = mTabBar->GetSelected();

        mGridArea->AddChild(mTabPanels[mSelectedTab].mContentArea);
        mGridLayout->AddElement(mTabPanels[mSelectedTab].mContentArea, 0, 1);
        mGridArea->AddChild(mTabPanels[mSelectedTab].mStatusArea);
        mGridLayout->AddElement(mTabPanels[mSelectedTab].mStatusArea, 0, 2);

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
void ShowScoreloopMenu(
  GameSettings& gameSettings, 
  ScoreloopMenu::TabPanelEnum initialTab, 
  const Score* score, 
  bool uploadScore,
  bool allowChallengeChange,
  const ChallengeSettings* challengeSettings,
  const char* challengeButtonTitle)
{
  TRACE_FUNCTION_ONLY(1);
  AudioManager::GetInstance().SetAllChannelsToZeroVolume();
  FixStateForMenus fixStateForMenus(true, true);

  ScoreloopStatus scoreloopStatus = SCORELOOP_UNSET;

  const Language language = gameSettings.mOptions.mLanguage;
  ChallengeSettings cs;
  cs.mTitle = TXT(PS_SELECTRACE);

  if (challengeSettings)
    cs = *challengeSettings;
  // Dont force the leaderboard for challenge 1 (which would be reasonable) because we don't have easy 
  // access to the chalenge names yet.

  LeaderboardType leaderboardType = LEADERBOARDTYPE_30DAYS;

  if (!score)
    uploadScore = false;

  while (true)
  {
    {
      ScoreloopMenu* scoreloopMenu = new ScoreloopMenu(
        gameSettings, initialTab, leaderboardType, score, uploadScore, allowChallengeChange, cs); // Gets deleted on IwGetUIView()->DestroyElements()

      // Only upload the score the first time
      score = 0;
      uploadScore = false;

      while (true)
      {
        scoreloopMenu->Update(gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_ANDROID, true, gameSettings);

        if (
          s3eDeviceCheckQuitRequest()!=0 ||
          scoreloopMenu->mStatus == SCORELOOP_BACK ||
          (s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED)
          )
        {
          IwGetUIView()->DestroyElements();
          return;
        }

        if (scoreloopMenu->mStatus != SCORELOOP_UNSET)
        {
          scoreloopStatus = scoreloopMenu->mStatus;
          break;
        }

        AudioManager::GetInstance().Update(1.0f/30.0f);
      }
      IwGetUIView()->DestroyElements();
    }

    const Language language = gameSettings.mOptions.mLanguage;
    if (scoreloopStatus == SCORELOOP_LOAD_CHALLENGE)
    {
      TRACE_FILE_IF(1) TRACE("Loading challenge from file for scoreloop menu");
      std::string file;
      FileMenuLoad(file, gameSettings, "SystemSettings/Challenge", NULL, ".xml", TXT(PS_SELECTARACE), TXT(PS_BACK), NULL);
      if (!file.empty())
      {
        TRACE_FILE_IF(1) TRACE("Loading challenge %s", file.c_str());
        bool result = cs.LoadFromFile(file);

        IwAssert(ROWLHOUSE, result);
        TRACE_FILE_IF(1) TRACE(" %s\n", result ? "success" : "failed");
      }
      else
      {
        TRACE_FILE_IF(1) TRACE("Got cancel on challenge choice");
      }
      initialTab = ScoreloopMenu::TAB_LEADERBOARD;
    }
    else if (scoreloopStatus == SCORELOOP_SELECT_LEADERBOARD)
    {
      const char* leaderboardTypeStrings[] = 
      {
        TXT(PS_ALLRESULTS),
        TXT(PS_LAST24HOURS),
        TXT(PS_LAST7DAYS),
        TXT(PS_LAST30DAYS)
      };
      int newType = SelectItemMenu(gameSettings, leaderboardTypeStrings, LEADERBOARDTYPE_MAX, TXT(PS_TIMEPERIODFORRESULTS));
      if (newType >= 0 && newType < LEADERBOARDTYPE_MAX)
        leaderboardType = (LeaderboardType) newType;
      initialTab = ScoreloopMenu::TAB_LEADERBOARD;
      TRACE_FILE_IF(1) TRACE("New leaderboard type = %d", leaderboardType);
    }
    else if (scoreloopStatus == SCORELOOP_SET_USERNAME)
    {
      ScoreloopSetData(gameSettings, SCORELOOPSETTYPE_USERNAME);
      initialTab = ScoreloopMenu::TAB_PROFILE;
    }
    else if (scoreloopStatus == SCORELOOP_SET_EMAIL)
    {
      ScoreloopSetData(gameSettings, SCORELOOPSETTYPE_EMAIL);
      initialTab = ScoreloopMenu::TAB_PROFILE;
    }
  }
}


//---------------------------------------------------------------------------------------------------------------------
void DisplayScoreloopMenu(GameSettings& gameSettings)
{
  const Language language = gameSettings.mOptions.mLanguage;
  ShowScoreloopMenu(gameSettings, ScoreloopMenu::TAB_PROFILE, 0, false, true, 0, TXT(PS_SELECTRACE));
}

//---------------------------------------------------------------------------------------------------------------------
void UploadScoreloop(struct GameSettings& gameSettings, const Score& score)
{
  ShowScoreloopMenu(gameSettings, ScoreloopMenu::TAB_LEADERBOARD, &score, true, false, &gameSettings.mChallengeSettings, gameSettings.mChallengeSettings.mTitle.c_str());
}

//---------------------------------------------------------------------------------------------------------------------
void DisplayScoreloopMenuLeaderboard(struct GameSettings& gameSettings, const Score* score)
{
  ShowScoreloopMenu(gameSettings, ScoreloopMenu::TAB_LEADERBOARD, score, false, false, &gameSettings.mChallengeSettings, gameSettings.mChallengeSettings.mTitle.c_str());
}

#else

void DisplayScoreloopMenu(struct GameSettings& gameSettings) {}

void DisplayScoreloopMenuLeaderboard(struct GameSettings& gameSettings, const Score* score) {}

void UploadScoreloop(struct GameSettings& gameSettings, const Score& score) {}

#endif