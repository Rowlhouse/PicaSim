#include "Shaders.h"
#include "Trace.h"

// Use appropriate GLSL version for platform
// Desktop GL 3.0 uses GLSL 130, OpenGL ES 2.0 uses GLSL 100
#if defined(PS_PLATFORM_ANDROID) || defined(PS_PLATFORM_IOS)
#define GLSL(src) "#version 100\n" #src
#else
// Desktop GL - use version 130 which supports precision qualifiers as no-ops
#define GLSL(src) "#version 130\n" #src
#endif

const char simpleVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    attribute vec4 a_position;
    attribute vec4 a_colour;
    varying lowp vec4 v_colour;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_colour = a_colour;
    }
);

const char simpleFragmentShaderStr[] = GLSL(
    precision lowp float;
    varying vec4 v_colour;
    void main()
    {
        gl_FragColor = v_colour;
    }
);

const char controllerVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    attribute vec4 a_position;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
    }
);

const char controllerFragmentShaderStr[] = GLSL(
    precision lowp float;
    uniform vec4 u_colour;
    void main()
    {
        gl_FragColor = u_colour;
    }
);

const char skyboxVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4   u_mvpMatrix;
    attribute vec4 a_position;
    attribute vec2 a_texCoord;
    varying lowp vec2 v_texCoord;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_texCoord = a_texCoord;
    }
);

const char skyboxFragmentShaderStr[] = GLSL(
    precision mediump float;
    varying vec2 v_texCoord;
    uniform sampler2D u_texture;
    void main()
    {
        gl_FragColor = texture2D(u_texture, v_texCoord);
    }
);

const char plainVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    uniform mat4 u_textureMatrix;
    attribute vec4 a_position;
    attribute lowp vec4 a_colour;
    varying lowp vec2 v_texCoord;
    varying lowp vec4 v_colour;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_texCoord = (u_textureMatrix * a_position).xy;
        v_colour = a_colour;
    }
);

const char plainFragmentShaderStr[] = GLSL(
    precision lowp float;
    varying vec2 v_texCoord;
    uniform sampler2D u_texture;
    varying vec4 v_colour;
    void main()
    {
        gl_FragColor = texture2D(u_texture, v_texCoord);
        gl_FragColor *= v_colour;
    }
);

const char terrainVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    uniform mat4 u_textureMatrix0;
    uniform mat4 u_textureMatrix1;
    attribute vec4 a_position;
    varying lowp vec2 v_texCoord1;
    varying lowp vec2 v_texCoord0;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_texCoord0 = (u_textureMatrix0 * a_position).xy;
        v_texCoord1 = (u_textureMatrix1 * a_position).xy;
    }
);

const char terrainFragmentShaderStr[] = GLSL(
    precision lowp float;
    varying vec2 v_texCoord0;
    varying vec2 v_texCoord1;
    uniform sampler2D u_texture0;
    uniform sampler2D u_texture1;
    void main()
    {
        gl_FragColor = texture2D(u_texture0, v_texCoord0);
        gl_FragColor *= texture2D(u_texture1, v_texCoord1);
    }
);

const char terrainPanoramaVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    attribute vec4 a_position;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
    }
);

const char terrainPanoramaFragmentShaderStr[] = GLSL(
    precision lowp float;
    void main()
    {
        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
);

const char overlayVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    attribute vec4 a_position;
    attribute vec2 a_texCoord;
    varying lowp vec2 v_texCoord;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_texCoord = a_texCoord;
    }
);

const char overlayFragmentShaderStr[] = GLSL(
    precision lowp float;
    varying vec2 v_texCoord;
    uniform sampler2D u_texture;
    uniform vec4 u_colour;
    void main()
    {
        gl_FragColor = u_colour * texture2D(u_texture, v_texCoord);
    }
);

const char modelVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    uniform mat3 u_normalMatrix;
    attribute vec4 a_position;
    attribute vec3 a_normal;
    attribute vec4 a_colour;
    varying lowp vec4 v_colour;
    varying lowp vec3 v_normal;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_normal    = u_normalMatrix * a_normal;
        v_colour    = a_colour;
    }
);

