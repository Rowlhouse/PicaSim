#include "FontRenderer.h"
#include "S3ECompat.h"
#include "../Framework/Graphics.h"
#include "../Framework/ShaderManager.h"
#include "../Framework/Shaders.h"
#include "../Framework/Helpers.h"
#include "../Framework/Trace.h"

// Windows defines DrawText as a macro - we need to undef it
#ifdef DrawText
#undef DrawText
#endif

#include <cstring>
#include <cstdio>
#include <cstdlib>

// stb_truetype for TTF rendering
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

// stb_image for loading PNG fallback
#include <stb_image.h>

// OpenGL headers
#if !defined(PICASIM_MACOS)
  #include <glad/glad.h>
#endif

// For esSetModelViewProjectionMatrix
void esSetModelViewProjectionMatrix(int location);

//======================================================================================================================
FontRenderer& FontRenderer::GetInstance()
{
    static FontRenderer instance;
    return instance;
}

//======================================================================================================================
FontRenderer::FontRenderer()
    : mTextureID(0)
    , mTextureWidth(0)
    , mTextureHeight(0)
    , mCharWidth(16)
    , mCharHeight(24)
    , mCharsPerRow(16)
    , mFirstChar(32)
    , mRectX(0), mRectY(0), mRectW(0), mRectH(0)
    , mAlignH(FONT_ALIGN_LEFT)
    , mAlignV(FONT_ALIGN_TOP)
    , mColour(0xFFFFFFFF)
    , mInitialized(false)
{
    // Initialize all character advances to default width
    for (int i = 0; i < 96; i++)
        mCharAdvance[i] = mCharWidth;
}

//======================================================================================================================
FontRenderer::~FontRenderer()
{
    Shutdown();
}

//======================================================================================================================
bool FontRenderer::Init()
{
    if (mInitialized)
        return true;

    TRACE_FILE_IF(1) TRACE("FontRenderer::Init");

    // Try to load font texture from PNG first
    if (LoadFontTexture("SystemData/Fonts/font.png"))
    {
        TRACE_FILE_IF(1) TRACE("Loaded font texture from PNG");
        mInitialized = true;
        return true;
    }

    // Try to generate from TTF
    if (GenerateFontTexture("fonts/FontRegular.ttf", 20))
    {
        TRACE_FILE_IF(1) TRACE("Generated font texture from TTF");
        mInitialized = true;
        return true;
    }

    // Fallback to embedded simple font
    if (CreateFallbackFont())
    {
        TRACE_FILE_IF(1) TRACE("Created fallback font");
        mInitialized = true;
        return true;
    }

    TRACE_FILE_IF(1) TRACE("FontRenderer::Init FAILED");
    return false;
}

//======================================================================================================================
void FontRenderer::Shutdown()
{
    if (mTextureID != 0)
    {
        glDeleteTextures(1, &mTextureID);
        mTextureID = 0;
    }
    mInitialized = false;
}

//======================================================================================================================
bool FontRenderer::LoadFontTexture(const char* pngPath)
{
    int width, height, channels;
    unsigned char* data = stbi_load(pngPath, &width, &height, &channels, 4);
    if (!data)
    {
        TRACE_FILE_IF(1) TRACE("Failed to load font texture: %s", pngPath);
        return false;
    }

    // Assume 16x16 grid of characters, each character cell is width/16 x height/16
    mTextureWidth = width;
    mTextureHeight = height;
    mCharsPerRow = 16;
    mCharWidth = width / 16;
    mCharHeight = height / 6; // 6 rows for ASCII 32-127 (96 characters)
    mFirstChar = 32;

    // PNG fonts are assumed monospace - all characters use the same advance
    for (int i = 0; i < 96; i++)
        mCharAdvance[i] = mCharWidth;

    // Create OpenGL texture
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
    return true;
}

