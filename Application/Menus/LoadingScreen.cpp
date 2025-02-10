#include "LoadingScreen.h"
#include "FonctionsMarmelade.h"
#include "Graphics.h"
#include "Trace.h"
#include "PicaStrings.h"
#include "GameSettings.h"

//---------------------------------------------------------------------------------------------------------------------
static const char* GetTip(const struct GameSettings& gameSettings, const char** tips, size_t numTips)
{
  ++gameSettings.mStatistics.mLoadCounter;
  TRACE_FILE_IF(1) TRACE("GetTip: load counter = %d", gameSettings.mStatistics.mLoadCounter);
  const Language language = gameSettings.mOptions.mLanguage;

  if (gameSettings.mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_RACE)
  {
    static char txt[1024];
    sprintf(txt, TXT(PS_RACETIP), gameSettings.mChallengeSettings.mInfo.c_str());
    return txt;
  }
  else if (gameSettings.mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_LIMBO)
  {
    static char txt[1024];
    sprintf(txt, TXT(PS_LIMBOTIP), gameSettings.mChallengeSettings.mInfo.c_str());
    return txt;
  }
  else if (gameSettings.mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_DURATION)
  {
    static char txt[1024];
    sprintf(txt, TXT(PS_DURATIONTIP), gameSettings.mChallengeSettings.mInfo.c_str());
    return txt;
  }

  if (gameSettings.mStatistics.mLoadCounter < 5)
    return 0;

  const uint32_t cycle = 4;
  if (gameSettings.mStatistics.mLoadCounter % cycle)
    return 0;

  int tip = (gameSettings.mStatistics.mLoadCounter/cycle) % numTips;

  return tips[tip];
}

//---------------------------------------------------------------------------------------------------------------------
LoadingScreen::LoadingScreen(const char* initialText, struct GameSettings& gameSettings, bool showTips, bool clearOnExit, bool showProgress) 
  : mFraction(0.0f), mGameSettings(gameSettings), mClearOnExit(clearOnExit)
{
  TRACE_METHOD_ONLY(1);
  mLastTimeMs = s3eTimerGetMs();
  mTipLabel = 0;
  mProgressImage = 0;
  mTexture = 0;
  mLabel = 0;

  const Language language = gameSettings.mOptions.mLanguage;
  int height = gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int width = gameSettings.mOptions.mFrameworkSettings.mScreenWidth;

  PrepareForIwGx(false);

  CIwUIElement* pContainer = new CIwUIElement;
  pContainer->SetSizeToContent(false);

  CIwUILayoutGrid* layout = new CIwUILayoutGrid;
  layout->SetSizeToSpace(true);
  layout->AddColumn();
  layout->AddRow();
  layout->AddRow();
  layout->AddRow();
  layout->AddRow();
  layout->AddRow();
  pContainer->SetLayout(layout);

  mTexture = AddCentredImage("Menus/StartBackground.jpg", gameSettings.mOptions.m16BitTextures);
  TRACE_FILE_IF(1) TRACE("Added image");

  // Add the label
  mLabel = new CIwUILabel;
  mLabel->SetStyle("<label_centered>");
  mLabel->SetCaption(initialText);
  Colour col;
  col.Set(0,0,0,255);
  mLabel->SetTextColour(col);

  pContainer->AddChild(mLabel);
  layout->AddElement(mLabel, 0, 0);

  // Tip label
  if (showTips)
  {
    const char* tips[] = {
      TXT(PS_TIPS1),
      TXT(PS_TIPS2),
      TXT(PS_TIPS3),
      TXT(PS_TIPS4),
      TXT(PS_TIPS5),
      TXT(PS_TIPS6),
      TXT(PS_TIPS7),
      TXT(PS_TIPS8),
      TXT(PS_TIPS9),
      TXT(PS_TIPS10),
      TXT(PS_TIPS11),
      TXT(PS_TIPS12),
      TXT(PS_TIPS13),
      TXT(PS_TIPS14),
      TXT(PS_TIPS15),
      TXT(PS_TIPS16),
    };
    int numTips = sizeof(tips) / sizeof(tips[0]);

    const char* tip = GetTip(gameSettings, tips, numTips);
    if (tip)
    {
      mTipLabel = new CIwUILabel;
      mTipLabel->SetStyle("<label_centered>");
      col.Set(220,220,220,255);
      mTipLabel->SetCaption(tip);
      mTipLabel->SetTextColour(col);
      pContainer->AddChild(mTipLabel);
      layout->AddElement(mTipLabel, 0, 2);
    }
  }

  if (showProgress)
  {
    // Load the image
    CIwTexture* texture = (CIwTexture*)IwGetResManager()->GetResNamed("Progress", "CIwTexture");
    uint32_t w = texture->GetWidth();
    uint32_t h = texture->GetHeight();
    float ar = float(w)/float(h);

    float desiredWidth = width/10.0f;
    float desiredHeight = desiredWidth / ar;

    mProgressImageWidth = (int32_t) desiredWidth;
    mProgressImageHeight = (int32_t) desiredHeight;

    // Add the image
    mProgressImage = new CIwUIImage;

    mProgressImage->SetProperty("texture", texture);
    mProgressImage->SetSizeMin(CIwVec2(mProgressImageWidth, mProgressImageHeight));
    mProgressImage->SetSizeMax(CIwVec2(mProgressImageWidth, mProgressImageHeight));

    pContainer->AddChild(mProgressImage);
  }

  IwGetUIView()->AddElement(pContainer);
  IwGetUIView()->AddElementToLayout(pContainer);

  // Two empty updates to make sure something is rendered even if the first proper update is slow
  Update(0);
  Update(0);
}

