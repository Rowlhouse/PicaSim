#ifndef SHADERS_H
#define SHADERS_H

#include "Graphics.h"

//======================================================================================================================
class Shader
{
public:
    Shader() : mShaderProgram(0), mVertexShaderStr(0), mFragmentShaderStr(0) {}
    virtual ~Shader() {}
    virtual void Init(const char* vertexShaderStr, const char* fragmentShaderStr);
    virtual void Init() = 0;
    void Terminate();
    void Use() const;
protected:
    friend class ShaderManager;
    GLuint      mShaderProgram;
    const char* mVertexShaderStr;
    const char* mFragmentShaderStr;
};

//======================================================================================================================
class SimpleShader : public Shader
{
public:
    void Init() OVERRIDE;
    int u_mvpMatrix, a_position, a_colour;
};

//======================================================================================================================
class ControllerShader : public Shader
{
public:
    void Init() OVERRIDE;
    int u_mvpMatrix, a_position;
    int u_colour;
};

//======================================================================================================================
class SkyboxShader : public Shader
{
public:
    void Init() OVERRIDE;
    int u_mvpMatrix, a_position, a_texCoord;
    int u_texture;
};

//======================================================================================================================
class OverlayShader : public Shader
{
public:
    void Init() OVERRIDE;
    int u_mvpMatrix, a_position, a_texCoord;
    int u_texture, u_colour;
};

struct LightShaderInfo
{
    LightShaderInfo() : u_lightDir(-1), u_lightAmbientColour(-1), u_lightDiffuseColour(-1), u_lightSpecularColour(-1) {}
    int u_lightDir;
    int u_lightAmbientColour;
    int u_lightDiffuseColour;
    int u_lightSpecularColour;
};

//======================================================================================================================
class ModelShader : public Shader
{
public:
    void Init() OVERRIDE;
    void SetupVars();
    LightShaderInfo lightShaderInfo[5];
    int u_mvpMatrix, u_normalMatrix;
    int u_specularAmount, u_specularExponent, a_position, a_normal, a_colour;
};

//======================================================================================================================
class TexturedModelShader : public ModelShader
{
public:
    void Init() OVERRIDE;
    int a_texCoord, u_texture, u_texBias;
};

//======================================================================================================================
class TexturedModelSeparateSpecularShader : public TexturedModelShader
{
public:
    void Init() OVERRIDE;
};

//======================================================================================================================
class PlainShader : public Shader
{
public:
    void Init() OVERRIDE;
    int u_mvpMatrix, u_textureMatrix, a_position, a_colour;
    int u_texture;
};

//======================================================================================================================
class TerrainShader : public Shader
{
public:
    void Init() OVERRIDE;
    int u_mvpMatrix, u_textureMatrix0, u_textureMatrix1, a_position;
    int u_texture0, u_texture1;
};

//======================================================================================================================
class TerrainPanoramaShader : public Shader
{
public:
    void Init() OVERRIDE;
    int u_mvpMatrix, a_position;
};

//======================================================================================================================
class ShadowShader : public Shader
{
public:
    void Init() OVERRIDE;
    int u_mvpMatrix, u_textureMatrix, a_position, a_texCoord;
    int u_texture, u_colour;
};

//======================================================================================================================
class SmokeShader : public Shader
{
public:
    void Init() OVERRIDE;
    int u_mvpMatrix, a_position, u_colour, a_texCoord;
    int u_texture;
};

//======================================================================================================================
class SkyboxVRParallaxShader : public Shader
{
public:
    void Init() OVERRIDE;
    int u_mvpMatrix, a_position, a_texCoord;
    int u_skyboxTexture, u_depthTexture;
    int u_eyeOffset;       // -1.0 for left eye, +1.0 for right eye
    int u_ipd;             // interpupillary distance
    int u_nearPlane, u_farPlane;
    int u_screenSize;      // screen dimensions for depth texture sampling
    int u_eyeRightLocal;   // eye separation direction in face-local coords (vec3)
    int u_tileScale;       // numPerSide - scales parallax for tile coordinates
    int u_tileOffset;      // tile translation offset (y, z)
    int u_tanFovMin;       // vec2(tanLeft, tanDown) for depth correction
    int u_tanFovMax;       // vec2(tanRight, tanUp) for depth correction
};


#endif
