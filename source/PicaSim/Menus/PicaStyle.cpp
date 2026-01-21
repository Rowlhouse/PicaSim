#include "PicaStyle.h"

namespace PicaStyle {

//======================================================================================================================
void PushMenuStyle()
{
    // Window and child backgrounds
    ImGui::PushStyleColor(ImGuiCol_WindowBg, Menu::WindowBg);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Menu::ChildBg);

    // Tab styling
    ImGui::PushStyleColor(ImGuiCol_Tab, Menu::Tab);
    ImGui::PushStyleColor(ImGuiCol_TabHovered, Menu::TabHovered);
    ImGui::PushStyleColor(ImGuiCol_TabActive, Menu::TabActive);

    // Text
    ImGui::PushStyleColor(ImGuiCol_Text, Common::TextBlack);

    // Buttons
    ImGui::PushStyleColor(ImGuiCol_Button, Menu::Button);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Menu::ButtonHovered);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, Menu::ButtonActive);

    // Frame backgrounds (for inputs)
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Common::FrameBgWhite);

    // Rounding
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, Common::CornerRadius);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, Common::CornerRadius);
}

//======================================================================================================================
void PopMenuStyle()
{
    ImGui::PopStyleVar(2);    // FrameRounding, WindowRounding
    ImGui::PopStyleColor(10); // All colors pushed in PushMenuStyle
}

//======================================================================================================================
void PushDialogStyle()
{
    // Window and child backgrounds (semi-transparent for overlay effect)
    ImGui::PushStyleColor(ImGuiCol_WindowBg, Dialog::WindowBg);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Dialog::ChildBg);

    // Text
    ImGui::PushStyleColor(ImGuiCol_Text, Common::TextBlack);

    // Buttons (use Menu button colors)
    ImGui::PushStyleColor(ImGuiCol_Button, Menu::Button);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Menu::ButtonHovered);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, Menu::ButtonActive);

    // Border
    ImGui::PushStyleColor(ImGuiCol_Border, Dialog::Border);

    // Rounding
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, Common::CornerRadius);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, Common::CornerRadius);
}

//======================================================================================================================
void PopDialogStyle()
{
    ImGui::PopStyleVar(2);   // FrameRounding, WindowRounding
    ImGui::PopStyleColor(7); // All colors pushed in PushDialogStyle
}

//======================================================================================================================
void PushSettingsStyle()
{
    // Window and child backgrounds
    ImGui::PushStyleColor(ImGuiCol_WindowBg, Settings::WindowBg);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Settings::BlockBg);

    // Tab styling (use Menu tab colors - they're the same)
    ImGui::PushStyleColor(ImGuiCol_Tab, Menu::Tab);
    ImGui::PushStyleColor(ImGuiCol_TabHovered, Menu::TabHovered);
    ImGui::PushStyleColor(ImGuiCol_TabActive, Menu::TabActive);

    // Text
    ImGui::PushStyleColor(ImGuiCol_Text, Common::TextBlack);

    // Buttons (shared with Menu)
    ImGui::PushStyleColor(ImGuiCol_Button, Menu::Button);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Menu::ButtonHovered);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, Menu::ButtonActive);

    // Frame backgrounds (for inputs)
    ImGui::PushStyleColor(ImGuiCol_FrameBg, Common::FrameBgWhite);

    // Rounding
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, Common::CornerRadius);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, Common::CornerRadius);
}

//======================================================================================================================
void PopSettingsStyle()
{
    ImGui::PopStyleVar(2);    // FrameRounding, WindowRounding
    ImGui::PopStyleColor(10); // All colors pushed in PushSettingsStyle
}

//======================================================================================================================
void PushStartMenuButtonStyle(float scale)
{
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, Common::CornerRadius * scale);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_Button, StartMenu::Button);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, StartMenu::ButtonHovered);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, StartMenu::ButtonActive);
    ImGui::PushStyleColor(ImGuiCol_Text, Common::TextBlack);
}

//======================================================================================================================
void PopStartMenuButtonStyle()
{
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(2);
}

} // namespace PicaStyle
