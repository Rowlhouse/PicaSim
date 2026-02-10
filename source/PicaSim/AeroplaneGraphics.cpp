#include "AeroplaneGraphics.h"
#include "Aeroplane.h"
#include "AeroplanePhysics.h"
#include "Wing.h"
#include "Engine.h"
#include "PicaSim.h"
#include "Environment.h"
#include "Terrain.h"
#include "ShaderManager.h"
#include "Shaders.h"
#include "ParticleEmitter.h"
#include "tinyxml.h"

GLfloat AeroplaneGraphics::mPropDiskPoints[mNumPropDiskPoints * 2];

//======================================================================================================================
void AeroplaneGraphics::Box::SetWingColours()
{
    mColourTop = mColourBottom = Vector3(0.3f, 0.3f, 0.5f);
    mColourSides = Vector3(0.7f, 0.7f, 0.7f);
    mColourFront = Vector3(0.0f, 0.0f, 1.0f);
}

//======================================================================================================================
void AeroplaneGraphics::Box::SetGrey()
{
    mColourTop = mColourBottom = mColourSides = mColourFront = Vector3(0.5f, 0.5f, 0.5f);
}

//======================================================================================================================
void AeroplaneGraphics::Box::SetRed()
{
    mColourTop = mColourBottom = mColourSides = mColourFront = Vector3(1.0f, 0.0f, 0.0f);
}

//======================================================================================================================
void AeroplaneGraphics::Box::SetBlack()
{
    mColourTop = mColourBottom = mColourSides = mColourFront = Vector3(0,0,0);
}

//======================================================================================================================
struct WingData
{
    WingData() : wingPosition(0,0,0), wingRotation(0,0,0), wingExtents(1,1,1), 
        colourTop(1,0,0), colourSides(1,1,0), colourFront(0,0,1), colourBottom(0,1,1), roll(0), pitch(0), yaw(0), render(true) {}

    std::string name;
    Vector3 wingPosition;
    Vector3 wingRotation;
    Vector3 wingExtents;
    Vector3 colourTop;
    Vector3 colourSides;
    Vector3 colourFront;
    Vector3 colourBottom;
    float roll, pitch, yaw;
    bool render;
};

//======================================================================================================================
bool ReadFromXML(TiXmlElement* wingElement, WingData& wingData)
{
    readFromXML(wingElement, "name", wingData.name);
    readFromXML(wingElement, "colourTop", wingData.colourTop);
    readFromXML(wingElement, "colourSides", wingData.colourSides);
    readFromXML(wingElement, "colourFront", wingData.colourFront);
    readFromXML(wingElement, "colourBottom", wingData.colourBottom);
    readFromXML(wingElement, "position", wingData.wingPosition);
    readFromXML(wingElement, "rotation", wingData.wingRotation);
    readFromXML(wingElement, "extents", wingData.wingExtents);
    readFromXML(wingElement, "roll", wingData.roll);
    readFromXML(wingElement, "pitch", wingData.pitch);
    readFromXML(wingElement, "yaw", wingData.yaw);
    readFromXML(wingElement, "render", wingData.render);

    return true;
}

