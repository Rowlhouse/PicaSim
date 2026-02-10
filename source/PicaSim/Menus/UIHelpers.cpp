#include "UIHelpers.h"
#include "Texture.h"
#include "Platform.h"
#include "Trace.h"

#include "../../Platform/Window.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <cstdio>
#include <cmath>

namespace UIHelpers
{

// Static state
static ImFont* sFont = nullptr;
static float sBaseFontSize = 18.0f;  // Base font size at 720p
static Uint64 sMenuTransitionTime = 0;

//======================================================================================================================
void Init()
{
    // Destroy any stale ImGui state from a previous Android launch
    // that didn't shut down cleanly. Safe no-op on fresh launch.
    ImGuiContext* ctx = ImGui::GetCurrentContext();
    if (ctx != nullptr)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (io.BackendRendererUserData != nullptr)
            ImGui_ImplOpenGL3_Shutdown();
        if (io.BackendPlatformUserData != nullptr)
            ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    // Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Platform/Renderer backends
    Window& window = Window::GetInstance();
    ImGui_ImplSDL2_InitForOpenGL(window.GetSDLWindow(), window.GetGLContext());
#if defined(PICASIM_ANDROID) || defined(PICASIM_IOS)
    ImGui_ImplOpenGL3_Init("#version 100");
#else
    ImGui_ImplOpenGL3_Init("#version 130");
#endif

    // Setup style
    ImGui::StyleColorsDark();

    // Font path relative to data directory (app runs from data folder)
    const char* fontPath = "Fonts/FontRegular.ttf";

    // Calculate initial font size based on current screen
    float scale = GetFontScale();
    float fontSize = sBaseFontSize * scale;

    // Custom glyph ranges: basic Latin + Latin Extended + General Punctuation (includes bullet •)
    static const ImWchar glyphRanges[] =
    {
        0x0020, 0x00FF,  // Basic Latin + Latin Supplement
        0x0100, 0x017F,  // Latin Extended-A
        0x2000, 0x206F,  // General Punctuation (includes bullet •, dashes, etc.)
        0x2100, 0x214F,  // Letterlike Symbols
        0,               // Null terminator
    };

    // Try to load the custom font with extended glyph ranges
    ImFontConfig fontConfig;
    fontConfig.OversampleH = 2;
    fontConfig.OversampleV = 2;
    sFont = io.Fonts->AddFontFromFileTTF(fontPath, fontSize, &fontConfig, glyphRanges);
    if (sFont)
    {
        TRACE_FILE_IF(ONCE_1) TRACE(
            "UIHelpers: Loaded font from %s at size %.1f with extended glyphs\n", fontPath, fontSize);
    }
    else
    {
        TRACE_FILE_IF(ONCE_1) TRACE("UIHelpers: Failed to load font from %s, using default\n", fontPath);
        sFont = io.Fonts->AddFontDefault();
    }

    // Build the font atlas
    io.Fonts->Build();
}

//======================================================================================================================
void Shutdown()
{
    // Font is managed by ImGui, no need to manually delete
    sFont = nullptr;

    // Shutdown Dear ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

//======================================================================================================================
float GetFontScale()
{
    // Cache the scale - dimensions don't change on mobile fullscreen,
    // and this is called 16+ times per frame
    static int sCachedWidth = 0;
    static int sCachedHeight = 0;
    static float sCachedScale = 1.0f;

    int w = Platform::GetDisplayWidth();
    int h = Platform::GetDisplayHeight();

    if (w == sCachedWidth && h == sCachedHeight)
        return sCachedScale;

    float scale;

#if defined(PICASIM_MOBILE)
    // DPI-aware formula for mobile:
    //   scale = sqrt(targetFontDp * dpiScale / baseFontSize)
    //
    // This accounts for double-scaling: font loaded at baseFontSize*scale,
    // then FontGlobalScale=scale, giving effective text = baseFontSize * scale^2.
    // So scale^2 = targetFontDp * dpiScale / baseFontSize => targetFontDp dp text.
    float dpiScale = Platform::GetDisplayScale();
    float diagonal = Platform::GetSurfaceDiagonalInches();

    // Device classification by physical screen diagonal
    float targetFontDp;
    const char* deviceClass;
    if (diagonal > 0.0f && diagonal < 7.0f)
    {
        targetFontDp = 18.0f;  // Phone: Android Material body text standard
        deviceClass = "Phone";
    }
    else if (diagonal >= 7.0f && diagonal <= 11.0f)
    {
        targetFontDp = 20.0f;  // Tablet: larger text for medium screens
        deviceClass = "Tablet";
    }
    else
    {
        targetFontDp = 24.0f;  // Large/unknown: desktop-equivalent sizing
        deviceClass = "Large";
    }

    scale = sqrtf(targetFontDp * dpiScale / sBaseFontSize);
    if (scale < 1.0f) scale = 1.0f;

    // Diagnostic logging (only on first computation or dimension change)
    TRACE_FILE_IF(ONCE_1) TRACE("UIHelpers: DPI-aware scale: dpi=%.0f dpiScale=%.2f diagonal=%.2f\" "
           "class=%s targetDp=%.0f scale=%.2f (res=%dx%d)\n",
           Platform::GetScreenDPI(), dpiScale, diagonal,
           deviceClass, targetFontDp, scale, w, h);
#else
    // Desktop: purely pixel-based (unchanged behaviour)
    scale = h / 720.0f;
    if (scale < 1.0f) scale = 1.0f;
#endif

    sCachedWidth = w;
    sCachedHeight = h;
    sCachedScale = scale;
    return scale;
}

//======================================================================================================================
ImVec2 BeginFullscreenWindow(const char* name, ImGuiWindowFlags flags)
{
    int screenW = Platform::GetDisplayWidth();
    int screenH = Platform::GetDisplayHeight();
    float insetX = Platform::GetSafeAreaInsetX();
    float insetY = Platform::GetSafeAreaInsetY();
    float winW = (float)screenW - 2 * insetX;
    float winH = (float)screenH - 2 * insetY;
    ImGui::SetNextWindowPos(ImVec2(insetX, insetY));
    ImGui::SetNextWindowSize(ImVec2(winW, winH));
    ImGui::Begin(name, nullptr, flags);
    return ImVec2(winW, winH);
}

//======================================================================================================================
void ApplyFontScale()
{
    float scale = GetFontScale();
    ImGui::GetIO().FontGlobalScale = scale;

    // Scale scrollbar and grab sizes for touch-friendliness.
    // Applied globally so combo popups and child windows all benefit.
    ImGuiStyle& style = ImGui::GetStyle();
#if defined(PICASIM_MOBILE)
    // Extra 1.5x multiplier on mobile for fat-finger-friendly scrollbars
    float scrollScale = scale * 1.5f;
#else
    float scrollScale = scale;
#endif
    style.ScrollbarSize = 14.0f * scrollScale;   // Default 14px
    style.GrabMinSize = 12.0f * scrollScale;     // Default 12px
    style.ScrollbarRounding = 9.0f * scrollScale; // Default 9px
}

//======================================================================================================================
ImFont* GetFont()
{
    return sFont;
}

//======================================================================================================================
ImDrawList* DrawBackground(Texture* texture)
{
    if (!texture)
        return ImGui::GetBackgroundDrawList();

    int width = Platform::GetDisplayWidth();
    int height = Platform::GetDisplayHeight();

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    GLuint texID = texture->GetTextureID();
    int texW = texture->GetWidth();
    int texH = texture->GetHeight();

    // Calculate UV coordinates for centered cropping
    ImVec2 uv0(0.0f, 0.0f);
    ImVec2 uv1(1.0f, 1.0f);

    // Height if we stretched the texture to fit horizontally
    float h = (float(width) / texW) * texH;
    if (h < height)
    {
        // Image is wider than screen - crop sides
        float f = 0.5f * (1.0f - h / height);
        uv0.x = f;
        uv1.x = 1.0f - f;
    }
    else
    {
        // Image is taller than screen - crop top/bottom
        float f = 0.5f * (1.0f - height / h);
        uv0.y = f;
        uv1.y = 1.0f - f;
    }

    drawList->AddImage(
        (ImTextureID)(intptr_t)texID,
        ImVec2(0, 0),
        ImVec2((float)width, (float)height),
        uv0, uv1
    );

    return drawList;
}

//======================================================================================================================
void DrawCenteredText(const char* text, float verticalPosition, ImU32 color, float scale)
{
    if (!text || !*text)
        return;

    int width = Platform::GetDisplayWidth();
    int height = Platform::GetDisplayHeight();

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    // Use the loaded font if available
    ImFont* font = sFont ? sFont : ImGui::GetFont();

    // Calculate font size: base font size * screen scale * optional extra scale
    float screenScale = GetFontScale();
    #if IMGUI_VERSION_NUM >= 19200
    float fontSize = font->LegacySize * screenScale * scale;
#else
    float fontSize = font->FontSize * screenScale * scale;
#endif

    ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text);
    float textX = (width - textSize.x) * 0.5f;
    float textY = height * verticalPosition - textSize.y * 0.5f;

    drawList->AddText(font, fontSize, ImVec2(textX, textY), color, text);
}

//======================================================================================================================
bool DrawImageButton(const char* id, Texture* texture, float x, float y, float size)
{
    if (!texture)
        return false;

    // Position the cursor
    ImGui::SetCursorPos(ImVec2(x, y));

    // Get texture ID and calculate size maintaining aspect ratio
    GLuint texID = texture->GetTextureID();
    int texW = texture->GetWidth();
    int texH = texture->GetHeight();

    // Calculate button size maintaining aspect ratio
    float aspect = (texH > 0) ? (float)texW / (float)texH : 1.0f;
    ImVec2 buttonSize;
    if (aspect >= 1.0f)
    {
        buttonSize = ImVec2(size, size / aspect);
    }
    else
    {
        buttonSize = ImVec2(size * aspect, size);
    }

    // Draw the image button with transparent background (light hover for dark backgrounds)
    ImGui::PushStyleColor(ImGuiCol_Button, PicaStyle::ImageButton::Transparent);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, PicaStyle::ImageButton::HoverLight);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, PicaStyle::ImageButton::ActiveLight);

    bool clicked = ImGui::ImageButton(id,
        (ImTextureID)(intptr_t)texID,
        buttonSize);

    ImGui::PopStyleColor(3);

    return clicked;
}

//======================================================================================================================
void NotifyMenuTransition()
{
    sMenuTransitionTime = SDL_GetTicks64();
}

//======================================================================================================================
bool IsInputMuted()
{
    return (SDL_GetTicks64() - sMenuTransitionTime) < 200;
}

} // namespace UIHelpers
