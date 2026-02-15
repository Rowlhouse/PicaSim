#include "VRMenuRenderer.h"
#include "../Framework/Graphics.h"
#include "../Framework/Trace.h"

#ifdef PICASIM_VR_SUPPORT

#include "VRManager.h"
#include "VRRuntime.h"
#include "Window.h"
#include "../Framework/ShaderManager.h"
#include "../Framework/Shaders.h"

#include <glad/glad.h>
#include <SDL.h>
#include "imgui.h"

//======================================================================================================================
// Module state
//======================================================================================================================
static GLuint sMenuFBO = 0;
static GLuint sMenuTexture = 0;
static GLuint sMenuDepthRB = 0;
static int sMenuFBOWidth = 0;
static int sMenuFBOHeight = 0;
static bool sVRActiveThisFrame = false;

// Configurable settings (set via SetUIScale / SetOverlayDistance)
static float sUIScale = 1.0f;             // 1 = fills eye view, <1 = smaller centered panel
static float sOverlayDistance = 1.0f;     // Stereo depth in meters (matches GameSettings default)

//======================================================================================================================
static void EnsureMenuFBO(int width, int height)
{
    if (sMenuFBO != 0 && sMenuFBOWidth == width && sMenuFBOHeight == height)
        return;

    // Destroy old resources
    if (sMenuFBO != 0)
    {
        glDeleteFramebuffers(1, &sMenuFBO);
        glDeleteTextures(1, &sMenuTexture);
        glDeleteRenderbuffers(1, &sMenuDepthRB);
        sMenuFBO = 0;
    }

    // Create color texture
    glGenTextures(1, &sMenuTexture);
    glBindTexture(GL_TEXTURE_2D, sMenuTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Create depth renderbuffer
    glGenRenderbuffers(1, &sMenuDepthRB);
    glBindRenderbuffer(GL_RENDERBUFFER, sMenuDepthRB);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Create FBO
    glGenFramebuffers(1, &sMenuFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, sMenuFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sMenuTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, sMenuDepthRB);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        TRACE_FILE_IF(ONCE_1) TRACE("VRMenuRenderer: FBO incomplete: 0x%x", status);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    sMenuFBOWidth = width;
    sMenuFBOHeight = height;

    TRACE_FILE_IF(ONCE_1) TRACE("VRMenuRenderer: Created menu FBO %dx%d", width, height);
}

//======================================================================================================================
// Render the menu texture as a scaled, aspect-ratio-correct quad into the
// currently bound framebuffer.  The quad preserves the menu's desktop aspect
// ratio, is centered in the eye viewport, then uniformly scaled by `scale`.
// Stereo shift is applied via the ortho projection.
//======================================================================================================================
static void RenderMenuQuad(int eyeWidth, int eyeHeight, float stereoPixelShift, float scale,
                           int menuTexWidth, int menuTexHeight)
{
    const OverlayShader* shader = static_cast<const OverlayShader*>(
        ShaderManager::GetInstance().GetShader(SHADER_OVERLAY));
    shader->Use();

    float ew = (float)eyeWidth;
    float eh = (float)eyeHeight;

    // Fit the menu texture into the eye viewport while preserving aspect ratio,
    // then apply the user's scale factor.
    float menuAspect = (float)menuTexWidth / (float)menuTexHeight;
    float eyeAspect  = ew / eh;

    float quadW, quadH;
    if (menuAspect > eyeAspect)
    {
        // Menu is wider than eye — fit to width, letterbox vertically
        quadW = ew * scale;
        quadH = quadW / menuAspect;
    }
    else
    {
        // Menu is taller than eye — fit to height, pillar-box horizontally
        quadH = eh * scale;
        quadW = quadH * menuAspect;
    }

    // Center the quad in the viewport
    float quadX = (ew - quadW) * 0.5f;
    float quadY = (eh - quadH) * 0.5f;

    // Orthographic projection: maps [0, eyeWidth] x [0, eyeHeight] to clip space.
    // Stereo shift moves the whole image left/right so the two eyes converge
    // at the intended depth.
    float left   = 0.0f - stereoPixelShift;
    float right  = ew   - stereoPixelShift;
    float bottom = 0.0f;
    float top    = eh;

    float ortho[16];
    memset(ortho, 0, sizeof(ortho));
    ortho[0]  = 2.0f / (right - left);
    ortho[5]  = 2.0f / (top - bottom);
    ortho[10] = -1.0f;
    ortho[12] = -(right + left) / (right - left);
    ortho[13] = -(top + bottom) / (top - bottom);
    ortho[15] = 1.0f;

    glUniformMatrix4fv(shader->u_mvpMatrix, 1, GL_FALSE, ortho);

    // Bind menu texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sMenuTexture);
    glUniform1i(shader->u_texture, 0);

    // White colour (no tint)
    glUniform4f(shader->u_colour, 1.0f, 1.0f, 1.0f, 1.0f);

    // Quad vertices — centered and scaled, with full [0,1] texture coords
    float x0 = quadX, y0 = quadY;
    float x1 = quadX + quadW, y1 = quadY + quadH;
    // clang-format off
    float vertices[] = {
        // x,  y,   z,    u,    v
        x0, y0, 0.0f, 0.0f, 0.0f,
        x1, y0, 0.0f, 1.0f, 0.0f,
        x0, y1, 0.0f, 0.0f, 1.0f,
        x1, y1, 0.0f, 1.0f, 1.0f,
    };
    // clang-format on

    glEnableVertexAttribArray(shader->a_position);
    glEnableVertexAttribArray(shader->a_texCoord);
    glVertexAttribPointer(shader->a_position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), vertices);
    glVertexAttribPointer(shader->a_texCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), vertices + 3);

    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glEnable(GL_DEPTH_TEST);

    glDisableVertexAttribArray(shader->a_position);
    glDisableVertexAttribArray(shader->a_texCoord);
    glBindTexture(GL_TEXTURE_2D, 0);
}

