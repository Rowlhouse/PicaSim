#ifndef PICASTYLE_H
#define PICASTYLE_H

#include "imgui.h"

//=================================================================================================
// PicaStyle - Centralized UI color and style definitions
//=================================================================================================
namespace PicaStyle {

    // === COMMON COLORS (truly shared across all UI) ===
    namespace Common {
        // Accent colors (professional blue #4A90B8)
        const ImVec4 Accent = ImVec4(0.29f, 0.56f, 0.72f, 1.0f);
        const ImVec4 AccentHovered = ImVec4(0.25f, 0.52f, 0.68f, 1.0f);
        const ImVec4 AccentActive = ImVec4(0.21f, 0.49f, 0.64f, 1.0f);

        // As ImU32 for custom drawing
        const ImU32 AccentU32 = IM_COL32(74, 144, 184, 255);
        const ImU32 AccentHoveredU32 = IM_COL32(64, 134, 174, 255);
        const ImU32 AccentActiveU32 = IM_COL32(54, 124, 164, 255);

        // Text colors
        const ImVec4 TextBlack = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        const ImVec4 TextWhite = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        const ImVec4 TextDimmed = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        const ImVec4 TextSecondary = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        const ImU32 TextPrimaryU32 = IM_COL32(44, 62, 80, 255);       // #2C3E50
        const ImU32 TextSecondaryU32 = IM_COL32(108, 122, 138, 255);  // #6C7A8A

        // Input fields
        const ImVec4 FrameBgWhite = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

        // Standard styling
        const float CornerRadius = 4.0f;
    }

    // === MENU STYLES (FileMenu, HelpMenu, WhatsNewMenu) ===
    namespace Menu {
        const ImVec4 WindowBg = ImVec4(0.70f, 0.75f, 0.82f, 1.0f);
        const ImVec4 ChildBg = ImVec4(0.85f, 0.88f, 0.92f, 1.0f);
        const ImVec4 Button = ImVec4(0.78f, 0.80f, 0.85f, 1.0f);
        const ImVec4 ButtonHovered = ImVec4(0.82f, 0.85f, 0.92f, 1.0f);
        const ImVec4 ButtonActive = ImVec4(0.68f, 0.71f, 0.78f, 1.0f);
        const ImVec4 Tab = ImVec4(0.75f, 0.78f, 0.85f, 1.0f);
        const ImVec4 TabHovered = ImVec4(0.85f, 0.88f, 0.95f, 1.0f);
        const ImVec4 TabActive = ImVec4(0.90f, 0.92f, 0.98f, 1.0f);
    }

    // === SETTINGS MENU STYLES ===
    namespace Settings {
        const ImVec4 WindowBg = ImVec4(0.70f, 0.75f, 0.82f, 1.0f);  // Same as Menu
        const ImVec4 BlockBg = ImVec4(0.91f, 0.91f, 0.93f, 1.0f);   // Settings blocks (lighter)
        // Button colors: use Menu::Button, Menu::ButtonHovered, Menu::ButtonActive

        // Section headers
        const ImVec4 SectionHeaderBg = ImVec4(0.35f, 0.42f, 0.48f, 1.0f);      // #5A6A7A
        const ImVec4 UberSectionHeaderBg = ImVec4(0.24f, 0.42f, 0.54f, 1.0f);  // #3D6B8A teal
        const float SectionHeaderRounding = 3.0f;
        const float BlockRounding = 4.0f;
        const ImVec2 BlockPadding = ImVec2(8.0f, 6.0f);

        // Custom control colors
        const ImU32 SliderTrackBg = IM_COL32(200, 203, 210, 255);
        const ImU32 SliderTrackFill = IM_COL32(74, 144, 184, 255);  // Accent
        const ImU32 SliderGrab = IM_COL32(74, 144, 184, 255);
        const ImU32 SliderGrabHovered = IM_COL32(64, 134, 174, 255);
        const ImU32 SliderGrabActive = IM_COL32(54, 124, 164, 255);
        const float SliderTrackHeight = 6.0f;
        const float SliderGrabRadius = 10.0f;

        const ImU32 CheckboxBorder = IM_COL32(150, 155, 165, 255);
        const ImU32 CheckboxFill = IM_COL32(74, 144, 184, 255);     // Accent
        const ImU32 CheckboxCheck = IM_COL32(255, 255, 255, 255);
        const float CheckboxSize = 18.0f;

        const ImVec4 ComboPopupBg = ImVec4(0.95f, 0.95f, 0.97f, 1.0f);
    }

    // === DIALOG STYLES (modal overlays) ===
    namespace Dialog {
        const ImVec4 WindowBg = ImVec4(0.90f, 0.92f, 0.95f, 0.94f);  // Lighter, semi-transparent
        const ImVec4 ChildBg = ImVec4(0.85f, 0.88f, 0.92f, 0.90f);   // Semi-transparent
        const ImVec4 Border = ImVec4(0.5f, 0.5f, 0.6f, 0.5f);
        // Uses Menu::Button colors for buttons
    }

    // === START MENU STYLES (on panorama background) ===
    namespace StartMenu {
        const ImVec4 Button = ImVec4(0.95f, 0.95f, 0.95f, 0.9f);
        const ImVec4 ButtonHovered = ImVec4(1.0f, 1.0f, 1.0f, 0.95f);
        const ImVec4 ButtonActive = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);
    }

    // === IMAGE BUTTON STYLES (transparent buttons over images) ===
    namespace ImageButton {
        const ImVec4 Transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        // For menu contexts (on light content backgrounds)
        const ImVec4 HoverDark = ImVec4(0.3f, 0.3f, 0.3f, 0.3f);
        const ImVec4 ActiveDark = ImVec4(0.2f, 0.2f, 0.2f, 0.3f);
        // For panorama/dark contexts
        const ImVec4 HoverLight = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
        const ImVec4 ActiveLight = ImVec4(1.0f, 1.0f, 1.0f, 0.4f);
        // Placeholder for missing thumbnails
        const ImVec4 Placeholder = ImVec4(0.5f, 0.5f, 0.5f, 0.5f);
    }

    // === LAYOUT CONSTANTS ===
    namespace Layout {
        const float ControlWidthFraction = 0.45f;   // How much of row width the control takes
        const float LabelRightPadding = 16.0f;      // Padding between label and value
        const float LabelValueButtonWidth = 80.0f;  // Width of value column in LabelValueButton
        const float RowExtraSpacing = 2.0f;         // Extra vertical spacing between rows
    }

    // === HELPER FUNCTIONS ===
    // Push/pop complete style sets for different UI contexts

    // For FileMenu, HelpMenu, WhatsNewMenu (10 colors + 2 style vars)
    void PushMenuStyle();
    void PopMenuStyle();

    // For PicaDialog (7 colors + 2 style vars)
    void PushDialogStyle();
    void PopDialogStyle();

    // For SettingsMenu (10 colors + 2 style vars)
    void PushSettingsStyle();
    void PopSettingsStyle();

    // For StartMenu buttons (4 colors + 2 style vars)
    void PushStartMenuButtonStyle(float scale);
    void PopStartMenuButtonStyle();

} // namespace PicaStyle

#endif // PICASTYLE_H