//======================================================================================================================
bool FontRenderer::GenerateFontTexture(const char* ttfPath, int fontSize)
{
    // Load TTF file
    FILE* fp = fopen(ttfPath, "rb");
    if (!fp)
    {
        TRACE_FILE_IF(1) TRACE("Failed to open TTF file: %s", ttfPath);
        return false;
    }

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char* ttfBuffer = (unsigned char*)malloc(fileSize);
    if (!ttfBuffer)
    {
        fclose(fp);
        return false;
    }

    fread(ttfBuffer, 1, fileSize, fp);
    fclose(fp);

    // Initialize stb_truetype
    stbtt_fontinfo font;
    if (!stbtt_InitFont(&font, ttfBuffer, stbtt_GetFontOffsetForIndex(ttfBuffer, 0)))
    {
        free(ttfBuffer);
        TRACE_FILE_IF(1) TRACE("Failed to init font from TTF");
        return false;
    }

    // Calculate scale for desired font size
    float scale = stbtt_ScaleForPixelHeight(&font, (float)fontSize);

    // Get font metrics
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    int scaledAscent = (int)(ascent * scale);
    int scaledDescent = (int)(descent * scale);

    // Texture size: 16 characters per row, 6 rows for ASCII 32-127
    mCharsPerRow = 16;
    int numRows = 6;
    mCharWidth = fontSize;  // Approximate width
    mCharHeight = scaledAscent - scaledDescent + 2;
    mTextureWidth = mCharsPerRow * mCharWidth;
    mTextureHeight = numRows * mCharHeight;
    mFirstChar = 32;

    // Allocate texture buffer (RGBA)
    unsigned char* texData = (unsigned char*)calloc(mTextureWidth * mTextureHeight * 4, 1);

    // Render each character
    for (int c = 32; c < 128; c++)
    {
        int charIndex = c - 32;
        int cellX = (charIndex % mCharsPerRow) * mCharWidth;
        int cellY = (charIndex / mCharsPerRow) * mCharHeight;

        // Get character advance width (horizontal metrics)
        int advanceWidth, leftSideBearing;
        stbtt_GetCodepointHMetrics(&font, c, &advanceWidth, &leftSideBearing);
        mCharAdvance[charIndex] = (int)(advanceWidth * scale);

        // Get character bitmap
        int w, h, xoff, yoff;
        unsigned char* charBitmap = stbtt_GetCodepointBitmap(&font, 0, scale, c, &w, &h, &xoff, &yoff);

        if (charBitmap)
        {
            // Copy bitmap to texture (positioned within cell)
            int startX = cellX + xoff;
            int startY = cellY + scaledAscent + yoff;

            for (int y = 0; y < h; y++)
            {
                for (int x = 0; x < w; x++)
                {
                    int texX = startX + x;
                    int texY = startY + y;
                    if (texX >= 0 && texX < mTextureWidth && texY >= 0 && texY < mTextureHeight)
                    {
                        int idx = (texY * mTextureWidth + texX) * 4;
                        unsigned char alpha = charBitmap[y * w + x];
                        texData[idx + 0] = 255;  // R
                        texData[idx + 1] = 255;  // G
                        texData[idx + 2] = 255;  // B
                        texData[idx + 3] = alpha; // A
                    }
                }
            }
            stbtt_FreeBitmap(charBitmap, nullptr);
        }
    }

    free(ttfBuffer);

    // Create OpenGL texture
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTextureWidth, mTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glBindTexture(GL_TEXTURE_2D, 0);

    free(texData);
    return true;
}

