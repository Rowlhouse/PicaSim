#include "Shaders.h"
#include "Trace.h"

// AI generated fix for mac build
// This prevent runtime crash on macos

// Use appropriate GLSL version for platform
// Desktop GL 3.3 Core (macOS) uses GLSL 150 core, OpenGL ES 2.0 uses GLSL 100
#if defined(PS_PLATFORM_ANDROID) || defined(PS_PLATFORM_IOS)
#define GLSL(src) "#version 100\n" #src
#define GLSL_VERSION "#version 100\n"
#define PRECISION_MEDIUMP "precision mediump float;\n"
#define PRECISION_LOWP "precision lowp float;\n"
#define LOWP "lowp "
#define MEDIUMP "mediump "
#define ATTRIBUTE "attribute"
#define VARYING_OUT "varying"
#define VARYING_IN "varying"
#define FRAGCOLOR "gl_FragColor"
#define FRAGCOLOR_DECLARATION ""
#define TEXTURE2D "texture2D"
#elif defined(PICASIM_MACOS)
// macOS - use GLSL 120 for OpenGL 2.1 compatibility
// (GLSL 120 does not support precision qualifiers)
#define GLSL_VERSION "#version 120\n"
#define PRECISION_MEDIUMP ""
#define PRECISION_LOWP ""
#define LOWP ""
#define MEDIUMP ""
#define ATTRIBUTE "attribute"
#define VARYING_OUT "varying"
#define VARYING_IN "varying"
#define FRAGCOLOR "gl_FragColor"
#define FRAGCOLOR_DECLARATION ""
#define TEXTURE2D "texture2D"
#else
// Desktop GL - use version 130 which supports precision qualifiers as no-ops
#define GLSL(src) "#version 130\n" #src
#define GLSL_VERSION "#version 130\n"
#define PRECISION_MEDIUMP "precision mediump float;\n"
#define PRECISION_LOWP "precision lowp float;\n"
#define LOWP "lowp "
#define MEDIUMP "mediump "
#define ATTRIBUTE "attribute"
#define VARYING_OUT "varying"
#define VARYING_IN "varying"
#define FRAGCOLOR "gl_FragColor"
#define FRAGCOLOR_DECLARATION ""
#define TEXTURE2D "texture2D"
#endif

const char simpleVertexShaderStr[] = 
    GLSL_VERSION
    PRECISION_MEDIUMP
    "uniform mat4 u_mvpMatrix;\n"
    ATTRIBUTE " vec4 a_position;\n"
    ATTRIBUTE " vec4 a_colour;\n"
    VARYING_OUT " vec4 v_colour;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_mvpMatrix * a_position;\n"
    "    v_colour = a_colour;\n"
    "}\n";

const char simpleFragmentShaderStr[] = 
    GLSL_VERSION
    PRECISION_LOWP
    FRAGCOLOR_DECLARATION
    VARYING_IN " vec4 v_colour;\n"
    "void main()\n"
    "{\n"
    "    " FRAGCOLOR " = v_colour;\n"
    "}\n";

const char controllerVertexShaderStr[] = 
    GLSL_VERSION
    PRECISION_MEDIUMP
    "uniform mat4 u_mvpMatrix;\n"
    ATTRIBUTE " vec4 a_position;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_mvpMatrix * a_position;\n"
    "}\n";

const char controllerFragmentShaderStr[] = 
    GLSL_VERSION
    PRECISION_LOWP
    FRAGCOLOR_DECLARATION
    "uniform vec4 u_colour;\n"
    "void main()\n"
    "{\n"
    "    " FRAGCOLOR " = u_colour;\n"
    "}\n";

const char skyboxVertexShaderStr[] =
    GLSL_VERSION
    PRECISION_MEDIUMP
    "uniform mat4   u_mvpMatrix;\n"
    ATTRIBUTE " vec4 a_position;\n"
    ATTRIBUTE " vec2 a_texCoord;\n"
    VARYING_OUT " vec2 v_texCoord;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_mvpMatrix * a_position;\n"
    "    v_texCoord = a_texCoord;\n"
    "}\n";

