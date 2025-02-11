#include "FileMenu.h"
#include "File.h"
#include "FonctionsMarmelade.h"
#include "FonctionsPointerKeyboardMarmelade.h"
// #include "GameSettings.h"
#include "Menu.h"
// #include <cstring>
// #include "Entete.h"
// #include "ClassesUIMarmelade.h"
#include "SettingsWidgets.h"
// #include "Helpers.h"
#include "PicaDialog.h"
// #include "GameSettings.h"
#include "Trace.h"

// #include <IwUI.h>
// #include <IwUITextField.h>
// #include <s3eSocket.h>
// #include <s3eOSExec.h>

// #include <string>
// #include <vector>
// #include <list>

static const size_t MAX_FILENAME_LEN = 48;

static const char validChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-=_+&!(){}[]#";
static const size_t numValidChars = sizeof(validChars);

//---------------------------------------------------------------------------------------------------------------------
bool IsCharValid(char c)
{
  for (size_t i = 0 ; i != numValidChars-1 ; ++i)
  {
    if (c == validChars[i])
      return true;
  }
  return false;
}

//---------------------------------------------------------------------------------------------------------------------
void RemoveInvalidCharacters(std::string& text)
{
  char result[MAX_FILENAME_LEN+1] = "";
  const char* origText = text.c_str();

  size_t origLen = strlen(origText); // not including null
  if (origLen > MAX_FILENAME_LEN)
    origLen = MAX_FILENAME_LEN;

  for (size_t i = 0 ; i != origLen ; ++i)
  {
    result[i] = origText[i];
    if (!IsCharValid(result[i]))
      result[i] = '_';
  }
  result[origLen] = 0;
  text = result;
}

//---------------------------------------------------------------------------------------------------------------------
class FileMenu : public Menu
{
public:
  enum Mode {LOAD, SAVE, DELETE};

  FileMenu(
    const GameSettings& gameSettings, 
    const char*         systemPath, 
    const char*         userPath, 
    const char*         extension, 
    const Mode          mode, 
    const char*         title,
    const char*         tabTitles[], 
    const size_t        numTabs,
    const int           selectedTab,
    const char*         cancelButtonText,
    const char*         altButtonText, 
    FileMenuType        fileMenuType,
    float               imagesPerScreen,
    IncludeCallback*    includeCallback);

  ~FileMenu();

  bool HandleEvent(CIwEvent* pEvent) OVERRIDE;
  int32_t Update(bool keepAwake, bool throttle, bool& resourcesHaveChanged, const struct GameSettings& gameSettings) OVERRIDE;

  // This will return -1 if the menu has been closed by selecting something rather than switching tab
  int GetSelectedTab() const {return mSelectedTab;}

  int AddFiles(const GameSettings& gameSettings, const char* path, const char* extension, CIwUIElement* pContainer, 
    CIwUILayoutVertical* layout, const char* labelText, FileMenuType fileMenuType, float imagesPerScreen,
    bool useTitleFromFile, IncludeCallback* includeCallback);
  void AddNewFile(const GameSettings& gameSettings, CIwUIElement* pContainer, CIwUILayoutGrid* layout);

  bool GetFinished() const {return mFinished;}

  FileMenuResult GetResult(std::string& selectedString) const {selectedString = mSelectedString; return mResult;}
private:
  struct Button
  {
    Button() : mTextButton(0), mImageButton(0), mAvailable(true), mUse(true) {}
    CIwUIButton* mTextButton;
    CIwUIButton* mImageButton;
    bool mAvailable;
    bool mUse;
  };
  typedef std::vector<Button> Buttons;

  Button CreateButton(
    const GameSettings& gameSettings, 
    CIwUIElement* pContainer, 
    CIwUILayoutVertical* layout, 
    const std::string& fullPath, 
    const char* label, 
    FileMenuType fileMenuType, 
    float imagesPerScreen,
    bool useTitleFromFile);

  void CreateContentAndButtonsAreaAndLayout(
    const GameSettings&   gameSettings, 
    CIwUIScrollableView*& filenameArea, 
    CIwUILayoutVertical*& filenameLayout, 
    const char*           cancelButtonText, 
    const char*           altButtonText,
    const char*           title,
    const char*           tabTitles[],
    const size_t          numTabs);

  CIwUIButton* mOKButton;
  CIwUIButton* mAltButton;
  CIwUIButton* mCancelButton;
  CIwUITextField* mTextField;
  CIwUIScrollableView* mFilenameArea;
  CIwUITabBar* mTabBar;

