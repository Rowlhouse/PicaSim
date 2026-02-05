#include "ChallengeFreeFly.h"
#include "PicaSim.h"
#include "Aeroplane.h"
#include "AeroplanePhysics.h"
#include "AIControllerGlider.h"
#include "AIControllerPowered.h"
#include "Menus/Menu.h"

#include "../Platform/S3ECompat.h"
#include "../Platform/FontRenderer.h"
#include "ShaderManager.h"
#include "Shaders.h"
#include "Graphics.h"

//======================================================================================================================
void ChallengeFreeFly::Relaunched()
{
    mMaxSpeed = 0.0f;
    mOnGroundTime = 0.0f;
    mSmoothedAscentRate = 0.0f;
}

//======================================================================================================================
void ChallengeFreeFly::Init(Aeroplane* aeroplane, LoadingScreenHelper* loadingScreen)
{
    TRACE_METHOD_ONLY(1);
    mAeroplane = aeroplane;
    RenderManager::GetInstance().RegisterRenderGxObject(this, 0);
    mNeedToCacheText = true;

    Relaunched();

    const GameSettings& gs = PicaSim::GetInstance().GetSettings();
    const AIControllersSettings& aics = gs.mAIControllersSettings;

    int numAIControllers = 0;

    while (numAIControllers < gs.mOptions.mFreeFlightMaxAI)
    {
        bool gotOne = false;
        for (size_t i = 0 ; i != aics.mAIControllers.size() && numAIControllers < gs.mOptions.mFreeFlightMaxAI; ++i)
        {
            const AIControllersSettings::AIControllerSetting& aic = aics.mAIControllers[i];

            bool available = true;
            AeroplaneSettings as;
            if (!as.LoadFromFile(aic.mAeroplaneFile.c_str()))
            {
                available = false;
            }

            AIController* controller = 0;
            if (available)
            {
                switch (as.mAIType)
                {
                case AeroplaneSettings::AITYPE_GLIDER:
                    controller = new AIControllerGlider(aic, numAIControllers);
                    break;
                case AeroplaneSettings::AITYPE_POWERED:
                    controller = new AIControllerPowered(aic, numAIControllers);
                    break;
                default:
                    break;
                }
            }
            if (controller)
            {
                if (controller->Init(loadingScreen))
                {
                    mAIControllers.push_back(controller);

                    ++numAIControllers;
                    gotOne = true;
                }
                else
                {
                    delete controller;
                }
            }
        }

        if (!gotOne)
            break;

        if (!aics.mCreateMaxNumControllers)
            break;
    }
}

//======================================================================================================================
void ChallengeFreeFly::Reset()
{
    for (size_t i = 0 ; i != mAIControllers.size() ; ++i)
    {
        AIController* aic = mAIControllers[i];
        aic->Reset();
    }
}

//======================================================================================================================
void ChallengeFreeFly::Terminate()
{
    TRACE_METHOD_ONLY(1);
    RenderManager::GetInstance().UnregisterRenderGxObject(this, 0);

    while (!mAIControllers.empty())
    {
        AIController* controller = mAIControllers.back();
        controller->Terminate();
        delete controller;
        mAIControllers.pop_back();
    }
}

//======================================================================================================================
Challenge::ChallengeResult ChallengeFreeFly::UpdateChallenge(float deltaTime)
{
    Challenge::UpdateChallenge(deltaTime);

    GameSettings& gs = PicaSim::GetInstance().GetSettings();

    // TODO better metric for this
    float speed = mAeroplane->GetVelocity().GetLength();
    bool touching = mAeroplane->GetPhysics()->GetContactTime() > 0.0f;

    if (mAeroplane->GetLaunchMode() == Aeroplane::LAUNCHMODE_AEROTOW)
        touching = false;

    if (speed > mMaxSpeed)
        mMaxSpeed = speed;

    if (speed < 1.0f && touching)
        mOnGroundTime += deltaTime;
    else
        mOnGroundTime = 0.0f;

    if (mOnGroundTime > gs.mAeroplaneSettings.mRelaunchTime && gs.mAeroplaneSettings.mRelaunchWhenStationary)
    {
        mOnGroundTime = 0.0f;
        return CHALLENGE_RELAUNCH;
    }

    if (mAeroplane->GetCrashed() && mOnGroundTime > gs.mAeroplaneSettings.mRelaunchTime)
    {
        mOnGroundTime = 0.0f;
        return CHALLENGE_RELAUNCH;
    }

    return CHALLENGE_CONTINUE;
}

