#include "HelpersXML.h"
#include "Trace.h"

//---------------------------------------------------------------------------------------------------------------------
bool readFromXML(TiXmlElement* elem, const char* name, bool& value, size_t* index)
{
  if (!elem)
  {
    return false;
  }
  int res;
  if (index)
  {
    char txt[256];
    sprintf(txt, "%s_%lu", name, (long unsigned) *index);
    res = elem->QueryBoolAttribute(txt, &value);
  }
  else
  {
    res = elem->QueryBoolAttribute(name, &value);
  }
  if (res != TIXML_SUCCESS)
  {
    TRACE_FILE_IF(2) TRACE("Failed to read bool %s, error = %s\n", name, res == TIXML_NO_ATTRIBUTE ? "TIXML_NO_ATTRIBUTE" : "TIXML_WRONG_TYPE");
    return false;
  }
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool readFromXML(TiXmlElement* elem, const char* name, unsigned int& value, size_t* index)
{
  if (!elem)
    return false;
  int res;
  if (index)
  {
    char txt[256];
    sprintf(txt, "%s_%lu", name, (long unsigned) *index);
    res = elem->QueryUnsignedAttribute(txt, &value);
  }
  else
  {
    res = elem->QueryUnsignedAttribute(name, &value);
  }
  if (res != TIXML_SUCCESS)
  {
    TRACE_FILE_IF(2) TRACE("Failed to read unsigned %s, error = %s\n", name, res == TIXML_NO_ATTRIBUTE ? "TIXML_NO_ATTRIBUTE" : "TIXML_WRONG_TYPE");
    return false;
  }
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool readFromXML(TiXmlElement* elem, const char* name, long unsigned int& value, size_t* index)
{
  unsigned int v = (unsigned int) value;
  bool ret = readFromXML(elem, name, v, index);
  value = (long unsigned int) v;
  return ret;
}

//---------------------------------------------------------------------------------------------------------------------
bool readFromXML(TiXmlElement* elem, const char* name, unsigned char& value, size_t* index)
{
  if (!elem)
    return false;
  unsigned int v = value;
  int res;
  if (index)
  {
    char txt[256];
    sprintf(txt, "%s_%lu", name, (long unsigned) *index);
    res = elem->QueryUnsignedAttribute(txt, &v);
  }
  else
  {
    res = elem->QueryUnsignedAttribute(name, &v);
  }
  if (res != TIXML_SUCCESS)
  {
    TRACE_FILE_IF(2) TRACE("Failed to read unsigned char %s, error = %s\n", name, res == TIXML_NO_ATTRIBUTE ? "TIXML_NO_ATTRIBUTE" : "TIXML_WRONG_TYPE");
    return false;
  }
  value = v;
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool readFromXML(TiXmlElement* elem, const char* name, int& value, size_t* index)
{
  if (!elem)
    return false;
  int res;
  if (index)
  {
    char txt[256];
    sprintf(txt, "%s_%lu", name, (long unsigned) *index);
    res = elem->QueryIntAttribute(txt, &value);
  }
  else
  {
    res = elem->QueryIntAttribute(name, &value);
  }
  if (res != TIXML_SUCCESS)
  {
    TRACE_FILE_IF(2) TRACE("Failed to read int %s, error = %s\n", name, res == TIXML_NO_ATTRIBUTE ? "TIXML_NO_ATTRIBUTE" : "TIXML_WRONG_TYPE");
    return false;
  }
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool readFromXML(TiXmlElement* elem, const char* name, float& value, size_t* index)
{
  if (!elem)
    return false;
  int res;
  if (index)
  {
    char txt[256];
    sprintf(txt, "%s_%lu", name, (long unsigned) *index);
    res = elem->QueryFloatAttribute(txt, &value);
  }
  else
  {
    res = elem->QueryFloatAttribute(name, &value);
  }
  if (res != TIXML_SUCCESS)
  {
    TRACE_FILE_IF(2) TRACE("Failed to read float %s, error = %s\n", name, res == TIXML_NO_ATTRIBUTE ? "TIXML_NO_ATTRIBUTE" : "TIXML_WRONG_TYPE");
    return false;
  }
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool readFromXML(TiXmlElement* elem, const char* name, std::string& value, size_t* index)
{
  if (!elem)
    return false;
  int res;
  if (index)
  {
    char txt[256];
    sprintf(txt, "%s_%lu", name, (long unsigned) *index);
    res = elem->QueryStringAttribute(txt, &value);
  }
  else
  {
    res = elem->QueryStringAttribute(name, &value);
  }
  if (res != TIXML_SUCCESS)
  {
    TRACE_FILE_IF(2) TRACE("Failed to read string %s, error = %s\n", name, res == TIXML_NO_ATTRIBUTE ? "TIXML_NO_ATTRIBUTE" : "TIXML_WRONG_TYPE");
    return false;
  }
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool readFromXML(TiXmlElement* elem, const char* name, glm::vec3& value, size_t* index)
{
  if (!elem)
    return false;
  char txtX[256];
  char txtY[256];
  char txtZ[256];
  sprintf(txtX, "%sX", name);
  sprintf(txtY, "%sY", name);
  sprintf(txtZ, "%sZ", name);
  bool result = true;
  if (!readFromXML(elem, txtX, value.x, index))
    result = false;
  if (!readFromXML(elem, txtY, value.y, index))
    result = false;
  if (!readFromXML(elem, txtZ, value.z, index))
    result = false;

  return result;
}

//---------------------------------------------------------------------------------------------------------------------
bool readFromXML(TiXmlElement* elem, const char* name, glm::mat4& value, size_t* index)
{
  if (!elem)
    return false;

  glm::vec3 rx;
  glm::vec3 ry;
  glm::vec3 rz;
  glm::vec3 t;
  bool result = true;

  char txt[256];
  sprintf(txt, "%s_rowx", name);
  if (!readFromXML(elem, txt, rx, index))
    result = false;
  sprintf(txt, "%s_rowy", name);
  if (!readFromXML(elem, txt, ry, index))
    result = false;
  sprintf(txt, "%s_rowz", name);
  if (!readFromXML(elem, txt, rz, index))
    result = false;
  sprintf(txt, "%s_trans", name);
  if (!readFromXML(elem, txt, t, index))
    result = false;

  SetRowX(value, rx);
  SetRowY(value, ry);
  SetRowZ(value, rz);
  value = glm::translate(value, t);

  return result;
}


//---------------------------------------------------------------------------------------------------------------------
bool readFromXML(TiXmlElement* elem, const char* name, glm::vec4& value, size_t* index)
{
  if (!elem)
    return false;
  char txtX[256];
  char txtY[256];
  char txtZ[256];
  char txtW[256];
  if (index)
  {
    sprintf(txtX, "%sX_%lu", name, (long unsigned) *index);
    sprintf(txtY, "%sY_%lu", name, (long unsigned) *index);
    sprintf(txtZ, "%sZ_%lu", name, (long unsigned) *index);
    sprintf(txtW, "%sW_%lu", name, (long unsigned) *index);
  }
  else
  {
    sprintf(txtX, "%sX", name);
    sprintf(txtY, "%sY", name);
    sprintf(txtZ, "%sZ", name);
    sprintf(txtW, "%sW", name);
  }
  bool result = true;
  if (!readFromXML(elem, txtX, value.x))
    result = false;
  if (!readFromXML(elem, txtY, value.y))
    result = false;
  if (!readFromXML(elem, txtZ, value.z))
    result = false;
  if (!readFromXML(elem, txtW, value.w))
    value.w = 1.0f;

  return result;
}

//---------------------------------------------------------------------------------------------------------------------
bool readFromXML(TiXmlElement* elem, const char* name, float* value, size_t* index)
{
  if (!elem)
    return false;
  char txtX[256];
  char txtY[256];
  char txtZ[256];
  if (index)
  {
    sprintf(txtX, "%sX_%lu", name, (long unsigned) *index);
    sprintf(txtY, "%sY_%lu", name, (long unsigned) *index);
    sprintf(txtZ, "%sZ_%lu", name, (long unsigned) *index);
  }
  else
  {
    sprintf(txtX, "%sX", name);
    sprintf(txtY, "%sY", name);
    sprintf(txtZ, "%sZ", name);
  }
  bool result = true;
  if (!readFromXML(elem, txtX, value[0]))
    result = false;
  if (!readFromXML(elem, txtY, value[1]))
    result = false;
  if (!readFromXML(elem, txtZ, value[2]))
    result = false;

  return result;
}

//---------------------------------------------------------------------------------------------------------------------
bool readBoolFromXML(TiXmlElement* elem, const char* name, size_t* index)
{
  bool result = false;
  readFromXML(elem, name, result, index);
  return result;
}

//---------------------------------------------------------------------------------------------------------------------
unsigned int readUnsignedIntFromXML(TiXmlElement* elem, const char* name, size_t* index)
{
  unsigned int result = 0;
  readFromXML(elem, name, result, index);
  return result;
}

//---------------------------------------------------------------------------------------------------------------------
int readIntFromXML(TiXmlElement* elem, const char* name, size_t* index)
{
  int result = 0;
  readFromXML(elem, name, result, index);
  return result;
}

//---------------------------------------------------------------------------------------------------------------------
float readFloatFromXML(TiXmlElement* elem, const char* name, size_t* index)
{
  float result = 0.0f;
  readFromXML(elem, name, result, index);
  return result;
}

//---------------------------------------------------------------------------------------------------------------------
std::string readStringFromXML(TiXmlElement* elem, const char* name, size_t* index)
{
  std::string result;
  readFromXML(elem, name, result, index);
  return result;
}

//---------------------------------------------------------------------------------------------------------------------
glm::vec3 readVector3FromXML(TiXmlElement* elem, const char* name, size_t* index)
{
  glm::vec3 result(0,0,0);
  readFromXML(elem, name, &result.x, index);
  return result;
}

//---------------------------------------------------------------------------------------------------------------------
glm::vec4 readVector4FromXML(TiXmlElement* elem, const char* name, size_t* index)
{
  glm::vec4 result(0,0,0,0);
  readFromXML(elem, name, result, index);
  return result;
}

//---------------------------------------------------------------------------------------------------------------------
glm::mat4 readTransformFromXML(TiXmlElement* elem, const char* name, size_t* index)
{
  glm::mat4 result;
  result = glm::mat4(1.0f);    
  readFromXML(elem, name, result, index);
  return result;
}

//---------------------------------------------------------------------------------------------------------------------
Colour readColourFromXML(TiXmlElement* elem, const char* name, size_t* index)
{
  Colour result;

  std::string textR(name); textR += "R";
  std::string textG(name); textG += "G";
  std::string textB(name); textB += "B";
  std::string textA(name); textA += "A";

  readFromXML(elem, textR.c_str(), result.col.r);
  readFromXML(elem, textG.c_str(), result.col.g);
  readFromXML(elem, textB.c_str(), result.col.b);
  readFromXML(elem, textA.c_str(), result.col.a);
  return result;
}

void writeStringToXML(const std::string& v, TiXmlElement* element, const char* name, size_t* index)
{
  if (index)
  {
    char txt[256];
    sprintf(txt, "%s_%lu", name, (long unsigned) *index);
    element->SetAttribute(txt, v);
  }
  else
  {
    element->SetAttribute(name, v);
  }
}

void writeFloatToXML(const float& v, TiXmlElement* element, const char* name, size_t* index)
{
  if (index)
  {
    char txt[256];
    sprintf(txt, "%s_%lu", name, (long unsigned) *index);
    element->SetDoubleAttribute(txt, v);
  }
  else
  {
    element->SetDoubleAttribute(name, v);
  }
}

void writeVector3ToXML(const glm::vec3& v, TiXmlElement* element, const char* name, size_t* index)
{
  char txt[256];
  if (index)
    sprintf(txt, "%sX_%lu", name, (long unsigned) *index);
  else
    sprintf(txt, "%sX", name);
  element->SetDoubleAttribute(txt, v.x);

  if (index)
    sprintf(txt, "%sY_%lu", name, (long unsigned) *index);
  else
    sprintf(txt, "%sY", name);
  element->SetDoubleAttribute(txt, v.y);

  if (index)
    sprintf(txt, "%sZ_%lu", name, (long unsigned) *index);
  else
    sprintf(txt, "%sZ", name);
  element->SetDoubleAttribute(txt, v.z);
}

void writeVector4ToXML(const glm::vec4& v, TiXmlElement* element, const char* name, size_t* index)
{
  char txt[256];
  if (index)
    sprintf(txt, "%sX_%lu", name, (long unsigned) *index);
  else
    sprintf(txt, "%sX", name);
  element->SetDoubleAttribute(txt, v.x);

  if (index)
    sprintf(txt, "%sY_%lu", name, (long unsigned) *index);
  else
    sprintf(txt, "%sY", name);
  element->SetDoubleAttribute(txt, v.y);

  if (index)
    sprintf(txt, "%sZ_%lu", name, (long unsigned) *index);
  else
    sprintf(txt, "%sZ", name);
  element->SetDoubleAttribute(txt, v.z);

  if (index)
    sprintf(txt, "%sW_%lu", name, (long unsigned) *index);
  else
    sprintf(txt, "%sW", name);
  element->SetDoubleAttribute(txt, v.w);
}

void writeTransformToXML(const glm::mat4& t, TiXmlElement* element, const char* name, size_t* index)
{
  char txt[256];
  sprintf(txt, "%s_rowx", name);
  writeVector3ToXML(glm::vec3(t[0]), element, txt, index);
  sprintf(txt, "%s_rowy", name);
  writeVector3ToXML(glm::vec3(t[1]), element, txt, index);
  sprintf(txt, "%s_rowz", name);
  writeVector3ToXML(glm::vec3(t[2]), element, txt, index);
  sprintf(txt, "%s_trans", name);
  writeVector3ToXML(glm::vec3(t[3]), element, txt, index);
}


