#include "ObjectEditingOverlay.h"
#include "Environment.h"
#include "GameSettings.h"
#include "PicaSim.h"
#include "SimpleObject.h"
#include "Terrain.h"
#include "Menus/Menu.h"

ObjectEditingOverlay::EditMode ObjectEditingOverlay::mEditMode = ObjectEditingOverlay::EDITMODE_MOVE;
ObjectEditingOverlay::EditSpace ObjectEditingOverlay::mEditSpace = ObjectEditingOverlay::EDITSPACE_CAMERA;
float ObjectEditingOverlay::mMoveSpeed = 1.0f;
float ObjectEditingOverlay::mRotateSpeed = 1.0f;
float ObjectEditingOverlay::mScaleSpeed = 1.0f;
float ObjectEditingOverlay::mColourSpeed = 1.0f;


struct InitData
{
    ObjectEditingOverlay::Button button;
    float x, y;
    const char* txt;
};

//======================================================================================================================
ObjectEditingOverlay::ObjectEditingOverlay(const GameSettings& gameSettings, Texture* texture, Environment& environment)
    : mGameSettings(gameSettings), mTexture(texture), mEnvironment(environment)
{
    float buttonSize = 0.05f;
    float paddingFraction = 0.25f;
    GLubyte alpha = 128;

    InitData initData[NUM_BUTTONS] =
    {
        {NEXT,           0.2f, 0.8f,   "Next"},
        {PREV,           0.2f, 0.65f,  "Prev"},
        {CREATE_DYNAMIC, 0.01f, 0.8f,  "Create dynamic"},
        {CREATE_STATIC,  0.01f, 0.65f, "Create static"},
        {DELETE,         0.01f, 0.5f,  "Delete"},
        {FASTER,         0.4f, 0.8f,   "Faster"},
        {SLOWER,         0.4f, 0.65f,  "Slower"},
        {MOVE,           0.55f, 0.65f, "Move"},
        {ROTATE,         0.55f, 0.65f, "Rotate"},
        {SCALE,          0.55f, 0.65f, "Scale"},
        {COLOUR,         0.55f, 0.65f, "Colour"},
        {VISIBLITY,      0.55f, 0.5f,  "Visibility"},
        {SHADOW,         0.4f,  0.5f,  "Shadow"},
        {WORLD,          0.55f, 0.8f,  "World"},
        {CAMERA,         0.55f, 0.8f,  "Camera"},
        {OBJECT,         0.55f, 0.8f,  "Object"},
        {PLUS_X,         0.8f, 0.8f,   "+X"},
        {MINUS_X,        0.8f, 0.5f,   " -X"},
        {PLUS_Y,         0.72f, 0.65f, "+Y"},
        {MINUS_Y,        0.88f, 0.65f, " -Y"},
        {PLUS_Z,         0.94f, 0.8f,  "+Z"},
        {MINUS_Z,        0.94f, 0.5f,  " -Z"},
        {RESET,          0.8f, 0.65f,  "Reset"},
    };

    for (size_t i = 0 ; i != NUM_BUTTONS ; ++i)
    {
        InitData& id = initData[i];
        mButtons[id.button] = new ButtonOverlay(mTexture, buttonSize, paddingFraction, ButtonOverlay::ANCHOR_H_LEFT, ButtonOverlay::ANCHOR_V_MID, id.x, id.y, alpha, false, true);
        mButtons[id.button]->SetText(id.txt, ButtonOverlay::ANCHOR_H_LEFT, ButtonOverlay::ANCHOR_V_BOT, 0.0f, 0.0f);
    }

    EntityManager::GetInstance().RegisterEntity(this, ENTITY_LEVEL_PRE_PHYSICS);

    mSelectedObject = -1;
}

//======================================================================================================================
ObjectEditingOverlay::~ObjectEditingOverlay()
{
    EntityManager::GetInstance().UnregisterEntity(this, ENTITY_LEVEL_PRE_PHYSICS);
    for (size_t i = 0 ; i != NUM_BUTTONS ; ++i)
        delete mButtons[i];
}

