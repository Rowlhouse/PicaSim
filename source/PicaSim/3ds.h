#ifndef _3DS_H
#define _3DS_H

#include "Framework.h"

#include <stdio.h>
#include <vector>

//================ Some classes that get used ======================//

// This is our face structure.  This is is used for indexing into the vertex 
// and texture coordinate arrays.  From this information we know which vertices
// from our vertex array go to which face, along with the correct texture coordinates.
struct ThreeDSFace
{
    short vertIndex[3];      // indicies for the verts that make up this triangle
    short coordIndex[3];      // indicies for the tex coords to texture this face
};

// This holds the information for a material.  It may be a texture map of a color.
// Some of these are not used, but I left them because you will want to eventually
// read in the UV tile ratio and the UV tile offset for some models.
struct ThreeDSMaterialInfo
{
    ThreeDSMaterialInfo() : uTile(0), vTile(0), uOffset(0), vOffset(0) {strFile[0] = 0;};
    ~ThreeDSMaterialInfo() {}
    char  strName[255];      // The texture name
    char  strFile[255];      // The texture file name (If this is set it's a texture map)
    GLubyte  ambientColor[4];        // The color of the object (R, G, B, A)
    GLubyte  diffuseColor[4];        // The color of the object (R, G, B, A)
    GLubyte  specularColor[4];        // The color of the object (R, G, B, A)
    float uTile;        // u tiling of texture  (Currently not used)
    float vTile;        // v tiling of texture  (Currently not used)
    float uOffset;          // u offset of texture  (Currently not used)
    float vOffset;        // v offset of texture  (Currently not used)
} ;

// This holds all the information for our model/scene. 
// You should eventually turn into a robust class that 
// has loading/drawing/querying functions like:
// LoadModel(...); DrawObject(...); DrawModel(...); DestroyModel(...);
struct ThreeDSObject 
{
    ThreeDSObject() : numOfVerts(0), numOfFaces(0), numTexVertex(0), materialID(0),
        bHasTexture(false), pVerts(0), pNormals(0), pTexVerts(0), pFaces(0) {}
    ~ThreeDSObject() {delete [] pVerts; delete [] pNormals; delete [] pTexVerts; delete [] pFaces;}
    short  numOfVerts;      // The number of verts in the model
    short  numOfFaces;      // The number of faces in the model
    short  numTexVertex;    // The number of texture coordinates
    int  materialID;      // The texture ID to use, which is the index into our texture array
    bool bHasTexture;      // This is TRUE if there is a texture map for this object
    char strName[255];      // The name of the object
    Vector3  *pVerts;      // The object's vertices
    Vector3  *pNormals;    // The object's normals
    Vector2  *pTexVerts;    // The texture's UV coordinates
    ThreeDSFace *pFaces;        // The faces information of the object
};

// This holds our model information.  This should also turn into a robust class.
// We use STL's (Standard Template Library) vector class to ease our link list burdens. :)
struct ThreeDSModel 
{
    ThreeDSModel() {}
    ~ThreeDSModel();
    std::vector<ThreeDSMaterialInfo*> mMaterials;  // The list of material information (Textures and colors)
    std::vector<ThreeDSObject*> mObjects;      // The object list for our model
};

//==================================================================//

// This class handles all of the loading code
class CLoad3DS
{
public:
    CLoad3DS();        // This inits the data members
    ~CLoad3DS();

    // This is the function that you call to load the 3DS
    bool Import3DS(ThreeDSModel *pModel, const char *strFileName);

private:
    // This holds the chunk info
    struct tChunk
    {
        unsigned short int ID;     // The chunk's ID  
        unsigned int length;     // The length of the chunk
        unsigned int bytesRead;     // The amount of bytes read within that chunk
    };

    void ShowChunk(tChunk * pChunk);

    // This reads in a string and saves it in the char array passed in
    int GetString(char *);

    // This reads the next chunk
    void ReadChunk(tChunk *);

    // This reads the next large chunk
    void ProcessNextChunk(ThreeDSModel *pModel, tChunk *);

    // This reads the object chunks
    void ProcessNextObjectChunk(ThreeDSModel *pModel, ThreeDSObject *pObject, tChunk *);

    // This reads the material chunks
    void ProcessNextMaterialChunk(ThreeDSModel *pModel, tChunk *);

    // This reads the RGB value for the object's color
    void ReadColorChunk(GLubyte color[4], tChunk *pChunk);

    // This reads the objects vertices
    void ReadVertices(ThreeDSObject *pObject, tChunk *);

    // This reads the objects face information
    void ReadVertexIndices(ThreeDSObject *pObject, tChunk *);

    // This reads the texture coodinates of the object
    void ReadUVCoordinates(ThreeDSObject *pObject, tChunk *);

    // This reads in the material name assigned to the object and sets the materialID
    void ReadObjectMaterial(ThreeDSModel *pModel, ThreeDSObject *pObject, tChunk *pPreviousChunk);

    // This computes the vertex normals for the object (used for lighting)
    void ComputeNormals(ThreeDSModel *pModel);

    // This frees memory and closes the file
    void CleanUp();

    // The file pointer
    FILE *m_FilePointer;

    // These are used through the loading process to hold the chunk information
    tChunk *m_CurrentChunk;
    tChunk *m_TempChunk;
};


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
// 
// This file is created in the hopes that you can just plug it into
// your code easily.  You will probably want to query more chunks
// though for animation, etc..
//
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
//
