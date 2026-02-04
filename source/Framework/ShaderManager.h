#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <vector>
#include <memory>

class LoadingScreenHelper;
class Shader;

enum ShaderType
{
    SHADER_SIMPLE,
    SHADER_CONTROLLER,
    SHADER_SKYBOX,
    SHADER_SKYBOX_VR_PARALLAX,
    SHADER_OVERLAY,
    SHADER_MODEL,
    SHADER_TEXTUREDMODEL,
    SHADER_TEXTUREDMODELSEPARATESPECULAR,
    SHADER_PLAIN,
    SHADER_TERRAIN,
    SHADER_TERRAIN_PANORAMA,
    SHADER_SHADOW,
    SHADER_SHADOW_BLUR,
    SHADER_SMOKE,
    NUM_SHADERS
};

//======================================================================================================================
class ShaderManager
{
public:
    /// Gets the singleton
    static ShaderManager& GetInstance(); 

    static void Init(LoadingScreenHelper* loadingScreen);
    static void Terminate();

    const Shader* GetShader(ShaderType type);

private:
    ShaderManager(LoadingScreenHelper* loadingScreen);
    static std::unique_ptr<ShaderManager> mInstance;
    std::unique_ptr<Shader> mShaders[NUM_SHADERS];
};


#endif