//---------------------------------------------------------------------------------------------------------------------
void LoadingScreen::DisableProgressIndicator()
{
  if (mProgressImage)
  {
    CIwColour colour;
    colour.r = colour.g = colour.b = colour.a = 0;
    mProgressImage->SetColour(colour);
  }
}


//---------------------------------------------------------------------------------------------------------------------
LoadingScreen::~LoadingScreen()
{
  IwGetUIView()->DestroyElements();
  delete mTexture;
  RecoverFromIwGx(mClearOnExit); // Blank to try to avoid a strange render artefact when starting OpenGL
}

//---------------------------------------------------------------------------------------------------------------------
void LoadingScreen::SetLabelColour(const Colour& colour)
{
  if (mLabel)
    mLabel->SetTextColour(colour);
}


//---------------------------------------------------------------------------------------------------------------------
void LoadingScreen::Update(const char* moduleName)
{
  if (moduleName)
    TRACE_FILE_IF(1) TRACE("LoadingScreen::Update %s", moduleName);
  else
    TRACE_FILE_IF(1) TRACE("LoadingScreen::Update()");

  int64_t currentTimeMs = s3eTimerGetMs();
  int32_t deltaTimeMs = (int32_t) (currentTimeMs - mLastTimeMs);

  if (!moduleName && deltaTimeMs < 33)
    return;

  int height = mGameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int width = mGameSettings.mOptions.mFrameworkSettings.mScreenWidth;

  if (moduleName && mLabel)
    mLabel->SetCaption(moduleName);

  mLastTimeMs = currentTimeMs;

  mFraction += (deltaTimeMs * 0.001f) * 0.25f;
  while (mFraction > 1.0f)
    mFraction -= 1.0f;

  if (mProgressImage)
  {
    CIwVec2 pos;
    pos.y = (int32_t) (0.95f * height); // bottom of the image
    pos.x = (int32_t) (mFraction * (width + mProgressImageWidth) - mProgressImageWidth/2);
    mProgressImage->SetPosAbsolute(pos, CIwVec2(-mProgressImageWidth/2, -mProgressImageHeight));
  }

  IwGxClear();
  IwGetUIController()->Update();

  IwGetUIView()->Update(deltaTimeMs);
  IwGetUIView()->Render();
  IwGxFlush();
  static bool log = false;
  if (log)
  {
    int maxLength;
    int length;
    glGetProgramiv(2,GL_INFO_LOG_LENGTH,&maxLength);
    char* log = new char[maxLength];
    glGetProgramInfoLog(2,maxLength,&length,log);
    printf(log);
    delete [] log;
  }
  IwGxSwapBuffers();

  s3eDeviceBacklightOn();
  s3eDeviceYield();
}
