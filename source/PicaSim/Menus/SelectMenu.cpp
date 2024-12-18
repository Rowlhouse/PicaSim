#include "SelectMenu.h"
#include "SettingsWidgets.h"

#include "Menu.h"
#include "Framework.h"
#include "../GameSettings.h"

//---------------------------------------------------------------------------------------------------------------------
class SelectMenu : public Menu
{
public:
  SelectMenu(const GameSettings& gameSettings, const char* items[], int numItems, const char* titleText);
  ~SelectMenu();

  void CreateButton(CIwUIElement* pContainer, CIwUILayoutVertical* layout, const char* label);

  virtual bool HandleEvent(CIwEvent* pEvent) OVERRIDE;

  typedef std::vector<CIwUIButton*> Buttons;
  Buttons mButtons;

  CIwUIButton* mCancelButton;

  int mResult;
};

//---------------------------------------------------------------------------------------------------------------------
SelectMenu::~SelectMenu()
{
}

//---------------------------------------------------------------------------------------------------------------------
bool SelectMenu::HandleEvent(CIwEvent* pEvent) 
{
  if (pEvent->GetID() == IWUI_EVENT_BUTTON)
  {
    CIwManaged* sender = pEvent->GetSender();
    if (sender == mCancelButton)
    {
      mResult = -1;
      return true;
    }
    for (size_t i = 0 ; i != mButtons.size() ; ++i)
    {
      if (sender == mButtons[i])
      {
        mResult = i; // allow for the cancel button
        return true;
      }
    }
  }
  return false;
}

//---------------------------------------------------------------------------------------------------------------------
void SelectMenu::CreateButton(CIwUIElement* pContainer, CIwUILayoutVertical* layout, const char* label)
{
  CIwUIButton* button = new CIwUIButton;
  button->SetStyle("<button_wide>");
  button->SetCaption(label);
  button->SetSizeToContent(false);
  pContainer->AddChild(button);
  layout->AddElement(button);

  mButtons.push_back(button);
}

//---------------------------------------------------------------------------------------------------------------------
SelectMenu::SelectMenu(
  const GameSettings& gameSettings, 
  const char* items[], 
  int numItems,
  const char* titleText) : mResult(-2)
{
  const Language language = gameSettings.mOptions.mLanguage;

  int height = gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int width = gameSettings.mOptions.mFrameworkSettings.mScreenWidth;

  // Screen area
  CIwUIImage* screenArea = new CIwUIImage;
  screenArea->SetSizeToContent(false);
  CIwTexture* texture = (CIwTexture*)IwGetResManager()->GetResNamed("MenuBackground", "CIwTexture");
  screenArea->SetTexture(texture);

  // Picks up all the events
  screenArea->AddEventHandler(this); 

  // Layout
  CIwUILayoutGrid* screenLayout = new CIwUILayoutGrid;
  screenLayout->SetSizeToSpace(true); // Stops the filename view pushing everything else down
  screenLayout->AddColumn();
  screenLayout->AddRow(); // Nav bar
  screenLayout->AddRow();
  screenLayout->AddRow(); // padding
  screenArea->SetLayout(screenLayout);

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

  CIwUILabel* titleLabel = CreateLabel(gameSettings, navigationArea, navigationLayout, titleText, LABEL_TYPE_CENTERED_TITLE);

  // Create the scrollabel buttons area
  CIwUIScrollableView* pContainer = new CIwUIScrollableView;
  pContainer->SetSizeToContent(false);
  pContainer->SetSizeHint(CIwVec2(0, height));

  // Set the layout
  CIwUILayoutVertical* layout = new CIwUILayoutVertical;
  layout->SetSizeToSpace(true);
  pContainer->SetLayout(layout);

  screenLayout->AddElement(pContainer, 0, 1);
  screenArea->AddChild(pContainer);

  // Create the buttons
  for (int i = 0 ; i != numItems ; ++i)
  {
    CreateButton(pContainer, layout, items[i]);
  }

  IwGetUIView()->AddElement(screenArea);
  IwGetUIView()->AddElementToLayout(screenArea);

  // Add padding if necessary
  IwGetUIController()->Update();
  CIwVec2 titleSize = titleLabel->GetSize();
  CIwVec2 cancelSize = mCancelButton->GetSize();
  int w = (width - titleSize.x)/2 - cancelSize.x;
  if (w > 1)
    spacer->SetSizeMin(CIwVec2(w,-1));
}


//---------------------------------------------------------------------------------------------------------------------
int SelectItemMenu(const GameSettings& gameSettings, const char* items[], int numItems, const char* titleText)
{
  SelectMenu* selectMenu = new SelectMenu(gameSettings, items, numItems, titleText);
  while (selectMenu->mResult == -2)
  {
    bool resourcesHaveChanged;
    selectMenu->Update(true, true, resourcesHaveChanged, gameSettings);
    if (
      s3eDeviceCheckQuitRequest()!=0 ||
      (s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED)
      )
    {
      selectMenu->mResult = -1;
    }
  }
  int result = selectMenu->mResult;
  IwGetUIView()->DestroyElements(); // delete selectMenu
  return result;
}