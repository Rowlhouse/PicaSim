#include "SettingsWidgets.h"
#include "UIHelpers.h"
#include "../Platform/Texture.h"

#include "imgui.h"

#include <cstdio>
#include <cmath>
#include <algorithm>

// Helper for clamping values
template<typename T>
static T Clamp(T v, T lo, T hi) { return (v < lo) ? lo : (v > hi) ? hi : v; }

//=====================================================================================================================
// Configurable constants
//=====================================================================================================================

// Layout
static const float kControlWidthFraction = 0.45f;   // How much of row width the control takes (vs label)
static const float kLabelRightPadding = 16.0f;      // Padding between label text and value
static const float kLabelValueButtonValueWidth = 60.0f;  // Width of value column in LabelValueButton

// Color palette (modernized)
static const ImU32 kAccentColor = IM_COL32(74, 144, 184, 255);           // #4A90B8 - professional blue
static const ImU32 kAccentColorHovered = IM_COL32(64, 134, 174, 255);    // Slightly darker
static const ImU32 kAccentColorActive = IM_COL32(54, 124, 164, 255);     // Even darker
static const ImU32 kTextPrimary = IM_COL32(44, 62, 80, 255);             // #2C3E50 - dark blue-gray
static const ImU32 kTextSecondary = IM_COL32(108, 122, 138, 255);        // #6C7A8A - medium gray

// Settings block styling
static const ImVec4 kSettingsBlockBgColor = ImVec4(0.91f, 0.91f, 0.93f, 1.0f);  // #E8E8EC warm light gray
static const float kSettingsBlockRounding = 4.0f;
static const ImVec2 kSettingsBlockPadding = ImVec2(8.0f, 6.0f);

// Section header styling
static const ImVec4 kSectionHeaderBgColor = ImVec4(0.35f, 0.42f, 0.48f, 1.0f);  // #5A6A7A softer blue-gray
static const ImVec4 kSectionHeaderTextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
static const float kSectionHeaderRounding = 3.0f;

// Uber section header (for parent sections containing sub-sections)
static const float kUberSectionHeaderColorR = 0.24f;  // #3D6B8A teal/dark cyan
static const float kUberSectionHeaderColorG = 0.42f;
static const float kUberSectionHeaderColorB = 0.54f;

// Text colors
static const ImVec4 kInfoValueTextColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);      // Dimmed info values
static const ImVec4 kThumbnailTitleColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);     // Black title text
static const ImVec4 kThumbnailInfoColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);      // Dimmed info text

// Combo/dropdown styling
static const ImVec4 kComboPopupBgColor = ImVec4(0.95f, 0.95f, 0.97f, 1.0f);    // Light popup background

// Slider styling (modernized)
static const float kSliderTrackHeight = 6.0f;                                   // Track height in pixels
static const float kSliderGrabRadius = 8.0f;                                    // Grab handle radius
static const ImU32 kSliderTrackBgColor = IM_COL32(200, 203, 210, 255);         // Light gray unfilled
static const ImU32 kSliderTrackFillColor = IM_COL32(74, 144, 184, 255);        // Accent color filled
static const ImU32 kSliderGrabColor = IM_COL32(74, 144, 184, 255);             // Accent color grab
static const ImU32 kSliderGrabHoveredColor = IM_COL32(64, 134, 174, 255);      // Darker on hover
static const ImU32 kSliderGrabActiveColor = IM_COL32(54, 124, 164, 255);       // Even darker when dragging

// Checkbox styling (modernized)
static const float kCheckboxSize = 18.0f;                                       // Checkbox size in pixels
static const ImU32 kCheckboxBorderColor = IM_COL32(150, 155, 165, 255);        // Border when unchecked
static const ImU32 kCheckboxFillColor = IM_COL32(74, 144, 184, 255);           // Fill when checked
static const ImU32 kCheckboxCheckColor = IM_COL32(255, 255, 255, 255);         // Checkmark color

// Thumbnail button styling
static const ImVec4 kThumbnailButtonBgColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);        // Transparent
static const ImVec4 kThumbnailButtonHoverColor = ImVec4(0.3f, 0.3f, 0.3f, 0.3f);     // Slight highlight
static const ImVec4 kThumbnailButtonActiveColor = ImVec4(0.2f, 0.2f, 0.2f, 0.3f);    // Darker when pressed
static const ImVec4 kThumbnailPlaceholderColor = ImVec4(0.5f, 0.5f, 0.5f, 0.5f);     // Gray placeholder

