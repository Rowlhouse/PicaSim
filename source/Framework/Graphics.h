#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Helpers.h"
#include "../Platform/Platform.h"
#include "../Platform/Texture.h"

// OpenGL headers
#ifdef _WIN32
#include <glad/glad.h>
#else
#ifdef PS_PLATFORM_ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif defined(PS_PLATFORM_IOS)
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#endif

extern int gGLVersion;

typedef GLfloat GLMat44[4][4];
typedef GLfloat GLMat33[3][3];
typedef GLfloat GLVec4[4];
typedef GLfloat GLVec3[3];

// Note: Texture typedef is now defined in Platform/Texture.h

// Graphics state reset functions (for transitioning between UI and 3D rendering)
void ResetGraphicsState(bool clear);
void PrepareForUIRendering(bool clear);

void SaveScreenshot();
void SaveScreenshotAsTexture(Texture* texture);

// Graphics initialization (now uses SDL2 Window - see Platform/Window.h)
// Legacy compatibility - these now delegate to Window class
int eglInit(bool createSurface, int msaaSamples = 0);
void eglTerm(bool destroySurface);

inline void ConvertTransformToGLMat44(const Transform& tm, GLMat44& mat44)
{
    mat44[0][0] = tm.RowX().x;
    mat44[0][1] = tm.RowX().y;
    mat44[0][2] = tm.RowX().z;
    mat44[0][3] = 0.0;
    mat44[1][0] = tm.RowY().x;
    mat44[1][1] = tm.RowY().y;
    mat44[1][2] = tm.RowY().z;
    mat44[1][3] = 0.0;
    mat44[2][0] = tm.RowZ().x;
    mat44[2][1] = tm.RowZ().y;
    mat44[2][2] = tm.RowZ().z;
    mat44[2][3] = 0.0;
    mat44[3][0] = tm.GetTrans().x;
    mat44[3][1] = tm.GetTrans().y;
    mat44[3][2] = tm.GetTrans().z;
    mat44[3][3] = 1.0;
}

inline void ConvertGLMat44ToTransform(const GLMat44& mat44, Transform& tm)
{
    tm.m[0][0] =     mat44[0][0];
    tm.m[0][1] =     mat44[0][1];
    tm.m[0][2] =     mat44[0][2];
    tm.m[1][0] =     mat44[1][0];
    tm.m[1][1] =     mat44[1][1];
    tm.m[1][2] =     mat44[1][2];
    tm.m[2][0] =     mat44[2][0];
    tm.m[2][1] =     mat44[2][1];
    tm.m[2][2] =     mat44[2][2];
    tm.t.x = mat44[3][0];
    tm.t.y = mat44[3][1];
    tm.t.z = mat44[3][2];
}

/// Loads an image into the texture, making sure that if it's too big then it gets scaled down safely.
/// colourOffset adjusts the HSV result - modifying H.
void LoadTextureFromFile(Texture& texture, const char* filename, float colourOffset = 0.0f);

/// \brief Load a shader, check for compile errors, print error messages to output log
/// \param type Type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
/// \param shaderSrc Shader source string
/// \return A new shader object on success, 0 on failure
GLuint esLoadShader( GLenum type, const char *shaderSrc );

/// \brief Load a vertex and fragment shader, create a program object, link program.
///        Errors output to log.
/// \param vertShaderSrc Vertex shader source code
/// \param fragShaderSrc Fragment shader source code
/// \return A new program object linked with the vertex/fragment shader pair, 0 on failure
GLuint esLoadProgram( const char *vertShaderSrc, const char *fragShaderSrc );

/// \brief multiply matrix specified by result with a scaling matrix and return new matrix in result
/// \param result Specifies the input matrix.  Scaled matrix is returned in result.
/// \param sx, sy, sz Scale factors along the x, y and z axes respectively
void esMatrixScale(GLMat44& result, GLfloat sx, GLfloat sy, GLfloat sz);

