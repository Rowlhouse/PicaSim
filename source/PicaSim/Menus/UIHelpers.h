#ifndef UIHELPERS_H
#define UIHELPERS_H

#include "Helpers.h"
#include "imgui.h"

class Texture;

// UI Helper functions for ImGui-based menus
namespace UIHelpers
{
    // Initialize the UI system (call once at startup)
    void Init();

    // Shutdown the UI system
    void Shutdown();

    // Get font scale based on current screen size (720p = 1.0)
    float GetFontScale();

    // Apply font scaling for current frame
    void ApplyFontScale();

    // Get the loaded font (or nullptr if not loaded)
    ImFont* GetFont();

    // Draw a full-screen background image with aspect-ratio-correct cropping
    // Returns the draw list used (for additional drawing)
    ImDrawList* DrawBackground(Texture* texture);

    // Draw centered text at a vertical position (0.0 = top, 1.0 = bottom)
    // Scale is relative to the screen-scaled font size (1.0 = normal)
    void DrawCenteredText(const char* text, float verticalPosition, ImU32 color, float scale = 1.0f);

    // Draw a clickable image button at absolute position
    // Returns true if clicked. Size is in pixels (will be scaled by GetFontScale internally)
    // Note: Must be called within an ImGui window context (Begin/End)
    bool DrawImageButton(const char* id, Texture* texture, float x, float y, float size);

    // Convert Colour to ImGui color
    inline ImU32 ColourToImU32(const Colour& col)
    {
        return IM_COL32(col.GetR(), col.GetG(), col.GetB(), col.GetA());
    }

    // Common colors
    namespace Colors
    {
        const ImU32 White = IM_COL32(255, 255, 255, 255);
        const ImU32 Black = IM_COL32(0, 0, 0, 255);
        const ImU32 LightGray = IM_COL32(220, 220, 220, 255);
        const ImU32 DarkGray = IM_COL32(64, 64, 64, 255);
        const ImU32 Yellow = IM_COL32(255, 255, 0, 255);
    }

    //=========================================================================
    // Unified Style System
    //=========================================================================
    namespace Style
    {
        // Color palette
        const ImVec4 Accent = ImVec4(0.29f, 0.56f, 0.72f, 1.0f);           // #4A90B8 professional blue
        const ImVec4 AccentHovered = ImVec4(0.25f, 0.52f, 0.68f, 1.0f);    // Darker
        const ImVec4 AccentActive = ImVec4(0.21f, 0.49f, 0.64f, 1.0f);     // Even darker

        const ImVec4 WindowBg = ImVec4(0.70f, 0.75f, 0.82f, 1.0f);         // Settings window bg
        const ImVec4 ChildBg = ImVec4(0.91f, 0.91f, 0.93f, 1.0f);          // #E8E8EC warm light gray
        const ImVec4 SectionHeader = ImVec4(0.35f, 0.42f, 0.48f, 1.0f);    // #5A6A7A softer blue-gray

        const ImVec4 TextPrimary = ImVec4(0.17f, 0.24f, 0.31f, 1.0f);      // #2C3E50 dark blue-gray
        const ImVec4 TextLight = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);           // Black (for menus)

        // Standard button (for settings menu - subtle gray)
        const ImVec4 Button = ImVec4(0.78f, 0.80f, 0.85f, 1.0f);
        const ImVec4 ButtonHovered = ImVec4(0.85f, 0.88f, 0.95f, 1.0f);
        const ImVec4 ButtonActive = ImVec4(0.68f, 0.71f, 0.78f, 1.0f);

        // Light button (for start menu - on panorama background)
        const ImVec4 ButtonLight = ImVec4(0.95f, 0.95f, 0.95f, 0.9f);
        const ImVec4 ButtonLightHovered = ImVec4(1.0f, 1.0f, 1.0f, 0.95f);
        const ImVec4 ButtonLightActive = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);

        // Tab styling
        const ImVec4 Tab = ImVec4(0.75f, 0.78f, 0.85f, 1.0f);
        const ImVec4 TabHovered = ImVec4(0.85f, 0.88f, 0.95f, 1.0f);
        const ImVec4 TabActive = ImVec4(0.90f, 0.92f, 0.98f, 1.0f);

        // Standard corner radius
        const float CornerRadius = 4.0f;
    }

    // Push settings menu style colors (call PopSettingsStyle when done)
    void PushSettingsStyle();
    void PopSettingsStyle();

    // Push start menu button style (call PopStartMenuButtonStyle when done)
    void PushStartMenuButtonStyle(float scale);
    void PopStartMenuButtonStyle();
}

#endif // UIHELPERS_H
