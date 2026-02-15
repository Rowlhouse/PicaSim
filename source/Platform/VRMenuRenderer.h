#ifndef VR_MENU_RENDERER_H
#define VR_MENU_RENDERER_H

//======================================================================================================================
// VRMenuRenderer - Renders ImGui menu content into VR headset as a floating virtual screen
//
// Usage: Call BeginMenuFrame() before IwGxClear(), and EndMenuFrame() instead of IwGxSwapBuffers().
// When VR is not active, BeginMenuFrame() is a no-op and EndMenuFrame() just calls IwGxSwapBuffers().
//======================================================================================================================
namespace VRMenuRenderer
{
    // Call before IwGxClear(). When VR is active, redirects rendering to an offscreen FBO.
    void BeginMenuFrame();

    // Call instead of IwGxSwapBuffers(). When VR is active, submits the menu FBO to
    // the headset as a floating screen, then blits to the desktop window.
    void EndMenuFrame();

    // Configure the VR menu appearance. Call these before the menu loop starts,
    // or they take effect on the next frame.
    void SetUIScale(float scale);           // 1 = full eye view, <1 = smaller centered panel
    void SetOverlayDistance(float distance); // Stereo depth distance in meters

    // Release GPU resources (call on shutdown or when VR is disabled).
    void Shutdown();
}

#endif // VR_MENU_RENDERER_H