// Row spacing
static const float kRowExtraSpacing = 2.0f;  // Extra vertical spacing between rows

//=====================================================================================================================
// State counters (reset each frame)
//=====================================================================================================================
static int sSettingsBlockCounter = 0;
static int sSectionHeaderCounter = 0;

namespace SettingsWidgets {

//======================================================================================================================
float GetControlWidthFraction()
{
    return kControlWidthFraction;
}

//======================================================================================================================
// Helper to render a right-aligned label in the label column
static void RenderRightAlignedLabel(const char* label, float labelWidth)
{
    ImGui::AlignTextToFramePadding();
    float textWidth = ImGui::CalcTextSize(label).x;
    float startX = ImGui::GetCursorPosX();

    // Position text so it ends at labelWidth - padding
    float textX = startX + labelWidth - textWidth - kLabelRightPadding;
    if (textX > startX)
        ImGui::SetCursorPosX(textX);

    ImGui::TextUnformatted(label);

    // Position next element at labelWidth from start
    ImGui::SameLine();
    ImGui::SetCursorPosX(startX + labelWidth);
}

//======================================================================================================================
void SectionHeader(const char* title)
{
    // Colored button-like header (not clickable, just visual)
    ImGui::PushStyleColor(ImGuiCol_Button, kSectionHeaderBgColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, kSectionHeaderBgColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, kSectionHeaderBgColor);
    ImGui::PushStyleColor(ImGuiCol_Text, kSectionHeaderTextColor);

    // Left-align text in button, add rounded corners
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, kSectionHeaderRounding);

    // Use unique ID to prevent conflicts when same title is used multiple times
    ImGui::PushID(sSectionHeaderCounter++);
    ImGui::Button(title, ImVec2(-1, 0));
    ImGui::PopID();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(4);

    // Add extra spacing after section header
    ImGui::Spacing();
}

//======================================================================================================================
void SectionHeaderColored(const char* title, float r, float g, float b)
{
    // Background color from parameters
    ImVec4 bgColor(r, g, b, 1.0f);

    // Calculate perceived luminance to determine text color
    // Using standard luminance formula: 0.299*R + 0.587*G + 0.114*B
    float luminance = 0.299f * r + 0.587f * g + 0.114f * b;

    // Use light text on dark backgrounds, dark text on light backgrounds
    ImVec4 textColor = (luminance > 0.5f) ? ImVec4(0.0f, 0.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    // Colored button-like header (not clickable, just visual)
    ImGui::PushStyleColor(ImGuiCol_Button, bgColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bgColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, bgColor);
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);

    // Left-align text in button, add rounded corners
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, kSectionHeaderRounding);

    // Use unique ID to prevent conflicts when same title is used multiple times
    ImGui::PushID(sSectionHeaderCounter++);
    ImGui::Button(title, ImVec2(-1, 0));
    ImGui::PopID();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(4);

    // Add extra spacing after section header
    ImGui::Spacing();
}

//======================================================================================================================
void UberSectionHeader(const char* title)
{
    SectionHeaderColored(title, kUberSectionHeaderColorR, kUberSectionHeaderColorG, kUberSectionHeaderColorB);
}

//======================================================================================================================
bool BeginSettingsBlock()
{
    // Push styling for the settings block
    ImGui::PushStyleColor(ImGuiCol_ChildBg, kSettingsBlockBgColor);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, kSettingsBlockRounding);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, kSettingsBlockPadding);

    // Generate unique ID for this settings block
    char id[32];
    snprintf(id, sizeof(id), "SettingsBlock%d", sSettingsBlockCounter++);

    // Begin a child region with auto-sizing height
    ImGui::BeginChild(id, ImVec2(-1, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border);

    return true;
}

//======================================================================================================================
void EndSettingsBlock()
{
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(1);
}

