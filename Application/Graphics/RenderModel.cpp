#include "RenderModel.h"
#include "Graphics.h"
#include "Helpers.h"
#include "Shaders.h"
#include "Trace.h"
#include "ShaderManager.h"

//----------------------------------------------------------------------------------------------------------------------
RenderModel::RenderModel()
{
  mDoingSeparateSpecularPass = false;
  mCullBackFaces = true;
  mBoundingRadius = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
RenderModel::~RenderModel()
{
  Terminate();
}

//----------------------------------------------------------------------------------------------------------------------
void RenderModel::Terminate()
{
  TRACE_METHOD_ONLY(1);
  for (Components::iterator it = mComponents.begin() ; it != mComponents.end() ; ++it)
  {
    if (it->mVertexBuffer)
      glDeleteBuffers(1, &it->mVertexBuffer);
  }
  mComponents.clear();
  for (Textures::iterator it = mTextures.begin() ; it != mTextures.end() ; ++it)
  {
    Texture* texture = it->second;
    TRACE_FILE_IF(1) TRACE("Releasing texture %s id %d", it->first.c_str(), texture->m_HWID);
    delete it->second;
  }
  mTextures.clear();
}



//----------------------------------------------------------------------------------------------------------------------
void RenderModel::Init(const ThreeDSModel& model, const Vector3& offset, float colourOffset, const Vector3& modelScale, bool cullBackFaces, const Colour& unsetColour)
{
  TRACE_METHOD_ONLY(1);
  Terminate();

  mCullBackFaces = cullBackFaces;

  mComponents.push_back(Component("3DS"));
  Component* component = &mComponents.back();

  for(size_t i = 0; i != model.mObjects.size(); i++)
  {
    // Get the current object that we are displaying
    const ThreeDSObject *pObject = model.mObjects[i];
    
    if (pObject->bHasTexture) 
      continue;

    // Go through all of the faces (polygons) of the object and draw them
    for (int j = 0; j < pObject->numOfFaces; j++)
    {
      bool degenerate = false;
      // Go through each corner of the triangle and draw it.
      Vector3 faceNormal(0,0,0);
      if (!pObject->pNormals)
      {
        int i0 = pObject->pFaces[j].vertIndex[0];
        int i1 = pObject->pFaces[j].vertIndex[1];
        int i2 = pObject->pFaces[j].vertIndex[2];
        Vector3 v0 = pObject->pVerts[i0];
        Vector3 v1 = pObject->pVerts[i1];
        Vector3 v2 = pObject->pVerts[i2];
        faceNormal = (v1-v0).Cross(v2-v0);
        if (faceNormal.IsZero())
          degenerate = true;
        else
          faceNormal.Normalise();
      }

      if (degenerate)
        continue;

      for (int whichVertex = 0; whichVertex < 3; whichVertex++)
      {
        // Get the index for each point of the face
        int index = pObject->pFaces[j].vertIndex[whichVertex];
        
        UntexturedVertex untexturedVertex;

        IwAssert(ROWLHOUSE, index < pObject->numOfVerts);
        untexturedVertex.mPoint = offset + ComponentMultiply(pObject->pVerts[index], modelScale);

        if (pObject->pNormals)
          untexturedVertex.mNormal = pObject->pNormals[index];
        else
          untexturedVertex.mNormal = faceNormal;

        if (!model.mMaterials.empty() && pObject->materialID >= 0) 
        {
          GLubyte *pColor = 
            model.mMaterials[pObject->materialID]->diffuseColor;
          untexturedVertex.mColour[0] = pColor[0]/255.0f;
          untexturedVertex.mColour[1] = pColor[1]/255.0f;
          untexturedVertex.mColour[2] = pColor[2]/255.0f;
          untexturedVertex.mColour[3] = pColor[3]/255.0f;
        }
        else
        {
          untexturedVertex.mColour[0] = unsetColour.r/255.0f;
          untexturedVertex.mColour[1] = unsetColour.g/255.0f;
          untexturedVertex.mColour[2] = unsetColour.b/255.0f;
          untexturedVertex.mColour[3] = unsetColour.a/255.0f;
        }
        OffsetColour(untexturedVertex.mColour, colourOffset);
        component->mUntexturedVertices.push_back(untexturedVertex);
      }
    }
  }
  CreateVertexBuffers();
  CalculateBoundingRadius();
}

//----------------------------------------------------------------------------------------------------------------------
void RenderModel::CreateVertexBuffers()
{
  for (Components::iterator it = mComponents.begin() ; it != mComponents.end() ; ++it)
  {
    glGenBuffers(1, &it->mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, it->mVertexBuffer);

    // allocate enough space for the VBO
    if (it->mTexture == 0)
    {
      glBufferData(GL_ARRAY_BUFFER, 
        sizeof(UntexturedVertex) * it->mUntexturedVertices.size(), &it->mUntexturedVertices[0], GL_STATIC_DRAW);
    }
    else
    {
      glBufferData(GL_ARRAY_BUFFER, 
        sizeof(TexturedVertex) * it->mTexturedVertices.size(), &it->mTexturedVertices[0], GL_STATIC_DRAW);
    }
  }

  // reset binding
  glBindBuffer(GL_ARRAY_BUFFER,0);
}

bool operator==(const RenderModel::Component& c, const std::string& name)
{
  return c.mName == name;
}

//----------------------------------------------------------------------------------------------------------------------
Texture* RenderModel::getTextureID(const std::string& textureName, const std::string& modelFile, bool rgb565, float colourOffset)
{
  std::string textureFile;
  std::string::size_type lastSlash = modelFile.find_last_of("/");
  if (lastSlash != std::string::npos)
    textureFile = modelFile.substr(0, lastSlash) + "/" + textureName;
  else
    textureFile = textureName;

  Textures::iterator it = mTextures.find(textureFile);
  if (it == mTextures.end())
  {
    Texture* texture = new Texture;
    LoadTextureFromFile(*texture, textureFile.c_str(), colourOffset);

    texture->SetClamping(false);
    texture->SetMipMapping(true);
    texture->SetModifiable(false);
    if (rgb565)
      texture->SetFormatHW(CIwImage::RGB_565);
    texture->Upload();
    TRACE_FILE_IF(1) TRACE("Uploaded texture %s id %d", textureFile.c_str(), texture->m_HWID);

    if (texture->GetFlags() & Texture::UPLOADED_F)
    {
      // Letting Marmalade handle the filtering/clamping doesn't seem to work... in particular clamping doesn't, so 
      // force it manually and re-upload
      glBindTexture(GL_TEXTURE_2D, texture->m_HWID);

      // filtering
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      texture->Upload();
      TRACE_FILE_IF(1) TRACE("Uploaded texture %s id %d", textureFile.c_str(), texture->m_HWID);
    }
    else
    {
      TRACE_FILE_IF(1) TRACE("Failed to upload texture %s", textureFile.c_str());
      delete texture;
      return 0;
    }

    mTextures[textureFile] = texture;
    return texture;
  }
  else
  {
    return it->second;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RenderModel::Init(
  const ACModel& model, const std::string& modelFile, const Vector3& offset, float colourOffset, 
  const Vector3& modelScale, bool cullBackFaces, bool rgb565, const NamedComponents* namedComponents)
{
  TRACE_METHOD_ONLY(1);
  Terminate();

  mCullBackFaces = cullBackFaces;

  for(size_t i = 0; i != model.mObject.mObjects.size(); i++)
  {
    // Get the current object that we are displaying
    const ACObject& object = model.mObject.mObjects[i];
    
    if (object.type == OBJECT_NORMAL)
    {
      Component* component = 0;
      if (namedComponents)
      {
        bool isNamed = std::find(namedComponents->begin(), namedComponents->end(), object.name) != namedComponents->end();
        if (isNamed)
        {
          Components::iterator existingComponent = std::find(mComponents.begin(), mComponents.end(), object.name);
          if (existingComponent == mComponents.end())
          {
            mComponents.push_back(Component(object.name));
            component = &mComponents.back();
          }
          else
          {
            component = &(*existingComponent);
          }
        }
        else
        {
          // When texturing there's one component per texture
          for (Components::iterator it = mComponents.begin() ; it != mComponents.end() ; ++it)
          {
            Component& c = *it;
            bool isNamed = std::find(namedComponents->begin(), namedComponents->end(), c.mName) != namedComponents->end();
            if (!isNamed)
            {
              if (it->mTextureName == object.textureName)
              {
                component = &(*it);
                break;
              }
            }
          }
          if (!component)
          {
            mComponents.push_back(Component(object.name));
            component = &mComponents.back();
          }
        }
      }
      else
      {
        // When texturing there's one component per texture
        for (Components::iterator it = mComponents.begin() ; it != mComponents.end() ; ++it)
        {
          if (it->mTextureName == object.textureName)
          {
            component = &(*it);
            break;
          }
        }
        if (!component)
        {
          mComponents.push_back(Component(object.name));
          component = &mComponents.back();
        }
      }

      if (!object.textureName.empty())
      {
        component->mTexture = getTextureID(object.textureName, modelFile, rgb565, colourOffset);
        component->mTextureName = object.textureName;

      }
      // Go through all of the faces (polygons) of the object and draw them
      for (size_t j = 0; j < object.surfaces.size(); j++)
      {
        const ACSurface& surface = object.surfaces[j];

        Vector3 faceNormal(surface.normal.x, surface.normal.y, surface.normal.z);

        if (surface.vertref.size() < 3)
          continue;

        bool degenerate = false;
        if (faceNormal.GetLengthSquared() < 0.9f)
        {
          const Vector3& v0 = object.vertices[surface.vertref[0]];
          const Vector3& v1 = object.vertices[surface.vertref[1]];
          const Vector3& v2 = object.vertices[surface.vertref[2]];
          faceNormal = (v1-v0).Cross(v2-v0);
          if (faceNormal.IsZero())
            degenerate = true;
          else
            faceNormal.Normalise();
        }

        if (degenerate)
          continue;

        size_t numTriangles = surface.vertref.size() - 2;
        if (component->mTexture)
        {
          for (size_t iTriangle = 0 ; iTriangle != numTriangles ; ++iTriangle)
          {
            for (size_t iVertex = 0 ; iVertex != 3 ; ++iVertex)
            {
              size_t surfaceVertexIndex = (iVertex == 0) ? iVertex : iVertex + iTriangle;
              size_t iV = surface.vertref[surfaceVertexIndex];

              TexturedVertex texturedVertex;

              texturedVertex.mPoint = object.vertices[iV];
              texturedVertex.mNormal = surface.vertexNormals[surfaceVertexIndex];

              const ACMaterial& mat = model.mMaterials[surface.mat];
              texturedVertex.mTexCoord[0] = surface.uvs[surfaceVertexIndex].u;
              texturedVertex.mTexCoord[1] = 1.0f - surface.uvs[surfaceVertexIndex].v;

              texturedVertex.mPoint += Vector3(object.loc.x, object.loc.y, object.loc.z);
              texturedVertex.mPoint = ComponentMultiply(texturedVertex.mPoint, modelScale) + offset;

              component->mTexturedVertices.push_back(texturedVertex);
            }
          }
        }
        else
        {
          for (size_t iTriangle = 0 ; iTriangle != numTriangles ; ++iTriangle)
          {
            for (size_t iVertex = 0 ; iVertex != 3 ; ++iVertex)
            {
              size_t surfaceVertexIndex = (iVertex == 0) ? iVertex : iVertex + iTriangle;
              size_t iV = surface.vertref[surfaceVertexIndex];

              UntexturedVertex untexturedVertex;

              untexturedVertex.mPoint = object.vertices[iV];
              untexturedVertex.mNormal = surface.vertexNormals[surfaceVertexIndex];

              const ACMaterial& mat = model.mMaterials[surface.mat];
              untexturedVertex.mColour[0] = mat.rgb.r;
              untexturedVertex.mColour[1] = mat.rgb.g;
              untexturedVertex.mColour[2] = mat.rgb.b;
              untexturedVertex.mColour[3] = mat.rgb.a;

              untexturedVertex.mPoint += Vector3(object.loc.x, object.loc.y, object.loc.z);

              untexturedVertex.mPoint = ComponentMultiply(untexturedVertex.mPoint, modelScale) + offset;

              OffsetColour(untexturedVertex.mColour, colourOffset);

              component->mUntexturedVertices.push_back(untexturedVertex);
            }
          }
        }
      }
    }
    else
    {
      TRACE("Unhandled Object: name = %s type = %d", object.name.c_str(), object.type);
    }
  }
  CreateVertexBuffers();
  CalculateBoundingRadius();
}

//----------------------------------------------------------------------------------------------------------------------
void RenderModel::CalculateBoundingRadius() 
{
  mBoundingRadius = 0.0f;
  for (size_t iComponent = 0 ; iComponent != mComponents.size() ; ++iComponent)
  {
    const Component& component = mComponents[iComponent];
    for (size_t i = 0 ; i != component.mTexturedVertices.size() ; ++i)
    {
      const Vector3& localPos = component.mTexturedVertices[i].mPoint;
      Vector3 p = component.mTM.TransformVec(localPos);
      float r = p.GetLength();
      if (r > mBoundingRadius)
        mBoundingRadius = r;
    }
    for (size_t i = 0 ; i != component.mUntexturedVertices.size() ; ++i)
    {
      const Vector3& localPos = component.mUntexturedVertices[i].mPoint;
      Vector3 p = component.mTM.TransformVec(localPos);
      float r = p.GetLength();
      if (r > mBoundingRadius)
        mBoundingRadius = r;
    }
  }
  mBoundingRadius *= 1.05f;
}

//----------------------------------------------------------------------------------------------------------------------
void RenderModel::Init(const Boxes& boxes)
{
  TRACE_METHOD_ONLY(1);
  Terminate();

  mCullBackFaces = true;

  // First component is the static/unnamed stuff
  mComponents.push_back(Component("Fixed"));
  Component* component = &mComponents.back();

  for(size_t i = 0; i != boxes.size(); i++)
  {
    const Box& box = boxes[i];
    Vector3 h = box.mExtents * 0.5f;

    UntexturedVertex untexturedVertex;
    untexturedVertex.mColour[0] = box.mColour.x;
    untexturedVertex.mColour[1] = box.mColour.y;
    untexturedVertex.mColour[2] = box.mColour.z;
    untexturedVertex.mColour[3] = 1.0f;

    // Top
    untexturedVertex.mNormal = box.mTM.RotateVec(Vector3(0.0f, 0.0f, 1.0f));
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x, -h.y, h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x,  h.y, h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x,  h.y, h.z)); component->mUntexturedVertices.push_back(untexturedVertex);

    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x, -h.y, h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x,  h.y, h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x, -h.y, h.z)); component->mUntexturedVertices.push_back(untexturedVertex);

    // Bottom
    untexturedVertex.mNormal = box.mTM.RotateVec(Vector3(0.0f, 0.0f, -1.0f));
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x, -h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x,  h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x,  h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);

    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x, -h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x,  h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x, -h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);

    // X Side
    untexturedVertex.mNormal = box.mTM.RotateVec(Vector3(1.0f, 0.0f, 0.0f));
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x, -h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x,  h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x,  h.y,  h.z)); component->mUntexturedVertices.push_back(untexturedVertex);

    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x, -h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x,  h.y,  h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x, -h.y,  h.z)); component->mUntexturedVertices.push_back(untexturedVertex);

    // X Opposite Side
    untexturedVertex.mNormal = box.mTM.RotateVec(Vector3(-1.0f, 0.0f, 0.0f));
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x, -h.y,  h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x,  h.y,  h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x,  h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);

    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x, -h.y,  h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x,  h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x, -h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);

    // Y Side
    untexturedVertex.mNormal = box.mTM.RotateVec(Vector3(0.0f, 1.0f, 0.0f));
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x,  h.y,  h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x,  h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x,  h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);

    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x,  h.y,  h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x,  h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x,  h.y, +h.z)); component->mUntexturedVertices.push_back(untexturedVertex);

    // Y Opposite Side
    untexturedVertex.mNormal = box.mTM.RotateVec(Vector3(0.0f, -1.0f, 0.0f));
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x, -h.y,  h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x, -h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x, -h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);

    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3(-h.x, -h.y,  h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x, -h.y, -h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
    untexturedVertex.mPoint = box.mTM.TransformVec(Vector3( h.x, -h.y, +h.z)); component->mUntexturedVertices.push_back(untexturedVertex);
  }
  CreateVertexBuffers();
  CalculateBoundingRadius();
}