  Buttons mButtons;
  std::vector<std::string> mFilenames;

  std::string mSelectedString;
  FileMenuResult mResult;

  int mNumLines;
  bool mFinished;
  Mode mMode;
  int mSelectedTab;

  const GameSettings& mGameSettings;
};

//---------------------------------------------------------------------------------------------------------------------
FileMenu::~FileMenu()
{
}

//---------------------------------------------------------------------------------------------------------------------
FileMenu::Button FileMenu::CreateButton(
  const GameSettings& gameSettings, 
  CIwUIElement* pContainer, 
  CIwUILayoutVertical* layout, 
  const std::string& fullPath, 
  const char* caption, 
  FileMenuType fileMenuType,
  float imagesPerScreen,
  bool useTitleFromFile)
{
  Button result;
  if (fileMenuType == FILEMENUTYPE_FILE)
  {
    result.mTextButton = new CIwUIButton;
    result.mTextButton->SetStyle("<button_expanding>");
    result.mTextButton->SetSizeToContent(false);
    result.mTextButton->SetCaption(caption);
    pContainer->AddChild(result.mTextButton);
    layout->AddElement(result.mTextButton);
    return result;
  }
  else
  {
    int height = gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
    int width = gameSettings.mOptions.mFrameworkSettings.mScreenWidth;

    std::string info;
    std::string title;
    Texture* texture = 0;

    CIwColour unavailableCol;
    unavailableCol.r = 255;
    unavailableCol.g = 255;
    unavailableCol.b = 255;
    unavailableCol.a = 64;

    bool requestFocus = false;
    switch (fileMenuType)
    {
    case FILEMENUTYPE_AEROPLANE:
      {
        AeroplaneSettings as;
        as.LoadBasicsFromFile(fullPath);
        info = as.mInfo;
        title = as.mTitle;
        if (mSelectedTab > 0)
          result.mUse = (as.mType & (1 << (mSelectedTab-1))) != 0;
        texture = result.mUse && as.mThumbnail.empty() ? 0 : GetCachedTexture(as.mThumbnail, gameSettings.mOptions.m16BitTextures);
        if (title == gameSettings.mAeroplaneSettings.mTitle)
          requestFocus = true;
        if (as.mAvailability > gameSettings.mStatistics.mAppType)
          result.mAvailable = false;
      }
      break;
    case FILEMENUTYPE_SCENERY:
      {
        EnvironmentSettings es;
        es.LoadBasicsFromFile(fullPath);
        info = es.mInfo;
        title = es.mTitle;
        if (mSelectedTab > 0)
          result.mUse = (es.mType & (1 << (mSelectedTab-1))) != 0;
        texture = result.mUse && es.mThumbnail.empty() ? 0 : GetCachedTexture(es.mThumbnail, gameSettings.mOptions.m16BitTextures);
        if (title == gameSettings.mEnvironmentSettings.mTitle)
          requestFocus = true;
        if (es.mAvailability > gameSettings.mStatistics.mAppType)
          result.mAvailable = false;
      }
      break;
    case FILEMENUTYPE_CHALLENGE:
      {
        ChallengeSettings cs;
        cs.LoadBasicsFromFile(fullPath);
        info = cs.mInfo;
        title = cs.mTitle;
        texture = cs.mThumbnail.empty() ? 0 : GetCachedTexture(cs.mThumbnail, gameSettings.mOptions.m16BitTextures);
      }
      break;
    case FILEMENUTYPE_LIGHTING:
      {
        LightingSettings ls;
        ls.LoadBasicsFromFile(fullPath);
        info = ls.mInfo;
        title = ls.mTitle;
        texture = ls.mThumbnail.empty() ? 0 : GetCachedTexture(ls.mThumbnail, gameSettings.mOptions.m16BitTextures);
        if (title == gameSettings.mLightingSettings.mTitle)
          requestFocus = true;
      }
      break;
    }

    if (!result.mUse)
      return result;

    if (!useTitleFromFile)
    {
      title = caption;
      info.clear();
    }

    // Summary area
    CIwUIElement* summaryArea = new CIwUIElement;
    summaryArea->SetSizeToContent(false);

    // Summary layout
    CIwUILayoutHorizontal* summaryLayout = new CIwUILayoutHorizontal;
    summaryLayout->SetSizeToSpace(true);
    summaryArea->SetLayout(summaryLayout);

    // Add the texture
    int32_t imageW = 0;
    int32_t imageH = (int32_t) (height/imagesPerScreen);
    if (texture)
    {
      uint32_t w = texture->GetWidth();
      uint32_t h = texture->GetHeight();
      if (w == 0 || h == 0)
        return result;

      CIwUIStyle style;
      style.InlinePropertySet();
      CIwUIPropertySet* pImageProps = style.GetInlinePropertySet();
      pImageProps->SetProperty("drawableType", IwHashString("image"));
      pImageProps->SetProperty("texture", texture);

      float ar = float(w)/float(h);
      result.mImageButton = new CIwUIButton;
      result.mImageButton->SetProperty("aspectRatio", CIwVec2(w,h));
      imageW = int32_t(ar * height/imagesPerScreen);
      result.mImageButton->SetSizeMin(CIwVec2(imageW, imageH));
      result.mImageButton->SetSizeMax(CIwVec2(imageW, imageH));
      result.mImageButton->SetProperty("buttonUp", style);
      result.mImageButton->SetProperty("buttonDown", style);
      if (!result.mAvailable)
      {
        result.mImageButton->SetColour(unavailableCol);
        CIwGxFont* font = GetFont(gameSettings);
        Language language = gameSettings.mOptions.mLanguage;
        result.mImageButton->SetFont(font);
        result.mImageButton->SetCaption(TXT(PS_INFULLVERSION));
        result.mImageButton->SetProperty("alignH", IW_UI_ALIGN_CENTRE);
        result.mImageButton->SetProperty("alignV", IW_UI_ALIGN_BOTTOM);
        CIwUIColour textColour(0,0,0,128);
        result.mImageButton->SetTextColour(textColour);
      }

      summaryLayout->AddElement(result.mImageButton, 0, IW_UI_ALIGN_CENTRE, IW_UI_ALIGN_MIDDLE, CIwSVec2(2,2));
      summaryArea->AddChild(result.mImageButton);
    }
    else
    {
      result.mImageButton = 0;
    }
    // Add the info

    // Summary area
    CIwUIElement* infoArea = new CIwUIElement;
    infoArea->SetSizeToContent(true);

    // Summary layout
    CIwUILayoutVertical* infoLayout = new CIwUILayoutVertical;
    infoLayout->SetSizeToSpace(true);
    infoArea->SetLayout(infoLayout);

    summaryArea->AddChild(infoArea);
    summaryLayout->AddElement(infoArea, 0, IW_UI_ALIGN_LEFT, IW_UI_ALIGN_MIDDLE);

    std::string buttonString;
    if (title.empty())
     buttonString = info;
    else if (info.empty())
     buttonString = title;
    else
     buttonString = title + ": " + info;

    result.mTextButton = new CIwUIButton;
    result.mTextButton->SetStyle("<button_expanding>");
    result.mTextButton->SetCaption(buttonString.c_str());
    result.mTextButton->SetSizeMin(CIwVec2(width-imageW, imageH));
    result.mTextButton->SetSizeMax(CIwVec2(width-imageW, imageH));
    if (!result.mAvailable)
    {
      result.mTextButton->SetColour(unavailableCol);
      CIwColour col;
      col.r = 128;
      col.g = 128;
      col.b = 128;
      col.a = 255;
      result.mTextButton->SetTextColour(col);
    }
    infoArea->AddChild(result.mTextButton);
    infoLayout->AddElement(result.mTextButton);

    // attach to the parent
    pContainer->AddChild(summaryArea);
    layout->AddElement(summaryArea, result.mAvailable ? 0 : 10);

    if (requestFocus)
    {
      result.mTextButton->RequestFocus();
    }
    return result;

  }
}

