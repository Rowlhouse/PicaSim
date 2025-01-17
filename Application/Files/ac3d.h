//#include <string>
//#include <vector>

//#include "Helpers.h"

#include "Vecteurs.h"
#include <vector>
struct ACUV
{
  float u, v;
};


struct ACSurface
{
  std::vector<int> vertref;
  std::vector<ACUV> uvs;
  std::vector<Vector3> vertexNormals;
  Vector3 normal;
  int flags;
  int mat;
};

struct ACObject
{
  Vector3 loc;
  std::string name;
  std::string data;
  std::string url;
  std::vector<Vector3> vertices;

  std::vector<ACSurface> surfaces;
  float texture_repeat_x, texture_repeat_y;
  float texture_offset_x, texture_offset_y;

  float creaseAngle;
  std::vector<ACObject> mObjects;
  float matrix[9];
  int type;
  std::string textureName;
};

struct ACCol
{
  float r, g, b, a;
};

struct ACMaterial
{
  ACCol rgb; /* diffuse **/
  ACCol ambient;
  ACCol specular;
  ACCol emissive;
  float shininess;
  float transparency;
  std::string name;
};

typedef std::vector<ACMaterial> Materials;

struct ACModel
{
  ACObject mObject;
  Materials mMaterials;
};

struct ACImage
{
  unsigned short width, height, depth;    
  void *data; 
  int index;
  std::string name;
  int amask;
  std::string origname; /** do not set - set automatically in texture_read function **/

};

#define OBJECT_WORLD 999
#define OBJECT_NORMAL 0
#define OBJECT_GROUP 1
#define OBJECT_LIGHT 2

#define SURFACE_SHADED (1<<4)
#define SURFACE_TWOSIDED (1<<5)

#define SURFACE_TYPE_POLYGON (0)
#define SURFACE_TYPE_CLOSEDLINE (1)
#define SURFACE_TYPE_LINE (2)

bool ACLoadModel(ACModel& model, const char *fname);

ACImage *ACGetTexture(int ind);
int ACLoadTexture(char *name);
int ACLoadRGBImage(char *fileName);
