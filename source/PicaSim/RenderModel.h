#ifndef RENDERMODEL_H
#define RENDERMODEL_H

#include "3ds.h"
#include "ac3d.h"
#include <string>
#include <vector>

struct ShaderProgramModelInfo
{
    int colourLoc;
    int mvpLoc;
    int normalMatrixLoc;
};

typedef std::vector<std::string> NamedComponents;

class RenderModel
{
public:
    RenderModel();
    ~RenderModel();

    /// Initialise with a 3DS model
    void Init(
        const ThreeDSModel& model, 
        const Vector3& offset, 
        float colourOffset, 
        const Vector3& modelScale, 
        bool cullBackFaces, 
        const Colour& unsetColour);

    /// Initialise with an AC3D model
    void Init(
        const ACModel& model,
        const std::string& modelFile, 
        const Vector3& offset, 
        float colourOffset, 
        const Vector3& modelScale, 
        bool cullBackFaces, 
        bool rgb565, // Set to convert textures to RGB565
        const NamedComponents* namedComponents = 0);

    struct Box
    {
        Transform mTM;
        Vector3 mExtents;
        Vector4 mColour;
    };
    typedef std::vector<Box> Boxes;

    void Init(const Boxes& boxes);

    /// Allow modification of a component transform
    void SetComponentTM(const std::string& componentName, const Transform& tm);

    /// Allow modification of the component alpha to blend it out
    void SetAlphaScale(const std::string& componentName, float alphaScale);

    // clears any loaded model
    void Terminate();
    /// if colour is non-zero then it is used for the colour. If forceColour is true then all texture/lighting etc is disabled
    void Render(const Vector4* colour, bool forceColour, bool separateSpecular) const;

    bool IsCreated() const;

    bool PartRenderPre(
        const Vector4*          colour, 
        const bool              forceColour, 
        ShaderProgramModelInfo& shaderInfo, 
        int                     componentIndex, 
        bool                    separateSpecular) const;
    void PartRender(const Vector4* colour, bool forceColour, ShaderProgramModelInfo& shaderInfo, int componentIndex) const;
    void PartRenderPost(const Vector4* colour, bool forceColour, int componentIndex) const;

    void CreateVertexBuffers();

    float GetBoundingRadius() const {return mBoundingRadius;}

    struct UntexturedVertex
    {
        Vector3 mPoint;
        Vector3 mNormal;
        GLfloat mColour[4];
    };
    typedef std::vector<UntexturedVertex> UntexturedVertices;

    struct TexturedVertex
    {
        Vector3 mPoint;
        Vector3 mNormal;
        GLfloat mTexCoord[2];
    };
    typedef std::vector<TexturedVertex> TexturedVertices;

    struct Component
    {
        Component(const std::string& name) : mVertexBuffer(0), mName(name), mTM(Transform::g_Identity), mTexture(0), mAlphaScale(1.0f) {}
        std::string        mName;
        std::string        mTextureName;
        UntexturedVertices mUntexturedVertices;
        TexturedVertices   mTexturedVertices;
        Transform          mTM;
        GLuint             mVertexBuffer;
        Texture*           mTexture;
        float              mAlphaScale;
    };
    typedef std::vector<Component> Components;

private:
    void CalculateBoundingRadius();

    /// Loads or returns a loaded/cached texture using the textureName and path from modelFile. 
    /// Converts to RGB565 if necessary, and also applies a HSV colour offset if desired.
    Texture* getTextureID(const std::string& textureName, const std::string& modelFile, bool rgb565, float colourOffset);

    Components mComponents;
    bool mCullBackFaces;

    typedef std::map<std::string, Texture*> Textures;
    Textures mTextures;

    float mBoundingRadius;

    mutable bool mDoingSeparateSpecularPass;
};

#endif