const char skyboxFragmentShaderStr[] =
    GLSL_VERSION
    PRECISION_MEDIUMP
    FRAGCOLOR_DECLARATION
    VARYING_IN " vec2 v_texCoord;\n"
    "uniform sampler2D u_texture;\n"
    "void main()\n"
    "{\n"
    "    " FRAGCOLOR " = " TEXTURE2D "(u_texture, v_texCoord);\n"
    "}\n";

const char plainVertexShaderStr[] =
    GLSL_VERSION
    PRECISION_MEDIUMP
    "uniform mat4 u_mvpMatrix;\n"
    "uniform mat4 u_textureMatrix;\n"
    ATTRIBUTE " vec4 a_position;\n"
    ATTRIBUTE " " LOWP " vec4 a_colour;\n"
    VARYING_OUT " " LOWP " vec2 v_texCoord;\n"
    VARYING_OUT " " LOWP " vec4 v_colour;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_mvpMatrix * a_position;\n"
    "    v_texCoord = (u_textureMatrix * a_position).xy;\n"
    "    v_colour = a_colour;\n"
    "}\n";

const char plainFragmentShaderStr[] =
    GLSL_VERSION
    PRECISION_LOWP
    FRAGCOLOR_DECLARATION
    VARYING_IN " vec2 v_texCoord;\n"
    "uniform sampler2D u_texture;\n"
    VARYING_IN " vec4 v_colour;\n"
    "void main()\n"
    "{\n"
    "    " FRAGCOLOR " = " TEXTURE2D "(u_texture, v_texCoord);\n"
    "    " FRAGCOLOR " *= v_colour;\n"
    "}\n";

const char terrainVertexShaderStr[] =
    GLSL_VERSION
    PRECISION_MEDIUMP
    "uniform mat4 u_mvpMatrix;\n"
    "uniform mat4 u_textureMatrix0;\n"
    "uniform mat4 u_textureMatrix1;\n"
    ATTRIBUTE " vec4 a_position;\n"
    VARYING_OUT " " LOWP " vec2 v_texCoord1;\n"
    VARYING_OUT " " LOWP " vec2 v_texCoord0;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_mvpMatrix * a_position;\n"
    "    v_texCoord0 = (u_textureMatrix0 * a_position).xy;\n"
    "    v_texCoord1 = (u_textureMatrix1 * a_position).xy;\n"
    "}\n";

const char terrainFragmentShaderStr[] =
    GLSL_VERSION
    PRECISION_LOWP
    FRAGCOLOR_DECLARATION
    VARYING_IN " vec2 v_texCoord0;\n"
    VARYING_IN " vec2 v_texCoord1;\n"
    "uniform sampler2D u_texture0;\n"
    "uniform sampler2D u_texture1;\n"
    "void main()\n"
    "{\n"
    "    " FRAGCOLOR " = " TEXTURE2D "(u_texture0, v_texCoord0);\n"
    "    " FRAGCOLOR " *= " TEXTURE2D "(u_texture1, v_texCoord1);\n"
    "}\n";

const char terrainPanoramaVertexShaderStr[] =
    GLSL_VERSION
    PRECISION_MEDIUMP
    "uniform mat4 u_mvpMatrix;\n"
    ATTRIBUTE " vec4 a_position;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_mvpMatrix * a_position;\n"
    "}\n";

const char terrainPanoramaFragmentShaderStr[] =
    GLSL_VERSION
    PRECISION_LOWP
    FRAGCOLOR_DECLARATION
    "void main()\n"
    "{\n"
    "    " FRAGCOLOR " = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "}\n";