//======================================================================================================================
void AeroplaneGraphics::Init(TiXmlDocument& aeroplaneDoc, Aeroplane* aeroplane)
{
    TRACE_METHOD_ONLY(ONCE_2);
    const AeroplaneSettings& as = aeroplane->GetAeroplaneSettings();

    mAeroplane = aeroplane;
    RenderManager::GetInstance().RegisterRenderObject(this, RENDER_LEVEL_OBJECTS);

    TiXmlHandle docHandle( &aeroplaneDoc );

    // Set up the wings
    TiXmlHandle aerodynamicsHandle = docHandle.FirstChild("Physics").FirstChild("Aerodynamics");
    TiXmlElement* aerodynamicsElement = aerodynamicsHandle.ToElement();

    TRACE_FILE_IF(ONCE_1) TRACE("AeroplaneGraphics::Init Wings");
    for (int iWing = 0 ; ; ++iWing)
    {
        TiXmlElement* wingElement = aerodynamicsHandle.Child("Wing", iWing).ToElement();
        if (!wingElement)
            break;

        WingData wingData;

        // Copy if required
        std::string copy = readStringFromXML(wingElement, "copy");
        if (!copy.empty())
        {
            for (int iWing = 0 ; ; ++iWing)
            {
                TiXmlElement* wingElementToCopy = aerodynamicsHandle.Child("Wing", iWing).ToElement();
                if (!wingElementToCopy)
                    break;
                const std::string name = readStringFromXML(wingElementToCopy, "name");

                if (name == copy)
                {
                    ReadFromXML(wingElementToCopy, wingData);
                    break;
                }
            }
        }

        ReadFromXML(wingElement, wingData);

        if (!wingData.render)
            continue;

        // Mirror
        bool mirror = readBoolFromXML(wingElement, "mirror");
        if (mirror)
        {
            wingData.wingPosition.y *= -1.0f;
            wingData.wingExtents.y *= -1.0f;
            wingData.wingRotation.x *= -1.0f;
            wingData.wingRotation.z *= -1.0f;
            wingData.roll *= -1.0f;
            wingData.yaw *= -1.0f;
        }

        // Process the roll/pitch/yaw
        ApplyRollPitchYawToRotationDegrees(wingData.roll, wingData.pitch, wingData.yaw, wingData.wingRotation);
        float angle = DegreesToRadians(wingData.wingRotation.GetLength());
        if (angle > 0.0f)
            wingData.wingRotation.Normalise();
        else
            wingData.wingRotation = Vector3(1,0,0);

        // Scaling
        wingData.wingPosition *= as.mSizeScale;
        wingData.wingExtents *= as.mSizeScale;

        Box box;
        box.mColourBottom = wingData.colourBottom;
        box.mColourSides = wingData.colourSides;
        box.mColourFront = wingData.colourFront;
        box.mColourTop = wingData.colourTop;
        box.mName = wingData.name;
        box.mExtents = wingData.wingExtents;

        // Set up configuration for each aerofoil
        box.mTM.SetAxisAngle(wingData.wingRotation, angle);
        box.mTM.SetTrans(wingData.wingPosition + box.mTM.RowY() * box.mExtents.y * 0.5f);
        box.mExtents.y = fabsf(box.mExtents.y);

        mBoxes.push_back(box);
    }

    TRACE_FILE_IF(ONCE_1) TRACE("AeroplaneGraphics::Init Fuselages");
    for (int iFuselage = 0 ; ; ++iFuselage)
    {
        TiXmlElement* fuselageElement = aerodynamicsHandle.Child("Fuselage", iFuselage).ToElement();
        if (!fuselageElement)
            break;

        bool render = true;
        readFromXML(fuselageElement, "render", render);
        if (!render)
            continue;

        Box box;
        box.SetGrey();

        // Note that the fuselage position is mid-height and mid-chord, but at one end
        Vector3 fuselagePosition = readVector3FromXML(fuselageElement, "position") * as.mSizeScale;
        Vector3 fuselageRotation = readVector3FromXML(fuselageElement, "rotation");
        box.mExtents  = readVector3FromXML(fuselageElement, "extents") * as.mSizeScale;
        readFromXML(fuselageElement, "colourTop", box.mColourTop);
        readFromXML(fuselageElement, "colourSides", box.mColourSides);
        readFromXML(fuselageElement, "colourFront", box.mColourFront);
        readFromXML(fuselageElement, "colourBottom", box.mColourBottom);
        box.mName = readStringFromXML(fuselageElement, "name");

        float roll = readFloatFromXML(fuselageElement, "roll");
        float pitch = readFloatFromXML(fuselageElement, "pitch");
        float yaw = readFloatFromXML(fuselageElement, "yaw");
        ApplyRollPitchYawToRotationDegrees(roll, pitch, yaw, fuselageRotation);

        float angle = DegreesToRadians(fuselageRotation.GetLength());
        if (angle > 0.0f)
            fuselageRotation.Normalise();
        else
            fuselageRotation = Vector3(1,0,0);

        // Set up configuration for each aerofoil
        box.mTM.SetAxisAngle(fuselageRotation, angle);
        box.mTM.SetTrans(fuselagePosition);

        mBoxes.push_back(box);
    }

    TRACE_FILE_IF(ONCE_1) TRACE("AeroplaneGraphics::Init PropellerEngines");
    for (int iPropDisk = 0 ; ; ++iPropDisk)
    {
        TiXmlElement* propDiskElement = aerodynamicsHandle.Child("PropellerEngine", iPropDisk).ToElement();
        if (!propDiskElement)
            break;

        mPropDisks.push_back(PropDisk());
        PropDisk& propDisk = mPropDisks.back();

        // Default values
        propDisk.mRadius = 0.0f;
        propDisk.mColour = Vector4(1,1,1,0.1f);
        Vector3 enginePosition(0,0,0);
        Vector3 engineRotation(0,0,0);
        float roll = 0, pitch = 0, yaw = 0;

        // Copy if required
        std::string copy = readStringFromXML(propDiskElement, "copy");
        if (!copy.empty())
        {
            for (int iEngine = 0 ; ; ++iEngine)
            {
                TiXmlElement* engineElementToCopy = aerodynamicsHandle.Child("PropellerEngine", iEngine).ToElement();
                if (!engineElementToCopy)
                    break;
                const std::string name = readStringFromXML(engineElementToCopy, "name");
                if (name == copy)
                {
                    readFromXML(engineElementToCopy, "radius", propDisk.mRadius);
                    readFromXML(engineElementToCopy, "colour", propDisk.mColour);
                    readFromXML(engineElementToCopy, "position", enginePosition);
                    readFromXML(engineElementToCopy, "rotation", engineRotation);
                    readFromXML(engineElementToCopy, "roll", roll);
                    readFromXML(engineElementToCopy, "pitch", pitch);
                    readFromXML(engineElementToCopy, "yaw", yaw);
                    break;
                }
            }
        }

        // Read from current element (overrides copied values)
        propDisk.mName = readStringFromXML(propDiskElement, "name");
        readFromXML(propDiskElement, "radius", propDisk.mRadius);
        readFromXML(propDiskElement, "colour", propDisk.mColour);
        readFromXML(propDiskElement, "position", enginePosition);
        readFromXML(propDiskElement, "rotation", engineRotation);
        readFromXML(propDiskElement, "roll", roll);
        readFromXML(propDiskElement, "pitch", pitch);
        readFromXML(propDiskElement, "yaw", yaw);

        // Apply scaling and compute transform
        propDisk.mRadius *= as.mSizeScale;
        enginePosition *= as.mSizeScale;
        ApplyRollPitchYawToRotationDegrees(roll, pitch, yaw, engineRotation);

        float angle = DegreesToRadians(engineRotation.GetLength());
        if (angle > 0.0f)
            engineRotation.Normalise();
        else
            engineRotation = Vector3(1,0,0);

        // Store the info about the engine
        propDisk.mTM.SetAxisAngle(engineRotation, angle);
        propDisk.mTM.SetTrans(enginePosition);
    }

    TRACE_FILE_IF(ONCE_1) TRACE("AeroplaneGraphics::Init Geometry");
    // Set up the additional geometry
    TiXmlHandle geometryHandle = docHandle.FirstChild("Physics").FirstChild("Geometry");
    TiXmlElement* geometryElement = geometryHandle.ToElement();
    std::string shapeType;
    for (int iShape = 0 ; ; ++iShape)
    {
        TiXmlElement* shapeElement = geometryHandle.Child("Shape", iShape).ToElement();
        if (!shapeElement)
            break;
        readFromXML(shapeElement, "type", shapeType);
        if (shapeType == "box")
        {
            bool render = true;
            readFromXML(shapeElement, "render", render);
            if (!render)
                continue;

            Box box;
            box.SetGrey();
            box.mExtents = readVector3FromXML(shapeElement, "extents") * as.mSizeScale;
            readFromXML(shapeElement, "colourTop", box.mColourTop);
            readFromXML(shapeElement, "colourSides", box.mColourSides);
            readFromXML(shapeElement, "colourFront", box.mColourFront);
            readFromXML(shapeElement, "colourBottom", box.mColourBottom);

            Vector3 rotation = readVector3FromXML(shapeElement, "rotation");

            float angle = DegreesToRadians(rotation.GetLength());
            if (angle > 0.0f)
                rotation.Normalise();
            else
                rotation = Vector3(1,0,0);

            box.mTM.SetAxisAngle(rotation, angle);
            box.mTM.t = readVector3FromXML(shapeElement, "position") * as.mSizeScale;
            mBoxes.push_back(box);
        }
        else
        {
            IwAssert(ROWLHOUSE, false);
        }
    } // Loop over shapes

    // Loop over the crash shapes
    TRACE_FILE_IF(ONCE_1) TRACE("AeroplaneGraphics::Init Crash Shapes");
    TiXmlHandle crashShapesHandle = docHandle.FirstChild("Physics").FirstChild("CrashShapes");
    TiXmlElement* crashShapesElement = crashShapesHandle.ToElement();
    for (int iShape = 0 ; ; ++iShape)
    {
        TiXmlElement* shapeElement = crashShapesHandle.Child("CrashShape", iShape).ToElement();
        if (!shapeElement)
            break;
        readFromXML(shapeElement, "type", shapeType);
        CrashObject crashObject;
        if (shapeType == "box")
        {
            bool render = true;
            readFromXML(shapeElement, "render", render);
            if (!render)
                continue;

            Box box;
            box.SetRed();
            box.mExtents = readVector3FromXML(shapeElement, "extents") * as.mSizeScale;
            Vector3 rotation = readVector3FromXML(shapeElement, "rotation");

            float angle = DegreesToRadians(rotation.GetLength());
            if (angle > 0.0f)
                rotation.Normalise();
            else
                rotation = Vector3(1,0,0);

            box.mTM.SetAxisAngle(rotation, angle);
            box.mTM.t = readVector3FromXML(shapeElement, "position") * as.mSizeScale;
            mBoxes.push_back(box);
        }
        else
        {
            IwAssert(ROWLHOUSE, false);
        }
    }

    // Load 3DS model
    TRACE_FILE_IF(ONCE_1) TRACE("AeroplaneGraphics::Init 3D Model");
    TiXmlHandle graphicsHandle = docHandle.FirstChild("Graphics");

    std::vector<std::string> models;
    std::vector<float> modelScales;
    std::vector<bool> cullBackFaces;
    std::vector<Colour> unsetColours;
    std::vector<Vector3> offsets;

    NamedComponents namedComponents;
    for (int iControlSurface = 0 ; ; ++iControlSurface)
    {
        TiXmlElement* dataElement = graphicsHandle.Child("ControlSurface", iControlSurface).ToElement();
        if (!dataElement)
            break;
        ControlSurface controlSurface;
        controlSurface.mName = readStringFromXML(dataElement, "name");
        controlSurface.mSource = readStringFromXML(dataElement, "source");
        controlSurface.mAngleMultiplier = readFloatFromXML(dataElement, "angleMultiplier");
        controlSurface.mHingePoint1 = readVector3FromXML(dataElement, "hingePoint1");
        controlSurface.mHingePoint2 = readVector3FromXML(dataElement, "hingePoint2");

        IwAssert(ROWLHOUSE, (controlSurface.mHingePoint1 - controlSurface.mHingePoint2).GetLength() > 0.0001f);

        mControlSurfaces.push_back(controlSurface);
        namedComponents.push_back(controlSurface.mName);
    }

    for (int iModel = 0 ; ; ++iModel)
    {
        TiXmlElement* dataElement = graphicsHandle.Child("Model", iModel).ToElement();
        if (!dataElement)
            break;
        std::string model = readStringFromXML(dataElement, "model3DS");
        float modelScale = readFloatFromXML(dataElement, "modelScale");
        bool cullBackFace = readBoolFromXML(dataElement, "cullBackFace");
        Colour unsetColour = readColourFromXML(dataElement, "unsetColour");
        Vector3 offset = readVector3FromXML(dataElement, "offset");
        models.push_back(model);
        modelScales.push_back(modelScale * as.mSizeScale);
        unsetColours.push_back(unsetColour);
        offsets.push_back(offset * as.mSizeScale);
        cullBackFaces.push_back(cullBackFace);
    }

    const GameSettings& gs = PicaSim::GetInstance().GetSettings();

    int numModels = models.size();
    if (numModels)
    {
        int iModel = as.mColourScheme % numModels;
        TiXmlElement* graphicsElement = graphicsHandle.ToElement();
        std::string modelFile = models[iModel];
        float modelScale = modelScales[iModel];
        Vector3 offset = offsets[iModel];
        Colour unsetColour = unsetColours[iModel];
        if (!modelFile.empty())
        {
            TRACE_FILE_IF(ONCE_1) TRACE("AeroplaneGraphics::Init - Loading %s", modelFile.c_str());
            if (modelFile.find(".3ds") != std::string::npos)
            {
                CLoad3DS load3DS;
                ThreeDSModel model;
                load3DS.Import3DS(&model, modelFile.c_str());
                mRenderModel.Init(model, offset, as.mColourOffset, Vector3(modelScale, modelScale, modelScale), cullBackFaces[iModel], unsetColour);
            }
            else if (modelFile.find(".ac") != std::string::npos)
            {
                ACModel model;
                if (ACLoadModel(model, modelFile.c_str()))
                {
                    bool rgb565 = gs.mOptions.m16BitTextures;

                    mRenderModel.Init(
                        model, modelFile, offsets[iModel], 
                        as.mColourOffset, Vector3(modelScale, modelScale, modelScale), cullBackFaces[iModel], rgb565, &namedComponents);
                }
                else
                {
                    TRACE("Error loading %s", modelFile.c_str());
                }
            }
            for (ControlSurfaces::iterator it = mControlSurfaces.begin() ; it != mControlSurfaces.end() ; ++it)
            {
                ControlSurface& cs = *it;
                cs.mHingePoint1 *= modelScale;
                cs.mHingePoint2 *= modelScale;
                cs.mHingePoint1 += offset;
                cs.mHingePoint2 += offset;
            }
        }
    }

    for (int i = 0 ; i != mNumPropDiskPoints ; ++i)
    {
        int j = i * 2;
        float angle = (2.0f * PI * i) / mNumPropDiskPoints;
        mPropDiskPoints[j] = FastSin(angle);
        mPropDiskPoints[j+1] = FastCos(angle);
    }

    for (int i = 0 ; i != AeroplaneSettings::MAX_NUM_SMOKES_PER_PLANE ; ++i)
    {
        // We'll check for enable/disable as we go, so don't create now
        mSmokeEmitterIDs[i] = -1;
        mSmokeHueOffset[i] = 0.0f;
    }

    // Render bounding radius for shadows and culling
    RenderManager::GetInstance().RegisterShadowCasterObject(this);

    EntityManager::GetInstance().RegisterEntity(this, ENTITY_LEVEL_LOOP_POST_PHYSICS);

    mLastGraphPointTime = 0.0f;
    TRACE_FILE_IF(ONCE_1) TRACE("AeroplaneGraphics::Init - Finished");
}

