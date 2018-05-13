/*
 * NOTICE OF RELEASE TO THE PUBLIC DOMAIN
 *
 * This work was created using public funds by employees of the 
 * USDA Forest Service's Fire Science Lab and Systems for Environmental 
 * Management.  It is therefore ineligible for copyright under title 17, 
 * section 105 of the United States Code.  You may treat it as you would 
 * treat any public domain work: it may be used, changed, copied, or 
 * redistributed, with or without permission of the authors, for free or 
 * for compensation.  You may not claim exclusive ownership of this code 
 * because it is already owned by everyone.  Use this software entirely 
 * at your own risk.  No warranty of any kind is given.
 * 
 * FARSITE is a trademark owned by Mark Finney.  You may not call derived 
 * works by the name FARSITE without explicit written permission.
 * 
 * A copy of 17-USC-105 should have accompanied this distribution in the file 
 * 17USC105.html.  If not, you may access the law via the US Government's 
 * public websites: 
 *   - http://www.copyright.gov/title17/92chap1.html#105
 *   - http://www.gpoaccess.gov/uscode/  (enter "17USC105" in the search box.)
 */
//------------------------------------------------------------------------------
//
//	Color Legend stuff for FARSITE Window
//
//------------------------------------------------------------------------------
#include "themes.h"
#include <math.h>

GridTheme::GridTheme()
{
	//ramp = 0;
	RedVal = 50;
	GreenVal = 0;
	BlueVal = 0;
	VarVal = 0;
	NumColors = 12;
	MaxBrite = 255;
	LegendNum = -1;
	OnOff = false;
	OnOff3d = false;
	WantNewRamp = true;                     
	WantNewColor = false;
	CatsOK = false;
	Changed3d = false;
	ConvertFuelColors = false;
	Priority = 0;
}

GridTheme::~GridTheme()
{
	//if (ramp)
	//	delete ramp;
	//ramp = 0;
}

void GridTheme::CreateRamp()
{
	//bool Reverse = false;
	//double MinBrite = 20;
     /*
	if (ramp)
	{
		RedVal = ramp->Rval;
		GreenVal = ramp->Gval;
		BlueVal = ramp->Bval;
		MinBrite = ramp->Min;
		Reverse = ramp->Reverse;
		MaxBrite = ramp->Max;
		delete ramp;
	}
	ramp = 0;
     */
	if (!Continuous)
	{
		NumColors = NumCats;
		VarVal = 18;
	}
	//ramp = new ColorRamp(NumColors, RedVal, GreenVal, BlueVal, VarVal,
	//			MinBrite, MaxBrite, Reverse);
	//WantNewRamp=true;
}


void GridTheme::DeleteRamp()
{
	//if (ramp)
	//	delete ramp;
	//ramp = 0;
}


/*
bool GridTheme::GetColor(double value, COLORREF* colr)
{
	bool yesno = true;
	long k, num;

	if (value < 0)//Maxval==Minval || Maxval<Minval)
	{
		*colr = ramp->GetColor(0);

		return true;
	}

	if (!Continuous)
	{
		for (k = 0; k < NumCats; k++)
		{
			if (ceil(value) == Cats[k])
				break;
		}
		if (WantNewColor)
		{
			if (k != ColorChange)
				yesno = false;
			else
				*colr = ramp->GetColor(k);
		}
		else
			//if(WantNewRamp)
			*colr = ramp->GetColor(k);
	}
	else
	{
		if (MaxVal == MinVal)
			num = 0;
		else
			num = (value - MinVal) / (MaxVal - MinVal) * NumColors;
		if (MaxVal != NumColors)
			num += 1;
		if (num > NumColors)
			num = NumColors;
		if (WantNewColor)
		{
			if (num != ColorChange)
				yesno = false;
			else
				*colr = ramp->GetColor(num);//Colors[num];
		}
		else
			//if(WantNewRamp)
			*colr = ramp->GetColor(num);
	}

	return yesno;
}
*/
//------------------------------------------------------------------------------
//
//   COLOR RAMP FUNCTIONS
//
//------------------------------------------------------------------------------

