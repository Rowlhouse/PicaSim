#include "ScrollHelper.h"
#include "UIHelpers.h"
#include "Helpers.h"
#include "imgui.h"
#include "imgui_internal.h"

#include <SDL.h>
#include <cmath>

// Edit widget tracking - used to prevent drag-scrolling from stealing input
// from sliders, combos, and other value-editing widgets.
int ScrollHelper::sEditWidgetActiveFrame = -1;

void ScrollHelper::MarkEditWidgetActive()
{
    sEditWidgetActiveFrame = ImGui::GetFrameCount();
}

bool ScrollHelper::WasEditWidgetActive()
{
    // Check if a drag-type widget was active on the previous frame.
    // ApplyDragScroll runs BEFORE widgets process each frame, so the most
    // recent MarkEditWidgetActive() call is from the previous frame.
    return sEditWidgetActiveFrame == ImGui::GetFrameCount() - 1;
}

// Tunable constants
static const float kDeadZoneBase = 6.0f;       // pixels at 720p before drag starts
static const float kMomentumHalfLife = 0.15f;   // seconds for velocity to halve
static const float kMaxVelocityBase = 5000.0f;  // max pixels/sec at 720p
static const float kMinVelocity = 1.0f;         // cutoff velocity in pixels/sec
static const float kSmoothingTime = 0.05f;      // velocity smoothing time constant (seconds)

//======================================================================================================================
static float GetDt(uint64_t& lastCounter)
{
    uint64_t now = SDL_GetPerformanceCounter();
    float dt = 0.0f;
    if (lastCounter > 0)
        dt = (float)(now - lastCounter) / (float)SDL_GetPerformanceFrequency();
    lastCounter = now;
    return dt;
}

//======================================================================================================================
// General drag-scroll: works on any axis with caller-managed scroll offset.
//======================================================================================================================
bool ScrollHelper::ApplyDragScroll(const char* id, int axis, bool isHovered, float& scrollOffset, float maxScroll)
{
    ScrollState& s = mScrollStates[id];
    float scale = UIHelpers::GetFontScale();
    float deadZone = kDeadZoneBase * scale;
    float maxVelocity = kMaxVelocityBase * scale;

    float dt = GetDt(s.lastCounter);

    bool mouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    float mousePos = ImGui::GetIO().MousePos[axis];

    switch (s.state)
    {
    case DragState::IDLE:
        if (mouseDown && isHovered)
        {
            s.state = DragState::POTENTIAL_DRAG;
            s.pressPos = mousePos;
            s.lastMousePos = mousePos;
        }
        break;

    case DragState::POTENTIAL_DRAG:
        if (!mouseDown)
        {
            // Released within dead zone - was a tap/click
            s.state = DragState::IDLE;
        }
        else
        {
            float delta = mousePos - s.pressPos;
            if (std::fabs(delta) > deadZone)
            {
                // If a drag-type widget (slider, combo, color picker) is
                // actively being interacted with, let it handle the input
                // instead of scrolling. Buttons/headers don't call
                // MarkEditWidgetActive(), so they still allow drag scrolling.
                if (WasEditWidgetActive())
                {
                    s.state = DragState::IDLE;
                }
                else
                {
                    s.state = DragState::DRAGGING;
                    s.lastMousePos = mousePos;
                    s.velocity = 0.0f;
                    // Detach any widget that was tracking the press so releasing
                    // after a drag doesn't trigger a button click
                    ImGui::ClearActiveID();
                }
            }
        }
        break;

    case DragState::DRAGGING:
    {
        if (!mouseDown)
        {
            // Released - capture current scroll position and start momentum
            s.velocity = ClampToRange(s.velocity, -maxVelocity, maxVelocity);
            s.targetScroll = scrollOffset;
            s.state = (std::fabs(s.velocity) > kMinVelocity) ? DragState::MOMENTUM : DragState::IDLE;
            break;
        }

        float delta = mousePos - s.lastMousePos;
        s.lastMousePos = mousePos;

        // Apply scroll (negative because dragging in +dir should scroll backwards)
        scrollOffset = ClampToRange(scrollOffset - delta, 0.0f, maxScroll);

        // Update smoothed velocity via exponential moving average.
        // Every frame contributes, including zero-delta frames (which pull
        // velocity toward 0). This naturally handles: high refresh rates,
        // batched mouse events, and pause-before-release.
        if (dt > 0.0f)
        {
            float instantVelocity = -delta / dt;
            float alpha = 1.0f - std::exp(-dt / kSmoothingTime);
            s.velocity += alpha * (instantVelocity - s.velocity);
        }
        break;
    }

    case DragState::MOMENTUM:
    {
        // Mouse wheel stops momentum and lets ImGui handle wheel natively
        if (ImGui::GetIO().MouseWheel != 0.0f)
        {
            s.state = DragState::IDLE;
            s.velocity = 0.0f;
            break;
        }

        // Click during momentum starts a new drag immediately
        if (mouseDown && isHovered)
        {
            s.state = DragState::POTENTIAL_DRAG;
            s.velocity = 0.0f;
            s.pressPos = mousePos;
            s.lastMousePos = mousePos;
            break;
        }

        // Apply velocity using our own tracked scroll position.
        // We don't read the caller's scrollOffset here because for ImGui child
        // windows, BeginChild() may override the scroll position we set on the
        // previous frame.
        if (dt > 0.0f)
        {
            s.targetScroll += s.velocity * dt;
            s.targetScroll = ClampToRange(s.targetScroll, 0.0f, maxScroll);
            scrollOffset = s.targetScroll;

            // Stop if we hit an edge
            if ((s.targetScroll <= 0.0f && s.velocity < 0.0f) || (s.targetScroll >= maxScroll && s.velocity > 0.0f))
            {
                s.state = DragState::IDLE;
                s.velocity = 0.0f;
                break;
            }

            // Exponential decay
            s.velocity *= std::pow(0.5f, dt / kMomentumHalfLife);

            // Stop when velocity is negligible
            if (std::fabs(s.velocity) < kMinVelocity)
            {
                s.state = DragState::IDLE;
                s.velocity = 0.0f;
            }
        }
        break;
    }
    }

    return s.state == DragState::DRAGGING;
}

//======================================================================================================================
// Convenience wrapper for ImGui child windows (vertical drag-scroll).
//======================================================================================================================
bool ScrollHelper::ApplyDragScroll(const char* childId)
{
    bool isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
    float scrollOffset = ImGui::GetScrollY();
    float maxScroll = ImGui::GetScrollMaxY();

    bool result = ApplyDragScroll(childId, 1, isHovered, scrollOffset, maxScroll);

    ImGui::SetScrollY(scrollOffset);
    return result;
}

//======================================================================================================================
void ScrollHelper::ResetScroll(const char* childId)
{
    auto it = mScrollStates.find(childId);
    if (it != mScrollStates.end())
        it->second.Reset();
}

//======================================================================================================================
void ScrollHelper::ResetAll()
{
    mScrollStates.clear();
}