const char overlayVertexShaderStr[] =
    GLSL_VERSION
    PRECISION_MEDIUMP
    "uniform mat4 u_mvpMatrix;\n"
    ATTRIBUTE " vec4 a_position;\n"
    ATTRIBUTE " vec2 a_texCoord;\n"
    VARYING_OUT " " LOWP " vec2 v_texCoord;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_mvpMatrix * a_position;\n"
    "    v_texCoord = a_texCoord;\n"
    "}\n";

const char overlayFragmentShaderStr[] =
    GLSL_VERSION
    PRECISION_LOWP
    FRAGCOLOR_DECLARATION
    VARYING_IN " vec2 v_texCoord;\n"
    "uniform sampler2D u_texture;\n"
    "uniform vec4 u_colour;\n"
    "void main()\n"
    "{\n"
    "    " FRAGCOLOR " = u_colour * " TEXTURE2D "(u_texture, v_texCoord);\n"
    "}\n";

const char modelVertexShaderStr[] =
    GLSL_VERSION
    PRECISION_MEDIUMP
    "uniform mat4 u_mvpMatrix;\n"
    "uniform mat3 u_normalMatrix;\n"
    ATTRIBUTE " vec4 a_position;\n"
    ATTRIBUTE " vec3 a_normal;\n"
    ATTRIBUTE " vec4 a_colour;\n"
    VARYING_OUT " " LOWP " vec4 v_colour;\n"
    VARYING_OUT " " LOWP " vec3 v_normal;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_mvpMatrix * a_position;\n"
    "    v_normal    = u_normalMatrix * a_normal;\n"
    "    v_colour    = a_colour;\n"
    "}\n";

const char modelFragmentShaderStr[] =
    GLSL_VERSION
    PRECISION_LOWP
    FRAGCOLOR_DECLARATION
    "uniform vec3      u_lightDir[5];\n"
    "uniform vec4      u_lightDiffuseColour[5];\n"
    "uniform vec4      u_lightSpecularColour[5];\n"
    "uniform vec4      u_lightAmbientColour[5];\n"
    "uniform float     u_specularAmount;\n"
    "uniform float     u_specularExponent;\n"
    VARYING_IN " vec4      v_colour;\n"
    VARYING_IN " " LOWP " vec3 v_normal;\n"
    "\n"
    "vec4 processLight(\n"
    "    vec3 normal,\n"
    "    vec3 lightDir,\n"
    "    vec4 lightDiffuseColour,\n"
    "    vec4 lightSpecularColour,\n"
    "    vec4 lightAmbientColour)\n"
    "{\n"
    "    vec4 colour = lightAmbientColour * v_colour;\n"
    "    // Diffuse\n"
    "    " LOWP " float ndotl = max(0.0, dot(normal, lightDir));\n"
    "    colour += ndotl * lightDiffuseColour * v_colour;\n"
    "    // Specular\n"
    "    " LOWP " vec3 h_vec = normalize(lightDir + vec3(0,0,1));\n"
    "    " LOWP " float ndoth = dot(normal, h_vec);\n"
    "    if (ndoth > 0.0)\n"
    "    {\n"
    "        colour += (pow(ndoth, u_specularExponent) * \n"
    "            vec4(u_specularAmount, u_specularAmount, u_specularAmount, 1) * lightSpecularColour);\n"
    "    }\n"
    "    return colour;\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec3 normal = normalize(v_normal);\n"
    "    " FRAGCOLOR "  = processLight(normal, u_lightDir[0], u_lightDiffuseColour[0], u_lightSpecularColour[0], u_lightAmbientColour[0]);\n"
    "    " FRAGCOLOR " += processLight(normal, u_lightDir[1], u_lightDiffuseColour[1], u_lightSpecularColour[1], u_lightAmbientColour[1]);\n"
    "    " FRAGCOLOR " += processLight(normal, u_lightDir[2], u_lightDiffuseColour[2], u_lightSpecularColour[2], u_lightAmbientColour[2]);\n"
    "    " FRAGCOLOR " += processLight(normal, u_lightDir[3], u_lightDiffuseColour[3], u_lightSpecularColour[3], u_lightAmbientColour[3]);\n"
    "    " FRAGCOLOR " += processLight(normal, u_lightDir[4], u_lightDiffuseColour[4], u_lightSpecularColour[4], u_lightAmbientColour[4]);\n"
    "    " FRAGCOLOR ".a = v_colour.a;\n"
    "}\n";

