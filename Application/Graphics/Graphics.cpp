#include "Graphics.h"
#include "FonctionsMarmelade.h"
#include "Helpers.h"
#include "Shaders.h"
#include "Trace.h"
//#include <cmath>

int gGLVersion = 2;

static const int MAX_STACK_SIZE = 8;
static const int MAX_MATRIX_MODES=3;
static const int MAX_NUM_LIGHTS = 8;
static GLMat44 gMatrixStack[MAX_MATRIX_MODES][MAX_STACK_SIZE];
static int gStackIndex[MAX_MATRIX_MODES];
static int gModeIndex = 0;


static GLVec4 gLightPos[MAX_NUM_LIGHTS] = {0,0,0,0};
static GLVec4 gLightDiffuseColour[MAX_NUM_LIGHTS] = {0,0,0,0};
static GLVec4 gLightAmbientColour[MAX_NUM_LIGHTS] = {0,0,0,0};
static GLVec4 gLightSpecularColour[MAX_NUM_LIGHTS] = {0,0,0,0};


static EGLSurface g_EGLSurface = NULL;
static EGLDisplay g_EGLDisplay = NULL;
static EGLContext g_EGLContext = NULL;

#define MAX_CONFIG 128

//---------------------------------------------------------------------------------------------------------------------
int eglInit(bool createSurface)
{
  EGLint major;
  EGLint minor;
  EGLint numFound = 0;
  EGLConfig configList[MAX_CONFIG];

  for (int i = 0 ; i != MAX_MATRIX_MODES ; ++i)
  {
    esMatrixLoadIdentity(gMatrixStack[i][0]);
    gStackIndex[i] = 0;
  }
  for (int i = 0 ; i != MAX_NUM_LIGHTS ; ++i)
  {
    esSetVector4(gLightPos[i], 0.0f, 0.0f, 0.0, 0.0f);
    esSetVector4(gLightDiffuseColour[i], 0.0f, 0.0f, 0.0, 0.0f);
    esSetVector4(gLightAmbientColour[i], 0.0f, 0.0f, 0.0, 0.0f);
    esSetVector4(gLightSpecularColour[i], 0.0f, 0.0f, 0.0, 0.0f);
  }

  g_EGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (!g_EGLDisplay)
  {
    s3eDebugErrorShow(S3E_MESSAGE_CONTINUE, "eglGetDisplay failed");
    return 1;
  }

  EGLBoolean res = eglInitialize(g_EGLDisplay, &major, &minor);
  if (!res)
  {
    s3eDebugErrorShow(S3E_MESSAGE_CONTINUE, "eglInitialize failed");
    return 1;
  }

  eglGetConfigs(g_EGLDisplay, configList, MAX_CONFIG, &numFound);
  if (!numFound)
  {
    s3eDebugErrorShow(S3E_MESSAGE_CONTINUE, "eglGetConfigs failed to find any configs");
    return 1;
  }

  int config = -1;
  printf("found %d configs\n", numFound);

  enum {
    LOOP_RGB8_D24, 
    LOOP_RGB8_D16,
    LOOP_RGB5_D16,
    LOOP_MAX
  };

  int version = s3eGLGetInt(S3E_GL_VERSION)>>8;

  for (int loop = 0 ; loop != LOOP_MAX ; ++loop)
  {
    for (int i = 0; i < numFound; i++)
    {
      EGLint caveat = EGL_NONE;
      eglGetConfigAttrib(g_EGLDisplay, configList[i], EGL_CONFIG_CAVEAT, &caveat);
      if (caveat == EGL_SLOW_CONFIG)
        continue;

      EGLint renderable = 0;
      eglGetConfigAttrib(g_EGLDisplay, configList[i], EGL_RENDERABLE_TYPE, &renderable);
      if (
        (version == 1 && renderable & EGL_OPENGL_ES_BIT) || 
        (version == 2 && renderable & EGL_OPENGL_ES2_BIT)
        )
      {
        EGLint d = 0;
        EGLint r = 0;
        EGLint g = 0;
        EGLint b = 0;
        eglGetConfigAttrib(g_EGLDisplay, configList[i], EGL_DEPTH_SIZE, &d);
        eglGetConfigAttrib(g_EGLDisplay, configList[i], EGL_RED_SIZE, &r);
        eglGetConfigAttrib(g_EGLDisplay, configList[i], EGL_GREEN_SIZE, &g);
        eglGetConfigAttrib(g_EGLDisplay, configList[i], EGL_BLUE_SIZE, &b);

        if (loop == LOOP_RGB8_D24)
        {
          if (r >= 8 && g >= 8 && b >= 8 && d >= 24)
          {
            config = i;
            goto GotConfig;
          }
        }
        else if (loop == LOOP_RGB8_D16)
        {
          if (r >= 8 && g >= 8 && b >= 8 && d >= 16)
          {
            config = i;
            goto GotConfig;
          }
        }
        else if (loop == LOOP_RGB5_D16)
        {
          if (r >= 5 && g >= 5 && b >= 5 && d >= 24)
          {
            config = i;
            goto GotConfig;
          }
        }
      }
    }
  }
GotConfig:
  if (config == -1)
  {
    s3eDebugErrorShow(S3E_MESSAGE_CONTINUE, "No suitable EGL configs reported. Trying first available config");
    config = 0;
  }

  printf("requesting GL version: %d\n", version);
  printf("choosing config: %d\n", config);

  EGLint attribs[] = { EGL_CONTEXT_CLIENT_VERSION, version, EGL_NONE, };
  g_EGLContext = eglCreateContext(g_EGLDisplay, configList[config], NULL, attribs);
  if (!g_EGLContext)
  {
    s3eDebugErrorShow(S3E_MESSAGE_CONTINUE, "eglCreateContext failed");
    return 1;
  }

  //////////////////////////////////////////////////////////////////////// void* nativeWindow = s3eGLGetNativeWindow();
  EGLNativeWindowType nativeWindow = s3eGLGetNativeWindow();
  if (createSurface)
    g_EGLSurface = eglCreateWindowSurface(g_EGLDisplay, configList[config], nativeWindow, NULL);
  eglMakeCurrent(g_EGLDisplay, g_EGLSurface, g_EGLSurface, g_EGLContext);
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
void eglTerm(bool destroySurface)
{
  if (g_EGLDisplay)
  {
    eglMakeCurrent(g_EGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (destroySurface)
      eglDestroySurface(g_EGLDisplay, g_EGLSurface);
    eglDestroyContext(g_EGLDisplay, g_EGLContext);
  }
  if (destroySurface)
  {
    eglTerminate(g_EGLDisplay);
    g_EGLDisplay = 0;
  }
}


//---------------------------------------------------------------------------------------------------------------------
static GLfloat UITextureMatrix[16] = {
  1, 0, 0, 0,
  0, 1, 0, 0,
  0, 0, 1, 0,
  0, 0, 0, 1};

//---------------------------------------------------------------------------------------------------------------------
void RecoverFromIwGx(bool clear)
{
  if (gGLVersion == 1)
  {
    // Marmalade clobbers some of these
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);
    glMatrixMode( GL_TEXTURE );
    glGetFloatv(GL_TEXTURE_MATRIX, UITextureMatrix);
    glLoadIdentity();

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glShadeModel(GL_SMOOTH);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    GLfloat mat[] = {0,0,0,0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat);

    float col = 0.0f;
    glClearColor(col, col, col, 1.0);
    if (clear)
    {
      IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);
      IwGxFlush();
      IwGxSwapBuffers();
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
void PrepareForIwGx(bool clear)
{
  if (gGLVersion == 1)
  {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glShadeModel(GL_FLAT);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glMultMatrixf(UITextureMatrix);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    float col = 1.0f;
    glClearColor(col, col, col, 1.0f);
    if (clear)
    {
      IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);
      IwGxFlush();
      IwGxSwapBuffers();
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
void LookAt(
  GLMat44& viewMatrix,
  GLfloat eyex, GLfloat eyey, GLfloat eyez,
  GLfloat centerx, GLfloat centery, GLfloat centerz,
  GLfloat upx, GLfloat upy, GLfloat upz)
{
  GLfloat* m=&viewMatrix[0][0];
  GLfloat x[3], y[3], z[3];
  GLfloat mag;

  /* Make rotation matrix */

  /* Z vector */
  z[0] = eyex - centerx;
  z[1] = eyey - centery;
  z[2] = eyez - centerz;
  mag = sqrtf(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
  if (mag) {          /* mpichler, 19950515 */
    z[0] /= mag;
    z[1] /= mag;
    z[2] /= mag;
  }

  /* Y vector */
  y[0] = upx;
  y[1] = upy;
  y[2] = upz;

  /* X vector = Y cross Z */
  x[0] = y[1] * z[2] - y[2] * z[1];
  x[1] = -y[0] * z[2] + y[2] * z[0];
  x[2] = y[0] * z[1] - y[1] * z[0];

  /* Recompute Y = Z cross X */
  y[0] = z[1] * x[2] - z[2] * x[1];
  y[1] = -z[0] * x[2] + z[2] * x[0];
  y[2] = z[0] * x[1] - z[1] * x[0];

  /* mpichler, 19950515 */
  /* cross product gives area of parallelogram, which is < 1.0 for
  * non-perpendicular unit-length vectors; so normalize x, y here
  */

  mag = sqrtf(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
  if (mag) {
    x[0] /= mag;
    x[1] /= mag;
    x[2] /= mag;
  }

  mag = sqrtf(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
  if (mag) {
    y[0] /= mag;
    y[1] /= mag;
    y[2] /= mag;
  }

#define M(row,col)  m[col*4+row]
  M(0, 0) = x[0];
  M(0, 1) = x[1];
  M(0, 2) = x[2];
  M(0, 3) = 0.0;
  M(1, 0) = y[0];
  M(1, 1) = y[1];
  M(1, 2) = y[2];
  M(1, 3) = 0.0;
  M(2, 0) = z[0];
  M(2, 1) = z[1];
  M(2, 2) = z[2];
  M(2, 3) = 0.0;
  M(3, 0) = 0.0;
  M(3, 1) = 0.0;
  M(3, 2) = 0.0;
  M(3, 3) = 1.0;
#undef M

  esMatrixTranslate(viewMatrix, -eyex, -eyey, -eyez);

  if (gGLVersion == 1)
  {
    glMultMatrixf(m);
  }
  else
  {
    esMatrixMultiply(gMatrixStack[gModeIndex][gStackIndex[gModeIndex]], viewMatrix, gMatrixStack[gModeIndex][gStackIndex[gModeIndex]]); // right way round?
  }
}

//---------------------------------------------------------------------------------------------------------------------
void LoadTextureFromFile(Texture& texture, const char* filename, float colourOffset)
{
  CIwImage image;
  image.LoadFromFile(filename);

  GLint maxTextureSize;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
  GLint origWidth = image.GetWidth();
  GLint origHeight = image.GetHeight();
  CIwImage::Format format = image.GetFormat();

  TRACE_FILE_IF(1) TRACE("Image %s size %d, %d format %d", filename, origWidth, origHeight, format);

  if (colourOffset)
  {
    for (GLint ii = 0 ; ii != origWidth ; ++ii)
    {
      for (GLint jj = 0 ; jj != origHeight ; ++jj)
      {
        float col[4];
        col[0] = image.GetTexels()[(jj*origWidth + ii)*3 + 2] / 255.0f; 
        col[1] = image.GetTexels()[(jj*origWidth + ii)*3 + 1] / 255.0f; 
        col[2] = image.GetTexels()[(jj*origWidth + ii)*3 + 0] / 255.0f; 
        col[3] = 1.0f;
        OffsetColour(col, colourOffset);
        image.GetTexels()[(jj*origWidth + ii)*3 + 2] = (uint8_t) (col[0] * 255.0f);
        image.GetTexels()[(jj*origWidth + ii)*3 + 1] = (uint8_t) (col[1] * 255.0f);
        image.GetTexels()[(jj*origWidth + ii)*3 + 0] = (uint8_t) (col[2] * 255.0f);
      }
    }
  }

  if (origWidth <= maxTextureSize && origHeight <= maxTextureSize)
  {
    texture.CopyFromImage(&image);
    return;
  }

  GLint newWidth = origWidth;
  GLint newHeight = origHeight;

  if (origWidth >= origHeight)
  {
    newWidth = maxTextureSize;
    newHeight = (origHeight * maxTextureSize) / origWidth;
  }
  else
  {
    newHeight = maxTextureSize;
    newWidth = (origWidth * maxTextureSize) / origHeight;
  }
  TRACE_FILE_IF(1) TRACE("Resizing texture %s to %d, %d", filename, newWidth, newHeight);
  CIwImage newImage;
  newImage.SetFormat(image.GetFormat());
  newImage.SetWidth(newWidth);
  newImage.SetHeight(newHeight);
  image.ConvertToImage(&newImage);

  texture.CopyFromImage(&newImage);
}

//---------------------------------------------------------------------------------------------------------------------
static void flipVertical(unsigned char *data, int w, int h)
{
  int x, y, i1, i2;
  for (x=0;x<w;x++){
    for (y=0;y<h/2;y++){
      i1 = (y*w + x)*4; // this pixel
      i2 = ((h - y - 1)*w + x)*4; // its opposite (across x-axis)
      // swap pixels
      std::swap(data[i1], data[i2]);
      i1++; i2++;
      std::swap(data[i1], data[i2]);
      i1++; i2++;
      std::swap(data[i1], data[i2]);
      i1++; i2++;
      data[i1] = data[i2] = 255;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
void SaveScreenshot()
{
  static int count = 0;
  static char file[] = "PicaSim-00000.png";
  sprintf(file, "PicaSim-%05d.png", count++);

  int w = s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
  int h = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); glPixelStorei(GL_PACK_ALIGNMENT, 1);

  int dataSize = w * h * 4;
  unsigned char *framebuffer = 
    (unsigned char *) malloc(dataSize * sizeof(unsigned char));
  glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer);
  flipVertical(framebuffer, w, h);

  CIwImage image;
  image.SetFormat(CIwImage::ABGR_8888);
  image.SetWidth(w);
  image.SetHeight(h);
  image.SetBuffers(framebuffer, dataSize);
  image.SavePng(file);

  free(framebuffer);
}

//---------------------------------------------------------------------------------------------------------------------
void SaveScreenshotAsTexture(CIwTexture* texture)
{
  int w = s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
  int h = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); glPixelStorei(GL_PACK_ALIGNMENT, 1);

  int dataSize = w * h * 4;
  unsigned char *framebuffer = 
    (unsigned char *) malloc(dataSize * sizeof(unsigned char));
  glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer);
  flipVertical(framebuffer, w, h);

  CIwImage image;
  image.SetFormat(CIwImage::ABGR_8888);
  image.SetWidth(w);
  image.SetHeight(h);
  image.SetBuffers(framebuffer, dataSize);

  texture->SetImage(&image);
}

//---------------------------------------------------------------------------------------------------------------------
void glMatrixRotateFast(GLfloat sinAngle, GLfloat cosAngle, GLfloat x, GLfloat y, GLfloat z)
{
  GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
  GLfloat oneMinusCos;
  GLMat44 rotMat;

  xx = x * x;
  yy = y * y;
  zz = z * z;
  xy = x * y;
  yz = y * z;
  zx = z * x;
  xs = x * sinAngle;
  ys = y * sinAngle;
  zs = z * sinAngle;
  oneMinusCos = 1.0f - cosAngle;

  rotMat[0][0] = (oneMinusCos * xx) + cosAngle;
  rotMat[1][0] = (oneMinusCos * xy) - zs;
  rotMat[2][0] = (oneMinusCos * zx) + ys;
  rotMat[3][0] = 0.0F; 

  rotMat[0][1] = (oneMinusCos * xy) + zs;
  rotMat[1][1] = (oneMinusCos * yy) + cosAngle;
  rotMat[2][1] = (oneMinusCos * yz) - xs;
  rotMat[3][1] = 0.0F;

  rotMat[0][2] = (oneMinusCos * zx) - ys;
  rotMat[1][2] = (oneMinusCos * yz) + xs;
  rotMat[2][2] = (oneMinusCos * zz) + cosAngle;
  rotMat[3][2] = 0.0F; 

  rotMat[0][3] = 0.0F;
  rotMat[1][3] = 0.0F;
  rotMat[2][3] = 0.0F;
  rotMat[3][3] = 1.0F;

  glMultMatrixf(&rotMat[0][0]);
}

//---------------------------------------------------------------------------------------------------------------------
GLuint esLoadShader ( GLenum type, const char *shaderSrc )
{
  GLuint shader;
  GLint compiled;

  // Create the shader object
  shader = glCreateShader ( type );

  if ( shader == 0 )
    return 0;

  // Load the shader source
  glShaderSource ( shader, 1, &shaderSrc, NULL );

  // Compile the shader
  glCompileShader ( shader );

  // Check the compile status
  glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

  if ( !compiled ) 
  {
    GLint infoLen = 0;

    glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );

    if ( infoLen > 1 )
    {
      char* infoLog = (char*) malloc (sizeof(char) * infoLen );

      glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
      TRACE( "Error compiling shader:\n%s\n", infoLog );            

      free ( infoLog );
    }

    glDeleteShader ( shader );
    return 0;
  }

  return shader;

}

//---------------------------------------------------------------------------------------------------------------------
GLuint esLoadProgram( const char *vertShaderSrc, const char *fragShaderSrc )
{
  GLuint vertexShader;
  GLuint fragmentShader;
  GLuint programObject;
  GLint linked;

  // Load the vertex/fragment shaders
  vertexShader = esLoadShader ( GL_VERTEX_SHADER, vertShaderSrc );
  if ( vertexShader == 0 )
    return 0;

  fragmentShader = esLoadShader ( GL_FRAGMENT_SHADER, fragShaderSrc );
  if ( fragmentShader == 0 )
  {
    glDeleteShader( vertexShader );
    return 0;
  }

  // Create the program object
  programObject = glCreateProgram ( );

  if ( programObject == 0 )
    return 0;

  glAttachShader ( programObject, vertexShader );
  glAttachShader ( programObject, fragmentShader );

  // Link the program
  glLinkProgram ( programObject );

  // Check the link status
  glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

  if ( !linked ) 
  {
    GLint infoLen = 0;

    glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );

    if ( infoLen > 1 )
    {
      char* infoLog = (char*) malloc (sizeof(char) * infoLen );

      glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
      TRACE( "Error linking program:\n%s\n", infoLog );            

      free ( infoLog );
    }

    glDeleteProgram ( programObject );
    return 0;
  }

  // Free up no longer needed shader resources
  glDeleteShader ( vertexShader );
  glDeleteShader ( fragmentShader );

  return programObject;
}

//---------------------------------------------------------------------------------------------------------------------
void esMatrixScale(GLMat44& result, GLfloat sx, GLfloat sy, GLfloat sz)
{
  result[0][0] *= sx;
  result[0][1] *= sx;
  result[0][2] *= sx;
  result[0][3] *= sx;

  result[1][0] *= sy;
  result[1][1] *= sy;
  result[1][2] *= sy;
  result[1][3] *= sy;

  result[2][0] *= sz;
  result[2][1] *= sz;
  result[2][2] *= sz;
  result[2][3] *= sz;
}

//---------------------------------------------------------------------------------------------------------------------
void esMatrixTranslate(GLMat44& result, GLfloat tx, GLfloat ty, GLfloat tz)
{
  result[3][0] += (result[0][0] * tx + result[1][0] * ty + result[2][0] * tz);
  result[3][1] += (result[0][1] * tx + result[1][1] * ty + result[2][1] * tz);
  result[3][2] += (result[0][2] * tx + result[1][2] * ty + result[2][2] * tz);
  result[3][3] += (result[0][3] * tx + result[1][3] * ty + result[2][3] * tz);
}

//---------------------------------------------------------------------------------------------------------------------
void esMatrixRotate(GLMat44& result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  GLfloat sinAngle, cosAngle;
  GLfloat mag = sqrtf(x * x + y * y + z * z);

  sinAngle = FastSin( angle * PI / 180.0f );
  cosAngle = FastCos( angle * PI / 180.0f );
  if ( mag > 0.0f )
  {
    GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
    GLfloat oneMinusCos;
    GLMat44 rotMat;

    x /= mag;
    y /= mag;
    z /= mag;

    xx = x * x;
    yy = y * y;
    zz = z * z;
    xy = x * y;
    yz = y * z;
    zx = z * x;
    xs = x * sinAngle;
    ys = y * sinAngle;
    zs = z * sinAngle;
    oneMinusCos = 1.0f - cosAngle;

    rotMat[0][0] = (oneMinusCos * xx) + cosAngle;
    rotMat[1][0] = (oneMinusCos * xy) - zs;
    rotMat[2][0] = (oneMinusCos * zx) + ys;
    rotMat[3][0] = 0.0F; 

    rotMat[0][1] = (oneMinusCos * xy) + zs;
    rotMat[1][1] = (oneMinusCos * yy) + cosAngle;
    rotMat[2][1] = (oneMinusCos * yz) - xs;
    rotMat[3][1] = 0.0F;

    rotMat[0][2] = (oneMinusCos * zx) - ys;
    rotMat[1][2] = (oneMinusCos * yz) + xs;
    rotMat[2][2] = (oneMinusCos * zz) + cosAngle;
    rotMat[3][2] = 0.0F; 

    rotMat[0][3] = 0.0F;
    rotMat[1][3] = 0.0F;
    rotMat[2][3] = 0.0F;
    rotMat[3][3] = 1.0F;

    esMatrixMultiply( result, rotMat, result );
  }
}

//---------------------------------------------------------------------------------------------------------------------
void esMatrixRotateFast(GLMat44& result, GLfloat sinAngle, GLfloat cosAngle, GLfloat x, GLfloat y, GLfloat z)
{
  GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
  GLfloat oneMinusCos;
  GLMat44 rotMat;

  xx = x * x;
  yy = y * y;
  zz = z * z;
  xy = x * y;
  yz = y * z;
  zx = z * x;
  xs = x * sinAngle;
  ys = y * sinAngle;
  zs = z * sinAngle;
  oneMinusCos = 1.0f - cosAngle;

  rotMat[0][0] = (oneMinusCos * xx) + cosAngle;
  rotMat[1][0] = (oneMinusCos * xy) - zs;
  rotMat[2][0] = (oneMinusCos * zx) + ys;
  rotMat[3][0] = 0.0F; 

  rotMat[0][1] = (oneMinusCos * xy) + zs;
  rotMat[1][1] = (oneMinusCos * yy) + cosAngle;
  rotMat[2][1] = (oneMinusCos * yz) - xs;
  rotMat[3][1] = 0.0F;

  rotMat[0][2] = (oneMinusCos * zx) - ys;
  rotMat[1][2] = (oneMinusCos * yz) + xs;
  rotMat[2][2] = (oneMinusCos * zz) + cosAngle;
  rotMat[3][2] = 0.0F; 

  rotMat[0][3] = 0.0F;
  rotMat[1][3] = 0.0F;
  rotMat[2][3] = 0.0F;
  rotMat[3][3] = 1.0F;

  esMatrixMultiply( result, rotMat, result );
}

//---------------------------------------------------------------------------------------------------------------------
void esMatrixFrustum(GLMat44& result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
  float       deltaX = right - left;
  float       deltaY = top - bottom;
  float       deltaZ = farZ - nearZ;
  GLMat44    frust;

  if ( (nearZ <= 0.0f) || (farZ <= 0.0f) ||
    (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f) )
    return;

  frust[0][0] = 2.0f * nearZ / deltaX;
  frust[0][1] = frust[0][2] = frust[0][3] = 0.0f;

  frust[1][1] = 2.0f * nearZ / deltaY;
  frust[1][0] = frust[1][2] = frust[1][3] = 0.0f;

  frust[2][0] = (right + left) / deltaX;
  frust[2][1] = (top + bottom) / deltaY;
  frust[2][2] = -(nearZ + farZ) / deltaZ;
  frust[2][3] = -1.0f;

  frust[3][2] = -2.0f * nearZ * farZ / deltaZ;
  frust[3][0] = frust[3][1] = frust[3][3] = 0.0f;

  esMatrixMultiply(result, frust, result);
}


//---------------------------------------------------------------------------------------------------------------------
void esMatrixPerspective(GLMat44& result, float fovy, float aspect, float nearZ, float farZ)
{
  GLfloat frustumW, frustumH;

  frustumH = tanf( fovy / 360.0f * PI ) * nearZ;
  frustumW = frustumH * aspect;

  esMatrixFrustum( result, -frustumW, frustumW, -frustumH, frustumH, nearZ, farZ );
}

//---------------------------------------------------------------------------------------------------------------------
void esMatrixOrtho(GLMat44& result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
  float       deltaX = right - left;
  float       deltaY = top - bottom;
  float       deltaZ = farZ - nearZ;
  GLMat44    ortho;

  if ( (deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f) )
    return;

  esMatrixLoadIdentity(ortho);
  ortho[0][0] = 2.0f / deltaX;
  ortho[3][0] = -(right + left) / deltaX;
  ortho[1][1] = 2.0f / deltaY;
  ortho[3][1] = -(top + bottom) / deltaY;
  ortho[2][2] = -2.0f / deltaZ;
  ortho[3][2] = -(nearZ + farZ) / deltaZ;

  esMatrixMultiply(result, ortho, result);
}


//---------------------------------------------------------------------------------------------------------------------
void esMatrixMultiply(GLMat44& result, const GLMat44& srcA, const GLMat44& srcB)
{
  GLMat44    tmp;
  int         i;

  for (i=0; i<4; i++)
  {
    tmp[i][0] =  (srcA[i][0] * srcB[0][0]) +
      (srcA[i][1] * srcB[1][0]) +
      (srcA[i][2] * srcB[2][0]) +
      (srcA[i][3] * srcB[3][0]) ;

    tmp[i][1] =  (srcA[i][0] * srcB[0][1]) + 
      (srcA[i][1] * srcB[1][1]) +
      (srcA[i][2] * srcB[2][1]) +
      (srcA[i][3] * srcB[3][1]) ;

    tmp[i][2] =  (srcA[i][0] * srcB[0][2]) + 
      (srcA[i][1] * srcB[1][2]) +
      (srcA[i][2] * srcB[2][2]) +
      (srcA[i][3] * srcB[3][2]) ;

    tmp[i][3] =  (srcA[i][0] * srcB[0][3]) + 
      (srcA[i][1] * srcB[1][3]) +
      (srcA[i][2] * srcB[2][3]) +
      (srcA[i][3] * srcB[3][3]) ;
  }
  memcpy(result, &tmp, sizeof(GLMat44));
}


//---------------------------------------------------------------------------------------------------------------------
void esMatrixLoadIdentity(GLMat44& result)
{
  memset(result, 0x0, sizeof(GLMat44));
  result[0][0] = 1.0f;
  result[1][1] = 1.0f;
  result[2][2] = 1.0f;
  result[3][3] = 1.0f;
}

//---------------------------------------------------------------------------------------------------------------------
void esMatrixTranspose(GLMat44& result, const GLMat44& src)
{
  GLMat44    tmp;
  for (int i = 0 ; i != 4 ; ++i)
    for (int j = 0 ; j != 4 ; ++j)
      tmp[i][j] = src[j][i];
  memcpy(result, &tmp, sizeof(GLMat44));
}


//---------------------------------------------------------------------------------------------------------------------
void esMatrixCopy(GLMat44& result, const GLMat44& src)
{
  memcpy(result, src, sizeof(GLMat44));
}

//---------------------------------------------------------------------------------------------------------------------
void esVector4Copy(GLVec4& result, const GLVec4& src)
{
  memcpy(result, src, sizeof(GLVec4));
}

void esSetVector4(GLVec4& result, float x, float y, float z, float w)
{
  result[0] = x;
  result[1] = y;
  result[2] = z;
  result[3] = w;
}

//---------------------------------------------------------------------------------------------------------------------
void esMatrixCopyRotation(GLMat33& result, const GLMat44& src)
{
  for (int i = 0 ; i != 3 ; ++i)
    for (int j = 0 ; j != 3 ; ++j)
      result[i][j] = src[i][j];
}


//---------------------------------------------------------------------------------------------------------------------
void esMatrixMode(GLenum mode)
{
  if (gGLVersion == 1)
    glMatrixMode(mode);
  else
    gModeIndex = mode - GL_MODELVIEW;
}

//---------------------------------------------------------------------------------------------------------------------
void esMatrixTransform(GLVec4& result, const GLMat44& m, const GLVec4& v)
{
  for (int j = 0 ; j != 4 ; ++j)
    result[j] = m[0][j] * v[0] + m[1][j] * v[1] + m[2][j] * v[2] + m[3][j] * v[3];
}

//---------------------------------------------------------------------------------------------------------------------
void esMatrixTransform3x3(GLVec4& result, const GLMat44& m, const GLVec4& v)
{
  for (int j = 0 ; j != 3 ; ++j)
    result[j] = m[0][j] * v[0] + m[1][j] * v[1] + m[2][j] * v[2];
  result[3] = v[3];
}

//---------------------------------------------------------------------------------------------------------------------
void esPushMatrix()
{
  if (gGLVersion == 1)
  {
    glPushMatrix();
  }
  else
  {
    esMatrixCopy(gMatrixStack[gModeIndex][gStackIndex[gModeIndex]+1], gMatrixStack[gModeIndex][gStackIndex[gModeIndex]]);
    ++gStackIndex[gModeIndex];
  }
}

void esLoadIdentity()
{
  if (gGLVersion == 1)
    glLoadIdentity();
  else
    esMatrixLoadIdentity(gMatrixStack[gModeIndex][gStackIndex[gModeIndex]]);
}

void esPopMatrix()
{
  if (gGLVersion == 1)
    glPopMatrix();
  else
    --gStackIndex[gModeIndex];
}

void esTranslatef(float x, float y, float z)
{
  if (gGLVersion == 1)
    glTranslatef(x, y, z);
  else
    esMatrixTranslate(gMatrixStack[gModeIndex][gStackIndex[gModeIndex]], x, y, z);
}

void esRotatef(float angle, float x, float y, float z)
{
  if (gGLVersion == 1)
    glRotatef(angle, x, y, z);
  else
    esMatrixRotate(gMatrixStack[gModeIndex][gStackIndex[gModeIndex]], angle, x, y, z);
}

void esRotateFast(float sinAngle, float cosAngle, float x, float y, float z)
{
  if (gGLVersion == 1)
  {
    glMatrixRotateFast(sinAngle, cosAngle, x, y, z);
  }
  else
  {
    esMatrixRotateFast(gMatrixStack[gModeIndex][gStackIndex[gModeIndex]], sinAngle, cosAngle, x, y, z);
  }
}

void esScalef(float x, float y, float z)
{
  if (gGLVersion == 1)
    glScalef(x, y, z);
  else
    esMatrixScale(gMatrixStack[gModeIndex][gStackIndex[gModeIndex]], x, y, z);
}

void esMultMatrixf(const float* m)
{
  if (gGLVersion == 1)
    glMultMatrixf(m);
  else
    esMatrixMultiply(gMatrixStack[gModeIndex][gStackIndex[gModeIndex]], 
    *((GLMat44*) m),
    gMatrixStack[gModeIndex][gStackIndex[gModeIndex]]);
}

void esGetMatrix(GLMat44& m, GLenum mode)
{
  if (gGLVersion == 1)
  {
    glGetFloatv(GL_MODELVIEW_MATRIX + mode - GL_MODELVIEW, &m[0][0]);
  }
  else
  {
    esMatrixCopy(m, gMatrixStack[mode - GL_MODELVIEW][gStackIndex[mode - GL_MODELVIEW]]);
  }
}

void esFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  if (gGLVersion == 1)
    glFrustumf(left, right, bottom, top, zNear, zFar);
  else 
    esMatrixFrustum(gMatrixStack[gModeIndex][gStackIndex[gModeIndex]], left, right, bottom, top, zNear, zFar);
}

void esOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  if (gGLVersion == 1)
    glOrthof(left, right, bottom, top, zNear, zFar);
  else
    esMatrixOrtho(gMatrixStack[gModeIndex][gStackIndex[gModeIndex]], left, right, bottom, top, zNear, zFar);
}

void esSetTextureMatrix(int textureMatrixLoc)
{
  if (gGLVersion > 1 && textureMatrixLoc != -1)
  {
    GLMat44 tMatrix; esGetMatrix(tMatrix, GL_TEXTURE);
    glUniformMatrix4fv(textureMatrixLoc, 1, GL_FALSE, (GLfloat*) &tMatrix);
  }
}

void esSetModelViewProjectionMatrix(int mvpMatrixLoc)
{
  if (gGLVersion > 1)
  {
    if (mvpMatrixLoc == -1)
      return;
    GLMat44 mvpMatrix;
    GLMat44 mvMatrix; esGetMatrix(mvMatrix, GL_MODELVIEW);
    GLMat44 pMatrix; esGetMatrix(pMatrix, GL_PROJECTION);
    esMatrixMultiply(mvpMatrix, mvMatrix, pMatrix);
    glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, (GLfloat*) &mvpMatrix[0][0]);
  }
}

void esSetModelViewProjectionAndNormalMatrix(int mvpMatrixLoc, int normalMatrixLoc)
{
  if (gGLVersion > 1)
  {
    esSetModelViewProjectionMatrix(mvpMatrixLoc);
    if (normalMatrixLoc == -1)
      return;

    GLMat33 normalMatrix;
    GLMat44 mvMatrix; esGetMatrix(mvMatrix, GL_MODELVIEW);
    esMatrixCopyRotation(normalMatrix, mvMatrix);
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, (GLfloat*) &normalMatrix);
  }
}

void esSetLighting(const LightShaderInfo lightShaderInfo[5])
{
  if (gGLVersion > 1)
  {
    for (int i = 0 ; i != 5 ; ++i)
    {
      if (lightShaderInfo[i].u_lightDir != -1)
      {
        GLVec4 lightDir;
        esVector4Copy(lightDir, gLightPos[i]);
        float d = sqrtf(Square(lightDir[0]) + Square(lightDir[1]) + Square(lightDir[2]));
        if (d > 0.0f)
        {
          lightDir[0] /= d;
          lightDir[1] /= d;
          lightDir[2] /= d;
        }
        glUniform3fv(lightShaderInfo[i].u_lightDir, 1, lightDir);
      }
      if (lightShaderInfo[i].u_lightAmbientColour != -1)
        glUniform4fv(lightShaderInfo[i].u_lightAmbientColour, 1, gLightAmbientColour[i]);
      if (lightShaderInfo[i].u_lightDiffuseColour != -1)
        glUniform4fv(lightShaderInfo[i].u_lightDiffuseColour, 1, gLightDiffuseColour[i]);
      if (lightShaderInfo[i].u_lightSpecularColour != -1)
        glUniform4fv(lightShaderInfo[i].u_lightSpecularColour, 1, gLightSpecularColour[i]);
    }
  }
}


void esSetLightPos(GLenum light, const GLVec4& lightPos)
{
  if (gGLVersion == 1)
  {
    glLightfv(light, GL_POSITION, lightPos);
  }
  else
  {
    GLMat44 mvMatrix; esGetMatrix(mvMatrix, GL_MODELVIEW);
    esMatrixTransform(gLightPos[light - GL_LIGHT0], mvMatrix, lightPos);
  }
}

void esSetLightDiffuseColour(GLenum light, const GLVec4& diffuseColour)
{
  if (gGLVersion == 1)
    glLightfv(light, GL_DIFFUSE, diffuseColour);
  else
    esVector4Copy(gLightDiffuseColour[light - GL_LIGHT0], diffuseColour);
}

void esSetLightAmbientColour(GLenum light, const GLVec4& ambientColour)
{
  if (gGLVersion == 1)
    glLightfv(light, GL_AMBIENT, ambientColour);
  else
    esVector4Copy(gLightAmbientColour[light - GL_LIGHT0], ambientColour);
}

void esSetLightSpecularColour(GLenum light, const GLVec4& specularColour)
{
  if (gGLVersion == 1)
    glLightfv(light, GL_SPECULAR, specularColour);
  else
    esVector4Copy(gLightSpecularColour[light - GL_LIGHT0], specularColour);
}

//----------------------------------------------------------------------------------------------------------------------
HSV RGB2HSV(RGB in)
{
  HSV        out;
  float      min, max, delta;

  min = Minimum(Minimum(in.r, in.g), in.b);
  max = Maximum(Maximum(in.r, in.g), in.b);

  out.v = max;                                // v
  delta = max - min;
  if (min == max)
  {
    out.s = 0.0f;
    out.h = 0.0f;                            // its now undefined
    return out;
  }
  if( max > 0.0f ) 
  {
    out.s = (delta / max);                  // s
  } 
  else 
  {
    // r = g = b = 0                        // s = 0, v is undefined
    out.s = 0.0f;
    out.h = 0.0f;                            // its now undefined
    return out;
  }
  if( in.r >= max )                           // > is bogus, just keeps compilor happy
    out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
  else
    if( in.g >= max )
      out.h = 2.0f + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
      out.h = 4.0f + ( in.r - in.g ) / delta;  // between magenta & cyan

  out.h *= 60.0f;                              // degrees

  if( out.h < 0.0f )
    out.h += 360.0f;

  return out;
}

//----------------------------------------------------------------------------------------------------------------------
RGB HSV2RGB(HSV in)
{
  float      hh, p, q, t, ff;
  long        i;
  RGB         out;

  if(in.s == 0.0f) 
  {
    // error - should never happen
    out.r = in.v;
    out.g = in.v;
    out.b = in.v;
    return out;
  }
  hh = in.h;
  if(hh >= 360.0f) hh = 0.0f;
  hh /= 60.0f;
  i = (long)hh;
  ff = hh - i;
  p = in.v * (1.0f - in.s);
  q = in.v * (1.0f - (in.s * ff));
  t = in.v * (1.0f - (in.s * (1.0f - ff)));

  switch(i) {
  case 0:
    out.r = in.v;
    out.g = t;
    out.b = p;
    break;
  case 1:
    out.r = q;
    out.g = in.v;
    out.b = p;
    break;
  case 2:
    out.r = p;
    out.g = in.v;
    out.b = t;
    break;

  case 3:
    out.r = p;
    out.g = q;
    out.b = in.v;
    break;
  case 4:
    out.r = t;
    out.g = p;
    out.b = in.v;
    break;
  case 5:
  default:
    out.r = in.v;
    out.g = p;
    out.b = q;
    break;
  }
  return out;     
}

void OffsetColour(float col[4], float offset)
{
  RGB rgb(col[0], col[1], col[2]);
  HSV hsv = RGB2HSV(rgb);

  hsv.h += offset * 360.0f;
  while (hsv.h > 360.0f)
    hsv.h -= 360.0f;

  rgb = HSV2RGB(hsv);
  col[0] = rgb.r;
  col[1] = rgb.g;
  col[2] = rgb.b;
}

Vector3 RGB2HSV(const Vector3& rgb)
{
  RGB in(rgb.x, rgb.y, rgb.z);
  HSV out = RGB2HSV(in);
  return Vector3(out.h/360.0f, out.s, out.v);
}

Vector3 HSV2RGB(const Vector3& hsv)
{
  HSV in(hsv.x * 360.0f, hsv.y, hsv.z);
  RGB out = HSV2RGB(in);
  return Vector3(out.r, out.g, out.b);
}

CIwColour ConvertToColour(const Vector3& colour)
{
  CIwColour col;
  col.Set(
    ClampToRange((int) (colour.x * 256), 0, 255),
    ClampToRange((int) (colour.y * 256), 0, 255),
    ClampToRange((int) (colour.z * 256), 0, 255)
    );
  return col;
}