//======================================================================================================================
void AeroplaneGraphics::Terminate()
{
    TRACE_METHOD_ONLY(ONCE_2);
    RenderManager::GetInstance().UnregisterRenderObject(this, RENDER_LEVEL_OBJECTS);
    RenderManager::GetInstance().UnregisterShadowCasterObject(this);
    EntityManager::GetInstance().UnregisterEntity(this, ENTITY_LEVEL_LOOP_POST_PHYSICS);
    mBoxes.clear();
    mPropDisks.clear();
    mRenderModel.Terminate();
    mControlSurfaces.clear();
}

//======================================================================================================================
void AeroplaneGraphics::EntityUpdate(float deltaTime, int entityLevel)
{
    // TODO expose
    const Transform& aeroplaneTM = mAeroplane->GetTransform();

    const AeroplaneSettings& as = mAeroplane->GetAeroplaneSettings();
    const GameSettings& gs = PicaSim::GetInstance().GetSettings();

    float sizeScale = as.mSizeScale;
    float speedScale = sqrtf(as.mSizeScale);
    float timeScale = sqrtf(as.mSizeScale);

    if (gs.mOptions.mEnableSmoke)
    {
        for (int i = 0 ; i != AeroplaneSettings::MAX_NUM_SMOKES_PER_PLANE ; ++i)
        {
            const AeroplaneSettings::SmokeSource& ss = as.mSmokeSources[i];
            if (ss.mEnable)
            {
                float a = ss.mMaxAlpha;
                if (ss.mChannelForAlpha < Controller::MAX_CHANNELS)
                {
                    float control = mAeroplane->GetController().GetControl(ss.mChannelForAlpha);
                    // map [-1,1] onto [0,1]
                    control = ClampToRange((control + 1.0f) * 0.5f, 0.0f, 1.0f);
                    a = ss.mMinAlpha + control * (ss.mMaxAlpha - ss.mMinAlpha);
                    a = Maximum(a, 0.0f);
                }
                float rate = ss.mMaxRate;
                if (ss.mChannelForRate < Controller::MAX_CHANNELS)
                {
                    float control = mAeroplane->GetController().GetControl(ss.mChannelForRate);
                    // map [-1,1] onto [0,1]
                    control = ClampToRange((control + 1.0f) * 0.5f, 0.0f, 1.0f);
                    rate = ss.mMinRate + control * (ss.mMaxRate - ss.mMinRate);
                    rate = Maximum(rate, 0.0f);
                }

                if (a <= 0.01f)
                    rate = 0.0f;

                if (rate <= 0.01f)
                    continue;

                Vector3 pos = aeroplaneTM.TransformVec(ss.mOffset * sizeScale);
                Vector3 targetVel = Environment::GetInstance().GetWindAtPosition(pos, Environment::WIND_TYPE_SMOOTH);
                Vector3 vel = mAeroplane->GetPhysics()->GetVelAtPoint(pos);
                Vector3 emissionVel = aeroplaneTM.RotateVec(ss.mVel * speedScale);

                // Add on prop-wash
                const AeroplanePhysics::Engines& engines = mAeroplane->GetPhysics()->GetEngines();
                for (AeroplanePhysics::Engines::const_iterator it = engines.begin() ; it != engines.end() ; ++it)
                {
                    const Engine& engine = **it;
                    Vector3 wash, angWash;
                    wash = engine.GetLastWash(angWash);
                    Vector3 smokeToEngine = engine.GetTM().GetTrans() - pos;
                    if (wash.Dot(smokeToEngine) < 0.0f)
                        emissionVel += wash * ss.mEngineWash;
                }
                vel += emissionVel;
                targetVel += emissionVel * 0.1f;

                float dampingTime = ss.mDampingTime * timeScale;
                float lifeTime = ss.mLifetime * timeScale;
                lifeTime *= RangedRandom(0.5f, 2.0f);
                float finalSize = ss.mFinalSize * sizeScale;
                finalSize *= RangedRandom(0.7f, 1.4f);

                if (dampingTime > 0.0f)
                {
                    float heightAboveGround = pos.z - Environment::GetInstance().GetTerrain().GetTerrainHeightFast(pos.x, pos.y, true);
                    // This makes sure the smoke doesn't punch through the ground before it comes up.
                    float maxVelTowardsGround = heightAboveGround / dampingTime; // Assumes dampingTime is < lifeTime
                    if (vel.z < -maxVelTowardsGround)
                        vel.z = -maxVelTowardsGround;
                    // This  makes sure the smoke isn't below the ground at the end of its lifeTime
                    heightAboveGround -= finalSize * 0.25f;
                    float E = expf(-lifeTime/dampingTime);
                    float v = vel.z;
                    float D = -heightAboveGround;
                    // Calculate the target velocity that will have z = D when t = lifeTime
                    float vt = (D + v * dampingTime * E - v * dampingTime) / (dampingTime * E + lifeTime - dampingTime);
                    if (vt > targetVel.z)
                    {
                        // Spill out on the ground
                        float excess = vt - targetVel.z;
                        targetVel.z = vt;
                        targetVel.x += 0.7f * excess * RangedRandom(-1.0f, 1.0f);
                        targetVel.y += 0.7f * excess * RangedRandom(-1.0f, 1.0f);
                    }
                }

                if (mSmokeEmitterIDs[i] == -1)
                {
                    mSmokeEmitterIDs[i] = PicaSim::GetInstance().GetParticleEngine().RegisterEmitter(
                          ParticleEngine::TYPE_SMOKE, pos, vel, targetVel, ss.mColour, ss.mMaxParticles,
                          0.0f, ss.mInitialSize*sizeScale, finalSize*sizeScale, lifeTime, ss.mDampingTime*timeScale, 0.0f);
                }
                else
                {
                    // Just update

                    // Want the overall alpha to be the same as if we weren't scaling by quality. So if 
                    // a is the original particle alpha, n is the effective number of original particles
                    // alpha is the overall particle amount, so
                    // alpha = 1 - (1-a)^n
                    // this gives (from Wolfram)
                    // n = log(1-alpha) / (log(1 - a))
                    //
                    // Now b is the modified particle alpha and m is the new particle number. If we want the 
                    // same overall alpha
                    // (1-a)^n = (1-b)^m
                    // Wolfram says:
                    // b = 1 - ((1-a)^n)^(1/m)
                    //
                    // It turns out that alpha drops out, and we have the simpler
                    // b = 1 - (1-a)^(1/quality)
                    float b = 1.0f - powf(1.0f - a, 1.0f / gs.mOptions.mSmokeQuality);

                    rate *= gs.mOptions.mSmokeQuality;

                    ParticleEmitter* emitter = PicaSim::GetInstance().GetParticleEngine().GetEmitter(mSmokeEmitterIDs[i]);
                    emitter->SetInitialAlpha(b);
                    emitter->SetRate(rate);
                    emitter->SetLifeTime(lifeTime);
                    emitter->SetFinalSize(finalSize);

                    if (ss.mHueCycleFreq > 0.0f)
                    {
                        mSmokeHueOffset[i] = WrapToRange(mSmokeHueOffset[i] + deltaTime * ss.mHueCycleFreq, 0.0f, 1.0f);

                        Vector3 hsv = RGB2HSV(ss.mColour);
                        hsv.x = WrapToRange(hsv.x + mSmokeHueOffset[i], 0.0f, 1.0f);
                        Vector3 rgb = HSV2RGB(hsv);
                        emitter->SetColour(rgb);
                    }
                    else
                    {
                        emitter->SetColour(ss.mColour);
                    }

                    PicaSim::GetInstance().GetParticleEngine().UpdateEmitter(
                        mSmokeEmitterIDs[i], deltaTime, pos, vel, targetVel, ss.mVelJitter*speedScale);


                    if (gs.mOptions.mSmokeOnlyInMainView)
                        emitter->SetViewportToUse(&PicaSim::GetInstance().GetMainViewport());
                    else
                        emitter->SetViewportToUse(0);
                }
            }
            else if (mSmokeEmitterIDs[i] >= 0)
            {
                PicaSim::GetInstance().GetParticleEngine().DeregisterEmitter(mSmokeEmitterIDs[i]);
                mSmokeEmitterIDs[i] = -1;
            }
        }
    }
}

