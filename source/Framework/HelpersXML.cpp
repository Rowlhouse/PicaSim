#include "HelpersXML.h"
#include "Trace.h"

//======================================================================================================================
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
        snprintf(txt, sizeof(txt), "%s_%lu", name, (long unsigned) *index);
        res = elem->QueryBoolAttribute(txt, &value);
    }
    else
    {
        res = elem->QueryBoolAttribute(name, &value);
    }
    if (res != TIXML_SUCCESS)
    {
        TRACE_FILE_IF(ONCE_2) TRACE("Failed to read bool %s, error = %s\n", name, res == TIXML_NO_ATTRIBUTE ? "TIXML_NO_ATTRIBUTE" : "TIXML_WRONG_TYPE");
        return false;
    }
    return true;
}

//======================================================================================================================
bool readFromXML(TiXmlElement* elem, const char* name, unsigned int& value, size_t* index)
{
    if (!elem)
        return false;
    int res;
    if (index)
    {
        char txt[256];
        snprintf(txt, sizeof(txt), "%s_%lu", name, (long unsigned) *index);
        res = elem->QueryUnsignedAttribute(txt, &value);
    }
    else
    {
        res = elem->QueryUnsignedAttribute(name, &value);
    }
    if (res != TIXML_SUCCESS)
    {
        TRACE_FILE_IF(ONCE_2) TRACE("Failed to read unsigned %s, error = %s\n", name, res == TIXML_NO_ATTRIBUTE ? "TIXML_NO_ATTRIBUTE" : "TIXML_WRONG_TYPE");
        return false;
    }
    return true;
}

//======================================================================================================================
bool readFromXML(TiXmlElement* elem, const char* name, long unsigned int& value, size_t* index)
{
    unsigned int v = (unsigned int) value;
    bool ret = readFromXML(elem, name, v, index);
    value = (long unsigned int) v;
    return ret;
}

//======================================================================================================================
// size_t overload for 64-bit Windows where size_t differs from unsigned long
#if defined(_WIN64)
bool readFromXML(TiXmlElement* elem, const char* name, size_t& value, size_t* index)
{
    unsigned int v = (unsigned int) value;
    bool ret = readFromXML(elem, name, v, index);
    value = (size_t) v;
    return ret;
}
#endif

//======================================================================================================================
bool readFromXML(TiXmlElement* elem, const char* name, unsigned char& value, size_t* index)
{
    if (!elem)
        return false;
    unsigned int v = value;
    int res;
    if (index)
    {
        char txt[256];
        snprintf(txt, sizeof(txt), "%s_%lu", name, (long unsigned) *index);
        res = elem->QueryUnsignedAttribute(txt, &v);
    }
    else
    {
        res = elem->QueryUnsignedAttribute(name, &v);
    }
    if (res != TIXML_SUCCESS)
    {
        TRACE_FILE_IF(ONCE_2) TRACE("Failed to read unsigned char %s, error = %s\n", name, res == TIXML_NO_ATTRIBUTE ? "TIXML_NO_ATTRIBUTE" : "TIXML_WRONG_TYPE");
        return false;
    }
    value = v;
    return true;
}

//======================================================================================================================
bool readFromXML(TiXmlElement* elem, const char* name, int& value, size_t* index)
{
    if (!elem)
        return false;
    int res;
    if (index)
    {
        char txt[256];
        snprintf(txt, sizeof(txt), "%s_%lu", name, (long unsigned) *index);
        res = elem->QueryIntAttribute(txt, &value);
    }
    else
    {
        res = elem->QueryIntAttribute(name, &value);
    }
    if (res != TIXML_SUCCESS)
    {
        TRACE_FILE_IF(ONCE_2) TRACE("Failed to read int %s, error = %s\n", name, res == TIXML_NO_ATTRIBUTE ? "TIXML_NO_ATTRIBUTE" : "TIXML_WRONG_TYPE");
        return false;
    }
    return true;
}

//======================================================================================================================
bool readFromXML(TiXmlElement* elem, const char* name, float& value, size_t* index)
{
    if (!elem)
        return false;
    int res;
    if (index)
    {
        char txt[256];
        snprintf(txt, sizeof(txt), "%s_%lu", name, (long unsigned) *index);
        res = elem->QueryFloatAttribute(txt, &value);
    }
    else
    {
        res = elem->QueryFloatAttribute(name, &value);
    }
    if (res != TIXML_SUCCESS)
    {
        TRACE_FILE_IF(ONCE_2) TRACE("Failed to read float %s, error = %s\n", name, res == TIXML_NO_ATTRIBUTE ? "TIXML_NO_ATTRIBUTE" : "TIXML_WRONG_TYPE");
        return false;
    }
    return true;
}

