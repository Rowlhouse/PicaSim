#include "PicaDialog.h"
#include "UIHelpers.h"
#include "../GameSettings.h"
#include "../PicaStrings.h"
#include "RenderManager.h"
#include "Trace.h"

#include "../../Platform/S3ECompat.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <cmath>

// Forward declarations from Graphics.cpp
void IwGxSwapBuffers();

//======================================================================================================================
// Helper function to draw an ellipse overlay
//======================================================================================================================
static void DrawEllipseOverlay(float x, float y, float rx, float ry, int screenWidth, int screenHeight)
{
    if (rx <= 0 || ry <= 0)
        return;

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    float cx = x * screenWidth;
    float cy = y * screenHeight;
    float radiusX = rx * screenWidth;
    float radiusY = ry * screenHeight;

    // Draw ellipse as a polygon (ImGui doesn't have AddEllipseFilled)
    const int numSegments = 64;
    ImVec2 points[numSegments];
    for (int i = 0; i < numSegments; ++i)
    {
        float angle = (float)i / numSegments * 2.0f * PI;
        points[i].x = cx + cosf(angle) * radiusX;
        points[i].y = cy + sinf(angle) * radiusY;
    }
    drawList->AddConvexPolyFilled(points, numSegments, IM_COL32(255, 204, 204, 96));
}

//======================================================================================================================
// Helper function to render dialog content using ImGui
// Returns button index (0, 1, or 2) or -1 if no button clicked
//======================================================================================================================
static int RenderDialogFrame(
    float dialogX, float dialogY, float dialogW, float dialogH,
    const char* title, const char* text,
    const char* button0, const char* button1, const char* button2,
    bool showButtons, float scale)
{
    int buttonClicked = -1;

    // Light theme styling for dialog (semi-transparent)
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.90f, 0.92f, 0.95f, 0.94f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.85f, 0.88f, 0.92f, 0.90f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.75f, 0.78f, 0.85f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.88f, 0.95f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.65f, 0.68f, 0.75f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.6f, 0.5f));

    ImGui::SetNextWindowPos(ImVec2(dialogX, dialogY));
    ImGui::SetNextWindowSize(ImVec2(dialogW, dialogH));
    ImGui::Begin("InGameDialog", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

    // Title
    float titleFontSize = 20.0f * scale;
    ImGui::PushFont(UIHelpers::GetFont());
    ImGui::SetWindowFontScale(titleFontSize / ImGui::GetFontSize());
    ImGui::TextWrapped("%s", title);
    ImGui::PopFont();
    ImGui::SetWindowFontScale(1.0f);

    ImGui::Separator();
    ImGui::Spacing();

    // Count buttons and calculate button dimensions
    int numButtons = 0;
    if (button0) numButtons++;
    if (button1) numButtons++;
    if (button2) numButtons++;

    float buttonH = 32.0f * scale;
    float bottomPadding = 12.0f * scale;
    float buttonAreaHeight = showButtons && numButtons > 0 ? (buttonH + bottomPadding) : 0.0f;

    // Text area (scrollable) - fills space between title and buttons
    float textAreaHeight = dialogH - ImGui::GetCursorPosY() - buttonAreaHeight - 10.0f * scale;
    ImGui::BeginChild("TextArea", ImVec2(-1, textAreaHeight), false);
    ImGui::TextWrapped("%s", text);
    ImGui::EndChild();

    // Buttons at bottom - position them explicitly
    if (showButtons && numButtons > 0)
    {
        float buttonW = (dialogW - 40.0f * scale) / numButtons - 10.0f * scale;

        // Position buttons near the bottom with padding
        float buttonY = dialogH - buttonH - bottomPadding;
        ImGui::SetCursorPosY(buttonY);

        // Center the button row horizontally
        float totalButtonWidth = numButtons * buttonW + (numButtons - 1) * 10.0f * scale;
        float startX = (dialogW - totalButtonWidth) / 2.0f;
        ImGui::SetCursorPosX(startX);

        if (button0)
        {
            if (ImGui::Button(button0, ImVec2(buttonW, buttonH)))
                buttonClicked = 0;
            if (button1 || button2)
                ImGui::SameLine(0, 10.0f * scale);
        }
        if (button1)
        {
            if (ImGui::Button(button1, ImVec2(buttonW, buttonH)))
                buttonClicked = 1;
            if (button2)
                ImGui::SameLine(0, 10.0f * scale);
        }
        if (button2)
        {
            if (ImGui::Button(button2, ImVec2(buttonW, buttonH)))
                buttonClicked = 2;
        }
    }

    ImGui::End();
    ImGui::PopStyleColor(7);

    return buttonClicked;
}