const char modelFragmentShaderStr[] = GLSL(
    precision lowp float;
    uniform vec3      u_lightDir[5];
    uniform vec4      u_lightDiffuseColour[5];
    uniform vec4      u_lightSpecularColour[5];
    uniform vec4      u_lightAmbientColour[5];
    uniform float     u_specularAmount;
    uniform float     u_specularExponent;
    varying vec4      v_colour;
    varying lowp vec3 v_normal;

    vec4 processLight(
        vec3 normal,
        vec3 lightDir,
        vec4 lightDiffuseColour,
        vec4 lightSpecularColour,
        vec4 lightAmbientColour)
    {
        vec4 colour = lightAmbientColour * v_colour;
        // Diffuse
        lowp float ndotl = max(0.0, dot(normal, lightDir));
        colour += ndotl * lightDiffuseColour * v_colour;
        // Specular
        lowp vec3 h_vec = normalize(lightDir + vec3(0,0,1));
        lowp float ndoth = dot(normal, h_vec);
        if (ndoth > 0.0)
        {
            colour += (pow(ndoth, u_specularExponent) * 
                vec4(u_specularAmount, u_specularAmount, u_specularAmount, 1) * lightSpecularColour);
        }
        return colour;
    }

    void main()
    {
        vec3 normal = normalize(v_normal);
        gl_FragColor  = processLight(normal, u_lightDir[0], u_lightDiffuseColour[0], u_lightSpecularColour[0], u_lightAmbientColour[0]);
        gl_FragColor += processLight(normal, u_lightDir[1], u_lightDiffuseColour[1], u_lightSpecularColour[1], u_lightAmbientColour[1]);
        gl_FragColor += processLight(normal, u_lightDir[2], u_lightDiffuseColour[2], u_lightSpecularColour[2], u_lightAmbientColour[2]);
        gl_FragColor += processLight(normal, u_lightDir[3], u_lightDiffuseColour[3], u_lightSpecularColour[3], u_lightAmbientColour[3]);
        gl_FragColor += processLight(normal, u_lightDir[4], u_lightDiffuseColour[4], u_lightSpecularColour[4], u_lightAmbientColour[4]);
        gl_FragColor.a = v_colour.a;
    }
);

const char texturedModelVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    uniform mat3 u_normalMatrix;
    attribute vec4 a_position;
    attribute vec3 a_normal;
    attribute vec4 a_colour;
    attribute vec2 a_texCoord;
    varying lowp vec4 v_colour;
    varying lowp vec3 v_normal;
    varying lowp vec2 v_texCoord;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_normal    = u_normalMatrix * a_normal;
        v_colour    = a_colour;
        v_texCoord = a_texCoord;
    }
);

const char texturedModelFragmentShaderStr[] = GLSL(
    precision lowp float;
    uniform vec3      u_lightDir[5];
    uniform vec4      u_lightDiffuseColour[5];
    uniform vec4      u_lightSpecularColour[5];
    uniform vec4      u_lightAmbientColour[5];
    uniform float     u_specularAmount;
    uniform float     u_specularExponent;
    uniform sampler2D u_texture;
    uniform float     u_texBias;
    varying vec4      v_colour;
    varying lowp vec3 v_normal;
    varying vec2      v_texCoord;

    vec4 processLight(
        vec3 normal,
        vec3 lightDir,
        vec4 lightDiffuseColour,
        vec4 lightSpecularColour,
        vec4 lightAmbientColour)
    {
        vec4 colour = lightAmbientColour * v_colour;
        // Diffuse
        lowp float ndotl = max(0.0, dot(normal, lightDir));
        colour += ndotl * lightDiffuseColour * v_colour;
        // Specular
        lowp vec3 h_vec = normalize(lightDir + vec3(0,0,1));
        lowp float ndoth = dot(normal, h_vec);
        if (ndoth > 0.0)
        {
            colour += (pow(ndoth, u_specularExponent) * 
                vec4(u_specularAmount, u_specularAmount, u_specularAmount, 1) * lightSpecularColour);
        }
        return colour;
    }

    void main()
    {
        vec3 normal = normalize(v_normal);
        gl_FragColor  = processLight(normal, u_lightDir[0], u_lightDiffuseColour[0], u_lightSpecularColour[0], u_lightAmbientColour[0]);
        gl_FragColor += processLight(normal, u_lightDir[1], u_lightDiffuseColour[1], u_lightSpecularColour[1], u_lightAmbientColour[1]);
        gl_FragColor += processLight(normal, u_lightDir[2], u_lightDiffuseColour[2], u_lightSpecularColour[2], u_lightAmbientColour[2]);
        gl_FragColor += processLight(normal, u_lightDir[3], u_lightDiffuseColour[3], u_lightSpecularColour[3], u_lightAmbientColour[3]);
        gl_FragColor += processLight(normal, u_lightDir[4], u_lightDiffuseColour[4], u_lightSpecularColour[4], u_lightAmbientColour[4]);
        gl_FragColor.a = v_colour.a;
        gl_FragColor = min(gl_FragColor, 1.0);
        gl_FragColor *= texture2D(u_texture, v_texCoord, u_texBias);
    }
);