//---------------------------------------------------------------------------------------------------------------------
void FileMenu::CreateContentAndButtonsAreaAndLayout(
  const GameSettings& gameSettings,
  CIwUIScrollableView*& filenameArea, 
  CIwUILayoutVertical*& filenameLayout,
  const char* cancelButtonText,
  const char* altButtonText,
  const char* title,
  const char* tabTitles[],
  const size_t numTabs)
{
  const Language language = gameSettings.mOptions.mLanguage;

  int height = gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int width = gameSettings.mOptions.mFrameworkSettings.mScreenWidth;

  // Screen area
  CIwUIImage* screenArea = new CIwUIImage;
  screenArea->SetSizeToContent(false);
  CIwTexture* texture = (CIwTexture*)IwGetResManager()->GetResNamed("MenuBackground", "CIwTexture");
  screenArea->SetTexture(texture);

  // Layout
  CIwUILayoutGrid* screenLayout = new CIwUILayoutGrid;
  screenLayout->SetSizeToSpace(true); // Stops the filename view pushing everything else down
  screenLayout->AddColumn();
  screenLayout->AddRow(); // Nav bar
  if (mMode == SAVE)
  {
    // Text entry plus a label
    screenLayout->AddRow();
    AddNewFile(gameSettings, screenArea, screenLayout);
  }
  screenLayout->AddRow();
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
  mCancelButton->SetCaption(cancelButtonText);
  navigationLayout->AddElement(mCancelButton);
  navigationArea->AddChild(mCancelButton);

  CIwUIElement* spacer1 = new CIwUIElement;
  navigationLayout->AddElement(spacer1);
  navigationArea->AddChild(spacer1);

  CIwUILabel* titleLabel = 0;
  mTabBar = 0;
  if (title)
  {
    titleLabel = CreateLabel(gameSettings, navigationArea, navigationLayout, title, LABEL_TYPE_CENTERED_TITLE);
  }

  CIwUIElement* spacer2 = new CIwUIElement;
  navigationLayout->AddElement(spacer2);
  navigationArea->AddChild(spacer2);

  if (numTabs && tabTitles)
  {
    mTabBar = new CIwUITabBar;
    mTabBar->SetSizeToContent(false);
    for (size_t i = 0 ; i != numTabs ; ++i)
    {
      if (i == 0)
        mTabBar->SetRadioButtonStyle((int16_t) i, "<radio_left>");
      else if (i == numTabs - 1)
        mTabBar->SetRadioButtonStyle((int16_t) i, "<radio_right>");
      else
        mTabBar->SetRadioButtonStyle((int16_t) i, "<radio_centre>");
      mTabBar->SetRadioButtonCaption((int16_t) i, tabTitles[i]);
    }
    mTabBar->SetSelected(mSelectedTab);
    navigationArea->AddChild(mTabBar);
    navigationLayout->AddElement(mTabBar, mTabBar->GetStyle());
  }

  // Filename Layout
  filenameLayout = new CIwUILayoutVertical;
  filenameLayout->SetSizeToSpace(true);

  // Filename area
  filenameArea = new CIwUIScrollableView;
  filenameArea->SetSizeToContent(false);
  filenameArea->SetSizeHint(CIwVec2(-1, height));
  filenameArea->SetLayout(filenameLayout);

  screenArea->AddChild(filenameArea);
  if (mMode == SAVE)
    screenLayout->AddElement(filenameArea, 0, 2);
  else
    screenLayout->AddElement(filenameArea, 0, 1);

  // Buttons layout
  int numButtons = mMode == SAVE ? 1 : 0;
  numButtons += altButtonText ? 1 : 0;
  if (numButtons)
  {
    screenLayout->AddRow();
    // Buttons area
    CIwUILabel* buttonsArea = new CIwUILabel;
    buttonsArea->SetStyle("<label_bar_background>");
    buttonsArea->SetSizeToContent(true);

    screenArea->AddChild(buttonsArea);
    if (mMode == SAVE)
      screenLayout->AddElement(buttonsArea, 0, 3);
    else
      screenLayout->AddElement(buttonsArea, 0, 2);

    CIwUILayoutGrid* buttonsLayout = new CIwUILayoutGrid;
    buttonsLayout->SetSizeToSpace(true);
    buttonsLayout->AddRow();
    buttonsArea->SetLayout(buttonsLayout);

    // Buttons

    mOKButton = 0;
    mAltButton = 0;

    int iButton = 0;
    // OK button
    if (mMode == SAVE)
    {
      mOKButton = new CIwUIButton;
      mOKButton->SetStyle("<button_expanding>");
      mOKButton->SetCaption(TXT(PS_OK));
      buttonsLayout->AddColumn(0, width/numButtons);
      buttonsLayout->AddElement(mOKButton, iButton++, 0);
      buttonsArea->AddChild(mOKButton);
    }

    // Alt button
    if (altButtonText)
    {
      mAltButton = new CIwUIButton;
      mAltButton->SetStyle("<button_expanding>");
      mAltButton->SetCaption(altButtonText);
      buttonsLayout->AddColumn(0, width/numButtons);
      buttonsLayout->AddElement(mAltButton, iButton++, 0);
      buttonsArea->AddChild(mAltButton);
    }
  }
  IwGetUIView()->AddElement(screenArea);
  IwGetUIView()->AddElementToLayout(screenArea);

  // Picks up all the events
  screenArea->AddEventHandler(this); 

  // Add padding if necessary
  IwGetUIController()->Update();
  int titleSize = titleLabel ? titleLabel->GetSize().x : 0;
  int tabSize = mTabBar ? mTabBar->GetSize().x : 0;
  int cancelSize = mCancelButton->GetSize().x;
  int s = (width - (cancelSize + titleSize + tabSize)) / 2;
  if (s > 1)
  {
    spacer1->SetSizeMin(CIwVec2(s,-1));
    spacer2->SetSizeMin(CIwVec2(s,-1));
  }
}