// points for the top face
static const float d = 0.5f;
static GLfloat boxPoints[] = {
    -d, -d,  d,
      d, -d,  d, 
      d,  d,  d, 
    -d,  d,  d, 
};

//======================================================================================================================
void AeroplaneGraphics::RenderUpdate(Viewport* viewport, int renderLevel)
{
    TRACE_METHOD_ONLY(ONCE_2);
    const Options& options = PicaSim::GetInstance().GetSettings().mOptions;
    
    if (renderLevel != RENDER_LEVEL_TERRAIN_SHADOW)
        RenderDebug(viewport, renderLevel);

    if (
        PicaSim::GetInstance().GetMode() == PicaSim::MODE_AEROPLANE &&
        viewport->GetCamera()->GetCameraTransform() == mAeroplane
        )
    {
        return;
    }

    // Update control surfaces
    if (mAeroplane->GetPhysics())
    {
        for (ControlSurfaces::iterator it = mControlSurfaces.begin() ; it != mControlSurfaces.end() ; ++it)
        {
            ControlSurface& cs = *it;
            const Wing* wing = mAeroplane->GetPhysics()->GetWing(cs.mSource);
            if (wing)
            {
                float flapAngle = wing->getFlapAngle();

                Transform T = Transform::g_Identity;
                T.SetTrans(cs.mHingePoint1);
                Transform TInv = Transform::g_Identity;
                TInv.SetTrans(-cs.mHingePoint1);

                Transform R = Transform::g_Identity;
                Vector3 axis = (cs.mHingePoint2 - cs.mHingePoint1).GetNormalised();
                R.SetAxisAngle(axis, DegreesToRadians(flapAngle) * cs.mAngleMultiplier);

                Transform tm = TInv * R * T;

                mRenderModel.SetComponentTM(cs.mName, tm);
            }
            else
            {
                const Engine* engine = mAeroplane->GetPhysics()->GetEngine(cs.mSource);
                if (engine)
                {
                    float angle, angVel;
                    engine->GetRotation(angle, angVel);

                    float graphicsDeltaTime = PicaSim::GetInstance().GetCurrentUpdateDeltaTime();
                    float graphicsAngle = angVel * graphicsDeltaTime;

                    Transform T = Transform::g_Identity;
                    T.SetTrans(cs.mHingePoint1);
                    Transform TInv = Transform::g_Identity;
                    TInv.SetTrans(-cs.mHingePoint1);

                    Transform R = Transform::g_Identity;
                    Vector3 axis = (cs.mHingePoint2 - cs.mHingePoint1).GetNormalised();

                    R.SetAxisAngle(axis, angle);

                    Transform tm = TInv * R * T;

                    mRenderModel.SetComponentTM(cs.mName, tm);

                    float alphaScale = ClampToRange(1.0f - graphicsAngle / DegreesToRadians(360.0f), 0.0f, 1.0f);
                    if (renderLevel == RENDER_LEVEL_TERRAIN_SHADOW)
                        alphaScale *= 0.0f;//mShadowAmount;
                    mRenderModel.SetAlphaScale(cs.mName, alphaScale);
                }
                else
                {
                    const Wheel* wheel = mAeroplane->GetPhysics()->GetWheel(cs.mSource);
                    if (wheel)
                    {
                        float amount = wheel->GetAngle();
                        Transform T = Transform::g_Identity;
                        T.SetTrans(cs.mHingePoint1);
                        Transform TInv = Transform::g_Identity;
                        TInv.SetTrans(-cs.mHingePoint1);
                        Transform R = Transform::g_Identity;
                        Vector3 axis = (cs.mHingePoint2 - cs.mHingePoint1).GetNormalised();
                        R.SetAxisAngle(axis, amount * DegreesToRadians(cs.mAngleMultiplier));
                        Transform tm = TInv * R * T;
                        mRenderModel.SetComponentTM(cs.mName, tm);
                    }
                }
            }
        }
    }


    if (mRenderModel.IsCreated())
    {
        if (options.mRenderPreference != Options::RENDER_PREFER_COMPONENTS)
            RenderUpdate3DS(viewport, renderLevel);
        if (options.mRenderPreference != Options::RENDER_PREFER_3DS)
            RenderUpdateComponents(viewport, renderLevel);
    }
    else
    {
        RenderUpdateComponents(viewport, renderLevel);
    }

    RenderUpdatePropDisks(viewport, renderLevel);
}

