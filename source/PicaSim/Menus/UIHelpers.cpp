#include "UIHelpers.h"
#include "Texture.h"
#include "Platform.h"

#include "imgui.h"

#include <cstdio>

namespace UIHelpers
{

// Static state
static ImFont* sFont = nullptr;
static float sBaseFontSize = 18.0f;  // Base font size at 720p

//======================================================================================================================
void Init()
{
    // Font path relative to data directory (app runs from data folder)
    const char* fontPath = "Fonts/FontRegular.ttf";

    // Calculate initial font size based on current screen
    int height = Platform::GetScreenHeight();
    float scale = height / 720.0f;
    if (scale < 1.0f) scale = 1.0f;
    float fontSize = sBaseFontSize * scale;

    // Load the font
    ImGuiIO& io = ImGui::GetIO();

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
        printf("UIHelpers: Loaded font from %s at size %.1f with extended glyphs\n", fontPath, fontSize);
    }
    else
    {
        printf("UIHelpers: Failed to load font from %s, using default\n", fontPath);
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
}

//======================================================================================================================
float GetFontScale()
{
    int height = Platform::GetScreenHeight();
    float scale = height / 720.0f;
    if (scale < 1.0f) scale = 1.0f;
    return scale;
}

//======================================================================================================================
void ApplyFontScale()
{
    ImGui::GetIO().FontGlobalScale = GetFontScale();
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

    int width = Platform::GetScreenWidth();
    int height = Platform::GetScreenHeight();

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

    int width = Platform::GetScreenWidth();
    int height = Platform::GetScreenHeight();

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    // Use the loaded font if available
    ImFont* font = sFont ? sFont : ImGui::GetFont();

    // Calculate font size: base font size * screen scale * optional extra scale
    float screenScale = GetFontScale();
    float fontSize = font->FontSize * screenScale * scale;

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

    // Draw the image button with transparent background
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.4f));

    bool clicked = ImGui::ImageButton(id,
        (ImTextureID)(intptr_t)texID,
        buttonSize);

    ImGui::PopStyleColor(3);

    return clicked;
}

//======================================================================================================================
void PushSettingsStyle()
{
    // Window and child backgrounds
    ImGui::PushStyleColor(ImGuiCol_WindowBg, Style::WindowBg);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Style::ChildBg);

    // Tab styling
    ImGui::PushStyleColor(ImGuiCol_Tab, Style::Tab);
    ImGui::PushStyleColor(ImGuiCol_TabHovered, Style::TabHovered);
    ImGui::PushStyleColor(ImGuiCol_TabActive, Style::TabActive);

    // Text
    ImGui::PushStyleColor(ImGuiCol_Text, Style::TextLight);

    // Buttons
    ImGui::PushStyleColor(ImGuiCol_Button, Style::Button);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Style::ButtonHovered);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, Style::ButtonActive);

    // Frame backgrounds (for inputs)
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    // Rounding
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, Style::CornerRadius);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, Style::CornerRadius);
}

//======================================================================================================================
void PopSettingsStyle()
{
    ImGui::PopStyleVar(2);   // FrameRounding, WindowRounding
    ImGui::PopStyleColor(10); // All colors pushed in PushSettingsStyle
}

//======================================================================================================================
void PushStartMenuButtonStyle(float scale)
{
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, Style::CornerRadius * scale);
    ImGui::PushStyleColor(ImGuiCol_Button, Style::ButtonLight);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Style::ButtonLightHovered);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, Style::ButtonLightActive);
    ImGui::PushStyleColor(ImGuiCol_Text, Style::TextLight);
}

//======================================================================================================================
void PopStartMenuButtonStyle()
{
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
}

} // namespace UIHelpers