//---------------------------------------------------------------------------------------------------------------------
int32_t FileMenu::Update(bool keepAwake, bool throttle, bool& resourcesHaveChanged, const struct GameSettings& gameSettings)
{
  mScrollArea = mFilenameArea;

  if (mMode == SAVE)
  {
    std::string txt = mTextField->GetCaption();
    RemoveInvalidCharacters(txt);
    mTextField->SetCaption(txt.c_str());
  }
  return Menu::Update(keepAwake, throttle, resourcesHaveChanged, gameSettings);
}

//---------------------------------------------------------------------------------------------------------------------
bool FileMenu::HandleEvent(CIwEvent* pEvent) 
{
  CIwManaged* sender = pEvent->GetSender();
  const uint32_t eventID = pEvent->GetID();
  const Language language = mGameSettings.mOptions.mLanguage;

  if (eventID == IWUI_EVENT_BUTTON)
  {
    if (sender == mOKButton)
    {
      if (mMode == SAVE)
      {
        mSelectedString = mTextField->GetCaption();
      }
      mResult = FILEMENURESULT_SELECTED;
      mFinished = true;
      return true;
    }
    if (sender == mAltButton)
    {
      mFinished = true;
      mResult = FILEMENURESULT_ALT;
      return true;
    }
    if (sender == mCancelButton)
    {
      mFinished = true;
      mResult = FILEMENURESULT_CANCEL;
      return true;
    }
    for (size_t i = 0 ; i != mButtons.size() ; ++i)
    {
      if (sender == mButtons[i].mTextButton || sender == mButtons[i].mImageButton)
      {
        if (mMode == SAVE)
        {
          mTextField->SetCaption(mButtons[i].mTextButton->GetCaption());
        }
        else
        {
          mSelectedString = mFilenames[i];
          mFinished = true;
          if (mButtons[i].mAvailable)
          {
            mResult = FILEMENURESULT_SELECTED;
          }
          else
          {
            mResult = FILEMENURESULT_PURCHASE;
          }
        }
        return true;
      }
    }
  }
  else if (eventID == IWUI_EVENT_TABBAR)
  {
    if (sender == mTabBar)
    {
      mSelectedTab = mTabBar->GetSelected();
      mResult = FILEMENURESULT_SELECTED;
      mFinished = true;
    }
  }
  return false;
}