//======================================================================================================================
void AeroplaneGraphics::RenderUpdateComponents(Viewport* viewport, int renderLevel)
{
    TRACE_METHOD_ONLY(ONCE_2);
    const Options& options = PicaSim::GetInstance().GetSettings().mOptions;
    EnableCullFace enableCullFace(GL_BACK);
    const ModelShader* modelShader = (ModelShader*) ShaderManager::GetInstance().GetShader(SHADER_MODEL);

    float specularAmount = 0.5f;
    float specularExponent = 100.0f;
    modelShader->Use();
    glUniform1f(modelShader->u_specularExponent, specularExponent);
    glUniform1f(modelShader->u_specularAmount, specularAmount);

    glDisableVertexAttribArray(modelShader->a_normal);

    glEnableVertexAttribArray(modelShader->a_position);
    glDisableVertexAttribArray(modelShader->a_colour);

    esSetLighting(modelShader->lightShaderInfo);

    Transform aeroplaneTM = mAeroplane->GetTransform();
    for (Boxes::iterator it = mBoxes.begin() ; it != mBoxes.end() ; ++it)
    {
        const Box& box = *it;
        Transform tm = box.mTM * aeroplaneTM;

        esPushMatrix();

        GLMat44 glTM;
        ConvertTransformToGLMat44(tm, glTM);
        esMultMatrixf(&glTM[0][0]);

        esScalef(box.mExtents.x, box.mExtents.y, box.mExtents.z);

        glVertexAttribPointer(modelShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) boxPoints);
        glVertexAttrib3f(modelShader->a_normal, 0, 0, 1.0f/box.mExtents.z);

        float c = ClampToRange(1.0f - mShadowAmount, 0.0f, 1.0f);
        Vector3 colourTop = renderLevel == RENDER_LEVEL_TERRAIN_SHADOW ? Vector3(c, c, c) : box.mColourTop;
        Vector3 colourSides = renderLevel == RENDER_LEVEL_TERRAIN_SHADOW ? Vector3(c, c, c) : box.mColourSides;
        Vector3 colourFront = renderLevel == RENDER_LEVEL_TERRAIN_SHADOW ? Vector3(c, c, c) : box.mColourFront;
        Vector3 colourBottom = renderLevel == RENDER_LEVEL_TERRAIN_SHADOW ? Vector3(c, c, c) : box.mColourBottom;


        // draw the top and then round the sides
        glVertexAttrib4f(modelShader->a_colour, colourTop.x, colourTop.y, colourTop.z, 1.0f);

        esSetModelViewProjectionAndNormalMatrix(modelShader->u_mvpMatrix, modelShader->u_normalMatrix);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        //==================================================================================
        // Front
        glVertexAttrib4f(modelShader->a_colour, colourFront.x, colourFront.y, colourFront.z, 1.0f);

        glVertexAttrib3f(modelShader->a_normal, 0, 0, 1.0f/box.mExtents.y);

        ROTATE_90_Y;
        esSetModelViewProjectionAndNormalMatrix(modelShader->u_mvpMatrix, modelShader->u_normalMatrix);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


        //==================================================================================
        // Sides
        glVertexAttrib4f(modelShader->a_colour, colourSides.x, colourSides.y, colourSides.z, 1.0f);

        glVertexAttrib3f(modelShader->a_normal, 0, 0, 1.0f/box.mExtents.x);

        ROTATE_90_X;
        esSetModelViewProjectionAndNormalMatrix(modelShader->u_mvpMatrix, modelShader->u_normalMatrix);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        //==================================================================================
        glVertexAttrib3f(modelShader->a_normal, 0, 0, 1.0f/box.mExtents.y);

        ROTATE_90_X;
        esSetModelViewProjectionAndNormalMatrix(modelShader->u_mvpMatrix, modelShader->u_normalMatrix);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glVertexAttrib3f(modelShader->a_normal, 0, 0, 1.0f/box.mExtents.x);

        ROTATE_90_X;
        esSetModelViewProjectionAndNormalMatrix(modelShader->u_mvpMatrix, modelShader->u_normalMatrix);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        //==================================================================================
        glVertexAttrib4f(modelShader->a_colour, colourBottom.x, colourBottom.y, colourBottom.z, 1.0f);

        glVertexAttrib3f(modelShader->a_normal, 0, 0, 1.0f/box.mExtents.z);

        ROTATE_90_Y;
        esSetModelViewProjectionAndNormalMatrix(modelShader->u_mvpMatrix, modelShader->u_normalMatrix);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        esPopMatrix();
    }
}