//======================================================================================================================
void ObjectEditingOverlay::UpdateButtons()
{
    // Enable by default
    for (int i = 0 ; i != NUM_BUTTONS ; ++i)
        mButtons[i]->Enable(true);

    mButtons[MOVE]->Enable(false);
    mButtons[ROTATE]->Enable(false);
    mButtons[SCALE]->Enable(false);
    mButtons[COLOUR]->Enable(false);

    switch (mEditMode)
    {
    case EDITMODE_MOVE:
        mButtons[MOVE]->Enable(true);
        break;
    case EDITMODE_SCALE:
        mButtons[SCALE]->Enable(true);
        break;
    case EDITMODE_ROTATE:
        mButtons[ROTATE]->Enable(true);
        break;
    case EDITMODE_COLOUR:
        mButtons[COLOUR]->Enable(true);
        break;
    default:
        break;
    }

    mButtons[WORLD]->Enable(false);
    mButtons[CAMERA]->Enable(false);
    mButtons[OBJECT]->Enable(false);
    switch (mEditSpace)
    {
    case EDITSPACE_WORLD:
        mButtons[WORLD]->Enable(true);
        break;
    case EDITSPACE_CAMERA:
        mButtons[CAMERA]->Enable(true);
        break;
    case EDITSPACE_OBJECT:
        mButtons[OBJECT]->Enable(true);
        break;
    default:
        break;
    }

    if (mSelectedObject < 0)
    {
        mButtons[PREV]->Enable(false);
        mButtons[MOVE]->Enable(false);
        mButtons[ROTATE]->Enable(false);
        mButtons[SCALE]->Enable(false);
        mButtons[COLOUR]->Enable(false);
        mButtons[VISIBLITY]->Enable(false);
        mButtons[SHADOW]->Enable(false);
        mButtons[FASTER]->Enable(false);
        mButtons[SLOWER]->Enable(false);
        mButtons[DELETE]->Enable(false);
        mButtons[WORLD]->Enable(false);
        mButtons[CAMERA]->Enable(false);
        mButtons[OBJECT]->Enable(false);
        mButtons[RESET]->Enable(false);
        for (int i = PLUS_X ; i <= MINUS_Z ; ++i)
            mButtons[i]->Enable(false);
    }
    if (mSelectedObject == (int) mEnvironment.mBoxes.size()-1)
        mButtons[NEXT]->Enable(false);

    if (mEditMode == EDITMODE_SCALE || mEditMode == EDITMODE_COLOUR)
    {
        mButtons[WORLD]->Enable(false);
        mButtons[CAMERA]->Enable(false);
        mButtons[OBJECT]->Enable(false);
    }

    // Colour on pressed buttons
    for (size_t i = 0 ; i != NUM_BUTTONS ; ++i)
    {
        ButtonOverlay& button = *mButtons[i];
        ButtonOverlay::PressFlag pressFlag;
        if (i >= PLUS_X && i <= MINUS_Z)
            pressFlag = ButtonOverlay::PRESS_ANY;
        else
            pressFlag = ButtonOverlay::PRESS_CLICKED;

        if (button.IsPressed(pressFlag))
        {
            button.SetColour(255,128,128);
        }
        else
        {
            button.SetColour(255,255,255);
        }
    }

    GameSettings& gs = PicaSim::GetInstance().GetSettings();
    ObjectsSettings& os = gs.mObjectsSettings;
    if (mSelectedObject >= 0 && mSelectedObject < (int) os.mBoxes.size())
    {
        ObjectsSettings::Box& box = os.mBoxes[mSelectedObject];
        if (box.mVisible)
            mButtons[VISIBLITY]->SetColour(0,255,0);
        else
            mButtons[VISIBLITY]->SetColour(255,0,0);

        if (box.mShadow)
            mButtons[SHADOW]->SetColour(0,255,0);
        else
            mButtons[SHADOW]->SetColour(255,0,0);
    }
}

//======================================================================================================================
bool ObjectEditingOverlay::WasClicked(Button button) const
{
    return mButtons[button]->IsPressed(ButtonOverlay::PRESS_CLICKED);
}

//======================================================================================================================
bool ObjectEditingOverlay::WasPressed(Button button) const
{
    return mButtons[button]->IsPressed(ButtonOverlay::PRESS_DOWN);
}