/// \brief multiply matrix specified by result with a translation matrix and return new matrix in result
/// \param result Specifies the input matrix.  Translated matrix is returned in result.
/// \param tx, ty, tz Scale factors along the x, y and z axes respectively
void esMatrixTranslate(GLMat44& result, GLfloat tx, GLfloat ty, GLfloat tz);

/// \brief multiply matrix specified by result with a rotation matrix and return new matrix in result
/// \param result Specifies the input matrix.  Rotated matrix is returned in result.
/// \param angle Specifies the angle of rotation, in degrees.
/// \param x, y, z Specify the x, y and z coordinates of a vector, respectively
void esMatrixRotate(GLMat44& result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);

/// \brief as esMatrixRotate, but pass in sin and cos of the angle, and x, y, z are assumed to be normalised
void esMatrixRotateFast(GLMat44& result, GLfloat sinAngle, GLfloat cosAngle, GLfloat x, GLfloat y, GLfloat z);

// \brief multiply matrix specified by result with a perspective matrix and return new matrix in result
/// \param result Specifies the input matrix.  new matrix is returned in result.
/// \param left, right Coordinates for the left and right vertical clipping planes
/// \param bottom, top Coordinates for the bottom and top horizontal clipping planes
/// \param nearZ, farZ Distances to the near and far depth clipping planes.  Both distances must be positive.
void esMatrixFrustum(GLMat44& result, float left, float right, float bottom, float top, float nearZ, float farZ);

/// \brief multiply matrix specified by result with a perspective matrix and return new matrix in result
/// \param result Specifies the input matrix.  new matrix is returned in result.
/// \param fovy Field of view y angle in degrees
/// \param aspect Aspect ratio of screen
/// \param nearZ Near plane distance
/// \param farZ Far plane distance
void esMatrixPerspective(GLMat44& result, float fovy, float aspect, float nearZ, float farZ);

/// \brief multiply matrix specified by result with a perspective matrix and return new matrix in result
/// \param result Specifies the input matrix.  new matrix is returned in result.
/// \param left, right Coordinates for the left and right vertical clipping planes
/// \param bottom, top Coordinates for the bottom and top horizontal clipping planes
/// \param nearZ, farZ Distances to the near and far depth clipping planes.  These values are negative if plane is behind the viewer
void esMatrixOrtho(GLMat44& result, float left, float right, float bottom, float top, float nearZ, float farZ);

/// \brief perform the following operation - result matrix = srcA matrix * srcB matrix
/// \param result Returns multiplied matrix
/// \param srcA, srcB Input matrices to be multiplied
void esMatrixMultiply(GLMat44& result, const GLMat44& srcA, const GLMat44& srcB);

//// \brief return an indentity matrix 
//// \param result returns identity matrix
void esMatrixLoadIdentity(GLMat44& result);

//// \brief return an transposed copy of src
void esMatrixTranspose(GLMat44& result, const GLMat44& src);

/// Copy src into result
void esMatrixCopy(GLMat44& result, const GLMat44& src);

/// Extract rotation - top left 3x3
void esMatrixCopyRotation(GLMat33& result, const GLMat44& src);

/// Transform a vec4
void esMatrixTransform(GLVec4& result, const GLMat44& m, const GLVec4& v);

/// Transform a vec4 with only the 3x3 part
void esMatrixTransform3x3(GLVec4& result, const GLMat44& m, const GLVec4& v);

/// Copy src into result
void esVector4Copy(GLVec4& result, const GLVec4& src);

/// Initialise a Vec4
void esSetVector4(GLVec4& result, float x, float y, float z, float w);

/// Wrappers for matrix stack
void esMatrixMode(GLenum mode);
void esLoadIdentity();
void esPushMatrix();
void esPopMatrix();
void esGetMatrix(GLMat44& m, GLenum mode);
void esTranslatef(float x, float y, float z);
void esRotatef(float angle, float x, float y, float z);
void esRotateFast(float sinAngle, float cosAngle, float x, float y, float z);
void esScalef(float x, float y, float z);
void esMultMatrixf(const float* m);
void esFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
void esOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);

