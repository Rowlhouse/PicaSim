#include <IwTextParserITX.h>
#include <dpiLength.h>

using namespace DPI;

void CdpiLength::Serialise()
{
	IwSerialiseInt32(Value);
	IwSerialiseEnum(&Unit);
}
void CdpiLength::ParseAttribute(CIwTextParserITX* pParser)
{
	pParser->ReadFixed(&Value);
	uint32 unitHash;
	pParser->ReadStringHash(&unitHash);

	switch (unitHash)
	{
	case 0x00597929:
		Unit = PT;
		break;
	case 0x0059792d:
		Unit = PX;
		break;
	case 0x0002b5ca:
		Unit = PERCENT;
		Value = Value/100;
		break;
	case 0x0059783c: //IN
		Unit = PT;
		Value *= 72;
		break;
	case 0x00597775: //CM
		Unit = PT;
		Value = Value*7200/254;
		break;
	default:
		IwAssertMsg(TOE,false,("Unknown unit name (pt,px,in,cm)"));
		break;
	}
}

void CdpiLength4::Serialise()
{
	left.Serialise();
	top.Serialise();
	right.Serialise();
	bottom.Serialise();
}
void CdpiLength4::ParseAttribute(CIwTextParserITX* pParser)
{
	left.ParseAttribute(pParser);
	top.ParseAttribute(pParser);
	right.ParseAttribute(pParser);
	bottom.ParseAttribute(pParser);
}
void CdpiLength2::Serialise()
{
	x.Serialise();
	y.Serialise();
}
void CdpiLength2::ParseAttribute(CIwTextParserITX* pParser)
{
	x.ParseAttribute(pParser);
	y.ParseAttribute(pParser);
	
}