const char texturedModelVertexShaderStr[] =
    GLSL_VERSION
    PRECISION_MEDIUMP
    "uniform mat4 u_mvpMatrix;\n"
    "uniform mat3 u_normalMatrix;\n"
    ATTRIBUTE " vec4 a_position;\n"
    ATTRIBUTE " vec3 a_normal;\n"
    ATTRIBUTE " vec4 a_colour;\n"
    ATTRIBUTE " vec2 a_texCoord;\n"
    VARYING_OUT " " LOWP " vec4 v_colour;\n"
    VARYING_OUT " " LOWP " vec3 v_normal;\n"
    VARYING_OUT " " LOWP " vec2 v_texCoord;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_mvpMatrix * a_position;\n"
    "    v_normal    = u_normalMatrix * a_normal;\n"
    "    v_colour    = a_colour;\n"
    "    v_texCoord = a_texCoord;\n"
    "}\n";

const char texturedModelFragmentShaderStr[] =
    GLSL_VERSION
    PRECISION_LOWP
    FRAGCOLOR_DECLARATION
    "uniform vec3      u_lightDir[5];\n"
    "uniform vec4      u_lightDiffuseColour[5];\n"
    "uniform vec4      u_lightSpecularColour[5];\n"
    "uniform vec4      u_lightAmbientColour[5];\n"
    "uniform float     u_specularAmount;\n"
    "uniform float     u_specularExponent;\n"
    "uniform sampler2D u_texture;\n"
    "uniform float     u_texBias;\n"
    VARYING_IN " vec4      v_colour;\n"
    VARYING_IN " " LOWP " vec3 v_normal;\n"
    VARYING_IN " vec2      v_texCoord;\n"
    "\n"
    "vec4 processLight(\n"
    "    vec3 normal,\n"
    "    vec3 lightDir,\n"
    "    vec4 lightDiffuseColour,\n"
    "    vec4 lightSpecularColour,\n"
    "    vec4 lightAmbientColour)\n"
    "{\n"
    "    vec4 colour = lightAmbientColour * v_colour;\n"
    "    // Diffuse\n"
    "    " LOWP " float ndotl = max(0.0, dot(normal, lightDir));\n"
    "    colour += ndotl * lightDiffuseColour * v_colour;\n"
    "    // Specular\n"
    "    " LOWP " vec3 h_vec = normalize(lightDir + vec3(0,0,1));\n"
    "    " LOWP " float ndoth = dot(normal, h_vec);\n"
    "    if (ndoth > 0.0)\n"
    "    {\n"
    "        colour += (pow(ndoth, u_specularExponent) * \n"
    "            vec4(u_specularAmount, u_specularAmount, u_specularAmount, 1) * lightSpecularColour);\n"
    "    }\n"
    "    return colour;\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec3 normal = normalize(v_normal);\n"
    "    " FRAGCOLOR "  = processLight(normal, u_lightDir[0], u_lightDiffuseColour[0], u_lightSpecularColour[0], u_lightAmbientColour[0]);\n"
    "    " FRAGCOLOR " += processLight(normal, u_lightDir[1], u_lightDiffuseColour[1], u_lightSpecularColour[1], u_lightAmbientColour[1]);\n"
    "    " FRAGCOLOR " += processLight(normal, u_lightDir[2], u_lightDiffuseColour[2], u_lightSpecularColour[2], u_lightAmbientColour[2]);\n"
    "    " FRAGCOLOR " += processLight(normal, u_lightDir[3], u_lightDiffuseColour[3], u_lightSpecularColour[3], u_lightAmbientColour[3]);\n"
    "    " FRAGCOLOR " += processLight(normal, u_lightDir[4], u_lightDiffuseColour[4], u_lightSpecularColour[4], u_lightAmbientColour[4]);\n"
    "    " FRAGCOLOR ".a = v_colour.a;\n"
    "    " FRAGCOLOR " = min(" FRAGCOLOR ", 1.0);\n"
    "    " FRAGCOLOR " *= " TEXTURE2D "(u_texture, v_texCoord, u_texBias);\n"
    "}\n";

