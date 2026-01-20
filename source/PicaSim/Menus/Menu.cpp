#include "Menu.h"
#include "StartMenu.h"
#include "Framework.h"
#include "LoadingScreen.h"
#include "../GameSettings.h"
#include "../PicaJoystick.h"
#include "../../Platform/S3ECompat.h"

typedef std::map<std::string, Texture*> TextureMap;
TextureMap* sTextureMap = 0;

//======================================================================================================================
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

//======================================================================================================================
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

//======================================================================================================================
bool UpdateResourceGroupForScreen(const GameSettings& gameSettings)
{
    gameSettings.mOptions.mFrameworkSettings.UpdateScreenDimensions(); // const but mutable...
    return true;
}

//======================================================================================================================
void MenuInit(const GameSettings& gameSettings)
{
    sTextureMap = new TextureMap;
    UpdateResourceGroupForScreen(gameSettings);
}

//======================================================================================================================
void MenuTerminate()
{
    ReleaseCachedTextures();
    delete sTextureMap;
    sTextureMap = 0;
}

//======================================================================================================================
void ReleaseCachedTextures()
{
    if (sTextureMap)
    {
        for (TextureMap::iterator it = sTextureMap->begin() ; it != sTextureMap->end() ; ++it)
            delete it->second;
    }
    sTextureMap->clear();
}

//======================================================================================================================
Texture* GetCachedTexture(std::string path, bool convertTo16Bit)
{
    IwAssert(ROWLHOUSE, sTextureMap);

    std::string pathLower = path;
    std::transform(pathLower.begin(), pathLower.end(), pathLower.begin(),
        [](unsigned char c) { return std::tolower(c); });

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
        // Note: Don't trace here - with ImGui immediate mode this is called every frame
        return it->second;
    }
}

//======================================================================================================================
void OpenWebsite(const GameSettings& gameSettings)
{
    s3eOSExecExecute("http://www.rowlhouse.co.uk/PicaSim", false);
}

//======================================================================================================================
void NewVersion()
{
    s3eOSExecExecute("http://www.rowlhouse.co.uk/PicaSim/download.html", false);
}