#define ROTATE_90_X  esRotateFast( 1.0f,  0.0f, 1.0f, 0.0f, 0.0f)
#define ROTATE_180_X esRotateFast( 0.0f, -1.0f, 1.0f, 0.0f, 0.0f)
#define ROTATE_270_X esRotateFast(-1.0f,  0.0f, 1.0f, 0.0f, 0.0f)
#define ROTATE_90_Y  esRotateFast( 1.0f,  0.0f, 0.0f, 1.0f, 0.0f)
#define ROTATE_180_Y esRotateFast( 0.0f, -1.0f, 0.0f, 1.0f, 0.0f)
#define ROTATE_270_Y esRotateFast(-1.0f,  0.0f, 0.0f, 1.0f, 0.0f)
#define ROTATE_90_Z  esRotateFast( 1.0f,  0.0f, 0.0f, 0.0f, 1.0f)
#define ROTATE_180_Z esRotateFast( 0.0f, -1.0f, 0.0f, 0.0f, 1.0f)
#define ROTATE_270_Z esRotateFast(-1.0f,  0.0f, 0.0f, 0.0f, 1.0f)

/// Wrappers for lighting
void esSetLightPos(GLenum light, const GLVec4& lightPos);
void esSetLightDiffuseColour(GLenum light, const GLVec4& diffuseColour);
void esSetLightAmbientColour(GLenum light, const GLVec4& ambientColour);
void esSetLightSpecularColour(GLenum light, const GLVec4& specularColour);

/// Sets just the modelview-projection matrix
void esSetModelViewProjectionMatrix(int mvpMatrixLoc);
/// Sets the mvp and the 3x3 matrix for transforming normals
void esSetModelViewProjectionAndNormalMatrix(int mvpMatrixLoc, int normalMatrixLoc);

/// Sets just the texture matrix
void esSetTextureMatrix(int textureMatrixLoc);

/// Sets the lighting shader variables
void esSetLighting(const struct LightShaderInfo lightShaderInfo[5]);

// Returns the view transform (inverse of camera) as well as setting up the GL modelview matrix (OpenGL 1)
void LookAt(
    GLMat44& viewTM,
    GLfloat eyex, GLfloat eyey, GLfloat eyez,
    GLfloat centerx, GLfloat centery, GLfloat centerz,
    GLfloat upx, GLfloat upy, GLfloat upz);

// Natural lighting state is off
struct EnableLighting {
    EnableLighting() {if (gGLVersion == 1) glEnable(GL_LIGHTING);}
    ~EnableLighting() {if (gGLVersion == 1) glDisable(GL_LIGHTING);}
};

struct EnableBlend {
    EnableBlend() {glEnable(GL_BLEND);}
    ~EnableBlend() {glDisable(GL_BLEND);}
};

struct DisableDepthMask {
    DisableDepthMask() {glDepthMask(GL_FALSE);}
    ~DisableDepthMask() {glDepthMask(GL_TRUE);}
};

struct DisableDepthTest {
    DisableDepthTest() {glDisable(GL_DEPTH_TEST);}
    ~DisableDepthTest() {glEnable(GL_DEPTH_TEST);}
};

struct EnableNormalScaling {
    EnableNormalScaling() {if (gGLVersion == 1) glEnable(GL_RESCALE_NORMAL);}
    ~EnableNormalScaling() {if (gGLVersion == 1) glDisable(GL_RESCALE_NORMAL);}
};

struct EnableNormalNormalisation {
    EnableNormalNormalisation() {if (gGLVersion == 1) glEnable(GL_NORMALIZE);}
    ~EnableNormalNormalisation() {if (gGLVersion == 1) glDisable(GL_NORMALIZE);}
};

struct EnableCullFace {
    EnableCullFace(GLenum mode) {glEnable(GL_CULL_FACE); glCullFace(mode);}
    ~EnableCullFace() {glDisable(GL_CULL_FACE);}
};

