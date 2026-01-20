#ifndef PICA_DIALOG_H
#define PICA_DIALOG_H

#include "Helpers.h"
#include <functional>

// Forward declarations
struct GameSettings;

// Background render callback type - called each frame to render content behind the dialog
using DialogBackgroundCallback = std::function<void()>;

//======================================================================================================================
// InGameDialog - Renders an ImGui dialog overlaid on the running game
// Handles animation (slide-in), button clicks, and optional ellipse overlays
//======================================================================================================================
class InGameDialog
{
public:
    // Dialog size as fraction of screen (0.0 - 1.0)
    InGameDialog(float widthFrac, float heightFrac);

    // Set the ellipse overlay position (fractions of screen, 0-1)
    // Set rx/ry to 0 to disable overlay
    void SetOverlay(float x, float y, float rx, float ry);

    // Set animation direction (true = slide from right, false = from left)
    void SetSlideFromRight(bool fromRight);

    // Run one frame of the dialog
    // Returns: -1 if no button clicked, 0/1/2 for button index
    // Sets shouldExit to true if Escape/Back pressed or quit requested
    int Update(float dt, const char* title, const char* text,
                          const char* button0, const char* button1 = nullptr, const char* button2 = nullptr,
                          bool* shouldExit = nullptr);

    // Check if the slide-in animation is complete enough to show buttons
    bool ShouldShowButtons() const { return mTotalTime > 0.2f; }

    // Reset animation state (call when changing pages in multi-page dialog)
    void ResetAnimation(bool slideFromRight);

    // Set optional background render callback (called each frame before dialog renders)
    void SetBackgroundCallback(DialogBackgroundCallback callback) { mBackgroundCallback = callback; }

private:
    float mWidthFrac;
    float mHeightFrac;

    // Overlay position (fractions of screen)
    float mOverlayX, mOverlayY, mOverlayRX, mOverlayRY;

    // Animation state
    float mOffsetFrac;      // Current offset as fraction of dialog width
    float mOffsetFracRate;  // Rate of change
    float mTotalTime;       // Time since dialog started
    bool mSlideFromRight;

    // Animation parameters
    static constexpr float SMOOTH_TIME = 0.12f;
    static constexpr float DAMPING_RATIO = 0.7f;

    // Optional background render callback
    DialogBackgroundCallback mBackgroundCallback;
};

//======================================================================================================================
// Convenience functions for common dialog patterns
//======================================================================================================================

// Show a simple in-game dialog with 1-3 buttons
// Returns button index (0, 1, 2) or -1 if cancelled
// Optional backgroundCallback is called each frame to render content behind the dialog
int ShowInGameDialog(
    float widthFrac, float heightFrac,
    const char* title, const char* text,
    const char* button0,
    const char* button1 = nullptr,
    const char* button2 = nullptr,
    DialogBackgroundCallback backgroundCallback = nullptr);

// Show the help overlay pages (Prev/OK/Next navigation)
void ShowHelpOverlays(const GameSettings& gameSettings);

// Show a simple dialog (not overlaid on game, uses background image)
// Returns button index (0, 1, 2) or -1 if cancelled
// Optional backgroundCallback is called each frame to render content behind the dialog
int ShowDialog(
    const char* title,
    const char* text,
    const char* button0,
    const char* button1 = nullptr,
    const char* button2 = nullptr,
    DialogBackgroundCallback backgroundCallback = nullptr);

#endif // PICA_DIALOG_H