//---------------------------------------------------------------------------------------------------------------------
FileMenu::FileMenu(
  const GameSettings& gameSettings, 
  const char*         systemPath, 
  const char*         userPath, 
  const char*         extension, 
  const Mode          mode, 
  const char*         title,
  const char*         tabTitles[], 
  const size_t        numTabs,
  const int           selectedTab,
  const char*         cancelButtonText,
  const char*         altButtonText,
  FileMenuType        fileMenuType,
  float               imagesPerScreen,
  IncludeCallback*    includeCallback = 0) 
  : mResult(FILEMENURESULT_CANCEL), mNumLines(0), mFinished(false), mMode(mode), mTextField(0), mGameSettings(gameSettings), mSelectedTab(selectedTab)
{
  CIwUILayoutVertical* filenameLayout;
  CreateContentAndButtonsAreaAndLayout(gameSettings, mFilenameArea, filenameLayout, cancelButtonText, altButtonText, title, tabTitles, numTabs);

  if (mMode == SAVE)
  {
    IwAssert(ROWLHOUSE, userPath);
  }
  else if (systemPath)
  {
    AddFiles(gameSettings, systemPath, extension, mFilenameArea, filenameLayout, 0, fileMenuType, imagesPerScreen, true, includeCallback);
  }

  if (userPath)
  {
    int numFiles = AddFiles(gameSettings, userPath, extension, mFilenameArea, filenameLayout, "User files:", fileMenuType, imagesPerScreen, false, includeCallback);
    if (!numFiles && mMode == DELETE)
    {
      CreateLabel(gameSettings, mFilenameArea, filenameLayout, "No files found", LABEL_TYPE_CENTERED_TITLE);
    }
  }

  mSelectedTab = -1;
}