const char texturedModelSeparateSpecularFragmentShaderStr[] = GLSL(
    precision lowp float;
    uniform vec3      u_lightDir[5];
    uniform vec4      u_lightDiffuseColour[5];
    uniform vec4      u_lightSpecularColour[5];
    uniform vec4      u_lightAmbientColour[5];
    uniform float     u_specularAmount;
    uniform float     u_specularExponent;
    uniform sampler2D u_texture;
    uniform float     u_texBias;
    varying vec4      v_colour;
    varying lowp vec3 v_normal;
    varying vec2      v_texCoord;

    vec4 processLight(
        vec4 fragColour,
        vec3 normal,
        vec3 lightDir,
        vec4 lightDiffuseColour,
        vec4 lightSpecularColour,
        vec4 lightAmbientColour)
    {
        vec4 colour = lightAmbientColour * fragColour;
        // Diffuse
        lowp float ndotl = max(0.0, dot(normal, lightDir));
        colour += ndotl * lightDiffuseColour * fragColour;
        // Specular
        lowp vec3 h_vec = normalize(lightDir + vec3(0,0,1));
        lowp float ndoth = dot(normal, h_vec);
        if (ndoth > 0.0)
        {
            colour += (pow(ndoth, u_specularExponent) * 
                vec4(u_specularAmount, u_specularAmount, u_specularAmount, 1) * lightSpecularColour);
        }
        return colour;
    }

    void main()
    {
        vec3 normal = normalize(v_normal);
        vec4 texColour = texture2D(u_texture, v_texCoord, u_texBias);
        vec4 fragColour = v_colour * texColour;
        gl_FragColor  = processLight(fragColour, normal, u_lightDir[0], u_lightDiffuseColour[0], u_lightSpecularColour[0], u_lightAmbientColour[0]);
        gl_FragColor += processLight(fragColour, normal, u_lightDir[1], u_lightDiffuseColour[1], u_lightSpecularColour[1], u_lightAmbientColour[1]);
        gl_FragColor += processLight(fragColour, normal, u_lightDir[2], u_lightDiffuseColour[2], u_lightSpecularColour[2], u_lightAmbientColour[2]);
        gl_FragColor += processLight(fragColour, normal, u_lightDir[3], u_lightDiffuseColour[3], u_lightSpecularColour[3], u_lightAmbientColour[3]);
        gl_FragColor += processLight(fragColour, normal, u_lightDir[4], u_lightDiffuseColour[4], u_lightSpecularColour[4], u_lightAmbientColour[4]);
        gl_FragColor.a = v_colour.a * texColour.a;
        gl_FragColor = min(gl_FragColor, 1.0);
    }
);


const char shadowVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    uniform mat4 u_textureMatrix;
    attribute vec4 a_position;
    attribute vec4 a_texCoord;
    varying lowp vec2 v_texCoord;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_texCoord = (u_textureMatrix * a_texCoord).xy;
    }
);

const char shadowFragmentShaderStr[] = GLSL(
    precision lowp float;
    varying vec2 v_texCoord;
    uniform sampler2D u_texture;
    uniform vec4 u_colour;
    void main()
    {
        gl_FragColor = u_colour * texture2D(u_texture, v_texCoord);
    }
);

const char smokeVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    attribute vec4 a_position;
    attribute vec2 a_texCoord;
    varying lowp vec2 v_texCoord;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_texCoord = a_texCoord.xy;
    }
);

const char smokeFragmentShaderStr[] = GLSL(
    precision lowp float;
    varying vec2 v_texCoord;
    uniform sampler2D u_texture;
    uniform lowp vec4 u_colour;
    void main()
    {
        gl_FragColor = texture2D(u_texture, v_texCoord);
        gl_FragColor *= u_colour;
    }
);

// VR Skybox with depth-based parallax for stereoscopic effect
const char skyboxVRParallaxVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    attribute vec4 a_position;
    attribute vec2 a_texCoord;
    varying vec2 v_texCoord;
    varying vec3 v_position;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_texCoord = a_texCoord;
        v_position = a_position.xyz;
    }
);