const char texturedModelSeparateSpecularFragmentShaderStr[] =
    GLSL_VERSION
    PRECISION_LOWP
    FRAGCOLOR_DECLARATION
    "uniform vec3      u_lightDir[5];\n"
    "uniform vec4      u_lightDiffuseColour[5];\n"
    "uniform vec4      u_lightSpecularColour[5];\n"
    "uniform vec4      u_lightAmbientColour[5];\n"
    "uniform float     u_specularAmount;\n"
    "uniform float     u_specularExponent;\n"
    "uniform sampler2D u_texture;\n"
    "uniform float     u_texBias;\n"
    VARYING_IN " vec4      v_colour;\n"
    VARYING_IN " " LOWP " vec3 v_normal;\n"
    VARYING_IN " vec2      v_texCoord;\n"
    "\n"
    "vec4 processLight(\n"
    "    vec4 fragColour,\n"
    "    vec3 normal,\n"
    "    vec3 lightDir,\n"
    "    vec4 lightDiffuseColour,\n"
    "    vec4 lightSpecularColour,\n"
    "    vec4 lightAmbientColour)\n"
    "{\n"
    "    vec4 colour = lightAmbientColour * fragColour;\n"
    "    // Diffuse\n"
    "    " LOWP " float ndotl = max(0.0, dot(normal, lightDir));\n"
    "    colour += ndotl * lightDiffuseColour * fragColour;\n"
    "    // Specular\n"
    "    " LOWP " vec3 h_vec = normalize(lightDir + vec3(0,0,1));\n"
    "    " LOWP " float ndoth = dot(normal, h_vec);\n"
    "    if (ndoth > 0.0)\n"
    "    {\n"
    "        colour += (pow(ndoth, u_specularExponent) * \n"
    "            vec4(u_specularAmount, u_specularAmount, u_specularAmount, 1) * lightSpecularColour);\n"
    "    }\n"
    "    return colour;\n"
    "}\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec3 normal = normalize(v_normal);\n"
    "    vec4 texColour = " TEXTURE2D "(u_texture, v_texCoord, u_texBias);\n"
    "    vec4 fragColour = v_colour * texColour;\n"
    "    " FRAGCOLOR "  = processLight(fragColour, normal, u_lightDir[0], u_lightDiffuseColour[0], u_lightSpecularColour[0], u_lightAmbientColour[0]);\n"
    "    " FRAGCOLOR " += processLight(fragColour, normal, u_lightDir[1], u_lightDiffuseColour[1], u_lightSpecularColour[1], u_lightAmbientColour[1]);\n"
    "    " FRAGCOLOR " += processLight(fragColour, normal, u_lightDir[2], u_lightDiffuseColour[2], u_lightSpecularColour[2], u_lightAmbientColour[2]);\n"
    "    " FRAGCOLOR " += processLight(fragColour, normal, u_lightDir[3], u_lightDiffuseColour[3], u_lightSpecularColour[3], u_lightAmbientColour[3]);\n"
    "    " FRAGCOLOR " += processLight(fragColour, normal, u_lightDir[4], u_lightDiffuseColour[4], u_lightSpecularColour[4], u_lightAmbientColour[4]);\n"
    "    " FRAGCOLOR ".a = v_colour.a * texColour.a;\n"
    "    " FRAGCOLOR " = min(" FRAGCOLOR ", 1.0);\n"
    "}\n";


