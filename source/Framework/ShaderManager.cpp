#include "Shaders.h"
#include "ShaderManager.h"
#include "LoadingScreenHelper.h"
#include "Trace.h"

//======================================================================================================================
ShaderManager* ShaderManager::mInstance = 0;

//======================================================================================================================
ShaderManager& ShaderManager::GetInstance()
{
    IwAssert(ROWLHOUSE, mInstance != 0);
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

    mShaders[SHADER_SIMPLE]                        = new SimpleShader();
    mShaders[SHADER_CONTROLLER]                    = new ControllerShader();
    mShaders[SHADER_SKYBOX]                        = new SkyboxShader();
    mShaders[SHADER_SKYBOX_VR_PARALLAX]            = new SkyboxVRParallaxShader();
    mShaders[SHADER_OVERLAY]                       = new OverlayShader();
    mShaders[SHADER_MODEL]                         = new ModelShader();
    mShaders[SHADER_TEXTUREDMODEL]                 = new TexturedModelShader();
    mShaders[SHADER_TEXTUREDMODELSEPARATESPECULAR] = new TexturedModelSeparateSpecularShader();
    mShaders[SHADER_PLAIN]                         = new PlainShader();
    mShaders[SHADER_TERRAIN]                       = new TerrainShader();
    mShaders[SHADER_TERRAIN_PANORAMA]              = new TerrainPanoramaShader();
    mShaders[SHADER_SHADOW]                        = new ShadowShader();
    mShaders[SHADER_SMOKE]                         = new SmokeShader();

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
    mInstance = new ShaderManager(loadingScreen);
}

//======================================================================================================================
void ShaderManager::Terminate()
{
    TRACE_FUNCTION_ONLY(1);
    IwAssert(ROWLHOUSE, mInstance);

    for (int i = 0 ; i != NUM_SHADERS ; ++i)
    {
        mInstance->mShaders[i]->Terminate();
        delete mInstance->mShaders[i];
    }

    delete mInstance;
    mInstance = 0;
}

//======================================================================================================================
const Shader* ShaderManager::GetShader(ShaderType type)
{
    return mShaders[type];
}