//======================================================================================================================
bool Checkbox(const char* label, bool& value)
{
    float availWidth = ImGui::GetContentRegionAvail().x;
    float controlWidth = availWidth * kControlWidthFraction;
    float labelWidth = availWidth - controlWidth;

    // Right-aligned label
    RenderRightAlignedLabel(label, labelWidth);

    // Custom checkbox drawing
    ImGui::PushID(label);

    // Scale sizes by font scale for large screens
    float scale = UIHelpers::GetFontScale();

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    float size = kCheckboxSize * scale;
    float rounding = 3.0f * scale;
    float lineWidth = 2.0f * scale;
    ImVec2 checkboxMin = pos;
    ImVec2 checkboxMax = ImVec2(pos.x + size, pos.y + size);

    // Invisible button for interaction
    bool changed = ImGui::InvisibleButton("##check", ImVec2(size, size));
    if (changed)
        value = !value;

    bool hovered = ImGui::IsItemHovered();

    // Draw checkbox background
    if (value)
    {
        // Filled when checked
        ImU32 fillColor = hovered ? kAccentColorHovered : kCheckboxFillColor;
        drawList->AddRectFilled(checkboxMin, checkboxMax, fillColor, rounding);

        // Draw checkmark
        float pad = size * 0.2f;
        ImVec2 p1(checkboxMin.x + pad, checkboxMin.y + size * 0.5f);
        ImVec2 p2(checkboxMin.x + size * 0.4f, checkboxMax.y - pad);
        ImVec2 p3(checkboxMax.x - pad, checkboxMin.y + pad);
        drawList->AddPolyline(&p1, 1, kCheckboxCheckColor, 0, lineWidth);
        drawList->AddLine(p1, p2, kCheckboxCheckColor, lineWidth);
        drawList->AddLine(p2, p3, kCheckboxCheckColor, lineWidth);
    }
    else
    {
        // Empty outline when unchecked
        ImU32 borderColor = hovered ? kAccentColor : kCheckboxBorderColor;
        drawList->AddRect(checkboxMin, checkboxMax, borderColor, rounding, 0, 1.5f * scale);
    }

    ImGui::PopID();

    // Add row spacing
    ImGui::Dummy(ImVec2(0, kRowExtraSpacing));

    return changed;
}

//======================================================================================================================
// Helper for custom slider rendering - slider takes full controlWidth
static bool CustomSliderBehavior(const char* label, float& value, float min, float max, float controlWidth)
{
    ImGui::PushID(label);

    // Scale sizes by font scale for large screens
    float scale = UIHelpers::GetFontScale();

    // Slider takes full control width
    float sliderWidth = controlWidth;
    float trackHeight = kSliderTrackHeight * scale;
    float grabRadius = kSliderGrabRadius * scale;

    // Get frame height for vertical centering
    float frameHeight = ImGui::GetFrameHeight();

    // Get position for slider track
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Calculate track position (vertically centered)
    float trackY = pos.y + (frameHeight - trackHeight) * 0.5f;
    float trackRadius = trackHeight * 0.5f;

    ImVec2 trackMin(pos.x, trackY);
    ImVec2 trackMax(pos.x + sliderWidth, trackY + trackHeight);

    // Calculate normalized value and grab position
    float t = (max > min) ? (value - min) / (max - min) : 0.0f;
    t = Clamp(t, 0.0f, 1.0f);
    float grabX = pos.x + grabRadius + t * (sliderWidth - grabRadius * 2.0f);
    float grabY = trackY + trackHeight * 0.5f;

    // Invisible button for slider interaction
    ImGui::InvisibleButton("##slider", ImVec2(sliderWidth, frameHeight));
    bool hovered = ImGui::IsItemHovered();
    bool active = ImGui::IsItemActive();

    // Handle dragging
    bool changed = false;
    if (active && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        float mouseX = ImGui::GetMousePos().x;
        float newT = (mouseX - pos.x - grabRadius) / (sliderWidth - grabRadius * 2.0f);
        newT = Clamp(newT, 0.0f, 1.0f);
        float newValue = min + newT * (max - min);
        if (newValue != value)
        {
            value = newValue;
            changed = true;
        }
        t = newT;
        grabX = pos.x + grabRadius + t * (sliderWidth - grabRadius * 2.0f);
    }
    else if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        float mouseX = ImGui::GetMousePos().x;
        float newT = (mouseX - pos.x - grabRadius) / (sliderWidth - grabRadius * 2.0f);
        newT = Clamp(newT, 0.0f, 1.0f);
        value = min + newT * (max - min);
        changed = true;
        t = newT;
        grabX = pos.x + grabRadius + t * (sliderWidth - grabRadius * 2.0f);
    }

    // Draw unfilled track (full length, rounded)
    drawList->AddRectFilled(trackMin, trackMax, kSliderTrackBgColor, trackRadius);

    // Draw filled track (from left to grab position)
    if (t > 0.0f)
    {
        ImVec2 fillMax(grabX, trackY + trackHeight);
        drawList->AddRectFilled(trackMin, fillMax, kSliderTrackFillColor, trackRadius);
    }

    // Draw grab handle (circle)
    ImU32 grabColor = active ? kSliderGrabActiveColor :
                      (hovered ? kSliderGrabHoveredColor : kSliderGrabColor);
    drawList->AddCircleFilled(ImVec2(grabX, grabY), grabRadius, grabColor);

    // Draw subtle border on grab handle
    drawList->AddCircle(ImVec2(grabX, grabY), grabRadius, IM_COL32(0, 0, 0, 40), 0, 1.0f);

    ImGui::PopID();

    // Add row spacing
    ImGui::Dummy(ImVec2(0, kRowExtraSpacing));

    return changed;
}