const char shadowVertexShaderStr[] =
    GLSL_VERSION
    PRECISION_MEDIUMP
    "uniform mat4 u_mvpMatrix;\n"
    "uniform mat4 u_textureMatrix;\n"
    ATTRIBUTE " vec4 a_position;\n"
    ATTRIBUTE " vec4 a_texCoord;\n"
    VARYING_OUT " " LOWP " vec2 v_texCoord;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_mvpMatrix * a_position;\n"
    "    v_texCoord = (u_textureMatrix * a_texCoord).xy;\n"
    "}\n";

const char shadowFragmentShaderStr[] =
    GLSL_VERSION
    PRECISION_LOWP
    FRAGCOLOR_DECLARATION
    VARYING_IN " vec2 v_texCoord;\n"
    "uniform sampler2D u_texture;\n"
    "uniform vec4 u_colour;\n"
    "void main()\n"
    "{\n"
    "    " FRAGCOLOR " = u_colour * " TEXTURE2D "(u_texture, v_texCoord);\n"
    "}\n";

const char smokeVertexShaderStr[] =
    GLSL_VERSION
    PRECISION_MEDIUMP
    "uniform mat4 u_mvpMatrix;\n"
    ATTRIBUTE " vec4 a_position;\n"
    ATTRIBUTE " vec2 a_texCoord;\n"
    VARYING_OUT " " LOWP " vec2 v_texCoord;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = u_mvpMatrix * a_position;\n"
    "    v_texCoord = a_texCoord.xy;\n"
    "}\n";

const char smokeFragmentShaderStr[] =
    GLSL_VERSION
    PRECISION_LOWP
    FRAGCOLOR_DECLARATION
    VARYING_IN " vec2 v_texCoord;\n"
    "uniform sampler2D u_texture;\n"
    "uniform " LOWP " vec4 u_colour;\n"
    "void main()\n"
    "{\n"
    "    " FRAGCOLOR " = " TEXTURE2D "(u_texture, v_texCoord);\n"
    "    " FRAGCOLOR " *= u_colour;\n"
    "}\n";

//======================================================================================================================
void Shader::Init(const char* vertexShaderStr, const char* fragmentShaderStr)
{
    mVertexShaderStr = vertexShaderStr;
    mFragmentShaderStr = fragmentShaderStr;
    mShaderProgram = esLoadProgram(vertexShaderStr, fragmentShaderStr);
}

//======================================================================================================================
void Shader::Terminate()
{
    if (mShaderProgram > 0)
        glDeleteProgram(mShaderProgram);
    mShaderProgram = 0;
}

//======================================================================================================================
void Shader::Use() const
{
    IwAssert(ROWLHOUSE, mShaderProgram);
    glUseProgram(mShaderProgram);
}

//======================================================================================================================
static int getUniformLocation(int shaderProgram, const char* str)
{
    int loc = glGetUniformLocation(shaderProgram, str);
    IwAssert(ROWLHOUSE, loc >= 0);
    return loc;
}

//======================================================================================================================
static int getAttribLocation(int shaderProgram, const char* str)
{
    int loc = glGetAttribLocation(shaderProgram, str);
    IwAssert(ROWLHOUSE, loc >= 0);
    return loc;
}

//======================================================================================================================
void SimpleShader::Init()
{
    Shader::Init(simpleVertexShaderStr, simpleFragmentShaderStr);
    u_mvpMatrix = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    a_position  = getAttribLocation(mShaderProgram, "a_position");
    a_colour    = getAttribLocation(mShaderProgram, "a_colour");
}

//======================================================================================================================
void ControllerShader::Init()
{
    Shader::Init(controllerVertexShaderStr, controllerFragmentShaderStr);
    u_mvpMatrix = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    a_position  = getAttribLocation(mShaderProgram, "a_position");
    u_colour  = getUniformLocation(mShaderProgram, "u_colour");
}

