#ifndef HELPERS_XML_H
#define HELPERS_XML_H

#include "tinyxml.h"
#include "Helpers.h"

bool readFromXML(TiXmlElement* elem, const char* name, bool& value, size_t* index = 0);
bool readFromXML(TiXmlElement* elem, const char* name, unsigned int& value, size_t* index = 0);
bool readFromXML(TiXmlElement* elem, const char* name, long unsigned int& value, size_t* index = 0);
bool readFromXML(TiXmlElement* elem, const char* name, unsigned char& value, size_t* index = 0);
bool readFromXML(TiXmlElement* elem, const char* name, int& value, size_t* index = 0);
bool readFromXML(TiXmlElement* elem, const char* name, float& value, size_t* index = 0);
bool readFromXML(TiXmlElement* elem, const char* name, std::string& value, size_t* index = 0);
bool readFromXML(TiXmlElement* elem, const char* name, Vector3& value, size_t* index = 0);
bool readFromXML(TiXmlElement* elem, const char* name, Vector4& value, size_t* index = 0);
bool readFromXML(TiXmlElement* elem, const char* name, Transform& value, size_t* index = 0);
bool readFromXML(TiXmlElement* elem, const char* name, float* value, size_t* index = 0); ///< Array of 3 floats

bool         readBoolFromXML(TiXmlElement* elem, const char* name, size_t* index = 0);
unsigned int readUnsignedIntFromXML(TiXmlElement* elem, const char* name, size_t* index = 0);
int          readIntFromXML(TiXmlElement* elem, const char* name, size_t* index = 0);
float        readFloatFromXML(TiXmlElement* elem, const char* name, size_t* index = 0);
std::string  readStringFromXML(TiXmlElement* elem, const char* name, size_t* index = 0);
Vector3      readVector3FromXML(TiXmlElement* elem, const char* name, size_t* index = 0);
Vector4      readVector4FromXML(TiXmlElement* elem, const char* name, size_t* index = 0);
Transform    readTransformFromXML(TiXmlElement* elem, const char* name, size_t* index = 0);
Colour       readColourFromXML(TiXmlElement* elem, const char* name, size_t* index = 0);

template<typename T> void writeToXML(const T& v, TiXmlElement* element, const char* name, size_t* index = 0);
void writeStringToXML(const std::string& v, TiXmlElement* element, const char* name, size_t* index = 0);
void writeFloatToXML(const float& v, TiXmlElement* element, const char* name, size_t* index = 0);
void writeVector3ToXML(const Vector3& v, TiXmlElement* element, const char* name, size_t* index = 0);
void writeVector4ToXML(const Vector4& v, TiXmlElement* element, const char* name, size_t* index = 0);
void writeTransformToXML(const Transform& t, TiXmlElement* element, const char* name, size_t* index = 0);

// These assume existence of element, and that the string name is the same as the variable
#define WRITE_ATTRIBUTE(v) element->SetAttribute(#v, v)
#define WRITE_DOUBLE_ATTRIBUTE(v) element->SetDoubleAttribute(#v, v)
#define READ_ATTRIBUTE(v) readFromXML(element, #v, v)
#define READ_ENUM_ATTRIBUTE(v) readFromXML(element, #v, (int&) v)

#define WRITE_DOUBLE_ATTRIBUTE_INDEX(v, index) writeFloatToXML(v, element, #v, &index);
#define READ_ATTRIBUTE_INDEX(v, index) readFromXML(element, #v, v, &index);

// Tmeplate implementations

template<typename T>
void writeToXML(const T& v, TiXmlElement* element, const char* name, size_t* index)
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



#endif