//======================================================================================================================
void AeroplaneGraphics::RenderUpdatePropDisks(Viewport* viewport, int renderLevel)
{
    TRACE_METHOD_ONLY(ONCE_2);

    EnableBlend enableBlend;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    DisableDepthMask disableDepthMask;

    const Options& options = PicaSim::GetInstance().GetSettings().mOptions;
    const ModelShader* modelShader = (ModelShader*) ShaderManager::GetInstance().GetShader(SHADER_MODEL);

    float specularAmount = 0.5f;
    float specularExponent = 10.0f;
    modelShader->Use();

    glUniform1f(modelShader->u_specularAmount, specularAmount);
    glUniform1f(modelShader->u_specularExponent, specularExponent);

    glDisableVertexAttribArray(modelShader->a_normal);

    glEnableVertexAttribArray(modelShader->a_position);
    glDisableVertexAttribArray(modelShader->a_colour);

    esSetLighting(modelShader->lightShaderInfo);

    Transform aeroplaneTM = mAeroplane->GetTransform();

    for (PropDisks::iterator it = mPropDisks.begin() ; it != mPropDisks.end() ; ++it)
    {
        const PropDisk& propDisk = *it;
        float opacity = 1.0f;

        if (mAeroplane->GetPhysics())
        {
            const Engine* engine = mAeroplane->GetPhysics()->GetEngine(propDisk.mName);
            if (engine)
            {
                float angle, angVel;
                engine->GetRotation(angle, angVel);
                float graphicsDeltaTime = PicaSim::GetInstance().GetCurrentUpdateDeltaTime();
                float graphicsAngle = angVel * graphicsDeltaTime;
                opacity = ClampToRange(graphicsAngle / DegreesToRadians(360.0f), 0.0f, 1.0f);
            }
        }

        GLfloat diskPoints[mNumPropDiskPoints * 3];
        for (int i = 0 ; i != mNumPropDiskPoints ; ++i)
        {
            int j = i * 3;
            int k = i * 2;
            diskPoints[j  ] = 0.0f;
            diskPoints[j+1] = propDisk.mRadius * mPropDiskPoints[k];
            diskPoints[j+2] = propDisk.mRadius * mPropDiskPoints[k+1];
        }

        Transform tm = propDisk.mTM * aeroplaneTM;

        esPushMatrix();

        GLMat44 glTM;
        ConvertTransformToGLMat44(tm, glTM);
        esMultMatrixf(&glTM[0][0]);

        glVertexAttribPointer(modelShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) diskPoints);
        glVertexAttrib3f(modelShader->a_normal, 0.0f, 0, 0.0f);

        // Set the colour - combine shadow colour with disk colour when rendering terrain shadow
        Vector4 diskColour = propDisk.mColour;
        diskColour.w *= opacity;
        if (renderLevel == RENDER_LEVEL_TERRAIN_SHADOW)
        {
            diskColour.w *= mShadowAmount;
        }

        glVertexAttrib4f(modelShader->a_colour, diskColour.x, diskColour.y, diskColour.z, diskColour.w);

        esSetModelViewProjectionAndNormalMatrix(modelShader->u_mvpMatrix, modelShader->u_normalMatrix);
        glDrawArrays(GL_TRIANGLE_FAN, 0, mNumPropDiskPoints);

        esPopMatrix();
    }
}

//======================================================================================================================
void AeroplaneGraphics::RenderUpdate3DS(Viewport* viewport, int renderLevel)
{
    TRACE_METHOD_ONLY(ONCE_2);
    const Options& options = PicaSim::GetInstance().GetSettings().mOptions;
    glDisable(GL_BLEND);

    Transform aeroplaneTM = mAeroplane->GetTransform();

    esPushMatrix();
    GLMat44 glTM;
    ConvertTransformToGLMat44(aeroplaneTM, glTM);
    esMultMatrixf(&glTM[0][0]);

    ROTATE_90_X;
    ROTATE_270_Y;
    if (renderLevel == RENDER_LEVEL_TERRAIN_SHADOW)
    {
        float c = ClampToRange(1.0f - mShadowAmount, 0.0f, 1.0f);
        Vector4 colour(c, c, c, 1);
        mRenderModel.Render(&colour, true, false);
    }
    else
    {
        mRenderModel.Render(0, false, options.mSeparateSpecular);
    }
    esPopMatrix();
}