//======================================================================================================================
bool readFromXML(TiXmlElement* elem, const char* name, std::string& value, size_t* index)
{
    if (!elem)
        return false;
    int res;
    if (index)
    {
        char txt[256];
        snprintf(txt, sizeof(txt), "%s_%lu", name, (long unsigned) *index);
        res = elem->QueryStringAttribute(txt, &value);
    }
    else
    {
        res = elem->QueryStringAttribute(name, &value);
    }
    if (res != TIXML_SUCCESS)
    {
        TRACE_FILE_IF(ONCE_2) TRACE("Failed to read string %s, error = %s\n", name, res == TIXML_NO_ATTRIBUTE ? "TIXML_NO_ATTRIBUTE" : "TIXML_WRONG_TYPE");
        return false;
    }
    return true;
}

//======================================================================================================================
bool readFromXML(TiXmlElement* elem, const char* name, Vector3& value, size_t* index)
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

//======================================================================================================================
bool readFromXML(TiXmlElement* elem, const char* name, Transform& value, size_t* index)
{
    if (!elem)
        return false;

    Vector3 rx;
    Vector3 ry;
    Vector3 rz;
    Vector3 t;
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
    value.SetTrans(t);

    return result;
}


//======================================================================================================================
bool readFromXML(TiXmlElement* elem, const char* name, Vector4& value, size_t* index)
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
        result = false;

    return result;
}

//======================================================================================================================
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

//======================================================================================================================
bool readBoolFromXML(TiXmlElement* elem, const char* name, size_t* index)
{
    bool result = false;
    readFromXML(elem, name, result, index);
    return result;
}

//======================================================================================================================
unsigned int readUnsignedIntFromXML(TiXmlElement* elem, const char* name, size_t* index)
{
    unsigned int result = 0;
    readFromXML(elem, name, result, index);
    return result;
}

//======================================================================================================================
int readIntFromXML(TiXmlElement* elem, const char* name, size_t* index)
{
    int result = 0;
    readFromXML(elem, name, result, index);
    return result;
}

//======================================================================================================================
float readFloatFromXML(TiXmlElement* elem, const char* name, size_t* index)
{
    float result = 0.0f;
    readFromXML(elem, name, result, index);
    return result;
}

//======================================================================================================================
std::string readStringFromXML(TiXmlElement* elem, const char* name, size_t* index)
{
    std::string result;
    readFromXML(elem, name, result, index);
    return result;
}

//======================================================================================================================
Vector3 readVector3FromXML(TiXmlElement* elem, const char* name, size_t* index)
{
    Vector3 result(0,0,0);
    readFromXML(elem, name, &result.x, index);
    return result;
}

//======================================================================================================================
Vector4 readVector4FromXML(TiXmlElement* elem, const char* name, size_t* index)
{
    Vector4 result(0,0,0,0);
    readFromXML(elem, name, result, index);
    return result;
}

//======================================================================================================================
Transform readTransformFromXML(TiXmlElement* elem, const char* name, size_t* index)
{
    Transform result;
    result.SetIdentity();    
    readFromXML(elem, name, result, index);
    return result;
}

//======================================================================================================================
Colour readColourFromXML(TiXmlElement* elem, const char* name, size_t* index)
{
    Colour result;

    std::string textR(name); textR += "R";
    std::string textG(name); textG += "G";
    std::string textB(name); textB += "B";
    std::string textA(name); textA += "A";

    readFromXML(elem, textR.c_str(), result.r);
    readFromXML(elem, textG.c_str(), result.g);
    readFromXML(elem, textB.c_str(), result.b);
    readFromXML(elem, textB.c_str(), result.a);
    return result;
}

void writeStringToXML(const std::string& v, TiXmlElement* element, const char* name, size_t* index)
{
    if (index)
    {
        char txt[256];
        snprintf(txt, sizeof(txt), "%s_%lu", name, (long unsigned) *index);
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
        snprintf(txt, sizeof(txt), "%s_%lu", name, (long unsigned) *index);
        element->SetDoubleAttribute(txt, v);
    }
    else
    {
        element->SetDoubleAttribute(name, v);
    }
}

void writeVector3ToXML(const Vector3& v, TiXmlElement* element, const char* name, size_t* index)
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

void writeVector4ToXML(const Vector4& v, TiXmlElement* element, const char* name, size_t* index)
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

void writeTransformToXML(const Transform& t, TiXmlElement* element, const char* name, size_t* index)
{
    char txt[256];
    sprintf(txt, "%s_rowx", name);
    writeVector3ToXML(t.RowX(), element, txt, index);
    sprintf(txt, "%s_rowy", name);
    writeVector3ToXML(t.RowY(), element, txt, index);
    sprintf(txt, "%s_rowz", name);
    writeVector3ToXML(t.RowZ(), element, txt, index);
    sprintf(txt, "%s_trans", name);
    writeVector3ToXML(t.GetTrans(), element, txt, index);
}


