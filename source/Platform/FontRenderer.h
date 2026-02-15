#pragma once

#include <cstdint>

// Font alignment enums
enum FontAlignHor
{
    FONT_ALIGN_LEFT = 0,
    FONT_ALIGN_CENTRE = 1,
    FONT_ALIGN_RIGHT = 2
};

enum FontAlignVer
{
    FONT_ALIGN_TOP = 0,
    FONT_ALIGN_MIDDLE = 1,
    FONT_ALIGN_BOTTOM = 2
};

// Font renderer using bitmap font texture
// Supports both GL1 (fixed function) and GL2 (shaders)
class FontRenderer
{
public:
    FontRenderer();
    ~FontRenderer();

    static FontRenderer& GetInstance();

    // Override the display height used for scaling (0 = use actual display height)
    // Used by VR overlay rendering to decouple from desktop window / headset resolution
    static void SetDisplayHeightOverride(int height) { sDisplayHeightOverride = height; }

    // Initialize font renderer - call after OpenGL context is ready
    bool Init();

    // Shutdown and release resources
    void Shutdown();

    // Set font state
    void SetRect(int16_t x, int16_t y, int16_t w, int16_t h);
    void SetAlignmentHor(FontAlignHor align);
    void SetAlignmentVer(FontAlignVer align);
    void SetColour(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void SetColourABGR(uint32_t abgr);

    // Get font info
    uint16_t GetFontHeight() const;
    uint32_t GetColour() const { return mColour; }
    uint32_t GetColourABGR() const;

    // Draw text (named RenderText to avoid Windows DrawText macro collision)
    void RenderText(const char* text);

private:
    // Disable copy
    FontRenderer(const FontRenderer&) = delete;
    FontRenderer& operator=(const FontRenderer&) = delete;

    // Display scale factor for resolution-independent text
    float GetDisplayScale() const;

    // Generate font texture from TTF file
    bool GenerateFontTexture(const char* ttfPath, int fontSize);

    // Load font texture from PNG file
    bool LoadFontTexture(const char* pngPath);

    // Create fallback font texture (embedded simple font)
    bool CreateFallbackFont();

    // Draw a single character at position
    void DrawCharacter(char c, float x, float y);

    // Calculate text width in pixels
    int CalculateTextWidth(const char* text) const;

    // Font texture
    uint32_t mTextureID;
    int mTextureWidth;
    int mTextureHeight;

    // Character metrics
    int mCharWidth;   // Width of each character cell in texture
    int mCharHeight;  // Height of each character cell
    int mCharsPerRow; // Characters per row in texture
    int mFirstChar;   // First ASCII character in font (usually 32 = space)
    int mCharAdvance[96]; // Per-character advance width for proper spacing

    // Font state
    int16_t mRectX, mRectY, mRectW, mRectH;
    FontAlignHor mAlignH;
    FontAlignVer mAlignV;
    uint32_t mColour; // ARGB format

    bool mInitialized;

    static int sDisplayHeightOverride;
};

// Global FontRenderer instance pointer (set in main())
extern FontRenderer* gFontRenderer;