//======================================================================================================================
bool SliderFloat(const char* label, float& value, float min, float max, const char* format)
{
    float availWidth = ImGui::GetContentRegionAvail().x;
    float controlWidth = availWidth * kControlWidthFraction;
    float labelWidth = availWidth - controlWidth;

    // Format label with value: "Label (value)"
    char labelWithValue[256];
    char valueBuf[64];
    snprintf(valueBuf, sizeof(valueBuf), format, value);
    snprintf(labelWithValue, sizeof(labelWithValue), "%s (%s)", label, valueBuf);

    // Right-aligned label (includes value)
    RenderRightAlignedLabel(labelWithValue, labelWidth);

    return CustomSliderBehavior(label, value, min, max, controlWidth);
}

//======================================================================================================================
bool SliderFloatPower(const char* label, float& value, float min, float max, float power, const char* format)
{
    float availWidth = ImGui::GetContentRegionAvail().x;
    float controlWidth = availWidth * kControlWidthFraction;
    float labelWidth = availWidth - controlWidth;

    // Format label with value: "Label (value)"
    char labelWithValue[256];
    char valueBuf[64];
    snprintf(valueBuf, sizeof(valueBuf), format, value);
    snprintf(labelWithValue, sizeof(labelWithValue), "%s (%s)", label, valueBuf);

    // Right-aligned label (includes value)
    RenderRightAlignedLabel(labelWithValue, labelWidth);

    // Convert actual value to normalized slider position using inverse power
    // This gives finer control at lower values when power > 1
    float range = max - min;
    float normalizedValue = (range > 0.0f) ? (value - min) / range : 0.0f;
    normalizedValue = Clamp(normalizedValue, 0.0f, 1.0f);
    float sliderFrac = powf(normalizedValue, 1.0f / power);

    // Use slider on the transformed value (0-1 range)
    float sliderMin = 0.0f;
    float sliderMax = 1.0f;
    bool changed = CustomSliderBehavior(label, sliderFrac, sliderMin, sliderMax, controlWidth);

    if (changed)
    {
        // Convert slider position back to actual value using power
        sliderFrac = Clamp(sliderFrac, 0.0f, 1.0f);
        float newNormalized = powf(sliderFrac, power);
        value = min + newNormalized * range;
        value = Clamp(value, min, max);
    }

    return changed;
}

//======================================================================================================================
bool SliderInt(const char* label, int& value, int min, int max)
{
    float availWidth = ImGui::GetContentRegionAvail().x;
    float controlWidth = availWidth * kControlWidthFraction;
    float labelWidth = availWidth - controlWidth;

    // Format label with value: "Label (value)"
    char labelWithValue[256];
    snprintf(labelWithValue, sizeof(labelWithValue), "%s (%d)", label, value);

    // Right-aligned label (includes value)
    RenderRightAlignedLabel(labelWithValue, labelWidth);

    // Convert to float for the slider behavior
    float floatValue = (float)value;
    bool changed = CustomSliderBehavior(label, floatValue, (float)min, (float)max, controlWidth);

    if (changed)
    {
        // Round to nearest integer
        value = (int)(floatValue + 0.5f);
        // Clamp to range
        if (value < min) value = min;
        if (value > max) value = max;
    }

    return changed;
}

