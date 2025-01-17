#include "Menu.h"
#include "ClassesMarmelade.h"
#include "FonctionsPointerKeyboardMarmelade.h"
#include "File.h"
#include "LoadingScreen.h"
#include "Trace.h"
#include "Helpers.h"
// #include "StartMenu.h"
// #include "LoadingScreen.h"
#include "GameSettings.h"
#include "PicaJoystick.h"
// #include "../WindowsBilling.h"
#include <map>
#include <string>
#include <algorithm>
#include <cctype>

// #include <IwUI.h>
// #include <IwGL.h>
// #include <s3eOSExec.h>

typedef std::map<std::string, Texture*> TextureMap;
TextureMap* sTextureMap = 0;
static CIwMaterial* gCursorMaterial = NULL;

//----------------------------------------------------------------------------------------------------------------------
void CacheThumbnailsFromDir(const char* path, bool convertTo16Bit, LoadingScreen* loadingScreen, const char* txt)
{
  s3eFileList* fileList = s3eFileListDirectory(path);
  if (loadingScreen)
    loadingScreen->Update(txt);
  if (fileList)
  {
    const int filenameLen = 512;
    char filename[filenameLen];
    std::string fullPath;
    while (true)
    {
      if (loadingScreen) loadingScreen->Update(0);
      s3eResult result = s3eFileListNext(fileList, filename, filenameLen);
      if (result == S3E_RESULT_SUCCESS)
      {
        size_t len = strnlen(filename, filenameLen);
        if (len > 4)
        {
          if (
            strcmp(&filename[len-4], ".jpg") == 0 ||
            strcmp(&filename[len-4], ".png") == 0
            )
          {
            fullPath = path;
            fullPath += "/";
            fullPath += filename;
            GetCachedTexture(fullPath, convertTo16Bit);
          }
        }
      }
      else
      {
        s3eFileListClose(fileList);
        return;
      }
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
float GetImagesPerLoadScreen(const GameSettings& gameSettings)
{
  int screenHeight = gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int heightForMinImages = 480;
  int heightForMaxImages = 800;
  float minImages = 3.0f;
  float maxImages = 5.0f;

  float f = float(screenHeight - heightForMinImages) / (heightForMaxImages - heightForMinImages);
  f = ClampToRange(f, 0.0f, 1.0f);
  return minImages + (maxImages - minImages) * f;
}

//---------------------------------------------------------------------------------------------------------------------
struct ResourceSetting
{
  ResourceSetting(int screenSize) : mScreenSize(screenSize), mResourceGroup(0) {}
  int          mScreenSize;
  CIwResGroup* mResourceGroup;
};

ResourceSetting sResourceSettings[] = 
{
  ResourceSetting(240),
  ResourceSetting(320),
  ResourceSetting(480),
  ResourceSetting(640),
  ResourceSetting(768),
  ResourceSetting(1080),
  ResourceSetting(1536),
  ResourceSetting(2048)
};

static int sNumResourceSettings = sizeof(sResourceSettings) / sizeof(sResourceSettings[0]);

//---------------------------------------------------------------------------------------------------------------------
bool UpdateResourceGroupForScreen(const GameSettings& gameSettings)
{
  gameSettings.mOptions.mFrameworkSettings.UpdateScreenDimensions(); // const but mutable...
  int width =  gameSettings.mOptions.mFrameworkSettings.mScreenWidth;
  int height =  gameSettings.mOptions.mFrameworkSettings.mScreenHeight;
  int minDim = Minimum(width, height);

  static int currentIndex = -1;
  int index = sNumResourceSettings - 1;
  for (int i = 0 ; i != sNumResourceSettings-1 ; ++i)
  {
    if (minDim < sResourceSettings[i+1].mScreenSize)
    {
      index = i;
      break;
    }
  }

  if (index == currentIndex)
    return false;

  PrepareForIwGx(false);

  currentIndex = index;

  char txt[128];
  sprintf(txt, "Menus/Menu_%d.group", sResourceSettings[index].mScreenSize);
  if (!sResourceSettings[index].mResourceGroup)
  {
    TRACE("Loading %s", txt);
    sResourceSettings[index].mResourceGroup = IwGetResManager()->LoadGroup(txt);
  }
  else
  {
    TRACE("Have already loaded %s", txt);
    IwGetResManager()->SetCurrentGroup(sResourceSettings[index].mResourceGroup);
  }

  sprintf(txt, "iwui%d", sResourceSettings[index].mScreenSize);
  CIwResource* pResource = IwGetResManager()->GetResNamed(txt, IW_UI_RESTYPE_STYLESHEET);
  
  IwGetUIStyleManager()->SetStylesheet(IwSafeCast<CIwUIStylesheet*>(pResource));

  RecoverFromIwGx(false);

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
void MenuInit(const GameSettings& gameSettings)
{
  sTextureMap = new TextureMap;

  new CIwUIView;
  new CIwUIController;
  new CIwUITextInput;

  IwGetResManager()->LoadGroup("Menus/Menu_Common.group");

#ifdef IW_DEBUG
  for (int i = 0 ; i != sNumResourceSettings ; ++i)
  {
    char txt[128];
    sprintf(txt, "Menus/Menu_%d.group", sResourceSettings[i].mScreenSize);
    IwGetResManager()->DestroyGroup(IwGetResManager()->LoadGroup(txt));
  }
#endif

  UpdateResourceGroupForScreen(gameSettings);

  IwGetUITextInput()->SetEditorMode(CIwUITextInput::eInlineKeyboard);
}


//---------------------------------------------------------------------------------------------------------------------
void MenuTerminate()
{
  delete IwGetUITextInput();
  delete IwGetUIController();
  delete IwGetUIView();

  ReleaseCachedTextures();
  delete sTextureMap;
  sTextureMap = 0;

  delete gCursorMaterial;
  gCursorMaterial = 0;
}

//---------------------------------------------------------------------------------------------------------------------
void ReleaseCachedTextures()
{
  if (sTextureMap)
  {
    for (TextureMap::iterator it = sTextureMap->begin() ; it != sTextureMap->end() ; ++it)
      delete it->second;
  }
  sTextureMap->clear();
}

//---------------------------------------------------------------------------------------------------------------------
Texture* GetCachedTexture(std::string path, bool convertTo16Bit)
{
  IwAssert(ROWLHOUSE, sTextureMap);

  std::string pathLower = path;
  std::transform(pathLower.begin(), pathLower.end(), pathLower.begin(), std::tolower);

  TextureMap::iterator it = sTextureMap->find(pathLower);
  if (it == sTextureMap->end())
  {
    TRACE_FILE_IF(1) TRACE("Loading texture %s", path.c_str());
    Texture* texture = new Texture;
    texture->LoadFromFile(path.c_str());
    if (texture->GetWidth() > 0 && texture->GetHeight() > 0)
    {
      (*sTextureMap)[pathLower] = texture;
      texture->SetMipMapping(false);
      if (convertTo16Bit)
        texture->SetFormatHW(CIwImage::RGB_565);
      texture->Upload();
      return texture;
    }
    else
    {
      TRACE_FILE_IF(1) TRACE("Failed to load texture %s", path.c_str());
      delete texture;
      return 0;
    }
  }
  else
  {
    TRACE_FILE_IF(1) TRACE("Found cached texture %s", path.c_str());
    return it->second;
  }
}

//---------------------------------------------------------------------------------------------------------------------
Menu::Menu()
{
  mLastTime = s3eTimerGetMs();
  mUpdateCounter = 0;
  mScrollArea = 0;
}

//---------------------------------------------------------------------------------------------------------------------
Menu::~Menu()
{
}

//---------------------------------------------------------------------------------------------------------------------
void RenderCursor()
{
  if (!s3ePointerGetInt(S3E_POINTER_AVAILABLE))
    return;

  if (!gCursorMaterial)
  {
    gCursorMaterial = new CIwMaterial();
    gCursorMaterial->SetColAmbient(0, 0, 255, 255);
  }

  IwGxSetMaterial(gCursorMaterial);
  int pointerx = s3ePointerGetX();
  int pointery = s3ePointerGetY();

  int cursor_size = 10;
  CIwSVec2 wh(cursor_size*2, 1);
  CIwSVec2 wh2(1, cursor_size*2);
  CIwSVec2 pos = CIwSVec2((int16_t)pointerx-cursor_size, (int16_t)pointery);
  CIwSVec2 pos2 = CIwSVec2((int16_t)pointerx, (int16_t)pointery-cursor_size);
  IwGxDrawRectScreenSpace(&pos, &wh);
  IwGxDrawRectScreenSpace(&pos2, &wh2);
}



//---------------------------------------------------------------------------------------------------------------------
int32_t Menu::Update(bool keepAwake, bool throttle, bool& resourcesHaveChanged, const GameSettings& gameSettings)
{
  resourcesHaveChanged = UpdateResourceGroupForScreen(gameSettings);

  s3eKeyboardUpdate();
  s3ePointerUpdate();
  UpdateJoystick(gameSettings.mOptions.mJoystickID);

  if (keepAwake)
  {
    // prevent backlight turning off, whatever the device is set to
    s3eDeviceBacklightOn();
  }

  // Allow mouse wheel scrolling
  if (mScrollArea)
  {
    int scrollAmount = 0;
    int scrollDelta = gameSettings.mOptions.mFrameworkSettings.mScreenHeight/2;
    if (s3ePointerGetState(S3E_POINTER_BUTTON_MOUSEWHEELDOWN) != 0)
      scrollAmount += scrollDelta;
    else if (s3ePointerGetState(S3E_POINTER_BUTTON_MOUSEWHEELUP) != 0)
      scrollAmount -= scrollDelta;
    if (scrollAmount != 0)
    {
      CIwVec2 pos = mScrollArea->GetScrollPosition();
      pos.y += scrollAmount;
      mScrollArea->ScrollToPosition(pos);
    }
  }

  if (mUpdateCounter == 0)
    IwGetUIView()->Update(1);

  IwGetUIController()->Update();

  // Calculate an update time step.
  uint64_t currentTime = s3eTimerGetMs();
  int32_t updateTime = currentTime > mLastTime ?
    MIN((int32_t)(currentTime - mLastTime), 1000) : 0;
  mLastTime = currentTime;

  IwGetUIView()->Update(updateTime);

  // Now render
  IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);
  IwGetUIView()->Render();

  static bool renderCursor = false;
  if (renderCursor)
    RenderCursor();

  IwGxFlush();

  //Render Debugging outlines of the element hierarchy
  static bool debugLayout = false;
  if (debugLayout)
    IwUIDebugRender(NULL, IW_UI_DEBUG_LAYOUT_OUTLINE_F | IW_UI_DEBUG_ELEMENT_OUTLINE_F);

  IwGxSwapBuffers();

  ++mUpdateCounter;

  if (throttle)
  {
    // Attempt to lock to max 30 frames per second
    s3eDeviceYield(1000/30);
  }
  else
  {
    s3eDeviceYield();
  }

  //if (s3eKeyboardGetState(s3eKeyS) & S3E_KEY_STATE_PRESSED) 
  //{
  //  SaveScreenshot();
  //}

  return updateTime;
}

//---------------------------------------------------------------------------------------------------------------------
Texture* AddCentredImage(const char* filename, bool use16Bit, CIwUIImage** image)
{
  TRACE_FUNCTION_ONLY(1);
  int width = s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
  int height = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);

  //Create an image
  TRACE_FILE_IF(1) TRACE("Creating texture");
  Texture* texture = new Texture;
  TRACE_FILE_IF(1) TRACE("Loading texture %s", filename);
  texture->LoadFromFile(filename);
  TRACE_FILE_IF(1) TRACE("Disable mipmapping");
  texture->SetMipMapping(false);
  if (use16Bit)
  {
    TRACE_FILE_IF(1) TRACE("Setting 565");
    texture->SetFormatHW(CIwImage::RGB_565);
  }
  TRACE_FILE_IF(1) TRACE("Before upload");
  texture->Upload();
  TRACE_FILE_IF(1) TRACE("After upload");

  CIwUIImage* pImage = new CIwUIImage;
  pImage->SetProperty("texture", texture);

  //Set the image to automatically resize to the size of the material's texture
  //pImage->SetSizeToContent(true);
  int16_t imageWidth  = texture->GetWidth();
  int16_t imageHeight = texture->GetHeight();

  CIwVec2 size;
  CIwSVec2 uv0;
  CIwSVec2 uv1;

  // Height if we stretched the texture to fit horizontally
  float h = (float(width) / imageWidth) * imageHeight;
  if (h < height)
  {
    float f = 0.5f * (1.0f - h/height);
    uv0.x = (int16_t) (f * imageWidth);
    uv0.y = 0;
    uv1.x = (int16_t) ((1.0f - f) * imageWidth);
    uv1.y = imageHeight;
  }
  else
  {
    float f = 0.5f * (1.0f - height/h);
    uv0.y = (int16_t) (f * imageHeight);
    uv0.x = 0;
    uv1.y = (int16_t) ((1.0f - f) * imageHeight);
    uv1.x = imageWidth;
  }
  uv0.x = ClampToRange(uv0.x, int16_t(0), imageWidth);
  uv1.x = ClampToRange(uv1.x, int16_t(0), imageWidth);
  uv0.y = ClampToRange(uv0.y, int16_t(0), imageHeight);
  uv1.y = ClampToRange(uv1.y, int16_t(0), imageHeight);
  pImage->SetProperty("uv0", uv0);
  pImage->SetProperty("uv1", uv1);
  pImage->SetSizeToContent(false);

  TRACE_FILE_IF(1) TRACE("Before adding element");
  IwGetUIView()->AddElement(pImage);
  TRACE_FILE_IF(1) TRACE("Before adding element to layout");
  IwGetUIView()->AddElementToLayout(pImage);
  TRACE_FILE_IF(1) TRACE("Finished AddCentredImage");

  if (image)
    *image = pImage;

  return texture;
}

//----------------------------------------------------------------------------------------------------------------------
void OpenWebsite(const GameSettings& gameSettings)
{
  s3eOSExecExecute("http://www.rowlhouse.co.uk/PicaSim", false);
}

//----------------------------------------------------------------------------------------------------------------------
void BuyFullVersion(const GameSettings& gameSettings)
{
  if (gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_ANDROID)
    s3eOSExecExecute("http://play.google.com/store/apps/details?id=com.rowlhouse.picasimdonate", false);
  else if (gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_IPHONE)
    s3eOSExecExecute("http://itunes.apple.com/app/id566983657", false);
  else if (gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_WIN10)
    //s3eOSExecExecute("ms-windows-store:PDP?PFN=43189TheRowlhouse.PicaSimFlightSimulator", false);
    WindowsBuyFullVersion();
  else
    OpenWebsite(gameSettings);
}

//----------------------------------------------------------------------------------------------------------------------
void NewVersion()
{
  s3eOSExecExecute("http://www.rowlhouse.co.uk/PicaSim/download.html", false);
}

//----------------------------------------------------------------------------------------------------------------------
void RateMe(const GameSettings& gameSettings)
{
  if (gameSettings.mStatistics.mIsAmazon)
  {
    s3eOSExecExecute("http://www.amazon.com/gp/mas/dl/android?asin=B015K0DY16", false);
  }
  else if (gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_IPHONE)
  {
    if (gameSettings.mStatistics.mAppType == Statistics::APP_FULL)
    {
      // Full version on iTunes
      s3eOSExecExecute("http://itunes.apple.com/app/id566983657", false);
    }
    else
    {
      // Demo version on iTunes
      s3eOSExecExecute("http://itunes.apple.com/app/id595535451", false);
    }
  }
  else if (gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_ANDROID)
  {
    if (gameSettings.mStatistics.mAppType == Statistics::APP_FULL)
    {
      // Full version on Google Play
      s3eOSExecExecute("http://play.google.com/store/apps/details?id=com.rowlhouse.picasimdonate", false);
    }
    else
    {
      // Demo version on Google Play
      s3eOSExecExecute("http://play.google.com/store/apps/details?id=com.rowlhouse.picasim", false);
    }
  }
  else if (gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_WIN10)
  {
    s3eOSExecExecute("ms-windows-store:REVIEW?PFN=43189TheRowlhouse.PicaSimFlightSimulator", false);
  }
  else
  {
    // Any other platform - link to web page
    s3eOSExecExecute("http://www.rowlhouse.co.uk/PicaSim", false);
  }
}

