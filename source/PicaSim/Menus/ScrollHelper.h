#ifndef SCROLLHELPER_H
#define SCROLLHELPER_H

#include <string>
#include <unordered_map>
#include <cstdint>

class ScrollHelper
{
public:
    // For ImGui child windows — call right after BeginChild().
    // Vertical drag-scroll using ImGui's own scroll state.
    // Returns true if currently dragging.
    bool ApplyDragScroll(const char* childId);

    // General drag-scroll for any axis and any scroll region.
    // axis: 0 = horizontal, 1 = vertical
    // isHovered: whether the mouse is in the scrollable region
    // scrollOffset: current scroll position (read/write)
    // maxScroll: maximum scroll value
    // Returns true if currently dragging.
    bool ApplyDragScroll(const char* id, int axis, bool isHovered, float& scrollOffset, float maxScroll);

    // Call when resetting scroll position for a child window
    void ResetScroll(const char* childId);

    // Clear all state
    void ResetAll();

    // Call from drag-type widgets (sliders, combos, color pickers) when they
    // are actively being interacted with. This prevents drag-scrolling from
    // stealing input from these widgets.
    static void MarkEditWidgetActive();

private:
    // Returns true if a drag-type widget was active on the previous frame
    static bool WasEditWidgetActive();
    static int sEditWidgetActiveFrame;
    enum class DragState
    {
        IDLE,
        POTENTIAL_DRAG,
        DRAGGING,
        MOMENTUM
    };

    struct ScrollState
    {
        DragState state = DragState::IDLE;
        float pressPos = 0.0f;
        float lastMousePos = 0.0f;
        float velocity = 0.0f;
        float targetScroll = 0.0f;  // our own scroll tracking for MOMENTUM
        uint64_t lastCounter = 0;   // SDL_GetPerformanceCounter value

        void Reset()
        {
            state = DragState::IDLE;
            velocity = 0.0f;
            targetScroll = 0.0f;
            lastCounter = 0;
        }
    };

    std::unordered_map<std::string, ScrollState> mScrollStates;
};

#endif // SCROLLHELPER_H