//======================================================================================================================
// InGameDialog implementation
//======================================================================================================================

InGameDialog::InGameDialog(float widthFrac, float heightFrac)
    : mWidthFrac(widthFrac)
    , mHeightFrac(heightFrac)
    , mOverlayX(0), mOverlayY(0), mOverlayRX(0), mOverlayRY(0)
    , mOffsetFrac(1.5f)
    , mOffsetFracRate(-1.5f / SMOOTH_TIME)
    , mTotalTime(0.0f)
    , mSlideFromRight(true)
{
}

void InGameDialog::SetOverlay(float x, float y, float rx, float ry)
{
    mOverlayX = x;
    mOverlayY = y;
    mOverlayRX = rx;
    mOverlayRY = ry;
}

void InGameDialog::SetSlideFromRight(bool fromRight)
{
    mSlideFromRight = fromRight;
}

void InGameDialog::ResetAnimation(bool slideFromRight)
{
    mSlideFromRight = slideFromRight;
    mOffsetFrac = slideFromRight ? 1.5f : -1.5f;
    mOffsetFracRate = -mOffsetFrac / SMOOTH_TIME;
    mTotalTime = 0.0f;
}

int InGameDialog::Update(float dt, const char* title, const char* text,
                                                  const char* button0, const char* button1, const char* button2,
                                                  bool* shouldExit)
{
    mTotalTime += dt;

    // Update animation
    SmoothSpringDamper(mOffsetFrac, mOffsetFracRate, dt, 0.0f, SMOOTH_TIME, DAMPING_RATIO);

    // Get screen dimensions
    int screenWidth = s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
    int screenHeight = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);
    float scale = UIHelpers::GetFontScale();

    // Calculate dialog dimensions
    float dialogW = screenWidth * mWidthFrac;
    float dialogH = screenHeight * mHeightFrac;
    float finalX = (screenWidth - dialogW) / 2.0f;
    float finalY = (screenHeight - dialogH) / 2.0f;

    // Render background content
    if (mBackgroundCallback)
    {
        // Background callback handles its own ImGui frame setup and rendering
        // It should NOT call ImGui::Render() or swap buffers
        mBackgroundCallback();
        // Dialog will render on top in the same ImGui frame
    }
    else
    {
        // Default: render the game, then start ImGui frame
        if (RenderManager::GetExists())
        {
            RenderManager::GetInstance().RenderWithoutSwap();
        }

        // Begin ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        UIHelpers::ApplyFontScale();
    }

    // Draw ellipse overlay if set
    DrawEllipseOverlay(mOverlayX, mOverlayY, mOverlayRX, mOverlayRY, screenWidth, screenHeight);

    // Calculate dialog position with animation offset
    float dialogX = finalX + mOffsetFrac * dialogW;
    float dialogY = finalY;

    // Render the dialog
    int buttonClicked = RenderDialogFrame(
        dialogX, dialogY, dialogW, dialogH,
        title, text, button0, button1, button2,
        ShouldShowButtons(), scale);

    // End ImGui frame and render
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    IwGxSwapBuffers();

    // Check for exit conditions
    if (shouldExit)
    {
        *shouldExit = s3eDeviceCheckQuitRequest() ||
                                    (s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED) ||
                                    (s3eKeyboardGetState(s3eKeyEsc) & S3E_KEY_STATE_PRESSED);
    }

    return buttonClicked;
}

//======================================================================================================================
// Convenience functions
//======================================================================================================================

int ShowInGameDialog(float widthFrac, float heightFrac,
                                          const char* title, const char* text,
                                          const char* button0, const char* button1, const char* button2,
                                          DialogBackgroundCallback backgroundCallback)
{
    TRACE("ShowInGameDialog: %s", text);

    InGameDialog dialog(widthFrac, heightFrac);
    dialog.SetBackgroundCallback(backgroundCallback);

    uint64 lastTime = Timer::GetMilliseconds();
    int buttonClicked = -1;

    while (buttonClicked < 0)
    {
        s3eDeviceBacklightOn();
        s3eKeyboardUpdate();
        s3ePointerUpdate();
        s3eDeviceYield(1);

        uint64 currentTime = Timer::GetMilliseconds();
        int32 updateTime = currentTime > lastTime ?
            MIN((int32)(currentTime - lastTime), 100) : 0;
        lastTime = currentTime;
        float dt = updateTime * 0.001f;

        bool shouldExit = false;
        buttonClicked = dialog.Update(dt, title, text, button0, button1, button2, &shouldExit);

        if (shouldExit)
            break;
    }

    TRACE("Clicked %d", buttonClicked);
    return buttonClicked;
}

