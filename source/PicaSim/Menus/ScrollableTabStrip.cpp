#include "ScrollableTabStrip.h"
#include "UIHelpers.h"
#include "PicaStyle.h"
#include "Helpers.h"
#include "imgui.h"

static const float kTabPaddingX = 10.0f;  // horizontal padding inside each tab (at 720p)
static const float kTabSpacing = 2.0f;    // gap between tabs (at 720p)

//======================================================================================================================
void ScrollableTabStrip::Render(const char* id, const char** labels, int count, int& selectedTab, float height)
{
    float scale = UIHelpers::GetFontScale();
    float tabPadX = kTabPaddingX * scale;
    float tabSpacing = kTabSpacing * scale;
    float rounding = PicaStyle::Common::CornerRadius * scale;

    ImGui::PushID(id);

    // Measure total tab width
    float totalWidth = 0.0f;
    for (int i = 0; i < count; ++i)
    {
        float textW = ImGui::CalcTextSize(labels[i]).x;
        totalWidth += textW + tabPadX * 2.0f;
        if (i < count - 1)
            totalWidth += tabSpacing;
    }
    mTotalTabsWidth = totalWidth;

    // Available region: from current cursor to end of content region
    float regionStartX = ImGui::GetCursorScreenPos().x;
    float regionStartY = ImGui::GetCursorScreenPos().y;
    float availableWidth = ImGui::GetContentRegionAvail().x;
    float maxScroll = (totalWidth > availableWidth) ? (totalWidth - availableWidth) : 0.0f;

    // Auto-scroll selected tab into view when selection changes externally
    if (selectedTab != mLastSelectedTab && selectedTab >= 0 && selectedTab < count)
    {
        float tabStart = 0.0f;
        for (int i = 0; i < selectedTab; ++i)
        {
            tabStart += ImGui::CalcTextSize(labels[i]).x + tabPadX * 2.0f + tabSpacing;
        }
        float tabEnd = tabStart + ImGui::CalcTextSize(labels[selectedTab]).x + tabPadX * 2.0f;

        if (tabStart < mScrollOffset)
            mScrollOffset = tabStart;
        else if (tabEnd > mScrollOffset + availableWidth)
            mScrollOffset = tabEnd - availableWidth;

        mLastSelectedTab = selectedTab;
    }

    // Drag-to-scroll via ScrollHelper (axis 0 = horizontal)
    ImVec2 regionMin(regionStartX, regionStartY);
    ImVec2 regionMax(regionStartX + availableWidth, regionStartY + height);
    bool isHovered = ImGui::IsMouseHoveringRect(regionMin, regionMax);

    bool isDragging = mScrollHelper.ApplyDragScroll(id, 0, isHovered, mScrollOffset, maxScroll);

    // Track previous-frame drag state so we can suppress clicks on drag release
    bool wasDragging = mWasDragging;
    mWasDragging = isDragging;

    // Mouse wheel horizontal scrolling (desktop)
    if (isHovered && !isDragging)
    {
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f)
        {
            mScrollOffset -= wheel * height * 2.0f;
            mScrollOffset = ClampToRange(mScrollOffset, 0.0f, maxScroll);
        }
    }

    // Clamp scroll
    mScrollOffset = ClampToRange(mScrollOffset, 0.0f, maxScroll);

    // Draw tabs with clipping
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->PushClipRect(regionMin, regionMax, true);

    float tabX = regionStartX - mScrollOffset;
    bool tabClicked = false;
    int clickedTab = -1;

    for (int i = 0; i < count; ++i)
    {
        float textW = ImGui::CalcTextSize(labels[i]).x;
        float tabW = textW + tabPadX * 2.0f;
        ImVec2 tabMin(tabX, regionStartY);
        ImVec2 tabMax(tabX + tabW, regionStartY + height);

        // Skip if completely out of view
        if (tabMax.x < regionMin.x || tabMin.x > regionMax.x)
        {
            tabX += tabW + tabSpacing;
            continue;
        }

        bool isSelected = (i == selectedTab);
        bool isTabHovered = ImGui::IsMouseHoveringRect(tabMin, tabMax) && !isDragging;

        // Tab background colors
        ImU32 bgColor;
        if (isSelected)
            bgColor = ImGui::ColorConvertFloat4ToU32(PicaStyle::Menu::TabActive);
        else if (isTabHovered)
            bgColor = ImGui::ColorConvertFloat4ToU32(PicaStyle::Menu::TabHovered);
        else
            bgColor = ImGui::ColorConvertFloat4ToU32(PicaStyle::Menu::Tab);

        drawList->AddRectFilled(tabMin, tabMax, bgColor, rounding);

        // Tab text (centered)
        ImVec2 textSize = ImGui::CalcTextSize(labels[i]);
        float textX = tabX + (tabW - textSize.x) * 0.5f;
        float textY = regionStartY + (height - textSize.y) * 0.5f;
        ImU32 textColor = ImGui::ColorConvertFloat4ToU32(PicaStyle::Common::TextBlack);
        drawList->AddText(ImVec2(textX, textY), textColor, labels[i]);

        // Click detection: suppress on drag and on the frame after drag release
        if (!isDragging && !wasDragging && isTabHovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            tabClicked = true;
            clickedTab = i;
        }

        tabX += tabW + tabSpacing;
    }

    drawList->PopClipRect();

    // Handle tab click
    if (tabClicked && clickedTab >= 0)
    {
        selectedTab = clickedTab;
        mLastSelectedTab = selectedTab;
    }

    // Advance cursor past the tab strip
    ImGui::Dummy(ImVec2(availableWidth, height));

    ImGui::PopID();
}