//---------------------------------------------------------------------------------------------------------------------
void RemoveExtension(char* filename, const char* extension)
{
  if (!extension || !filename)
    return;
  size_t extensionLen = strlen(extension);
  size_t filenameLen = strlen(filename);

  if (extensionLen > filenameLen)
    return;

  for (size_t i = extensionLen ; i-- != 0 ; )
  {
    char ext = extension[i];
    if (filename[filenameLen - extensionLen + i] != ext)
    {
      return;
    }
  }
  filename[(filenameLen - extensionLen)] = 0;
}

//---------------------------------------------------------------------------------------------------------------------
void RemovePath(char* filename)
{
  int origLen = strlen(filename);
  for (int i = origLen-1 ; i != -1 ; --i)
  {
    if (filename[i] == '/' || filename[i] == '\\')
    {
      if (i == 0)
        return;
      int j = 0;
      for (++i ; i != origLen + 1 ; ++i, ++j) // pick up the nul
      {
        filename[j] = filename[i];
      }
      return;
    }
  }
}


//---------------------------------------------------------------------------------------------------------------------
int FileMenu::AddFiles(
  const GameSettings& gameSettings,
  const char* path, 
  const char* extension, 
  CIwUIElement* pContainer, 
  CIwUILayoutVertical* layout, 
  const char* labelText,
  FileMenuType fileMenuType,
  float imagesPerScreen,
  bool useTitleFromFile,
  IncludeCallback* includeCallback)
{
  s3eFileList* fileList = s3eFileListDirectory(path);
  if (!fileList)
    return 0;

  const int filenameLen = 512;
  char filename[filenameLen];

  bool drawLabel = true && labelText;
  CIwUILabel* label = 0;
  int numFiles = 0;
  while (true)
  {
    s3eResult result = s3eFileListNext(fileList, filename, filenameLen);
    if (result == S3E_RESULT_SUCCESS)
    {
      if (drawLabel)
      {
        label = CreateLabel(gameSettings, pContainer, layout, labelText, LABEL_TYPE_LEFT);
        drawLabel = false;
      }
      std::string fullPath(path);
      fullPath += "/";
      fullPath += filename;
      if (
        !includeCallback ||
        includeCallback->GetInclude(fullPath.c_str())
        )
      {
        RemoveExtension(filename, extension);
        Button button = CreateButton(gameSettings, pContainer, layout, fullPath, filename, fileMenuType, imagesPerScreen, useTitleFromFile);
        if (button.mUse)
        {
          mButtons.push_back(button);        
          mFilenames.push_back(fullPath);
          ++mNumLines;
          ++numFiles;
        }
      }
    }
    else
    {
      if (numFiles == 0 && label)
      {
        label->SetVisible(false);
      }
      s3eFileListClose(fileList);
      return numFiles;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
void FileMenu::AddNewFile(const GameSettings& gameSettings, CIwUIElement* pContainer, CIwUILayoutGrid* layout)
{
  const Language language = gameSettings.mOptions.mLanguage;
  CIwUIElement* textArea = new CIwUIElement;
  textArea->SetSizeToContent(true);

  pContainer->AddChild(textArea);
  layout->AddElement(textArea, 0, 1);

  CIwUILayoutVertical* textLayout = new CIwUILayoutVertical;
  textArea->SetLayout(textLayout);

  CreateLabel(gameSettings, textArea, textLayout, TXT(PS_FILENAME), LABEL_TYPE_LEFT);

  mTextField = new CIwUITextField;
  mTextField->SetStyle("<textField_inline>");

  textArea->AddChild(mTextField);
  textLayout->AddElement(mTextField);
}

//---------------------------------------------------------------------------------------------------------------------
FileMenuResult FileMenuLoad(
  std::string& result, 
  const GameSettings& gameSettings, 
  const char* systemPath, 
  const char* userPath, 
  const char* extension, 
  const char* title,
  const char* tabTitles[],
  const size_t numTabs,
  const char* cancelButtonText,
  const char* altButtonText, 
  FileMenuType fileMenuType,
  float imagesPerScreen,
  IncludeCallback* includeCallback)
{
  const Language language = gameSettings.mOptions.mLanguage;
  PrepareForIwGx(false);
  if (imagesPerScreen < 0.0f)
    imagesPerScreen = GetImagesPerLoadScreen(gameSettings);
  int tab = 0;
  while (true)
  {
    FileMenu* fileMenu = new FileMenu(gameSettings, systemPath, userPath, extension, FileMenu::LOAD, title, tabTitles, numTabs, tab, cancelButtonText, altButtonText, fileMenuType, imagesPerScreen, includeCallback);
    while (!fileMenu->GetFinished())
    {
      if (s3eDeviceCheckQuitRequest() || (s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED))
        return FILEMENURESULT_CANCEL;
      bool resourcesHaveChanged;
      fileMenu->Update(gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_ANDROID, true, resourcesHaveChanged, gameSettings);
    }
    tab = fileMenu->GetSelectedTab();
    FileMenuResult ret = fileMenu->GetResult(result);

    if (ret == FILEMENURESULT_PURCHASE)
    {
      if (gameSettings.mStatistics.mIsInTrialMode)
      {
        int button = ShowDialog("PicaSim", TXT(PS_FULLVERSION), TXT(PS_NOTNOW), "Try", TXT(PS_PURCHASE));
        if (button == 2)
        {
          BuyFullVersion(gameSettings);
          return FILEMENURESULT_CANCEL;
        }
        else if (button == 1)
        {
          return FILEMENURESULT_SELECTED;
        }
      }
      else
      {
        int button = ShowDialog("PicaSim", TXT(PS_FULLVERSION), TXT(PS_NOTNOW), TXT(PS_PURCHASE));
        if (button == 1)
        {
          BuyFullVersion(gameSettings);
          return FILEMENURESULT_CANCEL;
        }
      }
    }
    else
    {
      IwGetUIView()->DestroyElements(); // delete fileMenu

      if (tab < 0)
        return ret;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
FileMenuResult FileMenuSave(
  std::string& selectedString, 
  const GameSettings& gameSettings, 
  const char* userPath, 
  const char* extension, 
  const char* title, 
  FileMenuType fileMenuType)
{
  const Language language = gameSettings.mOptions.mLanguage;
  FileMenu* fileMenu = new FileMenu(gameSettings, 0, userPath, extension, FileMenu::SAVE, title, 0, 0, 0, TXT(PS_BACK), NULL, fileMenuType, 3.0f);
  while (!fileMenu->GetFinished())
  {
    if (s3eDeviceCheckQuitRequest() || (s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED))
      return FILEMENURESULT_CANCEL;
    bool resourcesHaveChanged;
    fileMenu->Update(gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_ANDROID, true, resourcesHaveChanged, gameSettings);
  }

  FileMenuResult result = fileMenu->GetResult(selectedString);
  IwGetUIView()->DestroyElements(); // delete fileMenu

  if (selectedString.empty())
    return FILEMENURESULT_CANCEL;

  selectedString = std::string(userPath) + std::string("/") + selectedString + std::string(extension);
  return FILEMENURESULT_SELECTED;
}

//---------------------------------------------------------------------------------------------------------------------
void FileMenuDelete(
  const GameSettings& gameSettings, 
  const char* userPath, 
  const char* extension, 
  const char* title, 
  FileMenuType fileMenuType)
{
  const Language language = gameSettings.mOptions.mLanguage;
  FileMenu* fileMenu = new FileMenu(gameSettings, 0, userPath, extension, FileMenu::DELETE, title, 0, 0, 0, TXT(PS_BACK), NULL, fileMenuType, 3.0f);
  while (!fileMenu->GetFinished())
  {
    if (s3eDeviceCheckQuitRequest() || (s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED))
      return;
    bool resourcesHaveChanged;
    fileMenu->Update(gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_ANDROID, true, resourcesHaveChanged, gameSettings);
  }
  std::string selectedString;
  fileMenu->GetResult(selectedString);
  IwGetUIView()->DestroyElements(); // delete fileMenu

  if (selectedString.empty())
    return;

  TRACE_FILE_IF(1) TRACE("Deleting %s", selectedString.c_str());
  s3eFileDelete(selectedString.c_str());
}