//======================================================================================================================
void ShowHelpOverlays(const GameSettings& gameSettings)
{
    TRACE_FUNCTION_ONLY(1);

    const Language language = gameSettings.mOptions.mLanguage;

    // Help page content
    const char* titleTexts[] =
    {
        TXT(PS_QUITANDHELP),
        TXT(PS_CAMERAANDSETTINGS),
        TXT(PS_RESETANDPLAY),
        TXT(PS_THROTTLEANDRUDDER),
        TXT(PS_WINDDIRECTION),
        TXT(PS_AILERONSANDELEVATOR),
        TXT(PS_FLYINGINFO),
    };

    const char* helpTexts[] =
    {
        TXT(PS_QUITANDHELPTEXT),
        TXT(PS_CAMERAANDSETTINGSTEXT),
        TXT(PS_RESETANDPLAYTEXT),
        TXT(PS_THROTTLEANDRUDDERTEXT),
        TXT(PS_WINDDIRECTIONTEXT),
        TXT(PS_AILERONSANDELEVATORTEXT),
        TXT(PS_FLYINGINFOTEXT),
    };

    // Overlay positions for each help page (x, y, rx, ry as fractions of screen)
    struct OverlayPos { float x, y, rx, ry; };
    const OverlayPos overlays[] =
    {
        {0.0f, 0.0f, 0.25f, 0.4f},    // Quit/Help (top-left)
        {0.5f, 0.0f, 0.25f, 0.17f},   // Camera/Settings (top-center)
        {1.0f, 0.0f, 0.25f, 0.4f},    // Reset/Play (top-right)
        {0.0f, 1.0f, 0.35f, 0.5f},    // Throttle/Rudder (bottom-left)
        {0.5f, 1.0f, 0.35f, 0.17f},   // Wind direction (bottom-center)
        {1.0f, 1.0f, 0.35f, 0.5f},    // Ailerons/Elevator (bottom-right)
        {-1.0f, -1.0f, 0.0f, 0.0f},   // Flying info (no overlay)
    };

    const int numHelps = sizeof(titleTexts) / sizeof(titleTexts[0]);
    int helpIndex = 0;
    bool slideFromRight = true;

    InGameDialog dialog(0.65f, 0.55f);

    while (helpIndex >= 0 && !s3eDeviceCheckQuitRequest())
    {
        // Set overlay for current page
        const OverlayPos& overlay = overlays[helpIndex];
        dialog.SetOverlay(overlay.x, overlay.y, overlay.rx, overlay.ry);
        dialog.ResetAnimation(slideFromRight);

        uint64 lastTime = Timer::GetMilliseconds();
        int buttonClicked = -1;

        while (buttonClicked < 0)
        {
            s3eDeviceBacklightOn();
            s3eKeyboardUpdate();
            s3ePointerUpdate();
            s3eDeviceYield(1);

            uint64 currentTime = Timer::GetMilliseconds();
            int32 updateTime = currentTime > lastTime ?
                MIN((int32)(currentTime - lastTime), 100) : 0;
            lastTime = currentTime;
            float dt = updateTime * 0.001f;

            bool shouldExit = false;
            buttonClicked = dialog.Update(dt,
                titleTexts[helpIndex], helpTexts[helpIndex],
                TXT(PS_PREV), TXT(PS_OK), TXT(PS_NEXT),
                &shouldExit);

            if (shouldExit)
            {
                helpIndex = -1;
                break;
            }
        }

        // Handle button clicks
        if (buttonClicked == 0)
        {
            // Prev
            helpIndex = (helpIndex + numHelps - 1) % numHelps;
            slideFromRight = false;
        }
        else if (buttonClicked == 1)
        {
            // OK - exit
            helpIndex = -1;
        }
        else if (buttonClicked == 2)
        {
            // Next
            helpIndex = (helpIndex + 1) % numHelps;
            slideFromRight = true;
        }
    }
}

//======================================================================================================================
int ShowDialog(const char* title, const char* text,
                              const char* button0, const char* button1, const char* button2,
                              DialogBackgroundCallback backgroundCallback)
{
    return ShowInGameDialog(0.5f, 0.5f, title, text, button0, button1, button2, backgroundCallback);
}