//======================================================================================================================
void AeroplaneGraphics::RenderDebug(Viewport* viewport, int renderLevel)
{
    Options& options = PicaSim::GetInstance().GetSettings().mOptions;

    int numWindStreamers = options.mNumWindStreamers;
    if (numWindStreamers)
    {
        float deltaZ = options.mWindStreamerDeltaZ;
        float streamerTime = options.mWindStreamerTime;
        Transform aeroplaneTM = mAeroplane->GetTransform();

        Vector3 aeroplaneWind = Environment::GetInstance().GetWindAtPosition(
            aeroplaneTM.GetTrans(), Environment::WIND_TYPE_SMOOTH | Environment::WIND_TYPE_GUSTY);
        if (aeroplaneWind.GetLengthSquared() > 0.0001f)
            aeroplaneWind.Normalise();
        else
            aeroplaneWind = Vector3(1,0,0);
        Vector3 aeroplaneWindLeft(-aeroplaneWind.y, aeroplaneWind.x, 0.0f);

        Vector3 groundPos0 = aeroplaneTM.GetTrans();
        float numSide = 1;
        for (float side = 0 ; side < numSide-0.5f ; ++side)
        {
            Vector3 groundPos = groundPos0 + aeroplaneWindLeft * 20.0f * (0.5f + side - numSide/2.0f);
            Environment::GetInstance().GetTerrain().GetTerrainHeight(groundPos, true);

            float dt = 1.0f;
            int numPts = (int) (streamerTime / dt);

            for (int i = 0 ; i != numWindStreamers ; ++i)
            {
                float dz = deltaZ * float(i + 1);
                Vector3 windPos = groundPos + Vector3(0,0,dz);

                for (int t = 0 ; t != numPts ; ++t)
                {
                    if (i == numWindStreamers-1)
                    {
                        float zeroWindHeight = Environment::GetInstance().GetZeroWindHeight(windPos.x, windPos.y);
                        RenderManager::GetInstance().GetDebugRenderer().DrawPoint(Vector3(windPos.x, windPos.y, zeroWindHeight), 1.0f, Vector3(1,0,0));
                    }

                    Vector3 wind = Environment::GetInstance().GetWindAtPosition(windPos, Environment::WIND_TYPE_SMOOTH | Environment::WIND_TYPE_GUSTY);
                    Vector3 newWindPos = windPos + wind * dt;
                    RenderManager::GetInstance().GetDebugRenderer().DrawLine(windPos, newWindPos, Vector3(1,1,1));
                    windPos = newWindPos;
                }
            }
        }
    }

    if (mAeroplane->GetPhysics() && options.mDrawAeroplaneCoM != Options::COM_NONE)
    {
        const Transform& tm = mAeroplane->GetPhysics()->GetCoMTransform();
        RenderManager::GetInstance().GetDebugRenderer().DrawPoint(tm, 1.0f, Vector3(1,1,0));
        if (options.mDrawAeroplaneCoM == Options::COM_BOX)
        {
            Vector3 inertia = mAeroplane->GetPhysics()->GetInertiaLocal();
            float m = mAeroplane->GetPhysics()->GetMass();

            float a = 12.0f * inertia.x / m;
            float b = 12.0f * inertia.y / m;
            float c = 12.0f * inertia.z / m;

            float X = (c - a + b) * 0.5f; // squared width
            float Y = (a - b + c) * 0.5f;
            float Z = (b - c + a) * 0.5f;

            float x = 0.5f * sqrtf(X);
            float y = 0.5f * sqrtf(Y);
            float z = 0.5f * sqrtf(Z);

            Vector3 p0 = tm.TransformVec(Vector3(-x, -y, -z));
            Vector3 p1 = tm.TransformVec(Vector3(-x,  y, -z));
            Vector3 p2 = tm.TransformVec(Vector3( x,  y, -z));
            Vector3 p3 = tm.TransformVec(Vector3( x, -y, -z));
            Vector3 p4 = tm.TransformVec(Vector3(-x, -y,  z));
            Vector3 p5 = tm.TransformVec(Vector3(-x,  y,  z));
            Vector3 p6 = tm.TransformVec(Vector3( x,  y,  z));
            Vector3 p7 = tm.TransformVec(Vector3( x, -y,  z));

            RenderManager::GetInstance().GetDebugRenderer().DrawLine(p0, p1, Vector3(1,1,1));
            RenderManager::GetInstance().GetDebugRenderer().DrawLine(p1, p2, Vector3(1,1,1));
            RenderManager::GetInstance().GetDebugRenderer().DrawLine(p2, p3, Vector3(1,1,1));
            RenderManager::GetInstance().GetDebugRenderer().DrawLine(p3, p0, Vector3(1,1,1));

            RenderManager::GetInstance().GetDebugRenderer().DrawLine(p4, p5, Vector3(1,1,1));
            RenderManager::GetInstance().GetDebugRenderer().DrawLine(p5, p6, Vector3(1,1,1));
            RenderManager::GetInstance().GetDebugRenderer().DrawLine(p6, p7, Vector3(1,1,1));
            RenderManager::GetInstance().GetDebugRenderer().DrawLine(p7, p4, Vector3(1,1,1));

            RenderManager::GetInstance().GetDebugRenderer().DrawLine(p0, p4, Vector3(1,1,1));
            RenderManager::GetInstance().GetDebugRenderer().DrawLine(p1, p5, Vector3(1,1,1));
            RenderManager::GetInstance().GetDebugRenderer().DrawLine(p2, p6, Vector3(1,1,1));
            RenderManager::GetInstance().GetDebugRenderer().DrawLine(p3, p7, Vector3(1,1,1));
        }
    }

    if (options.mDrawGroundPosition)
    {
        Vector3 planePos = mAeroplane->GetTransform().GetTrans();
        Vector3 groundPos, groundNormal;
        float radius = GetRenderBoundingRadius();
        Environment::GetInstance().GetTerrain().GetLocalTerrain(planePos, groundPos, groundNormal, true);
        float height = planePos.z - groundPos.z;
        float offset = 0.05f;
        groundPos += offset * groundNormal;
        Vector3 colour = Vector3(1,1,1)*0.5f;
        RenderManager::GetInstance().GetDebugRenderer().DrawLine(groundPos, planePos,colour);
        RenderManager::GetInstance().GetDebugRenderer().DrawCircle(groundPos, groundNormal, radius, colour, 32);
    }

    // Draw most graphs only if we're not in a race
    float currentFlightTime = mAeroplane->GetFlightTime();
    if (currentFlightTime < mLastGraphPointTime)
        mLastGraphPointTime = currentFlightTime;
    const int numPoints = Minimum(options.mFrameworkSettings.mScreenWidth, 2048);
    const float graphPointTime = options.mGraphDuration / numPoints;

    const Camera* camera = PicaSim::GetInstance().GetMainViewport().GetCamera();
    if ( 
        (camera->GetCameraTarget() == mAeroplane || camera->GetCameraTransform() == mAeroplane) && 
        PicaSim::GetInstance().GetSettings().mChallengeSettings.mChallengeMode == ChallengeSettings::CHALLENGE_FREEFLY
        )
    {
        if (PicaSim::GetInstance().GetStatus() == PicaSim::STATUS_FLYING)
        {
            int numPts = (int) ((currentFlightTime - mLastGraphPointTime) / graphPointTime);
            if (numPts > 5)
            {
                numPts = 5;
                mLastGraphPointTime = currentFlightTime;
            }
            else
            {
                mLastGraphPointTime += numPts * graphPointTime;
            }
            for (int iPt = 0 ; iPt != numPts ; ++iPt)
            {
                if (options.mGraphAltitude)
                {
                    Transform aeroplaneTM = mAeroplane->GetTransform();
                    float altitude = aeroplaneTM.GetTrans().z - PicaSim::GetInstance().GetObserver().GetTransform().GetTrans().z;
                    RenderManager::GetInstance().GetDebugRenderer().SetGraphProperties(GRAPH_ALTITUDE, numPoints, 0.0f, options.mGraphAltitude, Vector3(0,1,1));
                    RenderManager::GetInstance().GetDebugRenderer().AddGraphPoint(GRAPH_ALTITUDE, altitude);
                }
                else
                {
                    RenderManager::GetInstance().GetDebugRenderer().DisableGraph(GRAPH_ALTITUDE);
                }

                if (options.mGraphAirSpeed)
                {
                    const Transform& aeroplaneTM = mAeroplane->GetTransform();
                    const Vector3& vel = mAeroplane->GetVelocity();
                    Vector3 wind = Environment::GetInstance().GetWindAtPosition(aeroplaneTM.GetTrans(), Environment::WIND_TYPE_SMOOTH | Environment::WIND_TYPE_GUSTY);
                    float airSpeed = (vel - wind).GetLength();

                    RenderManager::GetInstance().GetDebugRenderer().SetGraphProperties(GRAPH_AIR_SPEED, numPoints, 0.0f, options.mGraphAirSpeed, Vector3(1,0,0));
                    RenderManager::GetInstance().GetDebugRenderer().AddGraphPoint(GRAPH_AIR_SPEED, airSpeed);
                }
                else
                {
                    RenderManager::GetInstance().GetDebugRenderer().DisableGraph(GRAPH_AIR_SPEED);
                }

                if (options.mGraphGroundSpeed)
                {
                    const Vector3& vel = mAeroplane->GetVelocity();
                    RenderManager::GetInstance().GetDebugRenderer().SetGraphProperties(GRAPH_GROUND_SPEED, numPoints, 0.0f, options.mGraphGroundSpeed, Vector3(0,1,0));
                    RenderManager::GetInstance().GetDebugRenderer().AddGraphPoint(GRAPH_GROUND_SPEED, vel.GetLength());
                }
                else
                {
                    RenderManager::GetInstance().GetDebugRenderer().DisableGraph(GRAPH_GROUND_SPEED);
                }

                if (options.mGraphClimbRate)
                {
                    const Vector3& vel = mAeroplane->GetVelocity();
                    RenderManager::GetInstance().GetDebugRenderer().SetGraphProperties(
                        GRAPH_CLIMB_RATE, numPoints, -options.mGraphClimbRate, options.mGraphClimbRate, Vector3(0,0,1));
                    RenderManager::GetInstance().GetDebugRenderer().AddGraphPoint(GRAPH_CLIMB_RATE, vel.z);
                }
                else
                {
                    RenderManager::GetInstance().GetDebugRenderer().DisableGraph(GRAPH_CLIMB_RATE);
                }

                if (options.mGraphWindSpeed)
                {
                    Transform aeroplaneTM = mAeroplane->GetTransform();
                    Vector3 wind = Environment::GetInstance().GetWindAtPosition(aeroplaneTM.GetTrans(), Environment::WIND_TYPE_SMOOTH | Environment::WIND_TYPE_GUSTY);
                    float speed = wind.GetLength();
                    RenderManager::GetInstance().GetDebugRenderer().SetGraphProperties(GRAPH_WIND_SPEED, numPoints, 0.0f, options.mGraphWindSpeed, Vector3(1,1,0));
                    RenderManager::GetInstance().GetDebugRenderer().AddGraphPoint(GRAPH_WIND_SPEED, speed);
                }
                else
                {
                    RenderManager::GetInstance().GetDebugRenderer().DisableGraph(GRAPH_WIND_SPEED);
                }

                if (options.mGraphWindVerticalVelocity)
                {
                    Transform aeroplaneTM = mAeroplane->GetTransform();
                    Vector3 wind = Environment::GetInstance().GetWindAtPosition(
                        aeroplaneTM.GetTrans(), Environment::WIND_TYPE_SMOOTH | Environment::WIND_TYPE_GUSTY);
                    float speed = wind.z;
                    RenderManager::GetInstance().GetDebugRenderer().SetGraphProperties(
                        GRAPH_WIND_VERTICAL_VELOCITY, numPoints,
                        -options.mGraphWindVerticalVelocity, options.mGraphWindVerticalVelocity, Vector3(1,0,1));
                    RenderManager::GetInstance().GetDebugRenderer().AddGraphPoint(
                        GRAPH_WIND_VERTICAL_VELOCITY, speed);
                }
                else
                {
                    RenderManager::GetInstance().GetDebugRenderer().DisableGraph(GRAPH_WIND_VERTICAL_VELOCITY);
                }
                if (options.mGraphFPS > 0.0f)
                {
                    RenderManager::GetInstance().GetDebugRenderer().SetGraphProperties(
                        GRAPH_FPS, numPoints, 0.0f, options.mGraphFPS, Vector3(1,1,1));
                    RenderManager::GetInstance().GetDebugRenderer().AddGraphPoint(
                        GRAPH_FPS, PicaSim::GetInstance().GetSettings().mStatistics.mFPS);
                }
                else
                {
                    RenderManager::GetInstance().GetDebugRenderer().DisableGraph(GRAPH_FPS);
                }

                if (false) // Graph the tow rope force
                {
                    float towForce = mAeroplane->GetPhysics()->GetTowForce();
                    if (towForce >= 0.0f)
                    {
                        float weight = mAeroplane->GetPhysics()->GetMass() * EntityManager::GetInstance().GetDynamicsWorld().getGravity().length();
                        float ratio = towForce / weight;
                        float max = mAeroplane->GetAeroplaneSettings().mAeroTowRopeStrength;
                        RenderManager::GetInstance().GetDebugRenderer().SetGraphProperties(GRAPH_TOWFORCE, numPoints, 0.0f, max, Vector3(1,1,1));
                        RenderManager::GetInstance().GetDebugRenderer().AddGraphPoint(GRAPH_TOWFORCE, ratio);
                    }
                    else
                    {
                        RenderManager::GetInstance().GetDebugRenderer().DisableGraph(GRAPH_TOWFORCE);
                    }
                }
            }
        }
    }
}

//======================================================================================================================
const Transform& AeroplaneGraphics::GetTM() const
{
    return mAeroplane->GetTransform();
}

//======================================================================================================================
float AeroplaneGraphics::GetRenderBoundingRadius() const
{
    float propRadius = 0.0f;
    for (PropDisks::const_iterator it = mPropDisks.begin() ; it != mPropDisks.end() ; ++it)
    {
        const PropDisk& propDisk = *it;
        propRadius = Maximum(propRadius, propDisk.mRadius);
    }

    return mRenderModel.GetBoundingRadius() + propRadius;
}