//======================================================================================================================
bool Combo(const char* label, int& value, const char* const* items, int itemCount)
{
    float availWidth = ImGui::GetContentRegionAvail().x;
    float controlWidth = availWidth * kControlWidthFraction;
    float labelWidth = availWidth - controlWidth;

    // Right-aligned label
    RenderRightAlignedLabel(label, labelWidth);

    // Combo box on the right
    ImGui::PushID(label);
    ImGui::SetNextItemWidth(controlWidth);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, kComboPopupBgColor);

    bool changed = false;
    if (ImGui::BeginCombo("##combo", items[value]))
    {
        for (int i = 0; i < itemCount; ++i)
        {
            bool isSelected = (value == i);
            if (ImGui::Selectable(items[i], isSelected))
            {
                value = i;
                changed = true;
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::PopStyleColor();
    ImGui::PopID();

    // Add row spacing
    ImGui::Dummy(ImVec2(0, kRowExtraSpacing));

    return changed;
}

//======================================================================================================================
bool Combo(const char* label, int& value, const std::vector<std::string>& items)
{
    float availWidth = ImGui::GetContentRegionAvail().x;
    float controlWidth = availWidth * kControlWidthFraction;
    float labelWidth = availWidth - controlWidth;

    // Right-aligned label
    RenderRightAlignedLabel(label, labelWidth);

    // Combo box on the right
    ImGui::PushID(label);
    ImGui::SetNextItemWidth(controlWidth);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, kComboPopupBgColor);

    bool changed = false;
    int safeValue = (value >= 0 && value < (int)items.size()) ? value : 0;
    const char* preview = items.empty() ? "" : items[safeValue].c_str();

    if (ImGui::BeginCombo("##combo", preview))
    {
        for (int i = 0; i < (int)items.size(); ++i)
        {
            bool isSelected = (value == i);
            if (ImGui::Selectable(items[i].c_str(), isSelected))
            {
                value = i;
                changed = true;
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::PopStyleColor();
    ImGui::PopID();

    // Add row spacing
    ImGui::Dummy(ImVec2(0, kRowExtraSpacing));

    return changed;
}

//======================================================================================================================
void InfoLabel(const char* label, const char* value)
{
    float availWidth = ImGui::GetContentRegionAvail().x;
    float controlWidth = availWidth * kControlWidthFraction;
    float labelWidth = availWidth - controlWidth;

    // Right-aligned label
    RenderRightAlignedLabel(label, labelWidth);

    // Value on the right (dimmed)
    ImGui::PushStyleColor(ImGuiCol_Text, kInfoValueTextColor);
    ImGui::TextUnformatted(value);
    ImGui::PopStyleColor();

    // Add row spacing
    ImGui::Dummy(ImVec2(0, kRowExtraSpacing));
}

//======================================================================================================================
void InfoLabel(const char* label, const std::string& value)
{
    InfoLabel(label, value.c_str());
}

//======================================================================================================================
void CenteredLabel(const char* text)
{
    float availWidth = ImGui::GetContentRegionAvail().x;
    ImVec2 textSize = ImGui::CalcTextSize(text);
    float startX = (availWidth - textSize.x) * 0.5f;
    if (startX > 0)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + startX);
    ImGui::TextUnformatted(text);
    ImGui::Dummy(ImVec2(0, kRowExtraSpacing));
}

//======================================================================================================================
void InfoText(const char* text)
{
    float availWidth = ImGui::GetContentRegionAvail().x;
    float controlWidth = availWidth * kControlWidthFraction;
    float labelWidth = availWidth - controlWidth;

    // Add indent to match other controls
    ImGui::Indent(labelWidth);

    // Display dimmed text
    ImGui::PushStyleColor(ImGuiCol_Text, kInfoValueTextColor);
    ImGui::TextUnformatted(text);
    ImGui::PopStyleColor();

    ImGui::Unindent(labelWidth);

    // Add row spacing
    ImGui::Dummy(ImVec2(0, kRowExtraSpacing));
}

//======================================================================================================================
void InfoTextWrapped(const char* text)
{
    // Display dimmed text that spans full width and wraps
    ImGui::PushStyleColor(ImGuiCol_Text, kInfoValueTextColor);
    ImGui::TextWrapped("%s", text);
    ImGui::PopStyleColor();

    // Add row spacing
    ImGui::Dummy(ImVec2(0, kRowExtraSpacing));
}

//======================================================================================================================
bool LabelValueButton(const char* label, const char* value, const char* buttonText)
{
    float availWidth = ImGui::GetContentRegionAvail().x;
    float startX = ImGui::GetCursorPosX();

    // Scale sizes by font scale for large screens
    float scale = UIHelpers::GetFontScale();

    // Use same layout as other widgets for label column
    float labelWidth = availWidth * (1.0f - kControlWidthFraction);
    float controlWidth = availWidth * kControlWidthFraction;

    // Split control area: value (small) + button (rest)
    float valueWidth = kLabelValueButtonValueWidth * scale;
    float buttonWidth = controlWidth - valueWidth - ImGui::GetStyle().ItemSpacing.x;

    // Right-aligned label
    RenderRightAlignedLabel(label, labelWidth);

    // Value
    ImGui::TextUnformatted(value);

    // Button
    ImGui::SameLine();
    ImGui::SetCursorPosX(startX + labelWidth + valueWidth);
    ImGui::PushID(label);
    bool clicked = ImGui::Button(buttonText, ImVec2(buttonWidth, 0));
    ImGui::PopID();

    return clicked;
}

//======================================================================================================================
bool LabelButton(const char* label, const char* buttonText)
{
    float availWidth = ImGui::GetContentRegionAvail().x;
    float startX = ImGui::GetCursorPosX();

    // Use same layout as other widgets for label column
    float labelWidth = availWidth * (1.0f - kControlWidthFraction);
    float controlWidth = availWidth * kControlWidthFraction;

    // Right-aligned label
    RenderRightAlignedLabel(label, labelWidth);

    // Button fills the control width, showing the value as its text
    ImGui::PushID(label);
    bool clicked = ImGui::Button(buttonText, ImVec2(controlWidth, 0));
    ImGui::PopID();

    // Add row spacing
    ImGui::Dummy(ImVec2(0, kRowExtraSpacing));

    return clicked;
}

//======================================================================================================================
bool Button(const char* label)
{
    return ImGui::Button(label, ImVec2(-1, 0));
}

//======================================================================================================================
bool ButtonSized(const char* label, float width, float height)
{
    return ImGui::Button(label, ImVec2(width, height));
}

//======================================================================================================================
bool ThumbnailButton(Texture* tex, const char* title, const char* info,
                     float imageHeight, float* outX, float* outY,
                     float* outW, float* outH)
{
    bool clicked = false;
    float scale = UIHelpers::GetFontScale();
    float imgW = imageHeight;  // Start with square assumption

    // Store position before drawing
    ImVec2 startPos = ImGui::GetCursorScreenPos();

    // Draw thumbnail if available
    if (tex)
    {
        uint32_t texW = tex->GetWidth();
        uint32_t texH = tex->GetHeight();
        GLuint texID = tex->GetTextureID();

        if (texW > 0 && texH > 0)
        {
            // Calculate display size maintaining aspect ratio
            float ar = (float)texW / (float)texH;
            imgW = imageHeight * ar;

            ImGui::PushStyleColor(ImGuiCol_Button, kThumbnailButtonBgColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, kThumbnailButtonHoverColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, kThumbnailButtonActiveColor);

            if (ImGui::ImageButton("thumb", (ImTextureID)(intptr_t)texID,
                    ImVec2(imgW, imageHeight)))
            {
                clicked = true;
            }
            ImGui::PopStyleColor(3);
        }
        else
        {
            // Invalid texture - draw placeholder
            ImGui::Dummy(ImVec2(imageHeight, imageHeight));
            imgW = imageHeight;
        }
    }
    else
    {
        // No texture - draw placeholder area
        ImGui::PushStyleColor(ImGuiCol_Button, kThumbnailPlaceholderColor);
        if (ImGui::Button("##placeholder", ImVec2(imageHeight, imageHeight)))
        {
            clicked = true;
        }
        ImGui::PopStyleColor();
        imgW = imageHeight;
    }

    // Title and info text next to thumbnail
    ImGui::SameLine();

    ImGui::BeginGroup();
    {
        // Title text
        ImGui::PushStyleColor(ImGuiCol_Text, kThumbnailTitleColor);
        ImGui::TextUnformatted(title ? title : "");
        ImGui::PopStyleColor();

        // Info in dimmed text
        if (info && info[0] != '\0')
        {
            ImGui::PushStyleColor(ImGuiCol_Text, kThumbnailInfoColor);
            ImGui::TextWrapped("%s", info);
            ImGui::PopStyleColor();
        }
    }
    ImGui::EndGroup();

    // Output button rect if requested (for GetImageButtonInfo)
    if (outX) *outX = startPos.x;
    if (outY) *outY = startPos.y;
    if (outW) *outW = imgW;
    if (outH) *outH = imageHeight;

    return clicked;
}

//======================================================================================================================
void Spacing()
{
    ImGui::Spacing();
}

//======================================================================================================================
void ResetFrameState()
{
    sSettingsBlockCounter = 0;
    sSectionHeaderCounter = 0;
}

} // namespace SettingsWidgets