//======================================================================================================================
void SkyboxShader::Init()
{
    Shader::Init(skyboxVertexShaderStr, skyboxFragmentShaderStr);
    u_mvpMatrix  = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    a_position   = getAttribLocation(mShaderProgram, "a_position");
    a_texCoord   = getAttribLocation(mShaderProgram, "a_texCoord");
    u_texture    = getUniformLocation(mShaderProgram, "u_texture");
}

//======================================================================================================================
void OverlayShader::Init()
{
    Shader::Init(overlayVertexShaderStr, overlayFragmentShaderStr);
    u_mvpMatrix = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    a_position  = getAttribLocation(mShaderProgram, "a_position");
    a_texCoord  = getAttribLocation(mShaderProgram, "a_texCoord");
    u_texture   = getUniformLocation(mShaderProgram, "u_texture");
    u_colour    = getUniformLocation(mShaderProgram, "u_colour");
}

//======================================================================================================================
void ModelShader::Init()
{
    Shader::Init(modelVertexShaderStr, modelFragmentShaderStr);
    SetupVars();
}

//======================================================================================================================
void ModelShader::SetupVars()
{
    u_mvpMatrix           = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    u_normalMatrix        = getUniformLocation(mShaderProgram, "u_normalMatrix");
    lightShaderInfo[0].u_lightDir         = getUniformLocation(mShaderProgram, "u_lightDir[0]");
    lightShaderInfo[1].u_lightDir         = getUniformLocation(mShaderProgram, "u_lightDir[1]");
    lightShaderInfo[2].u_lightDir         = getUniformLocation(mShaderProgram, "u_lightDir[2]");
    lightShaderInfo[3].u_lightDir         = getUniformLocation(mShaderProgram, "u_lightDir[3]");
    lightShaderInfo[4].u_lightDir         = getUniformLocation(mShaderProgram, "u_lightDir[4]");
    lightShaderInfo[0].u_lightAmbientColour  = getUniformLocation(mShaderProgram, "u_lightAmbientColour[0]");
    lightShaderInfo[1].u_lightAmbientColour  = getUniformLocation(mShaderProgram, "u_lightAmbientColour[1]");
    lightShaderInfo[2].u_lightAmbientColour  = getUniformLocation(mShaderProgram, "u_lightAmbientColour[2]");
    lightShaderInfo[3].u_lightAmbientColour  = getUniformLocation(mShaderProgram, "u_lightAmbientColour[3]");
    lightShaderInfo[4].u_lightAmbientColour  = getUniformLocation(mShaderProgram, "u_lightAmbientColour[4]");
    lightShaderInfo[0].u_lightDiffuseColour  = getUniformLocation(mShaderProgram, "u_lightDiffuseColour[0]");
    lightShaderInfo[1].u_lightDiffuseColour  = getUniformLocation(mShaderProgram, "u_lightDiffuseColour[1]");
    lightShaderInfo[2].u_lightDiffuseColour  = getUniformLocation(mShaderProgram, "u_lightDiffuseColour[2]");
    lightShaderInfo[3].u_lightDiffuseColour  = getUniformLocation(mShaderProgram, "u_lightDiffuseColour[3]");
    lightShaderInfo[4].u_lightDiffuseColour  = getUniformLocation(mShaderProgram, "u_lightDiffuseColour[4]");
    lightShaderInfo[0].u_lightSpecularColour = getUniformLocation(mShaderProgram, "u_lightSpecularColour[0]");
    lightShaderInfo[1].u_lightSpecularColour = getUniformLocation(mShaderProgram, "u_lightSpecularColour[1]");
    lightShaderInfo[2].u_lightSpecularColour = getUniformLocation(mShaderProgram, "u_lightSpecularColour[2]");
    lightShaderInfo[3].u_lightSpecularColour = getUniformLocation(mShaderProgram, "u_lightSpecularColour[3]");
    lightShaderInfo[4].u_lightSpecularColour = getUniformLocation(mShaderProgram, "u_lightSpecularColour[4]");
    u_specularAmount      = getUniformLocation(mShaderProgram, "u_specularAmount");
    u_specularExponent    = getUniformLocation(mShaderProgram, "u_specularExponent");
    a_position            = getAttribLocation(mShaderProgram, "a_position");
    a_normal              = getAttribLocation(mShaderProgram, "a_normal");
    a_colour              = getAttribLocation(mShaderProgram, "a_colour");
}