//======================================================================================================================
void VRMenuRenderer::BeginMenuFrame()
{
    sVRActiveThisFrame = false;

    if (!VRManager::IsAvailable())
        return;

    VRManager& vrManager = VRManager::GetInstance();
    if (!vrManager.IsVREnabled())
        return;

    sVRActiveThisFrame = true;

    // The main loop in Main.cpp wraps PicaSim::Update() inside a
    // BeginVRFrame/EndVRFrame pair.  Menu loops run *inside* Update(),
    // so the outer VR frame is still open.  We must end it so that each
    // menu frame can manage its own VR frame timing.
    // VRManager::EndVRFrame() is a safe no-op when !mInVRFrame.
    if (vrManager.IsInVRFrame())
    {
        vrManager.EndVRFrame(vrManager.GetCurrentFrameInfo());
    }

    // Get desktop window size for the offscreen FBO
    Window& window = Window::GetInstance();
    int width = window.GetWidth();
    int height = window.GetHeight();

    EnsureMenuFBO(width, height);

    // Redirect rendering to the menu FBO
    glBindFramebuffer(GL_FRAMEBUFFER, sMenuFBO);
    glViewport(0, 0, width, height);

    // Show software mouse cursor (renders into the FBO, visible in headset)
    ImGui::GetIO().MouseDrawCursor = true;
}