//======================================================================================================================
const char* GetSpeedUnitText(const Options& options)
{
    switch (options.mFreeFlightUnits)
    {
    case Options::UNITS_MPS:
    default:
        return "m/s";
    case Options::UNITS_KPH:
        return "km/h";
    case Options::UNITS_MPH:
        return "mph";
    }
}

//======================================================================================================================
const float GetSpeed(const Options& options, float speed)
{
    switch (options.mFreeFlightUnits)
    {
    case Options::UNITS_MPS:
    default:
        return speed;
    case Options::UNITS_KPH:
        return speed * 3.6f;
    case Options::UNITS_MPH:
        return speed * 2.23693629f; // to mph
    }
}

//======================================================================================================================
const char* GetAscentRateUnitText(const Options& options)
{
    switch (options.mFreeFlightUnits)
    {
    case Options::UNITS_MPS:
    case Options::UNITS_KPH:
    default:
        return "m/s";
    case Options::UNITS_MPH:
        return "ft/s";
    }
}

//======================================================================================================================
const float GetAscentRate(const Options& options, float speed)
{
    switch (options.mFreeFlightUnits)
    {
    case Options::UNITS_MPS:
    case Options::UNITS_KPH:
    default:
        return speed;
    case Options::UNITS_MPH:
        return speed * 3.2808399f; // to ft/s
    }
}

//======================================================================================================================
const char* GetDistanceUnitText(const Options& options)
{
    switch (options.mFreeFlightUnits)
    {
    case Options::UNITS_MPS:
    case Options::UNITS_KPH:
    default:
        return "m";
    case Options::UNITS_MPH:
        return "ft";
    }
}

//======================================================================================================================
const float GetDistance(const Options& options, float distance)
{
    switch (options.mFreeFlightUnits)
    {
    case Options::UNITS_MPS:
    case Options::UNITS_KPH:
    default:
        return distance;
    case Options::UNITS_MPH:
        return distance * 3.2808399f; // to feet
    }
}

