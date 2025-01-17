#include "ScoreloopSetData.h"

#ifdef ENABLE_SCORELOOP

// #include "SettingsWidgets.h"
// #include "Menu.h"
// #include "Helpers.h"
// #include "../GameSettings.h"

// #include <IwUI.h>
// #include <IwUITextField.h>

// #include <string>
// #include <vector>
// #include <list>


//---------------------------------------------------------------------------------------------------------------------
class SetMenu : public Menu
{
public:
  SetMenu(const struct GameSettings& gameSettings, ScoreloopSetType scoreloopSetType);

  ~SetMenu();

  bool HandleEvent(CIwEvent* pEvent) OVERRIDE;
  int32 Update(bool keepAwake, bool throttle, const struct GameSettings& gameSettings) OVERRIDE;

  enum State
  {
    STATE_INPUT,
    STATE_SUBMITTED,
    STATE_FINISHED
  };
  State GetState() const {return mState;}

  void OnUserSetDataCompleted(SC_Error_t completionStatus);

private:
  void CreateContentAndButtonsAreaAndLayout(const GameSettings& gameSettings, const char* caption, const char* title);
  void AddTextEntry(const GameSettings& gameSettings, CIwUIElement* pContainer, CIwUILayoutGrid* layout, const char* caption);

  CIwUIButton* mOKButton;
  CIwUIButton* mCancelButton;
  CIwUITextField* mTextField;
  CIwUILabel* mCaption;

  SC_UserController_h mUserController;

  State mState;
  ScoreloopSetType mScoreloopSetMode;
  const GameSettings& mGameSettings;
};

//---------------------------------------------------------------------------------------------------------------------
SetMenu::~SetMenu()
{
  if (mUserController)
    SC_UserController_Release(mUserController);
}

//---------------------------------------------------------------------------------------------------------------------
void SetMenu::CreateContentAndButtonsAreaAndLayout(const GameSettings& gameSettings, const char* caption, const char* title)
{
  const Language language = gameSettings.mOptions.mLanguage;
  int height = gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int width = gameSettings.mOptions.mFrameworkSettings.mScreenWidth;

  // Layout
  CIwUILayoutGrid* screenLayout = new CIwUILayoutGrid;
  screenLayout->SetSizeToSpace(true); // Stops the filename view pushing everything else down
  screenLayout->AddRow();
  screenLayout->AddRow();
  screenLayout->AddRow();
  screenLayout->AddRow();
  screenLayout->AddColumn();

  // Screen area
  CIwUIImage* screenArea = new CIwUIImage;
  screenArea->SetSizeToContent(false);
  screenArea->SetLayout(screenLayout);
  CIwTexture* texture = (CIwTexture*)IwGetResManager()->GetResNamed("MenuBackground", "CIwTexture");
  screenArea->SetTexture(texture);

  // Navigation bar area
  CIwUILabel* navigationArea = new CIwUILabel;
  navigationArea->SetStyle("<label_bar_background>");
  navigationArea->SetSizeToContent(false);
  screenArea->AddChild(navigationArea);
  screenLayout->AddElement(navigationArea, 0, 0);
  // Layout
  CIwUILayoutHorizontal* navigationLayout = new CIwUILayoutHorizontal;
  navigationLayout->SetSizeToSpace(true);
  navigationArea->SetLayout(navigationLayout);
  // Cancel button
  mCancelButton = new CIwUIButton;
  mCancelButton->SetStyle("<button_wide>");
  mCancelButton->SetCaption(TXT(PS_BACK));
  navigationLayout->AddElement(mCancelButton);
  navigationArea->AddChild(mCancelButton);

  CIwUIElement* spacer = new CIwUIElement;
  navigationLayout->AddElement(spacer);
  navigationArea->AddChild(spacer);

  CIwUILabel* titleLabel = CreateLabel(gameSettings, navigationArea, navigationLayout, title, LABEL_TYPE_CENTERED_TITLE);
  titleLabel->SetSizeToContent(false);

  // Text entry
  AddTextEntry(gameSettings, screenArea, screenLayout, caption);

  // Add a blank section that absorbs the remaining space
  CIwUIElement* blank = new CIwUIElement;
  blank->SetSizeToContent(false);
  blank->SetSizeHint(CIwVec2(0, height));
  screenArea->AddChild(blank);
  screenLayout->AddElement(blank, 0, 2);

  // Buttons area
  CIwUILabel* buttonsArea = new CIwUILabel;
  buttonsArea->SetStyle("<label_bar_background>");
  buttonsArea->SetSizeToContent(true);

  screenArea->AddChild(buttonsArea);
  screenLayout->AddElement(buttonsArea, 0, 3);

  // Buttons layout
  CIwUILayoutGrid* buttonsLayout = new CIwUILayoutGrid;
  buttonsLayout->SetSizeToSpace(true);
  buttonsLayout->AddRow();
  buttonsArea->SetLayout(buttonsLayout);

  // Buttons

  // OK button
  mOKButton = new CIwUIButton;
  mOKButton->SetStyle("<button_expanding>");
  mOKButton->SetCaption(TXT(PS_OK));
  buttonsLayout->AddColumn(0, width);
  buttonsLayout->AddElement(mOKButton, 0, 0);
  buttonsArea->AddChild(mOKButton);

  IwGetUIView()->AddElement(screenArea);
  IwGetUIView()->AddElementToLayout(screenArea);

  // Picks up all the events
  screenArea->AddEventHandler(this); 

  // Add padding if necessary
  IwGetUIController()->Update();
  CIwVec2 titleSize = titleLabel->GetSize();
  CIwVec2 backSize = mCancelButton->GetSize();
  int w = (width - titleSize.x)/2 - backSize.x;
  if (w > 1)
    spacer->SetSizeMin(CIwVec2(w,-1));
}