//======================================================================================================================
void VRMenuRenderer::EndMenuFrame()
{
    if (!sVRActiveThisFrame)
    {
        // Non-VR path: just swap buffers as before
        IwGxSwapBuffers();
        return;
    }

    // Unbind menu FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    VRManager& vrManager = VRManager::GetInstance();
    VRRuntime* runtime = vrManager.GetRuntime();

    // Keep OpenXR session alive
    vrManager.PollEvents();

    if (vrManager.IsVRReady() && runtime)
    {
        VRFrameInfo frameInfo;
        if (vrManager.BeginVRFrame(frameInfo))
        {
            if (frameInfo.shouldRender)
            {
                for (int eye = 0; eye < VR_EYE_COUNT; ++eye)
                {
                    // Acquire swapchain image
                    uint32_t imageIndex;
                    if (!runtime->AcquireSwapchainImage((VREye)eye, imageIndex))
                        continue;
                    runtime->WaitSwapchainImage((VREye)eye);

                    int eyeWidth = runtime->GetSwapchainWidth((VREye)eye);
                    int eyeHeight = runtime->GetSwapchainHeight((VREye)eye);

                    // Get the swapchain texture and create a temp FBO for it
                    uint32_t swapchainTex = runtime->GetSwapchainTexture((VREye)eye, imageIndex);

                    GLuint swapchainFBO;
                    glGenFramebuffers(1, &swapchainFBO);
                    glBindFramebuffer(GL_FRAMEBUFFER, swapchainFBO);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                           GL_TEXTURE_2D, swapchainTex, 0);

                    // Add depth renderbuffer for completeness
                    GLuint depthRB;
                    glGenRenderbuffers(1, &depthRB);
                    glBindRenderbuffer(GL_RENDERBUFFER, depthRB);
                    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, eyeWidth, eyeHeight);
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                              GL_RENDERBUFFER, depthRB);

                    // Clear to black
                    glViewport(0, 0, eyeWidth, eyeHeight);
                    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    // Compute stereo pixel shift (same formula as RenderManager)
                    float eyeSign = (eye == VR_EYE_LEFT) ? 1.0f : -1.0f;
                    glm::mat4 projMatrix = runtime->GetProjectionMatrix((VREye)eye, 0.1f, 100.0f);
                    float ipd = runtime->GetIPD();
                    float ndcShift = projMatrix[0][0] * (ipd * 0.5f) / sOverlayDistance;
                    float stereoPixelShift = ndcShift * float(eyeWidth) * 0.5f * eyeSign;

                    // Render menu texture as a scaled, aspect-correct quad
                    RenderMenuQuad(eyeWidth, eyeHeight, stereoPixelShift, sUIScale,
                                   sMenuFBOWidth, sMenuFBOHeight);

                    // Release swapchain image and clean up temp FBO
                    runtime->ReleaseSwapchainImage((VREye)eye);

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    glDeleteRenderbuffers(1, &depthRB);
                    glDeleteFramebuffers(1, &swapchainFBO);
                }
            }

            vrManager.EndVRFrame(frameInfo);
        }
    }

    // Blit menu FBO to the desktop window for mirror display
    Window& window = Window::GetInstance();
    int winWidth = window.GetWidth();
    int winHeight = window.GetHeight();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, sMenuFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, sMenuFBOWidth, sMenuFBOHeight,
                      0, 0, winWidth, winHeight,
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    IwGxSwapBuffers();

    // Reset so the OS cursor reappears on desktop after the menu loop exits.
    // During the menu loop, BeginMenuFrame() sets this to true each frame.
    // We must also explicitly call SDL_ShowCursor because there may be no
    // subsequent ImGui frame to process the change (e.g. non-VR gameplay).
    ImGui::GetIO().MouseDrawCursor = false;
    SDL_ShowCursor(SDL_TRUE);
}

//======================================================================================================================
void VRMenuRenderer::SetUIScale(float scale)
{
    sUIScale = scale;
}

//======================================================================================================================
void VRMenuRenderer::SetOverlayDistance(float distance)
{
    sOverlayDistance = distance;
}

//======================================================================================================================
void VRMenuRenderer::Shutdown()
{
    if (sMenuFBO != 0)
    {
        glDeleteFramebuffers(1, &sMenuFBO);
        glDeleteTextures(1, &sMenuTexture);
        glDeleteRenderbuffers(1, &sMenuDepthRB);
        sMenuFBO = 0;
        sMenuTexture = 0;
        sMenuDepthRB = 0;
        sMenuFBOWidth = 0;
        sMenuFBOHeight = 0;
    }
}

#else // !PICASIM_VR_SUPPORT

//======================================================================================================================
// Non-VR stubs
//======================================================================================================================
void VRMenuRenderer::BeginMenuFrame()
{
    // No-op without VR
}

void VRMenuRenderer::EndMenuFrame()
{
    IwGxSwapBuffers();
}

void VRMenuRenderer::SetUIScale(float) {}
void VRMenuRenderer::SetOverlayDistance(float) {}

void VRMenuRenderer::Shutdown()
{
    // No-op without VR
}

#endif // PICASIM_VR_SUPPORT
