#pragma once

#include "dpiInfo.h"
#include "dpiLength.h"
#include <iwgx.h>

namespace DPI
{
	struct CdpiLength
	{
		enum dpiLengthUnit
		{
			PX, //Pixels
			PT, //Points (1/72 of inch)
			PERCENT //Percent
		};

		iwfixed Value;
		dpiLengthUnit Unit;

		inline bool IsZero() const {return Value == 0;}

		int32 GetPx(int32 total) const
		{
			if (Value == 0)
				return 0;
			int32 res = 0;
			switch (Unit)
			{
			case PX:
				res = Value/IW_GEOM_ONE;
				if (res == 0) { if (Value>0) res=1; else res=-1;}
				break;
			case PT:
				res = Value*dpiGetScreenDPI()/72/IW_GEOM_ONE;
				if (res == 0) { if (Value>0) res=1; else res=-1;}
				break;
			case PERCENT:
				res = Value*total/IW_GEOM_ONE;
				if (res == 0 && total != 0) { if (Value*total>0) res=1; else res=-1;}
				break;
			default:
				break;
			}
			return res;
		}
		CdpiLength(int32 v, dpiLengthUnit u):Value(v),Unit(u){}
		CdpiLength():Value(0),Unit(PX){}
		void Serialise();
		void	ParseAttribute(CIwTextParserITX* pParser);
		bool operator == (const CdpiLength & other) const
		{
			return (Value == other.Value)
				&& (Unit == other.Unit || Value == 0);
		}
		

	};
	struct CdpiLength2
	{
		CdpiLength x;
		CdpiLength y;
		inline bool IsZero() const {return x.IsZero() && y.IsZero();}
		bool operator == (const CdpiLength2 & other) const
		{
			return (x == other.y)
				&& (x == other.y);
		}

		void Serialise();
		void ParseAttribute(CIwTextParserITX* pParser);
	};
	struct CdpiLength4
	{
		CdpiLength left;
		CdpiLength top;
		CdpiLength right;
		CdpiLength bottom;

		inline bool IsZero() const {return left.IsZero() && right.IsZero() && top.IsZero() && bottom.IsZero();}
		bool operator == (const CdpiLength4 & other) const
		{
			return (left == other.left)
				&& (top == other.top)
				&& (right == other.right)
				&& (bottom == other.bottom);
		}

		void Serialise();
		void ParseAttribute(CIwTextParserITX* pParser);
	};
}