//======================================================================================================================
void ChallengeFreeFly::GxRender(int renderLevel, DisplayConfig& displayConfig)
{
    TRACE_METHOD_ONLY(2);
    const GameSettings& gs = PicaSim::GetInstance().GetSettings();
    const Options& options = gs.mOptions;
    if (
        !options.mFreeFlightDisplayAltitude && 
        !options.mFreeFlightDisplayDistance && 
        !options.mFreeFlightDisplayAscentRate && 
        !options.mFreeFlightDisplaySpeed && 
        !options.mFreeFlightDisplayAirSpeed && 
        !options.mFreeFlightDisplayMaxSpeed && 
        !options.mFreeFlightDisplayTime &&
        !mAeroplane->GetCrashed())
    {
        return;
    }

    if (!PicaSim::GetInstance().GetShowUI())
        return;

    FontRenderer& font = FontRenderer::GetInstance();
    uint32 origColour = font.GetColourABGR();

    uint16 fontHeight = font.GetFontHeight();
    int16 textRectY;
    if (PicaSim::GetInstance().GetStatus() == PicaSim::STATUS_FLYING && options.mFreeFlightTextAtTop)
        textRectY = displayConfig.mBottom + fontHeight*1/4;
    else
        textRectY = (int16)(displayConfig.mBottom + displayConfig.mHeight - fontHeight*5/4);
    font.SetRect(displayConfig.mLeft, textRectY, (int16)displayConfig.mWidth, fontHeight);
    font.SetAlignmentHor(FONT_ALIGN_CENTRE);

    IwChar txt[256] = "";
    size_t txtLen = 0;

    const CameraTarget* cameraTarget = PicaSim::GetInstance().GetMainViewport().GetCamera()->GetCameraTarget();
    const Aeroplane* aeroplane = dynamic_cast<const Aeroplane*>(cameraTarget);
    if (!aeroplane)
        aeroplane = dynamic_cast<const Aeroplane*>(PicaSim::GetInstance().GetMainViewport().GetCamera()->GetCameraTransform());
    if (!aeroplane)
        aeroplane = mAeroplane;

    int crashFlags = aeroplane->GetCrashFlags();
    if (crashFlags)
    {
        Language language = options.mLanguage;
        txtLen = snprintf(txt, sizeof(txt), "%s:", TXT(PS_CRASHED));
        if (txtLen < sizeof(txt))
        {
            if (crashFlags & Aeroplane::CRASHFLAG_AIRFRAME)
                txtLen += snprintf(txt + txtLen, sizeof(txt) - txtLen, " %s", TXT(PS_AIRFRAME));
            if (txtLen < sizeof(txt) && (crashFlags & Aeroplane::CRASHFLAG_PROPELLER))
                txtLen += snprintf(txt + txtLen, sizeof(txt) - txtLen, " %s", TXT(PS_PROPELLER));
            if (txtLen < sizeof(txt) && (crashFlags & Aeroplane::CRASHFLAG_UNDERCARRIAGE))
                txtLen += snprintf(txt + txtLen, sizeof(txt) - txtLen, " %s", TXT(PS_UNDERCARRIAGE));
        }
        font.SetColourABGR(0xff0000ff);
    }
    else
    {
        if (options.mFreeFlightDisplayTime)
        {
            float flightTime = aeroplane->GetFlightTime();
            int minutes = (int) (flightTime / 60.0f);
            float seconds = flightTime - minutes * 60.0f;
            if (minutes > 0)
                txtLen += snprintf(txt + txtLen, sizeof(txt) - txtLen, "%dm%04.1fs", minutes, seconds);
            else
                txtLen += snprintf(txt + txtLen, sizeof(txt) - txtLen, "%4.1fs", seconds);
        }

        if (txtLen < sizeof(txt) && options.mFreeFlightDisplaySpeed)
        {
            float speed = aeroplane->GetVelocity().GetLength();
            txtLen += snprintf(txt + txtLen, sizeof(txt) - txtLen, "  %5.1f%s", GetSpeed(options, speed), GetSpeedUnitText(options));
        }

        if (txtLen < sizeof(txt) && options.mFreeFlightDisplayAirSpeed)
        {
            Vector3 windVel = Environment::GetInstance().GetWindAtPosition(aeroplane->GetTransform().GetTrans(), Environment::WIND_TYPE_SMOOTH | Environment::WIND_TYPE_GUSTY);
            float speed = (aeroplane->GetVelocity() - windVel).GetLength();
            txtLen += snprintf(txt + txtLen, sizeof(txt) - txtLen, "  %5.1f%s", GetSpeed(options, speed), GetSpeedUnitText(options));
        }

        if (txtLen < sizeof(txt) && options.mFreeFlightDisplayMaxSpeed)
        {
            txtLen += snprintf(txt + txtLen, sizeof(txt) - txtLen, "  (max %5.1f%s)", GetSpeed(options, mMaxSpeed), GetSpeedUnitText(options));
        }

        float ascentRate = aeroplane->GetVelocity().z;
        mSmoothedAscentRate += (ascentRate - mSmoothedAscentRate) * 0.01f;
        if (txtLen < sizeof(txt) && options.mFreeFlightDisplayAscentRate)
        {
            txtLen += snprintf(txt + txtLen, sizeof(txt) - txtLen, "  %5.2f%s",
                GetAscentRate(options, ascentRate), GetAscentRateUnitText(options));
        }

        if (txtLen < sizeof(txt) && options.mFreeFlightDisplayAltitude)
        {
            float z = aeroplane->GetTransform().GetTrans().z;
            float launchZ = PicaSim::GetInstance().GetObserver().GetTransform().GetTrans().z;
            txtLen += snprintf(txt + txtLen, sizeof(txt) - txtLen, "  %5.1f%s", GetDistance(options, z - launchZ), GetDistanceUnitText(options));
        }

        if (txtLen < sizeof(txt) && options.mFreeFlightDisplayDistance)
        {
            Vector3 delta = aeroplane->GetTransform().GetTrans() - PicaSim::GetInstance().GetObserver().GetTransform().GetTrans();
            float dist = delta.GetLength();
            txtLen += snprintf(txt + txtLen, sizeof(txt) - txtLen, "  %5.1f%s", GetDistance(options, dist), GetDistanceUnitText(options));
        }

        if (options.mFreeFlightColourText)
        {
            // 1 is up, 0 is down
            float ascentAmount = ClampToRange(0.5f + ascentRate / 12.0f, 0.0f, 1.0f);
            int r = ClampToRange((int) ((1.0f - ascentAmount)*255.0f), 0, 255);
            int g = ClampToRange((int) (ascentAmount * 255.0f), 0, 255);
            int b = 0;  
            int32 col = r + (g << 8) + (b << 16) + (255 << 24);
            font.SetColourABGR(col);
        }
        else
        {
            font.SetColourABGR(0xff000000);
        }
    }

    // Text background
    if (options.mFreeFlightTextBackgroundOpacity > 0.0f)
    {
        uint32 w = displayConfig.mWidth;
        uint32 rectW = strlen(txt) * fontHeight * 1 / 2;
        uint32 rectX = (w - rectW) / 2;
        float c = options.mFreeFlightTextBackgroundColour;
        float a = options.mFreeFlightTextBackgroundOpacity;

        // Draw filled rectangle using OpenGL
        DisableDepthTest disableDepthTest;
        DisableDepthMask disableDepthMask;
        EnableBlend enableBlend;
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Get viewport height for coordinate conversion
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        int viewportHeight = viewport[3];

        // Convert from screen coordinates (Y=0 at top) to GL coordinates (Y=0 at bottom)
        float x0 = (float)rectX;
        float x1 = x0 + (float)rectW;
        float y0 = (float)(viewportHeight - (textRectY + fontHeight));  // Bottom of rect in GL coords
        float y1 = y0 + fontHeight * 1.2f;                              // Top of rect in GL coords

        GLfloat pts[] = {
            x0, y0, 0,
            x1, y0, 0,
            x1, y1, 0,
            x0, y1, 0,
        };

        const ControllerShader* shader = (ControllerShader*) ShaderManager::GetInstance().GetShader(SHADER_CONTROLLER);
        shader->Use();
        glVertexAttribPointer(shader->a_position, 3, GL_FLOAT, GL_FALSE, 0, pts);
        glEnableVertexAttribArray(shader->a_position);
        glUniform4f(shader->u_colour, c, c, c, a);
        esSetModelViewProjectionMatrix(shader->u_mvpMatrix);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glDisableVertexAttribArray(shader->a_position);
    }
    font.RenderText(txt);

    if (gs.mOptions.mDisplayFPS)
    {
        font.SetRect(displayConfig.mLeft,(int16)(displayConfig.mBottom + displayConfig.mHeight - fontHeight*5/4),(int16)displayConfig.mWidth,fontHeight);
        font.SetAlignmentHor(FONT_ALIGN_RIGHT);
        sprintf(txt, "%ld", std::lround(gs.mStatistics.mSmoothedFPS));
        font.SetColourABGR(0xff00ffff);
        font.RenderText(txt);
    }

    if (mNeedToCacheText)
    {
        font.SetColourABGR(0x00ffffff);
        font.RenderText("0123456789.");
        mNeedToCacheText = false;
    }

    font.SetColourABGR(origColour);
}