//======================================================================================================================
bool FontRenderer::CreateFallbackFont()
{
    // Create a very simple 8x8 bitmap font
    // This is a minimal fallback - just draws basic ASCII
    mCharWidth = 8;
    mCharHeight = 8;
    mCharsPerRow = 16;
    mTextureWidth = 128;  // 16 chars * 8 pixels
    mTextureHeight = 48;  // 6 rows * 8 pixels
    mFirstChar = 32;

    // Set all characters to use the same 8-pixel advance
    for (int i = 0; i < 96; i++)
        mCharAdvance[i] = 8;

    // Simple 8x8 font data (just basic characters)
    // Each character is 8 bytes (8 rows of 8 bits)
    static const unsigned char fontData[96][8] = {
        // Space through ~
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 32 space
        {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00}, // 33 !
        {0x36,0x36,0x00,0x00,0x00,0x00,0x00,0x00}, // 34 "
        {0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00}, // 35 #
        {0x0C,0x3E,0x03,0x1E,0x30,0x1F,0x0C,0x00}, // 36 $
        {0x00,0x63,0x33,0x18,0x0C,0x66,0x63,0x00}, // 37 %
        {0x1C,0x36,0x1C,0x6E,0x3B,0x33,0x6E,0x00}, // 38 &
        {0x06,0x06,0x03,0x00,0x00,0x00,0x00,0x00}, // 39 '
        {0x18,0x0C,0x06,0x06,0x06,0x0C,0x18,0x00}, // 40 (
        {0x06,0x0C,0x18,0x18,0x18,0x0C,0x06,0x00}, // 41 )
        {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, // 42 *
        {0x00,0x0C,0x0C,0x3F,0x0C,0x0C,0x00,0x00}, // 43 +
        {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x06}, // 44 ,
        {0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00}, // 45 -
        {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x00}, // 46 .
        {0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x00}, // 47 /
        {0x3E,0x63,0x73,0x7B,0x6F,0x67,0x3E,0x00}, // 48 0
        {0x0C,0x0E,0x0C,0x0C,0x0C,0x0C,0x3F,0x00}, // 49 1
        {0x1E,0x33,0x30,0x1C,0x06,0x33,0x3F,0x00}, // 50 2
        {0x1E,0x33,0x30,0x1C,0x30,0x33,0x1E,0x00}, // 51 3
        {0x38,0x3C,0x36,0x33,0x7F,0x30,0x78,0x00}, // 52 4
        {0x3F,0x03,0x1F,0x30,0x30,0x33,0x1E,0x00}, // 53 5
        {0x1C,0x06,0x03,0x1F,0x33,0x33,0x1E,0x00}, // 54 6
        {0x3F,0x33,0x30,0x18,0x0C,0x0C,0x0C,0x00}, // 55 7
        {0x1E,0x33,0x33,0x1E,0x33,0x33,0x1E,0x00}, // 56 8
        {0x1E,0x33,0x33,0x3E,0x30,0x18,0x0E,0x00}, // 57 9
        {0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x00}, // 58 :
        {0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x06}, // 59 ;
        {0x18,0x0C,0x06,0x03,0x06,0x0C,0x18,0x00}, // 60 <
        {0x00,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00}, // 61 =
        {0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00}, // 62 >
        {0x1E,0x33,0x30,0x18,0x0C,0x00,0x0C,0x00}, // 63 ?
        {0x3E,0x63,0x7B,0x7B,0x7B,0x03,0x1E,0x00}, // 64 @
        {0x0C,0x1E,0x33,0x33,0x3F,0x33,0x33,0x00}, // 65 A
        {0x3F,0x66,0x66,0x3E,0x66,0x66,0x3F,0x00}, // 66 B
        {0x3C,0x66,0x03,0x03,0x03,0x66,0x3C,0x00}, // 67 C
        {0x1F,0x36,0x66,0x66,0x66,0x36,0x1F,0x00}, // 68 D
        {0x7F,0x46,0x16,0x1E,0x16,0x46,0x7F,0x00}, // 69 E
        {0x7F,0x46,0x16,0x1E,0x16,0x06,0x0F,0x00}, // 70 F
        {0x3C,0x66,0x03,0x03,0x73,0x66,0x7C,0x00}, // 71 G
        {0x33,0x33,0x33,0x3F,0x33,0x33,0x33,0x00}, // 72 H
        {0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, // 73 I
        {0x78,0x30,0x30,0x30,0x33,0x33,0x1E,0x00}, // 74 J
        {0x67,0x66,0x36,0x1E,0x36,0x66,0x67,0x00}, // 75 K
        {0x0F,0x06,0x06,0x06,0x46,0x66,0x7F,0x00}, // 76 L
        {0x63,0x77,0x7F,0x7F,0x6B,0x63,0x63,0x00}, // 77 M
        {0x63,0x67,0x6F,0x7B,0x73,0x63,0x63,0x00}, // 78 N
        {0x1C,0x36,0x63,0x63,0x63,0x36,0x1C,0x00}, // 79 O
        {0x3F,0x66,0x66,0x3E,0x06,0x06,0x0F,0x00}, // 80 P
        {0x1E,0x33,0x33,0x33,0x3B,0x1E,0x38,0x00}, // 81 Q
        {0x3F,0x66,0x66,0x3E,0x36,0x66,0x67,0x00}, // 82 R
        {0x1E,0x33,0x07,0x0E,0x38,0x33,0x1E,0x00}, // 83 S
        {0x3F,0x2D,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, // 84 T
        {0x33,0x33,0x33,0x33,0x33,0x33,0x3F,0x00}, // 85 U
        {0x33,0x33,0x33,0x33,0x33,0x1E,0x0C,0x00}, // 86 V
        {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00}, // 87 W
        {0x63,0x63,0x36,0x1C,0x1C,0x36,0x63,0x00}, // 88 X
        {0x33,0x33,0x33,0x1E,0x0C,0x0C,0x1E,0x00}, // 89 Y
        {0x7F,0x63,0x31,0x18,0x4C,0x66,0x7F,0x00}, // 90 Z
        {0x1E,0x06,0x06,0x06,0x06,0x06,0x1E,0x00}, // 91 [
        {0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0x00}, // 92 backslash
        {0x1E,0x18,0x18,0x18,0x18,0x18,0x1E,0x00}, // 93 ]
        {0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00}, // 94 ^
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF}, // 95 _
        {0x0C,0x0C,0x18,0x00,0x00,0x00,0x00,0x00}, // 96 `
        {0x00,0x00,0x1E,0x30,0x3E,0x33,0x6E,0x00}, // 97 a
        {0x07,0x06,0x06,0x3E,0x66,0x66,0x3B,0x00}, // 98 b
        {0x00,0x00,0x1E,0x33,0x03,0x33,0x1E,0x00}, // 99 c
        {0x38,0x30,0x30,0x3e,0x33,0x33,0x6E,0x00}, // 100 d
        {0x00,0x00,0x1E,0x33,0x3f,0x03,0x1E,0x00}, // 101 e
        {0x1C,0x36,0x06,0x0f,0x06,0x06,0x0F,0x00}, // 102 f
        {0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x1F}, // 103 g
        {0x07,0x06,0x36,0x6E,0x66,0x66,0x67,0x00}, // 104 h
        {0x0C,0x00,0x0E,0x0C,0x0C,0x0C,0x1E,0x00}, // 105 i
        {0x30,0x00,0x30,0x30,0x30,0x33,0x33,0x1E}, // 106 j
        {0x07,0x06,0x66,0x36,0x1E,0x36,0x67,0x00}, // 107 k
        {0x0E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, // 108 l
        {0x00,0x00,0x33,0x7F,0x7F,0x6B,0x63,0x00}, // 109 m
        {0x00,0x00,0x1F,0x33,0x33,0x33,0x33,0x00}, // 110 n
        {0x00,0x00,0x1E,0x33,0x33,0x33,0x1E,0x00}, // 111 o
        {0x00,0x00,0x3B,0x66,0x66,0x3E,0x06,0x0F}, // 112 p
        {0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x78}, // 113 q
        {0x00,0x00,0x3B,0x6E,0x66,0x06,0x0F,0x00}, // 114 r
        {0x00,0x00,0x3E,0x03,0x1E,0x30,0x1F,0x00}, // 115 s
        {0x08,0x0C,0x3E,0x0C,0x0C,0x2C,0x18,0x00}, // 116 t
        {0x00,0x00,0x33,0x33,0x33,0x33,0x6E,0x00}, // 117 u
        {0x00,0x00,0x33,0x33,0x33,0x1E,0x0C,0x00}, // 118 v
        {0x00,0x00,0x63,0x6B,0x7F,0x7F,0x36,0x00}, // 119 w
        {0x00,0x00,0x63,0x36,0x1C,0x36,0x63,0x00}, // 120 x
        {0x00,0x00,0x33,0x33,0x33,0x3E,0x30,0x1F}, // 121 y
        {0x00,0x00,0x3F,0x19,0x0C,0x26,0x3F,0x00}, // 122 z
        {0x38,0x0C,0x0C,0x07,0x0C,0x0C,0x38,0x00}, // 123 {
        {0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00}, // 124 |
        {0x07,0x0C,0x0C,0x38,0x0C,0x0C,0x07,0x00}, // 125 }
        {0x6E,0x3B,0x00,0x00,0x00,0x00,0x00,0x00}, // 126 ~
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 127 DEL (blank)
    };

    // Create texture data
    unsigned char* texData = (unsigned char*)calloc(mTextureWidth * mTextureHeight * 4, 1);

    for (int c = 0; c < 96; c++)
    {
        int cellX = (c % mCharsPerRow) * mCharWidth;
        int cellY = (c / mCharsPerRow) * mCharHeight;

        for (int y = 0; y < 8; y++)
        {
            unsigned char row = fontData[c][y];
            for (int x = 0; x < 8; x++)
            {
                if (row & (1 << (7 - x)))
                {
                    int texX = cellX + x;
                    int texY = cellY + y;
                    int idx = (texY * mTextureWidth + texX) * 4;
                    texData[idx + 0] = 255;
                    texData[idx + 1] = 255;
                    texData[idx + 2] = 255;
                    texData[idx + 3] = 255;
                }
            }
        }
    }

    // Create OpenGL texture
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTextureWidth, mTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glBindTexture(GL_TEXTURE_2D, 0);

    free(texData);
    return true;
}

//======================================================================================================================
void FontRenderer::SetRect(int16_t x, int16_t y, int16_t w, int16_t h)
{
    mRectX = x;
    mRectY = y;
    mRectW = w;
    mRectH = h;
}

//======================================================================================================================
void FontRenderer::SetAlignmentHor(FontAlignHor align)
{
    mAlignH = align;
}

//======================================================================================================================
void FontRenderer::SetAlignmentVer(FontAlignVer align)
{
    mAlignV = align;
}

//======================================================================================================================
void FontRenderer::SetColour(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    mColour = (a << 24) | (r << 16) | (g << 8) | b;
}

//======================================================================================================================
void FontRenderer::SetColourABGR(uint32_t abgr)
{
    // ABGR format: 0xAABBGGRR
    // Convert to our internal ARGB format: 0xAARRGGBB
    uint8_t a = (abgr >> 24) & 0xFF;
    uint8_t b = (abgr >> 16) & 0xFF;
    uint8_t g = (abgr >> 8) & 0xFF;
    uint8_t r = abgr & 0xFF;
    mColour = (a << 24) | (r << 16) | (g << 8) | b;
}

//======================================================================================================================
uint32_t FontRenderer::GetColourABGR() const
{
    // Convert internal ARGB to ABGR format
    uint8_t a = (mColour >> 24) & 0xFF;
    uint8_t r = (mColour >> 16) & 0xFF;
    uint8_t g = (mColour >> 8) & 0xFF;
    uint8_t b = mColour & 0xFF;
    return (a << 24) | (b << 16) | (g << 8) | r;
}

//======================================================================================================================
int FontRenderer::CalculateTextWidth(const char* text) const
{
    if (!text) return 0;
    int width = 0;
    for (const char* c = text; *c; c++)
    {
        char ch = *c;
        if (ch >= mFirstChar && ch < mFirstChar + 96)
            width += mCharAdvance[ch - mFirstChar];
        else
            width += mCharWidth; // fallback for unknown chars
    }
    return width;
}

//======================================================================================================================
void FontRenderer::DrawCharacter(char c, float x, float y)
{
    if (c < mFirstChar || c >= mFirstChar + 96)
        return;

    int charIndex = c - mFirstChar;
    int cellX = charIndex % mCharsPerRow;
    int cellY = charIndex / mCharsPerRow;

    // Get the actual advance width for this character (for proper proportional rendering)
    int charAdvance = mCharAdvance[charIndex];

    // Calculate UV coordinates - only sample the portion of the cell that contains the glyph
    // Swap v0 and v1 to flip the texture vertically (GL has Y=0 at bottom, texture was created with Y=0 at top)
    float u0 = (float)(cellX * mCharWidth) / (float)mTextureWidth;
    float v0 = (float)((cellY + 1) * mCharHeight) / (float)mTextureHeight;  // Bottom of character in texture
    // Use advance width for u1 to avoid sampling empty space in the cell
    float u1 = (float)(cellX * mCharWidth + charAdvance) / (float)mTextureWidth;
    float v1 = (float)(cellY * mCharHeight) / (float)mTextureHeight;  // Top of character in texture

    float x0 = x;
    float y0 = y;
    float x1 = x + charAdvance;  // Draw quad width matches character advance
    float y1 = y + mCharHeight;

    // UV mapping for quad (v0 is now bottom, v1 is top - matching GL's Y axis)
    float uvs[] = {
        u0, v0,  // bottom-left vertex gets bottom of texture
        u1, v0,  // bottom-right vertex gets bottom of texture
        u1, v1,  // top-right vertex gets top of texture
        u0, v1,  // top-left vertex gets top of texture
    };

    float pts[] = {
        x0, y0, 0,
        x1, y0, 0,
        x1, y1, 0,
        x0, y1, 0,
    };

    if (gGLVersion == 1)
    {
        glVertexPointer(3, GL_FLOAT, 0, pts);
        glTexCoordPointer(2, GL_FLOAT, 0, uvs);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    else
    {
        const OverlayShader* overlayShader = (OverlayShader*)ShaderManager::GetInstance().GetShader(SHADER_OVERLAY);
        glVertexAttribPointer(overlayShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, pts);
        glVertexAttribPointer(overlayShader->a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, uvs);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}

//======================================================================================================================
void FontRenderer::RenderText(const char* text)
{
    if (!text || !*text || !mInitialized || mTextureID == 0)
        return;

    // Get viewport height for coordinate conversion
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int viewportHeight = viewport[3];

    // Calculate text dimensions
    int textWidth = CalculateTextWidth(text);
    int textHeight = mCharHeight;

    // Calculate starting position based on rect and alignment
    // Note: The rect Y uses screen coordinates (Y=0 at top, Y increases downward)
    // Pattern in code: mBottom + mHeight - offset means "offset pixels from top of display"
    float x = (float)mRectX;
    float y = (float)mRectY;

    // Horizontal alignment
    if (mAlignH == FONT_ALIGN_CENTRE)
        x += (mRectW - textWidth) / 2.0f;
    else if (mAlignH == FONT_ALIGN_RIGHT)
        x += mRectW - textWidth;

    // Vertical alignment (in screen coords where Y increases downward)
    if (mAlignV == FONT_ALIGN_MIDDLE)
        y += (mRectH - textHeight) / 2.0f;
    else if (mAlignV == FONT_ALIGN_BOTTOM)
        y += mRectH - textHeight;
    // FONT_ALIGN_TOP - y stays at mRectY

    // Convert from screen coordinates (Y=0 at top) to GL coordinates (Y=0 at bottom)
    // In screen coords, y is the TOP of the text
    // In GL coords, we need the BOTTOM of the text for rendering
    y = viewportHeight - y - textHeight;

    // Extract color components
    uint8_t a = (mColour >> 24) & 0xFF;
    uint8_t r = (mColour >> 16) & 0xFF;
    uint8_t g = (mColour >> 8) & 0xFF;
    uint8_t b = mColour & 0xFF;

    // Set up rendering state
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (gGLVersion == 1)
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glDisableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glColor4ub(r, g, b, a);
    }
    else
    {
        const OverlayShader* overlayShader = (OverlayShader*)ShaderManager::GetInstance().GetShader(SHADER_OVERLAY);
        overlayShader->Use();
        glUniform1i(overlayShader->u_texture, 0);
        glEnableVertexAttribArray(overlayShader->a_position);
        glEnableVertexAttribArray(overlayShader->a_texCoord);
        glUniform4f(overlayShader->u_colour, r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
        esSetModelViewProjectionMatrix(overlayShader->u_mvpMatrix);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    // Draw each character
    for (const char* c = text; *c; c++)
    {
        char ch = *c;
        DrawCharacter(ch, x, y);
        // Advance by the actual character width
        if (ch >= mFirstChar && ch < mFirstChar + 96)
            x += mCharAdvance[ch - mFirstChar];
        else
            x += mCharWidth;
    }

    // Restore state
    glBindTexture(GL_TEXTURE_2D, 0);

    if (gGLVersion == 1)
    {
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }
    else
    {
        const OverlayShader* overlayShader = (OverlayShader*)ShaderManager::GetInstance().GetShader(SHADER_OVERLAY);
        glDisableVertexAttribArray(overlayShader->a_position);
        glDisableVertexAttribArray(overlayShader->a_texCoord);
    }
}
