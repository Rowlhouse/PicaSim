#include "Shaders.h"
#include "ShaderManager.h"
#include "LoadingScreenHelper.h"
#include "Trace.h"

//======================================================================================================================
std::unique_ptr<ShaderManager> ShaderManager::mInstance;

//======================================================================================================================
ShaderManager& ShaderManager::GetInstance()
{
    IwAssert(ROWLHOUSE, mInstance != nullptr);
    return *mInstance;
}

//======================================================================================================================
ShaderManager::ShaderManager(LoadingScreenHelper* loadingScreen)
{
    int maxVertexUniforms, maxVertexAttribs, maxVaryingVectors, maxFragmentUniformVectors;
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertexUniforms);
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
    glGetIntegerv(GL_MAX_VARYING_VECTORS, &maxVaryingVectors);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &maxFragmentUniformVectors);

    mShaders[SHADER_SIMPLE]                        = std::make_unique<SimpleShader>();
    mShaders[SHADER_CONTROLLER]                    = std::make_unique<ControllerShader>();
    mShaders[SHADER_SKYBOX]                        = std::make_unique<SkyboxShader>();
    mShaders[SHADER_SKYBOX_VR_PARALLAX]            = std::make_unique<SkyboxVRParallaxShader>();
    mShaders[SHADER_OVERLAY]                       = std::make_unique<OverlayShader>();
    mShaders[SHADER_MODEL]                         = std::make_unique<ModelShader>();
    mShaders[SHADER_TEXTUREDMODEL]                 = std::make_unique<TexturedModelShader>();
    mShaders[SHADER_TEXTUREDMODELSEPARATESPECULAR] = std::make_unique<TexturedModelSeparateSpecularShader>();
    mShaders[SHADER_PLAIN]                         = std::make_unique<PlainShader>();
    mShaders[SHADER_TERRAIN]                       = std::make_unique<TerrainShader>();
    mShaders[SHADER_TERRAIN_PANORAMA]              = std::make_unique<TerrainPanoramaShader>();
    mShaders[SHADER_SHADOW]                        = std::make_unique<ShadowShader>();
    mShaders[SHADER_SHADOW_BLUR]                   = std::make_unique<ShadowBlurShader>();
    mShaders[SHADER_SMOKE]                         = std::make_unique<SmokeShader>();

    if (gGLVersion > 1)
    {
        if (loadingScreen)
            loadingScreen->Update("Compiling shaders");
        for (int i = 0 ; i != NUM_SHADERS ; ++i)
        {
            mShaders[i]->Init();
            IwAssert(ROWLHOUSE, mShaders[i]->mShaderProgram > 0);
        }
    }
}

//======================================================================================================================
void ShaderManager::Init(LoadingScreenHelper* loadingScreen)
{
    TRACE_FUNCTION_ONLY(1);
    IwAssert(ROWLHOUSE, !mInstance);
    mInstance.reset(new ShaderManager(loadingScreen));
}

//======================================================================================================================
void ShaderManager::Terminate()
{
    TRACE_FUNCTION_ONLY(1);
    IwAssert(ROWLHOUSE, mInstance);

    for (int i = 0 ; i != NUM_SHADERS ; ++i)
    {
        mInstance->mShaders[i]->Terminate();
        mInstance->mShaders[i].reset();
    }

    mInstance.reset();
}

//======================================================================================================================
const Shader* ShaderManager::GetShader(ShaderType type)
{
    return mShaders[type].get();
}