/*
ColorRamp::ColorRamp(long numcolors, long R, long G, long B, long Var,
	double min, double max, bool reverse)
{
	Min = min;
	Max = max;
	NumColors = 0;
	Colors = 0;
	Reverse = reverse;
	SetRamp(numcolors, R, G, B, Var, max);
	memset(ColorFile, 0x0, sizeof(ColorFile));
}

void ColorRamp::GetColorChanges(double numcolors, long R, long G, long B,
	long Var)
{
	Rval = R;
	Gval = G;
	Bval = B;

	switch (Var)
	{
	case 0:
		ColorIncG = ColorIncB = 0.0;
		ColorIncR = (double) (Max - R) / (double) numcolors;
		Min = R;
		break;
	case 1:
		ColorIncR = ColorIncB = 0.0;
		ColorIncG = (double) (Max - G) / (double) numcolors;
		Min = G;
		break;
	case 2:
		ColorIncR = ColorIncG = 0.0;
		ColorIncB = (double) (Max - B) / (double) numcolors;
		Min = B;
		break;
	case 3:
		ColorIncB = 0.0;
		ColorIncR = (double) (Max - R) / (double) numcolors;
		ColorIncG = (double) (Max - G) / (double) numcolors;
		Min = R;
		break;
	case 4:
		ColorIncR = 0.0;
		ColorIncG = (double) (Max - R) / (double) numcolors;
		ColorIncB = (double) (Max - B) / (double) numcolors;
		Min = R;
		break;
	case 5:
		ColorIncG = 0.0;
		ColorIncR = (double) (Max - G) / (double) numcolors;
		ColorIncB = (double) (Max - B) / (double) numcolors;
		Min = G;
		break;
	case 6:
		ColorIncR = (double) (Max - R) / (double) numcolors;
		ColorIncG = (double) (Max - G) / (double) numcolors;
		ColorIncB = (double) (Max - B) / (double) numcolors;
		Min = R;
		break;
	case 7:
		ColorIncR = 0.0;
		ColorIncG = (double) (Max - G) / (double) numcolors;
		ColorIncB = (double) (Max - B) / (double) numcolors;
		break;
	case 8:
		ColorIncG = 0.0;
		ColorIncR = (double) (Max - R) / (double) numcolors;
		ColorIncB = (double) (Max - B) / (double) numcolors;
		break;
	case 9:
		ColorIncB = 0.0;
		ColorIncR = (double) (Max - R) / (double) numcolors;
		ColorIncG = (double) (Max - G) / (double) numcolors;
		break;
	case 10:
		ColorIncR = ColorIncG = 0.0;
		ColorIncB = (double) (Max - B) / (double) numcolors;
		break;
	case 11:
		ColorIncG = ColorIncB = 0.0;
		ColorIncR = (double) (Max - R) / (double) numcolors;
		break;
	case 12:
		ColorIncR = ColorIncB = 0.0;
		ColorIncG = (double) (Max - G) / (double) numcolors;
		break;
	}
}


void ColorRamp::SetRamp(long numcolors, long R, long G, long B, long Var,
	double max)
{
	long j, tvar;
	double tmax, tmin;
	long red, green, blue;
	double Range;
	Max = max;
	if (Colors)
		delete[] Colors;//GlobalFree(Colors);
	Colors = 0;
	Colors = new COLORREF[numcolors + 1];//(COLORREF *) GlobalAlloc(GMEM_FIXED |  GMEM_ZEROINIT, (numcolors+1)*sizeof(COLORREF));
	NumColors = numcolors;

	if (Var < 7)
	{
		GetColorChanges(NumColors, R, G, B, Var);
		Colors[0] = RGB(0, 0, 0);
		for (i = 1; i <= NumColors; i++)
		{
			Colors[i] = RGB(R + ((long) (double) i * ColorIncR),
							G + ((long) (double) i * ColorIncG),
							B + ((long) (double) i * ColorIncB));
		}
	}
	else if (Var < 13)
	{
		Range = (double) NumColors / 2.0;
		tmin = Min;
		GetColorChanges(Range, R, G, B, Var - 7);
		Colors[0] = RGB(0, 0, 0);
		i = j = 1;
		while (j <= Range)
		{
			red = R + (long) ((double) i * ColorIncR);
			if (red > 255)
				red = 255;
			if (red < 0)
				red = 0;
			green = G + (long) ((double) i * ColorIncG);
			if (green > 255)
				green = 255;
			if (green < 0)
				green = 0;
			blue = B + (long) ((double) i * ColorIncB);
			if (blue > 255)
				blue = 255;
			if (blue < 0)
				blue = 0;
			Colors[j++] = RGB(red, green, blue);
			i++;
		};
		R = GetRValue(Colors[(long) Range]);
		G = GetGValue(Colors[(long) Range]);
		B = GetBValue(Colors[(long) Range]);
		GetColorChanges(Range, R, G, B, Var);
		i = 1;
		while (j <= Range * 2.0)
		{
			red = R + (long) ((double) i * ColorIncR);
			if (red > 255)
				red = 255;
			if (red < 0)
				red = 0;
			green = G + (long) ((double) i * ColorIncG);
			if (green > 255)
				green = 255;
			if (green < 0)
				green = 0;
			blue = B + (long) ((double) i * ColorIncB);
			if (blue > 255)
				blue = 255;
			if (blue < 0)
				blue = 0;
			Colors[j++] = RGB(red, green, blue);
			i++;
		};
		Min = tmin;
		Rval = Gval = Bval = Min;
	}
	else if (Var < 17)
	{
		switch (Var)
		{
		case 13:
			tvar = 0; break;
		case 14:
			tvar = 0; break;
		case 15:
			tvar = 2; break;
		case 16:
			tvar = 5; break;
		}
		Range = (double) NumColors / 3.0;
		tmin = Min;
		GetColorChanges(Range, R, G, B, tvar);
		Colors[0] = RGB(0, 0, 0);
		i = j = 1;
		while (j <= Range)
		{
			red = R + (long) ((double) i * ColorIncR);
			if (red > 255)
				red = 255;
			if (red < 0)
				red = 0;
			green = G + (long) ((double) i * ColorIncG);
			if (green > 255)
				green = 255;
			if (green < 0)
				green = 0;
			blue = B + (long) ((double) i * ColorIncB);
			if (blue > 255)
				blue = 255;
			if (blue < 0)
				blue = 0;
			Colors[j++] = RGB(red, green, blue);
			i++;
		};
		R = GetRValue(Colors[(long) Range]);
		G = GetGValue(Colors[(long) Range]);
		B = GetBValue(Colors[(long) Range]);
		tmax = max;
		switch (Var)
		{
		case 13:
			tvar = 2; break;
		case 14:
			tvar = 1; break;
		case 15:
			tvar = 1; break;
		case 16:
			tvar = 2; tmax = Max; Max = 0; break;
		}
		GetColorChanges(Range, R, G, B, tvar);
		Max = tmax;
		i = 1;
		while (j <= Range * 2.0)
		{
			red = R + (long) ((double) i * ColorIncR);
			if (red > 255)
				red = 255;
			if (red < 0)
				red = 0;
			green = G + (long) ((double) i * ColorIncG);
			if (green > 255)
				green = 255;
			if (green < 0)
				green = 0;
			blue = B + (long) ((double) i * ColorIncB);
			if (blue > 255)
				blue = 255;
			if (blue < 0)
				blue = 0;
			Colors[j++] = RGB(red, green, blue);
			i++;
		};
		R = GetRValue(Colors[(long) (2.0 * Range)]);
		G = GetGValue(Colors[(long) (2.0 * Range)]);
		B = GetBValue(Colors[(long) (2.0 * Range)]);
		switch (Var)
		{
		case 13:
			tvar = 0; tmax = Max; Max = 0;  break;
		case 14:
			tvar = 0; tmax = Max; Max = 0;  break;
		case 15:
			tvar = 2; tmax = Max; Max = 0;  break;
		case 16:
			tvar = 1; tmax = Max; break;
		}
		GetColorChanges(Range, R, G, B, tvar);
		Min = tmin;
		Max = tmax;
		i = 1;
		while (j <= Range * 3.0)
		{
			red = R + (long) ((double) i * ColorIncR);
			if (red > 255)
				red = 255;
			if (red < 0)
				red = 0;
			green = G + (long) ((double) i * ColorIncG);
			if (green > 255)
				green = 255;
			if (green < 0)
				green = 0;
			blue = B + (long) ((double) i * ColorIncB);
			if (blue > 255)
				blue = 255;
			if (blue < 0)
				blue = 0;
			Colors[j++] = RGB(red, green, blue);
			i++;
		};
		Rval = Gval = Bval = Min;
	}
	else if (Var < 18)  // full spectrum
	{
		 	//if(NumColors<6)
		 	//{    if(Colors)
		 	//		delete[] Colors;//GlobalFree(Colors);
		 	//     NumColors=numcolors=6;
		 	//	Colors=new COLORREF[numcolors+1];//(COLORREF *) GlobalAlloc(GMEM_FIXED |  GMEM_ZEROINIT, (numcolors+1)*sizeof(COLORREF));
		 	//}
		 	Range = (double) NumColors / 6.0;
		 	tmin = Min;
		 	GetColorChanges(Range, R, G, B, 0);
		 	Colors[0] = RGB(0, 0, 0);
		 	j = i = 1;
		while (j <= Range)
		{
			red = R + (long) ((double) i * ColorIncR);
			if (red > 255)
				red = 255;
			if (red < 0)
				red = 0;
			green = G + (long) ((double) i * ColorIncG);
			if (green > 255)
				green = 255;
			if (green < 0)
				green = 0;
			blue = B + (long) ((double) i * ColorIncB);
			if (blue > 255)
				blue = 255;
			if (blue < 0)
				blue = 0;
			Colors[j++] = RGB(red, green, blue);
			i++;
		};
		 	R = Max;//255
		 	GetColorChanges(Range, R, G, B, 1);
		 	i = 1;
		while (j <= Range * 2.0)
		{
			red = R + (long) ((double) i * ColorIncR);
			if (red > 255)
				red = 255;
			if (red < 0)
				red = 0;
			green = G + (long) ((double) i * ColorIncG);
			if (green > 255)
				green = 255;
			if (green < 0)
				green = 0;
			blue = B + (long) ((double) i * ColorIncB);
			if (blue > 255)
				blue = 255;
			if (blue < 0)
				blue = 0;
			Colors[j++] = RGB(red, green, blue);
			i++;
		};
		 	R = tmin;//0;
		 	G = Max;//255;
		 	GetColorChanges(Range, R, G, B, 0);
		 	i = 1;
		while (j <= Range * 3.0)
		{
			red = R - (long) ((double) i * ColorIncR);
			if (red < 0)
				red = 0;
			green = G + (long) ((double) i * ColorIncG);
			if (green > 255)
				green = 255;
			if (green < 0)
				green = 0;
			blue = B + (long) ((double) i * ColorIncB);
			if (blue > 255)
				blue = 255;
			if (blue < 0)
				blue = 0;
			Colors[j++] = RGB(red, green, blue);
			i++;
		};
		 	R = tmin;//0;
		 	G = Max;//255;
		 	B = tmin;//0;
		 	GetColorChanges(Range, R, G, B, 2);
		 	i = 1;
		while (j <= Range * 4.0)
		{
			red = R + (long) ((double) i * ColorIncR);
			if (red > 255)
				red = 255;
			if (red < 0)
				red = 0;
			green = G + (long) ((double) i * ColorIncG);
			if (green > 255)
				green = 255;
			if (green < 0)
				green = 0;
			blue = B + (long) ((double) i * ColorIncB);
			if (blue > 255)
				blue = 255;
			if (blue < 0)
				blue = 0;
			Colors[j++] = RGB(red, green, blue);
			i++;
		};
		 	R = tmin;//0;
		 	G = tmin;//0;
		 	B = Max;//255;
		 	GetColorChanges(Range, R, G, B, 2);
		 	i = 1;
		while (j <= Range * 5.0)
		{
			red = R + (long) ((double) i * ColorIncR);
			if (red > 255)
				red = 255;
			if (red < 0)
				red = 0;
			green = G - (long) ((double) i * ColorIncG);
			if (green < 0)
				green = 0;
			if (green < 0)
				green = 0;
			blue = B + (long) ((double) i * ColorIncB);
			if (blue > 255)
				blue = 255;
			if (blue < 0)
				blue = 0;
			Colors[j++] = RGB(red, green, blue);
			i++;
		};
		 	R = tmin;//0;
		 	G = tmin;//0;
		 	B = Max;//255;
		 	GetColorChanges(Range, R, G, B, 0);
		 	i = 1;
		while (j <= Range * 6.0)
		{
			red = R + (long) ((double) i * ColorIncR);
			if (red > 255)
				red = 255;
			if (red < 0)
				red = 0;
			green = G + (long) ((double) i * ColorIncG);
			if (green > 255)
				green = 255;
			if (green < 0)
				green = 0;
			blue = B + (long) ((double) i * ColorIncB);
			if (blue > 255)
				blue = 255;
			if (blue < 0)
				blue = 0;
			Colors[j++] = RGB(red, green, blue);
			i++;
		}
		 	Min = tmin;
		 	Rval = Gval = Bval = Min;
	}
	else
	{
		 	//struct timeb tb;			// time struct for randomizing wind direction
		 	//ftime(&tb);
		 	//srand(tb.time+tb.millitm);
		SYSTEMTIME tb;
		 	GetSystemTime(&tb);
		 	srand(tb.wMilliseconds);

		for (i = 1; i <= NumColors; i++)
		{
			R = rand() % 200 + 56;
			G = rand() % 200 + 56;
			B = rand() % 200 + 56;
			Colors[i] = RGB(R, G, B);
		}
	}
	ColorType = Var;

	if (Var < 18 && Reverse)
	{
		COLORREF tcolr;
		for (i = 1; i <= numcolors / 2; i++)
		{
			j = numcolors - i + 1;
			tcolr = Colors[i];
			Colors[i] = Colors[j];
			Colors[j] = tcolr;
		}
	}
}


ColorRamp::~ColorRamp()
{
	if (Colors)
		delete[] Colors;//GlobalFree(Colors);
	Colors = 0;
}

COLORREF ColorRamp::GetColor(long order)
{
	return Colors[order];
}


bool ColorRamp::SetSpecificColor(long numcolor, long R, long G, long B)
{
	if (numcolor >= NumColors + 1)
		return false;

	Colors[numcolor] = RGB(R, G, B);
	//ColorChange=numcolor;
	//WantNewColor=true;

	return true;
}


long ColorRamp::GetNumColors()
{
	return NumColors;
}
*/