const char skyboxVRParallaxFragmentShaderStr[] = GLSL(
    precision mediump float;
    varying vec2 v_texCoord;
    varying vec3 v_position;
    uniform sampler2D u_skyboxTexture;
    uniform sampler2D u_depthTexture;
    uniform float u_eyeOffset;      // -1.0 for left eye, +1.0 for right eye
    uniform float u_ipd;            // interpupillary distance in world units
    uniform float u_nearPlane;
    uniform float u_farPlane;
    uniform float u_skyDistance;    // distance for sky pixels (far plane)
    uniform vec2 u_screenSize;      // screen width and height
    uniform float u_parallaxScale;  // scale factor for parallax effect
    uniform vec2 u_parallaxDir;     // direction of parallax in UV space
    uniform float u_tileScale;      // numPerSide - scales parallax for tile coordinates
    uniform int u_faceType;         // 0 = side faces, 1 = up/down faces (disabled)

    void main()
    {
        // Sample depth at this fragment's screen position
        vec2 screenUV = gl_FragCoord.xy / u_screenSize;
        float depthSample = texture2D(u_depthTexture, screenUV).r;

        // Convert from normalized depth [0,1] to linear depth
        float linearDepth = u_nearPlane * u_farPlane /
                            (u_farPlane - depthSample * (u_farPlane - u_nearPlane));

        // For sky pixels (at or near far plane), use configurable sky distance
        if (depthSample > 0.9999)
        {
            linearDepth = u_skyDistance;
        }

        // Base parallax angle (radians, approximately)
        float parallaxAngle = (u_ipd * 0.5) / linearDepth * u_eyeOffset;

        // Convert to UV space: dU/dtheta is approx 0.5 for cube skybox faces
        // Also scale by tileScale for tiled panoramas (each tile UV spans 1/numPerSide)
        float parallaxMagnitude = parallaxAngle * 0.5 * u_parallaxScale * u_tileScale;

        // Perspective correction for oblique viewing angles
        // At face center, the view is perpendicular (angle phi = 0)
        // At edges/corners, the view is oblique (phi > 0), reducing parallax effect
        // Correction factor: 1/cos(phi) = length(position) / position.x
        // All faces use the same vertex data with x = scale (rotations are in MVP matrix)
        float correction = length(v_position) / v_position.x;
        parallaxMagnitude *= correction;

        // Disable parallax for up/down faces (complex circular motion)
        if (u_faceType == 1)
            parallaxMagnitude = 0.0;

        // Apply parallax offset in the face-specific direction
        vec2 offsetUV = v_texCoord + u_parallaxDir * parallaxMagnitude;

        // Clamp UV to valid range
        offsetUV = clamp(offsetUV, 0.0, 1.0);

        gl_FragColor = texture2D(u_skyboxTexture, offsetUV);
    }
);

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

//======================================================================================================================
void SkyboxVRParallaxShader::Init()
{
    Shader::Init(skyboxVRParallaxVertexShaderStr, skyboxVRParallaxFragmentShaderStr);
    u_mvpMatrix      = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    a_position       = getAttribLocation(mShaderProgram, "a_position");
    a_texCoord       = getAttribLocation(mShaderProgram, "a_texCoord");
    u_skyboxTexture  = getUniformLocation(mShaderProgram, "u_skyboxTexture");
    u_depthTexture   = getUniformLocation(mShaderProgram, "u_depthTexture");
    u_eyeOffset      = getUniformLocation(mShaderProgram, "u_eyeOffset");
    u_ipd            = getUniformLocation(mShaderProgram, "u_ipd");
    u_nearPlane      = getUniformLocation(mShaderProgram, "u_nearPlane");
    u_farPlane       = getUniformLocation(mShaderProgram, "u_farPlane");
    u_skyDistance    = getUniformLocation(mShaderProgram, "u_skyDistance");
    u_screenSize     = getUniformLocation(mShaderProgram, "u_screenSize");
    u_parallaxScale  = getUniformLocation(mShaderProgram, "u_parallaxScale");
    u_parallaxDir    = getUniformLocation(mShaderProgram, "u_parallaxDir");
    u_tileScale      = getUniformLocation(mShaderProgram, "u_tileScale");
    u_faceType       = getUniformLocation(mShaderProgram, "u_faceType");
}