//======================================================================================================================
void TexturedModelShader::Init()
{
    Shader::Init(texturedModelVertexShaderStr, texturedModelFragmentShaderStr);
    ModelShader::SetupVars();
    a_texCoord            = getAttribLocation(mShaderProgram, "a_texCoord");
    u_texture             = getUniformLocation(mShaderProgram, "u_texture");
    u_texBias             = getUniformLocation(mShaderProgram, "u_texBias");
}

//======================================================================================================================
void TexturedModelSeparateSpecularShader::Init()
{
    Shader::Init(texturedModelVertexShaderStr, texturedModelSeparateSpecularFragmentShaderStr);
    ModelShader::SetupVars();
    a_texCoord            = getAttribLocation(mShaderProgram, "a_texCoord");
    u_texture             = getUniformLocation(mShaderProgram, "u_texture");
    u_texBias             = getUniformLocation(mShaderProgram, "u_texBias");
}

//======================================================================================================================
void PlainShader::Init()
{
    Shader::Init(plainVertexShaderStr, plainFragmentShaderStr);
    u_mvpMatrix     = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    u_textureMatrix = getUniformLocation(mShaderProgram, "u_textureMatrix");
    a_position      = getAttribLocation(mShaderProgram, "a_position");
    a_colour        = getAttribLocation(mShaderProgram, "a_colour");
    u_texture       = getUniformLocation(mShaderProgram, "u_texture");
}

//======================================================================================================================
void TerrainShader::Init()
{
    Shader::Init(terrainVertexShaderStr, terrainFragmentShaderStr);
    u_mvpMatrix      = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    u_textureMatrix0 = getUniformLocation(mShaderProgram, "u_textureMatrix0");
    u_textureMatrix1 = getUniformLocation(mShaderProgram, "u_textureMatrix1");
    a_position       = getAttribLocation(mShaderProgram, "a_position");
    u_texture0       = getUniformLocation(mShaderProgram, "u_texture0");
    u_texture1       = getUniformLocation(mShaderProgram, "u_texture1");
}

//======================================================================================================================
void TerrainPanoramaShader::Init()
{
    Shader::Init(terrainPanoramaVertexShaderStr, terrainPanoramaFragmentShaderStr);
    u_mvpMatrix      = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    a_position       = getAttribLocation(mShaderProgram, "a_position");
}

//======================================================================================================================
void ShadowShader::Init()
{
    Shader::Init(shadowVertexShaderStr, shadowFragmentShaderStr);
    u_mvpMatrix      = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    u_textureMatrix  = getUniformLocation(mShaderProgram, "u_textureMatrix");
    a_position       = getAttribLocation(mShaderProgram, "a_position");
    a_texCoord       = getAttribLocation(mShaderProgram, "a_texCoord");
    u_texture        = getUniformLocation(mShaderProgram, "u_texture");
    u_colour         = getUniformLocation(mShaderProgram, "u_colour");
}

//======================================================================================================================
void SmokeShader::Init()
{
    Shader::Init(smokeVertexShaderStr, smokeFragmentShaderStr);
    u_mvpMatrix      = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    a_position       = getAttribLocation(mShaderProgram, "a_position");
    a_texCoord       = getAttribLocation(mShaderProgram, "a_texCoord");
    u_colour         = getUniformLocation(mShaderProgram, "u_colour");
    u_texture        = getUniformLocation(mShaderProgram, "u_texture");
}