//======================================================================================================================
void ObjectEditingOverlay::EntityUpdate(float deltaTime, int entityLevel)
{
    GameSettings& gs = PicaSim::GetInstance().GetSettings();
    ObjectsSettings& os = gs.mObjectsSettings;

    // Create
    bool createDynamic = WasClicked(CREATE_DYNAMIC);
    bool createStatic = WasClicked(CREATE_STATIC);
    if (createDynamic || createStatic)
    {
        // Copy?
        Transform tm = Transform::g_Identity;
        Vector3 extents(1,1,1);
        Vector3 colour(0.5f, 0.5f, 0.5f);
        float mass = 0.0f;
        bool shadow = true;
        if (createDynamic)
            mass = 1.0f;
        if (mSelectedObject >= 0 && mSelectedObject < (int) os.mBoxes.size())
        {
            BoxObject* box = os.mBoxes[mSelectedObject].mObject;
            tm = box->GetTM();
            extents = box->GetExtents();
            colour = box->GetColour();
            shadow = box->GetShadowVisible();
        }
        else
        {
            tm.SetTrans(PicaSim::GetInstance().GetObserver().GetTM().GetTrans());
            mEnvironment.mTerrain.GetTerrainHeight(tm.t, true);
            tm.t.z += extents.z * 0.5f;
        }
        ObjectsSettings::Box box(extents, tm, colour, 0, mass, true, shadow, true);
        os.mBoxes.push_back(box);
        mSelectedObject = os.mBoxes.size() - 1;
        mEnvironment.mBoxes.insert(box.mObject);
    }
    // Delete
    if (WasClicked(DELETE)) 
    {
        if (mSelectedObject >= 0 && mSelectedObject < (int) os.mBoxes.size())
        {
            BoxObject* box = os.mBoxes[mSelectedObject].mObject;
            delete box;
            os.mBoxes.erase(os.mBoxes.begin() + mSelectedObject);
            mEnvironment.mBoxes.erase(box);
        }
    }
    float* speed = &mMoveSpeed;
    if (mEditMode == EDITMODE_ROTATE)
        speed = &mRotateSpeed;
    else if (mEditMode == EDITMODE_SCALE)
        speed = &mScaleSpeed;
    else if (mEditMode == EDITMODE_COLOUR)
        speed = &mColourSpeed;

    if (WasClicked(FASTER))
        *speed *= 1.2f;
    if (WasClicked(SLOWER))
        *speed /= 1.2f;

    // Work through the boxes
    if (WasClicked(NEXT)) 
        ++mSelectedObject;
    else if (WasClicked(PREV)) 
        --mSelectedObject;
    mSelectedObject = ClampToRange<int>(mSelectedObject, -1, os.mBoxes.size()-1);

    if (WasClicked(MOVE))
        mEditMode = EDITMODE_ROTATE;
    else if (WasClicked(ROTATE))
        mEditMode = EDITMODE_SCALE;
    else if (WasClicked(SCALE))
        mEditMode = EDITMODE_COLOUR;
    else if (WasClicked(COLOUR))
        mEditMode = EDITMODE_MOVE;

    if (WasClicked(WORLD))
        mEditSpace = EDITSPACE_CAMERA;
    else if (WasClicked(CAMERA))
        mEditSpace = EDITSPACE_OBJECT;
    else if (WasClicked(OBJECT))
        mEditSpace = EDITSPACE_WORLD;

    if (mSelectedObject >= 0 && mSelectedObject < (int) os.mBoxes.size())
    {
        float realTimeDelta = PicaSim::GetInstance().GetCurrentUpdateDeltaTime();
        ObjectsSettings::Box& box = os.mBoxes[mSelectedObject];
        BoxObject* boxObject = box.mObject;
        if (boxObject)
        {
            if (WasClicked(VISIBLITY))
            {
                box.mVisible = !box.mVisible;
            }
            if (WasClicked(SHADOW))
            {
                box.mShadow = !box.mShadow;
            }

            char deleteButtonTxt[128];
            sprintf(deleteButtonTxt, "Delete %d", mSelectedObject);
            mButtons[DELETE]->UpdateText(deleteButtonTxt);

            Transform spaceTM;
            spaceTM.SetIdentity();
            if (mEditSpace == EDITSPACE_CAMERA)
            {
                const Vector3 cameraPos = PicaSim::GetInstance().GetMainViewport().GetCamera()->GetPosition();
                Vector3 fwd = boxObject->GetTM().GetTrans() - cameraPos;
                fwd.z = 0.0f;
                if (fwd.GetLength() > 0.5f)
                {
                    fwd.Normalise();
                }
                else
                {
                    fwd = PicaSim::GetInstance().GetMainViewport().GetCamera()->GetTransform().RowX();
                    fwd.z = 0.0f;
                    fwd.Normalise();
                }
                Vector3 up(0,0,1);
                Vector3 left = up.Cross(fwd);
                SetRowX(spaceTM, fwd);
                SetRowY(spaceTM, left);
                SetRowZ(spaceTM, up);
            }
            else if (mEditSpace == EDITSPACE_OBJECT)
            {
                spaceTM = boxObject->GetTM();
            }

            Vector3 extents = boxObject->GetExtents();
            Transform tm = boxObject->GetTM();
            Vector3 colour = boxObject->GetColour();

            if (mEditMode == EDITMODE_SCALE) 
            {
                // Change size
                float sizeSpeed = mScaleSpeed * 1.0f * realTimeDelta;
                Vector3 vel(0,0,0);
                if (WasPressed(PLUS_X)) 
                    vel.x += sizeSpeed;
                if (WasPressed(MINUS_X)) 
                    vel.x -= sizeSpeed;
                if (WasPressed(PLUS_Y)) 
                    vel.y += sizeSpeed;
                if (WasPressed(MINUS_Y)) 
                    vel.y -= sizeSpeed;
                if (WasPressed(PLUS_Z)) 
                    vel.z += sizeSpeed;
                if (WasPressed(MINUS_Z)) 
                    vel.z -= sizeSpeed;
                vel += Vector3(1,1,1);
                extents.x *= vel.x;
                extents.y *= vel.y;
                extents.z *= vel.z;
                extents = Maximum(extents, Vector3(0.01f, 0.01f, 0.01f));
                if (WasClicked(RESET)) 
                {
                    extents = Vector3(1,1,1);
                }
                box.SetExtents(extents);
            }
            else if (mEditMode == EDITMODE_MOVE) 
            {
                float speed = 2.0f * mMoveSpeed;
                // Move
                Vector3 vel(0,0,0);
                if (WasPressed(PLUS_X)) 
                    vel.x += speed;
                if (WasPressed(MINUS_X)) 
                    vel.x -= speed;
                if (WasPressed(PLUS_Y)) 
                    vel.y += speed;
                if (WasPressed(MINUS_Y)) 
                    vel.y -= speed;
                if (WasPressed(PLUS_Z)) 
                    vel.z += speed;
                if (WasPressed(MINUS_Z)) 
                    vel.z -= speed;
                vel = spaceTM.RotateVec(vel);
                if (vel.GetLengthSquared() > 0.0f)
                {
                    tm.t += vel * realTimeDelta;
                    box.SetInitialTM(tm);
                    boxObject->SetTM(tm);
                    boxObject->forceAwake();
                }
                if (WasClicked(RESET)) 
                {
                    mEnvironment.mTerrain.GetTerrainHeight(tm.t, true);
                    tm.t.z += extents.z * 0.5f;
                    box.SetInitialTM(tm);
                    boxObject->SetTM(tm);
                    boxObject->forceAwake();
                }
            }
            else if (mEditMode == EDITMODE_ROTATE) 
            {
                float speed = 0.5f * mMoveSpeed;
                // Rotate
                Vector3 vel(0,0,0);
                if (WasPressed(PLUS_X)) 
                    vel.x += speed;
                if (WasPressed(MINUS_X)) 
                    vel.x -= speed;
                if (WasPressed(PLUS_Y)) 
                    vel.y += speed;
                if (WasPressed(MINUS_Y)) 
                    vel.y -= speed;
                if (WasPressed(PLUS_Z)) 
                    vel.z += speed;
                if (WasPressed(MINUS_Z)) 
                    vel.z -= speed;
                vel = spaceTM.RotateVec(vel);
                Transform rot;
                rot.SetIdentity();
                if (vel.GetLengthSquared() > 0.0f)
                {
                    rot.SetAxisAngle(vel.GetNormalised(), vel.GetLength() * realTimeDelta);
                }
                Vector3 t = tm.GetTrans();
                tm.t = Vector3(0,0,0);
                tm = tm * rot;
                if (WasClicked(RESET)) 
                {
                    tm.SetAxisAngle(Vector3(1,0,0), 0);
                }
                tm.t = t;
                box.SetInitialTM(tm);
                boxObject->SetTM(tm);
            }
            else if (mEditMode == EDITMODE_COLOUR) 
            {
                float speed = 0.5f * mColourSpeed;
                // Move
                Vector3 vel(0,0,0);
                if (WasPressed(PLUS_X)) 
                    vel.x += speed;
                if (WasPressed(MINUS_X)) 
                    vel.x -= speed;
                if (WasPressed(PLUS_Y)) 
                    vel.y += speed;
                if (WasPressed(MINUS_Y)) 
                    vel.y -= speed;
                if (WasPressed(PLUS_Z)) 
                    vel.z += speed;
                if (WasPressed(MINUS_Z)) 
                    vel.z -= speed;
                colour += vel * realTimeDelta;
                colour = ClampToRange(colour, 0.0f, 1.0f);

                if (WasClicked(RESET)) 
                {
                    colour = Vector3(1,1,1) * 0.5f;
                }
                box.SetColour(colour);
            }
        }
    }

    for (size_t iBox = 0 ; iBox != os.mBoxes.size() ; ++iBox)
    {
        os.mBoxes[iBox].mObject->SetWireframe(iBox == (size_t) mSelectedObject);
        os.mBoxes[iBox].mObject->SetVisible(os.mBoxes[iBox].mVisible || os.mForceAllVisible);
        os.mBoxes[iBox].mObject->SetShadowVisible(os.mBoxes[iBox].mShadow);
    }

    UpdateButtons();
}