//----------------------------------------------------------------------------------------------------------------------
bool RenderModel::IsCreated() const
{
  return !mComponents.empty();
}

//----------------------------------------------------------------------------------------------------------------------
void RenderModel::SetComponentTM(const std::string& componentName, const Transform& tm)
{
  for (Components::iterator it = mComponents.begin() ; it != mComponents.end() ; ++it)
  {
    Component& component = *it;
    if (component.mName == componentName)
    {
      component.mTM = tm;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RenderModel::SetAlphaScale(const std::string& componentName, float alphaScale)
{
  for (Components::iterator it = mComponents.begin() ; it != mComponents.end() ; ++it)
  {
    Component& component = *it;
    if (component.mName == componentName)
    {
      component.mAlphaScale = alphaScale;

      for (UntexturedVertices::iterator it = component.mUntexturedVertices.begin() ; it != component.mUntexturedVertices.end() ; ++it)
      {
        it->mColour[3] = alphaScale;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RenderModel::Render(const Vector4* colour, bool forceColour, bool separateSpecular) const
{
  if (!IsCreated())
    return;

  // To get specular to be applied to textured models in GL1 we have to render the model twice - 
  // the first time without specular, and the second time over the top. This is because ES 1 
  // does the lighting and then multiplies the result by the texture colour. This means a black texture kills specular.

  GLVec4 specularColour;
  if (gGLVersion == 1 && separateSpecular)
  {
    GLfloat zeros[] = {0, 0, 0, 0};
    glGetLightfv(GL_LIGHT0, GL_SPECULAR, specularColour);
    glLightfv(GL_LIGHT0, GL_SPECULAR, zeros);
  }

  Vector4 col(1, 1, 1, 0);

  int index = 0;
  for (Components::const_iterator it = mComponents.begin() ; it != mComponents.end() ; ++it, ++index)
  {
    const Component& component = *it;
    const Vector4* thisColour = colour;

    if (component.mAlphaScale < 1.0f)
    {
      col.w = component.mAlphaScale;
      thisColour = &col;
    }

    esPushMatrix();
    GLMat44 glTM;
    ConvertTransformToGLMat44(component.mTM, glTM);
    esMultMatrixf(&glTM[0][0]);

    ShaderProgramModelInfo shaderInfo;
    if (PartRenderPre(thisColour, forceColour, shaderInfo, index, separateSpecular))
    {
      PartRender(thisColour, forceColour, shaderInfo, index);
      PartRenderPost(thisColour, forceColour, index);
    }
    else
    {
      glDisable(GL_BLEND);
      glDisable(GL_CULL_FACE); 
      glDepthMask(GL_TRUE);
    }
    esPopMatrix();
  }

  if (gGLVersion == 1 && separateSpecular)
  {
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularColour);
  }


  if (gGLVersion == 1 && separateSpecular)
  {
    mDoingSeparateSpecularPass = true;
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHT3);
    glDisable(GL_LIGHT4);

    GLfloat zeros[] = {0, 0, 0, 0};
    GLVec4 diffuseColour;
    GLVec4 ambientColour;
    glGetLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColour);
    glGetLightfv(GL_LIGHT0, GL_AMBIENT, ambientColour);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, zeros);
    glLightfv(GL_LIGHT0, GL_AMBIENT, zeros);

    Vector4 col(1.0f, 1.0f, 1.0f, -1.0f);
    Render(&col, true, false);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColour);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColour);

    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);
    glEnable(GL_LIGHT4);
    mDoingSeparateSpecularPass = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool RenderModel::PartRenderPre(const Vector4* colour, bool forceColour, ShaderProgramModelInfo& shaderInfo, int componentIndex, bool separateSpecular) const
{
  const Component& component = mComponents[componentIndex];
  if (component.mTexture == 0 && component.mUntexturedVertices.empty())
    return false;
  if (component.mTexture != 0 && component.mTexturedVertices.empty())
    return false;

  Texture* texture = forceColour ? 0 : component.mTexture;
  const TexturedModelShader* texturedModelShader = separateSpecular ? 
    (TexturedModelShader*) ShaderManager::GetInstance().GetShader(SHADER_TEXTUREDMODELSEPARATESPECULAR) :
    (TexturedModelShader*) ShaderManager::GetInstance().GetShader(SHADER_TEXTUREDMODEL);
  const ModelShader* modelShader = (ModelShader*) ShaderManager::GetInstance().GetShader(SHADER_MODEL);

  int positionLoc=-1, normalLoc=-1, texCoordLoc=-1;
  const LightShaderInfo (* lightShaderInfo)[5] = 0;

  if (colour)
  {
    if (mDoingSeparateSpecularPass && colour->w < 1.0f && colour->w >= 0.0f)
      return false;

    glEnable(GL_BLEND);
    if (colour->w < 0.0f) // signal for doing the separate specular pass
    {
      glBlendFunc(GL_ONE, GL_ONE);
    }
    else
    {
      if (separateSpecular)
      {
        glDepthMask(GL_FALSE);
      }
      else if (colour->w == 0.0f)
      {
        return false;
      }
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
  }
  else
  {
    glDisable(GL_BLEND);
  }

  float specularAmount1 = 0.6f;
  float specularExponent1 = 15.0f;

  float specularAmount2 = 0.5f;
  float specularExponent2 = 20.0f;

  if (gGLVersion == 1)
  {
    if (texture)
    {
      glEnable(GL_TEXTURE_2D);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    else
    {
      glDisable(GL_TEXTURE_2D);
      if (!colour)
        glEnableClientState(GL_COLOR_ARRAY);
      else
        glColor4f(colour->x, colour->y, colour->z, colour->w);
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    GLfloat s[] = {specularAmount1, specularAmount1, specularAmount1, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, specularExponent1); // smooth surface = large numbers = small highlights

    // Overwrites the actual material for ambient and diffuse, front and back (no glColorMaterial in ogl es)
    if (!forceColour)
      glEnable(GL_COLOR_MATERIAL);
  }
  else
  {
    if (texture)
    {
      texturedModelShader->Use();

      shaderInfo.colourLoc = texturedModelShader->a_colour;
      shaderInfo.mvpLoc = texturedModelShader->u_mvpMatrix;
      shaderInfo.normalMatrixLoc = texturedModelShader->u_normalMatrix;

      positionLoc = texturedModelShader->a_position;
      normalLoc = texturedModelShader->a_normal;
      texCoordLoc = texturedModelShader->a_texCoord;

      lightShaderInfo = &texturedModelShader->lightShaderInfo;

      glUniform1i(texturedModelShader->u_texture, 0);
      glUniform1f(texturedModelShader->u_texBias, -0.5f);
      glUniform1f(texturedModelShader->u_specularAmount, specularAmount2);
      glUniform1f(texturedModelShader->u_specularExponent, specularExponent2);
    }
    else
    {
      modelShader->Use();

      shaderInfo.colourLoc = modelShader->a_colour;
      shaderInfo.mvpLoc = modelShader->u_mvpMatrix;
      shaderInfo.normalMatrixLoc = modelShader->u_normalMatrix;

      positionLoc = modelShader->a_position;
      normalLoc = modelShader->a_normal;

      lightShaderInfo = &modelShader->lightShaderInfo;

      glUniform1f(texturedModelShader->u_specularAmount, specularAmount2);
      glUniform1f(modelShader->u_specularExponent, specularExponent2);
    }
  }

  if (texture)
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->m_HWID);
  }

  if (mCullBackFaces)
  {
    glEnable(GL_CULL_FACE); 
    glCullFace(GL_BACK);
  }

  size_t elementSize = component.mTexture == 0 ? sizeof(UntexturedVertex) : sizeof(TexturedVertex);

  size_t start = component.mTexture == 0 ? (size_t) (&component.mUntexturedVertices[0]) : (size_t) (&component.mTexturedVertices[0]);
  if (component.mVertexBuffer)
  {
    glBindBuffer(GL_ARRAY_BUFFER, component.mVertexBuffer);
    start = 0;
  }

  if (gGLVersion == 1)
  {
    glVertexPointer(3, GL_FLOAT, elementSize, (GLvoid*) start);
    glNormalPointer(GL_FLOAT, elementSize, (GLvoid*) (start + sizeof(Vector3)));
    if (texture)
    {
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      glTexCoordPointer(2, GL_FLOAT, elementSize, (GLvoid*) (start + sizeof(Vector3) + sizeof(Vector3)));
    }
    else
    {
      if (!colour)
        glColorPointer(4, GL_FLOAT, elementSize, (GLvoid*) (start + sizeof(Vector3) + sizeof(Vector3)));
    }
  }
  else
  {
    glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, elementSize, (GLvoid*) start);
    glEnableVertexAttribArray(positionLoc);

    if (forceColour)
    {
      glDisableVertexAttribArray(normalLoc);
      glVertexAttrib3f(modelShader->a_normal, 0, 0, 0.0f);
    }
    else
    {
      glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, elementSize, (GLvoid*) (start + sizeof(Vector3)));
      glEnableVertexAttribArray(normalLoc);
    }

    if (texture)
    {
      glDisableVertexAttribArray(shaderInfo.colourLoc);
      glVertexAttrib4f(shaderInfo.colourLoc, 1.0f, 1.0f, 1.0f, 1.0f);

      glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, elementSize, (GLvoid*) (start + sizeof(Vector3) + sizeof(Vector3)));
      glEnableVertexAttribArray(texCoordLoc);
    }
    else
    {
      if (!colour && shaderInfo.colourLoc != -1)
      {
        glVertexAttribPointer(shaderInfo.colourLoc, 4, GL_FLOAT, GL_FALSE, elementSize, (GLvoid*) (start + sizeof(Vector3) + sizeof(Vector3)));
        glEnableVertexAttribArray(shaderInfo.colourLoc);
      }
    }
    esSetLighting(*lightShaderInfo);
    if (forceColour)
    {
      for (int i = 0 ; i != 5 ; ++i)
      {
        if (i == 0)
          glUniform4f((*lightShaderInfo)[i].u_lightAmbientColour, 1.0f, 1.0f, 1.0f, 1.0f);
        else
          glUniform4f((*lightShaderInfo)[i].u_lightAmbientColour, 0.0f, 0.0f, 0.0f, 1.0f);
        glUniform4f((*lightShaderInfo)[i].u_lightDiffuseColour, 0.0f, 0.0f, 0.0f, 1.0f);
        glUniform4f((*lightShaderInfo)[i].u_lightSpecularColour, 0.0f, 0.0f, 0.0f, 1.0f);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void RenderModel::PartRender(const Vector4* colour, bool forceColour, ShaderProgramModelInfo& shaderInfo, int componentIndex) const
{
  const Component& component = mComponents[componentIndex];

  if (colour)
  {
    if (gGLVersion == 1)
    {
      glColor4f(colour->x, colour->y, colour->z, colour->w);
    }
    else
    {
      glVertexAttrib4fv(shaderInfo.colourLoc, &colour->x);
      glDisableVertexAttribArray(shaderInfo.colourLoc);
    }
  }
  esSetModelViewProjectionAndNormalMatrix(shaderInfo.mvpLoc, shaderInfo.normalMatrixLoc);
  if (component.mTexture)
    glDrawArrays(GL_TRIANGLES, 0, component.mTexturedVertices.size());
  else
    glDrawArrays(GL_TRIANGLES, 0, component.mUntexturedVertices.size());
}

//----------------------------------------------------------------------------------------------------------------------
void RenderModel::PartRenderPost(const Vector4* colour, bool forceColour, int componentIndex) const
{
  const Component& component = mComponents[componentIndex];

  Texture* texture = forceColour ? 0 : component.mTexture;

  if (gGLVersion == 1)
  {
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
  }

  if (texture)
  {
    glDisable(GL_TEXTURE_2D);
  }

 if (component.mVertexBuffer)
   glBindBuffer(GL_ARRAY_BUFFER, 0);

 glDisable(GL_BLEND);
 glDisable(GL_CULL_FACE); 
 glDepthMask(GL_TRUE);
}