struct FrontFaceCW {
    FrontFaceCW() {glFrontFace(GL_CW);}
    ~FrontFaceCW() {glFrontFace(GL_CCW);}
};

struct PushMatrix {
    PushMatrix() {esPushMatrix();}
    ~PushMatrix() {esPopMatrix();}
};

// Fog is disabled - DisableFog is a no-op RAII guard
struct DisableFog {
    DisableFog() {}
    ~DisableFog() {}
};

//======================================================================================================================
struct RGB 
{
    RGB(float R = 0.0f, float G = 0.0f, float B = 0.0f) : r(R), g(G), b(B) {}
    float r;       // 0-1
    float g;       // 0-1
    float b;       // 0-1
};

//======================================================================================================================
struct HSV 
{
    HSV(float H = 0.0f, float S = 0.0f, float V = 0.0f) : h(H), s(S), v(V) {}
    float h;       // angle in degrees
    float s;       // 0-1
    float v;       // 0-1
};

HSV RGB2HSV(RGB in);
RGB HSV2RGB(HSV in);

Vector3 RGB2HSV(const Vector3& rgb);
Vector3 HSV2RGB(const Vector3& rgb);

// Offsets the HSV version of col, with offset = 0-1
void OffsetColour(float col[4], float offset);

// Convert Vector3 (0-1 RGB) to Colour class
Colour ConvertToColour(const Vector3& colour);

//==============================================================================
// OpenGL ES 1.x Compatibility Layer
// These functions/macros provide compatibility for code written for OpenGL ES 1.x
//==============================================================================

#ifdef _WIN32
// Desktop OpenGL uses double versions - provide float wrappers
inline void glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
        glFrustum((GLdouble)left, (GLdouble)right, (GLdouble)bottom, (GLdouble)top, (GLdouble)zNear, (GLdouble)zFar);
}

inline void glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
        glOrtho((GLdouble)left, (GLdouble)right, (GLdouble)bottom, (GLdouble)top, (GLdouble)zNear, (GLdouble)zFar);
}

// OpenGL ES 1.x OES extension function mappings for framebuffers
// These are the same as the standard GL_ARB_framebuffer_object extensions
#ifndef GL_FRAMEBUFFER_OES
#define GL_FRAMEBUFFER_OES GL_FRAMEBUFFER
#define GL_RENDERBUFFER_OES GL_RENDERBUFFER
#define GL_DEPTH_COMPONENT16_OES GL_DEPTH_COMPONENT16
#define GL_COLOR_ATTACHMENT0_OES GL_COLOR_ATTACHMENT0
#define GL_DEPTH_ATTACHMENT_OES GL_DEPTH_ATTACHMENT
#define GL_FRAMEBUFFER_COMPLETE_OES GL_FRAMEBUFFER_COMPLETE
#endif

// OES function mappings
#ifndef glGenFramebuffersOES
#define glGenFramebuffersOES glGenFramebuffers
#define glDeleteFramebuffersOES glDeleteFramebuffers
#define glBindFramebufferOES glBindFramebuffer
#define glFramebufferTexture2DOES glFramebufferTexture2D
#define glFramebufferRenderbufferOES glFramebufferRenderbuffer
#define glCheckFramebufferStatusOES glCheckFramebufferStatus
#define glGenRenderbuffersOES glGenRenderbuffers
#define glDeleteRenderbuffersOES glDeleteRenderbuffers
#define glBindRenderbufferOES glBindRenderbuffer
#define glRenderbufferStorageOES glRenderbufferStorage
#endif

#endif // _WIN32

// OpenGL ES 1.x fixed-function compatibility stubs (for GL 2.0 path)
// When gGLVersion == 1, these functions are called. On desktop, they may not exist.
#ifndef GL_RESCALE_NORMAL
#define GL_RESCALE_NORMAL 0x803A
#endif

#ifndef GL_LUMINANCE
#define GL_LUMINANCE 0x1909
#endif

#ifndef GL_LUMINANCE_ALPHA
#define GL_LUMINANCE_ALPHA 0x190A
#endif

#endif
