#ifndef UIHELPERS_H
#define UIHELPERS_H

#include "Helpers.h"
#include "imgui.h"
#include "PicaStyle.h"

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

    // Set up a fullscreen ImGui window with safe area insets applied.
    // Returns the usable content size (width, height) inside the window.
    // Call ImGui::Begin() immediately after this.
    ImVec2 BeginFullscreenWindow(const char* name, ImGuiWindowFlags flags);

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

    // Call when transitioning to a new menu to suppress stale input
    void NotifyMenuTransition();

    // Returns true if input should be suppressed (within 200ms of a menu transition)
    bool IsInputMuted();

    // NOTE: Style colors and Push/Pop functions have moved to PicaStyle.h
    // Use PicaStyle::PushSettingsStyle(), PicaStyle::PushMenuStyle(), etc.
}

#endif // UIHELPERS_H