//---------------------------------------------------------------------------------------------------------------------
int32 SetMenu::Update(bool keepAwake, bool throttle, const struct GameSettings& gameSettings)
{
  return Menu::Update(keepAwake, throttle, gameSettings);
}

//---------------------------------------------------------------------------------------------------------------------
bool SetMenu::HandleEvent(CIwEvent* pEvent) 
{
  const Language language = mGameSettings.mOptions.mLanguage;
  if (pEvent->GetID() == IWUI_EVENT_BUTTON)
  {
    CIwManaged* sender = pEvent->GetSender();
    if (sender == mOKButton)
    {
      std::string newData = mTextField->GetCaption();

      if (newData.length() == 0)
      {
        mState = STATE_FINISHED;
        return true;
      }

      SC_Session_h session = SC_Client_GetSession(gScoreloopClient);
      SC_User_h user = SC_Session_GetUser(session);

      if (mScoreloopSetMode == SCORELOOPSETTYPE_USERNAME)
      {
         SC_User_SetLogin(user, newData.c_str());
      }
      else if (mScoreloopSetMode == SCORELOOPSETTYPE_EMAIL)
      {
        SC_User_SetEmail(user, newData.c_str());
      }

      SC_UserController_UpdateUser(mUserController);
      SC_UserValidationError_t validationError = SC_UserController_GetValidationErrors(mUserController);

      mCaption->SetCaption(TXT(PS_UPLOADINGDATA));

      mState = STATE_SUBMITTED;
      return true;
    }
    if (sender == mCancelButton)
    {
      mState = STATE_FINISHED;
      return true;
    }
  }
  return false;
}

//---------------------------------------------------------------------------------------------------------------------
void SetMenu::OnUserSetDataCompleted(SC_Error_t completionStatus)
{
  const Language language = mGameSettings.mOptions.mLanguage;
  if (completionStatus == SC_OK)
  {
    mState = STATE_FINISHED;
  }
  else
  {
    SC_UserValidationError_t validationError = SC_UserController_GetValidationErrors(mUserController);
    char txt[1024];
    sprintf(txt, TXT(PS_FAILED));
    if (validationError & SC_EMAIL_ALREADY_TAKEN)
      strcat(txt, TXT(PS_EMAILTAKEN));
    if (validationError & SC_EMAIL_FORMAT_INVALID)
      strcat(txt, TXT(PS_EMAILINVALID));
    if (validationError & SC_USERNAME_ALREADY_TAKEN)
      strcat(txt, TXT(PS_USERTAKEN));
    if (validationError & SC_USERNAME_FORMAT_INVALID)
      strcat(txt, TXT(PS_USERINVALID));
    if (validationError & SC_USERNAME_TOO_SHORT)
      strcat(txt, TXT(PS_USERSHORT));

    mCaption->SetCaption(txt);
    mState = STATE_INPUT;
  }
}

//---------------------------------------------------------------------------------------------------------------------
void UserSetDataCallback(void* userData, SC_Error_t completionStatus)
{
  TRACE_FILE_IF(1) TRACE("UserSetDataCallback %d", completionStatus);
  IwAssert(ROWLHOUSE, userData);
  if (!userData)
    return;
  ((SetMenu*) userData)->OnUserSetDataCompleted(completionStatus);
}

//---------------------------------------------------------------------------------------------------------------------
SetMenu::SetMenu(const struct GameSettings& gameSettings, ScoreloopSetType scoreloopSetType) 
  : mState(STATE_INPUT), mScoreloopSetMode(scoreloopSetType), mGameSettings(gameSettings)
{
  int errorCode = SC_Client_CreateUserController(gScoreloopClient, &mUserController, &UserSetDataCallback, this);
  const Language language = gameSettings.mOptions.mLanguage;

  const char* caption = "unset";
  const char* title = "unset";
  switch (scoreloopSetType)
  {
  case SCORELOOPSETTYPE_USERNAME: caption = TXT(PS_USERNAME); title = TXT(PS_SETUSERNAME); break;
  case SCORELOOPSETTYPE_EMAIL: caption = TXT(PS_EMAIL); title = TXT(PS_SETEMAIL); break;
  }
  CreateContentAndButtonsAreaAndLayout(gameSettings, "", title);
}

//---------------------------------------------------------------------------------------------------------------------
void SetMenu::AddTextEntry(const GameSettings& gameSettings, CIwUIElement* pContainer, CIwUILayoutGrid* layout, const char* caption)
{
  CIwUIElement* textArea = new CIwUIElement;
  textArea->SetSizeToContent(true);

  pContainer->AddChild(textArea);
  layout->AddElement(textArea, 0, 1);

  CIwUILayoutVertical* textLayout = new CIwUILayoutVertical;
  textLayout->SetSizeToSpace(false);
  textArea->SetLayout(textLayout);

  mTextField = new CIwUITextField;
  mTextField->SetStyle("<textField_inline>");

  textArea->AddChild(mTextField);
  textLayout->AddElement(mTextField);

  mCaption = CreateLabel(gameSettings, textArea, textLayout, caption, LABEL_TYPE_SETTING);
}

//---------------------------------------------------------------------------------------------------------------------
void ScoreloopSetData(const struct GameSettings& gameSettings, ScoreloopSetType scoreloopSetType)
{
  SetMenu* setMenu = new SetMenu(gameSettings, scoreloopSetType);
  while (setMenu->GetState() != SetMenu::STATE_FINISHED)
  {
    setMenu->Update(gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_ANDROID, true, gameSettings);
  }
  IwGetUIView()->DestroyElements(); // delete fileMenu
}


#else

void ScoreloopSetData(const struct GameSettings& gameSettings, ScoreloopSetType scoreloopSetType)
{
}
#endif
