#ifndef SETTINGS_WIDGETS_H
#define SETTINGS_WIDGETS_H

#include <vector>
#include <string>

class Texture;

// ImGui-based settings widgets namespace
// These are simple helper functions for rendering settings controls
namespace SettingsWidgets {

    // Render a section header with colored background
    void SectionHeader(const char* title);

    // Render a section header with custom RGB background color (0-1 range)
    // Text color automatically switches between light/dark for visibility
    void SectionHeaderColored(const char* title, float r, float g, float b);

    // Render an "uber section" header for parent sections containing sub-sections
    // Uses a distinct color to stand out from regular section headers
    void UberSectionHeader(const char* title);

    // Begin/End a settings block with rounded background
    // Returns false if the block should be skipped (for collapsible sections)
    bool BeginSettingsBlock();
    void EndSettingsBlock();

    // Checkbox widget - returns true if value changed
    bool Checkbox(const char* label, bool& value);

    // Slider widgets - return true if value changed
    bool SliderFloat(const char* label, float& value, float min, float max,
                     const char* format = "%.3f");
    // Power slider - gives finer control at lower values when power > 1
    // Useful for values with wide ranges (e.g., 10-200 where most useful values are 10-50)
    bool SliderFloatPower(const char* label, float& value, float min, float max,
                          float power, const char* format = "%.3f");
    bool SliderInt(const char* label, int& value, int min, int max);

    // Enum selection using combo box - returns true if value changed
    bool Combo(const char* label, int& value, const char* const* items, int itemCount);
    bool Combo(const char* label, int& value, const std::vector<std::string>& items);

    // Info display (label + read-only value)
    void InfoLabel(const char* label, const char* value);
    void InfoLabel(const char* label, const std::string& value);

    // Centered label (text centered in the content area)
    void CenteredLabel(const char* text);

    // Display informational text (dimmed, for notes/hints)
    void InfoText(const char* text);

    // Display full-width wrapped text (spans the entire block, wraps long text)
    void InfoTextWrapped(const char* text);

    // Label + value + button on same row (for calibration UI)
    // Returns true if button clicked
    bool LabelValueButton(const char* label, const char* value, const char* buttonText);

    // Label + button with value text (button shows the current value, clicking selects new)
    // Returns true if button clicked
    bool LabelButton(const char* label, const char* buttonText);

    // Full-width button - returns true if clicked
    bool Button(const char* label);

    // Button with specific width - returns true if clicked
    bool ButtonSized(const char* label, float width, float height = 0);

    // Clickable thumbnail with title and info text
    // Returns true if clicked, sets outRect to button position for GetImageButtonInfo
    bool ThumbnailButton(Texture* tex, const char* title, const char* info,
                         float imageHeight, float* outX = nullptr, float* outY = nullptr,
                         float* outW = nullptr, float* outH = nullptr);

    // Spacing helper
    void Spacing();

    // Get the standard control width fraction for layout consistency
    float GetControlWidthFraction();

    // Reset frame state (call at start of each frame before rendering widgets)
    void ResetFrameState();

}

#endif
