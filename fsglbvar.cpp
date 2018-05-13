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
//******************************************************************************
//	FSGLBVAR.CPP		Global Variable and Functions for FARSITE
//
//
//
//					Copyright 1994, 1995
//					Mark A. Finney, Systems for Environmental Management
//******************************************************************************

//#include <direct.h>
#include "fsglbvar.h"
#include <stdio.h>
//#include <owl\dc.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>
//#include <dir.h>
#include <sys/stat.h>
#include "fcopy.h"
#include "portablestrings.h"

//static TWindow * FarsiteWindow=0;	// globally declared pointer to the FarsiteWindow
//static TFarsiteWindow * FarsiteWindow=0;// globally declared pointer to the FarsiteWindow
//static TThreeDWindow *ThreeDWindow=0;
static bool InputChanged = true;		  // records new input mode changes
//static HBITMAP hBitmap1 = 0;			  // bitmaps for landscape window
//static HBITMAP hBitmap2 = 0;			// 1 is unchanged for resetting, 2 is workmap
//static HBITMAP hBitmap3 = 0;			// 3 is temp fuel changes && barriers
static double xrat, yrat;			// ratio of original window to current
static double MaxX, MaxY;			// max coords of farsite display window
static double tint;					// smallest interval of x and y extent of visible landscape
static double FullTint;				// smallest interval of x & y for whole landscape
static long MaxWindowDim = 8000;		// maximum farsite window dimension
static bool ViewPort_Changed = false;	//
static bool ViewPort_Status = false;
static double ViewPortNorth = 0;
static double ViewPortSouth = 0;
static double ViewPortEast = 0;
static double ViewPortWest = 0;
static long NumViewNorth;
static long NumViewEast;
static long AdjustIgSpreadRates = 1;	   // can use initial spread rate dialog for ignitions
static int FuelsAltered = 0;
static FuelConversions fuelconversion;
static bool NEED_CUST_MODELS = false;	// custom fuel models
static bool HAVE_CUST_MODELS = false;
static bool NEED_CONV_MODELS = false;     // fuel model conversions
static bool HAVE_CONV_MODELS = false;
//static FuelModels FuelMod;
//static ThemeColors themecolors;		// contains all non-fuel theme color assignments
static long CanSetRastRes = true;		// flag to allow setting raster resolution
static long RasterVPExtent = false;
static double RasterCellResolutionX;
static double RasterCellResolutionY;
static double SimulationDuration = 0.0;
static bool CondPeriod = false;
static bool EnvtChanged[4][5] =
{
	{false, false, false, false, false}, {false, false, false, false, false},
	{false, false, false, false, false}, {false, false, false, false, false}
};

static char RasterArrivalTime[256];
static char RasterFireIntensity[256];
static char RasterFlameLength[256];
static char RasterSpreadRate[256];
static char RasterHeatPerArea[256];
static char RasterCrownFire[256];
static char RasterFireDirection[256];
static char RasterReactionIntensity[256];

static bool rast_arrivaltime = false;
static bool rast_fireintensity = false;
static bool rast_spreadrate = false;
static bool rast_flamelength = false;
static bool rast_heatperarea = false;
static bool rast_crownfire = false;
static bool rast_firedirection = false;
static bool rast_reactionintensity = false;
static long ZoomReplayDelay = 0;

static char VectFName[256];
static char RastFName[256];
static char ShapeFileName[256] = "";
static long ShapeVisibleStepsOnly = 1;
static long ShapePolygonsNotLines = 1;
static long ShapeBarriersSeparately = 0;
static char ThreeDRasterFile[256];
static long ThreeDRasterHi = 0;
static long ThreeDRasterLo = 0;
static bool View3DVectorFile = false;
static char ThreeDVectorFile[256];
static COLORREF VectorColor = RGB(255, 255, 255);
static long VectorFileType = 0; 		  // 0=vis, 1=GRASS, 2-4=ARC
static long VisibleThemeNumber = 3;		// fuels default visible theme
static long LandscapeInputMode = NULLLOCATION;
static bool CrownDensityLinkToCC = false;    // crown bulk density is a function of canopy cover

static bool RastMake = false;
static bool VectMake = false;
static bool ShapeMake = false;
static bool VISONLY = false;		// visible time step only for vector file
static bool TOPOSHADING = true;		// topographic shading from display dialog
static long RastFormat = 1;
static long VectFormat = 0;
static char VisPerim[256] = "";//"c:\\visperim.vsp";
static FILE* VP = 0;
static WindData* wddt[5] =
{
	0, 0, 0, 0, 0
};
static WeatherData* wtrdt[5] =
{
	0, 0, 0, 0, 0
};
static long MaxWeatherObs[5] =
{
	0, 0, 0, 0, 0
};
static long MaxWindObs[5] =
{
	0, 0, 0, 0, 0
};
static long DurationReset = 0;			// force DurationReset at ReStart
static long IgnitionReset = 0;
static long RotateIgnitions = 0;
static long ShowVectsAsGrown = 1;
static bool InactiveEnclaves = true;
static char CurrentPath[256] = "";
static char getThemeFileName_name[256] = "";
static bool UseCurrentPath = false;

static long LightSourceDirection = 225;
static long OverlayGridInterval = 0;
//static long OverlayGridColor = RGB(255, 255, 255);

//------------------ Display Fire Characteristics on 2-D and 3-D Landscapes
static long FireOutputParam = 4;
static double FireMaxChx = 1000.0;
static double FireMinChx = 0.0;
static long FireNumInterval = 16;
//------------------------------------------------------------------------------

static streamdata FirstMonth[5];
static streamdata FirstDay[5];
static streamdata FirstHour[5];
static streamdata LastMonth[5];
static streamdata LastDay[5];
static streamdata LastHour[5];

bool PreCalcFuelMoistures = true;
long MoistCalcInterval[4][5] =
{
	{60, 200, 10, 45, 15},  		 // 1hr
	{120, 200, 10, 45, 15}, 		 // 10hr
	{180, 400, 10, 45, 20}, 		 // 100hr
	{240, 400, 15, 45, 30}
};		   // 1000hr

NewFuel newfuels[257];									// custom and standard fuel models

/*
static int FirstMonth[5]={0,0,0,0,0};
static int FirstDay[5]={0,0,0,0,0};
static int FirstHour[5]={-1,-1,-1,-1,-1};
static int LastMonth[5]={0,0,0,0,0};
static int LastDay[5]={0,0,0,0,0};
static int LastHour[5]={-1,-1,-1,-1,-1};
*/

static long conditmonth = 0, conditday = 0;
static long startmonth = 0, startday = 0, starthour = 0, startmin = 0,
	startdate = 0;		//INITIAL CONDITIONS AT STARTPOINT
static long endmonth = 0, endday = 0, endhour = 0, endmin = 0, enddate = 0;
static long OldFilePosition = 0;	//__thread 
static headdata2 Header2;
static headdata Header;
static oldheaddata OldHeader;
//static celldata __thread cell;
//static crowndata __thread cfuel;
//static grounddata __thread gfuel;
static size_t headsize = sizeof(headdata32bit);
static FILE* landfile = 0;
static char LandFName[256] = "";
static StationGrid grid;
static long NumWeatherStations = 0;
static long NumWindStations = 0;
static double NorthGridOffset = 0.0;
static double EastGridOffset = 0.0;

static short* landscape = 0;

//static LOGPALETTE* FarPal = 0;
//static HPALETTE hLogPal = 0, hMemPal = 0, PrevPalette1 = 0, PrevPalette2 = 0;
//static bool ValidPalette = false;
static bool XORArrows = true;   			   // draw XORArrows for wind vetors

static CanopyCharacteristics CanopyChx;//={15.0, 4.0, 0.20, 20.0, 100, 2, 1}; //__thread
//static double __thread DuffLoad=0.0;
//static double __thread WoodyModel=0;

//static VectorStorage StoreVect[5];
static long NumVectorThemes = 0;

static AtmosphereGrid* AtmGrid = 0;			// pointer to AtmGrid;

/*
FuelModels::FuelModels()					// constructor for fuel models
{
	FCOLOR[0]=RGB(50,50,50);
	FCOLOR[1]=RGB(0,180,0);
	FCOLOR[2]=RGB(80,80,0);
	FCOLOR[3]=RGB(140,140,0);
	FCOLOR[4]=RGB(200,0,0);
	FCOLOR[5]=RGB(160,0,0);
	FCOLOR[6]=RGB(120,0,0);
	FCOLOR[7]=RGB(80,0,0);
	FCOLOR[8]=RGB(0,0,80);
	FCOLOR[9]=RGB(0,0,120);
	FCOLOR[10]=RGB(0,0,160);
	FCOLOR[11]=RGB(80,0,80);
	FCOLOR[12]=RGB(120,0,120);
	FCOLOR[13]=RGB(180,180,180);
	FCOLOR[51]=RGB(0,0,255);
	for(NumChanges=0; NumChanges<9; NumChanges++)
		OrderToFuel[NumChanges]=0;
	for(NumChanges=0; NumChanges<37; NumChanges++)
		FuelMod.ModelIsHere[NumChanges]=false;
	NumChanges=0;
}
*/

void ResetFuelConversions()
{
	long i;

	for (i = 0;
		i < 257;
		i++)   		  		// could also read default file here
		fuelconversion.Type[i] = i;
	fuelconversion.Type[0] = -1;
	for (i = 91; i < 100; i++)
		fuelconversion.Type[i] -= 100;
	HAVE_CONV_MODELS = false;
}


void ResetCustomFuelModels()
{
	/*
		long Changes;
		for(Changes=0; Changes<9; Changes++)
			FuelMod.OrderToFuel[Changes]=0;
		for(Changes=0; Changes<37; Changes++)
		{	SetFuel(Changes, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "");
			FuelMod.ModelIsHere[Changes]=false;
		}
		FuelMod.NumChanges=0;
		HAVE_CUST_MODELS=false;
	*/
}

void mainCmd(int argc, char* argv[])
{
	char ProjectFile[256] = "";
	bool FilesOK = true;
	char ErrMsg[256] = "";

	if (argc == 2)
	{
		FILE* testfile;

		strcpy(ProjectFile, argv[1]);
		if ((testfile = fopen(ProjectFile, "r")) == NULL)
		{
			sprintf(ErrMsg, "%s %s", ProjectFile, "Could Not Be Opened");
			FilesOK = false;
		}
		fclose(testfile);
	}
	if (!FilesOK)
	{
		printf("ERROR:%s\n\n", ErrMsg);		exit(1);
	}
}

/** Hopefully will read in an ignition file for us.
*
*/
bool ReadIgnitions(char* igfi)
{
	double PI = 3.1415926536;
	long NumIgnitions;
	char IgnitionFile[256] = "";		 //"ignition.txt";
	char points[256], lines[256], polys[256], TestEnd[20],
		NewIgnitionFile[256];
	long count, count1, count2, startsize = 5;
	long fposition1, OldNumIgnitions = 0;
	double CenterX, CenterY, xpt, ypt, angle;//, FireArea;
	FILE* ignitions;
	FILE* ignitionfile;
	headdata NewHeader;			// header information structure
	bool IgnitionFileExists = false;
	memset(points, 0x0, sizeof points);
	memset(lines, 0x0, sizeof lines);
	memset(polys, 0x0, sizeof polys);
	memset(TestEnd, 0x0, sizeof TestEnd);

	if ((ignitions = fopen(IgnitionFile, "r")) == NULL)
	{
		//	ErrorNumber=9;
		return false;
	}

	while (!feof(ignitions))
	{
		fscanf(ignitions, "%s", NewIgnitionFile);
		if (feof(ignitions))
			break;
		OldNumIgnitions++;

		if (OldNumIgnitions <= NumIgnitions)
			continue;

		if ((ignitionfile = fopen(NewIgnitionFile, "r")) == NULL)
			continue;

		NumIgnitions = OldNumIgnitions;
		fscanf(ignitionfile, "%s", points);
		if (feof(ignitionfile))
		{
			fclose(ignitionfile);
			continue;
		}
		fscanf(ignitionfile, "%s %s", lines, polys);
		fclose(ignitionfile);
		if ((ignitionfile = fopen(points, "r")) != NULL)
		{
			fscanf(ignitionfile, "%s", TestEnd);
			if (feof(ignitionfile))
			{
				fclose(ignitionfile);
				ignitionfile = 0;
			}
			else if (ignitionfile)
			{
				while (!feof(ignitionfile))
				{
					if (strcmp(TestEnd, "END"))
					{
						fscanf(ignitionfile, "%lf %lf", &CenterX, &CenterY);
						CenterX -= ((long) (NewHeader.WestUtm / 1000)) * 1000;
						CenterY -= ((long) (NewHeader.SouthUtm / 1000)) * 1000;
						AllocPerimeter1(GetNewFires(), 11);		// 10 points, 11th is for bounding box
						for (count = 0; count <= 9; count++)
						{
							angle = (double (count) * (PI / 5.0));
							xpt = CenterX + (cos(angle) * startsize);
							ypt = CenterY + (sin(angle) * startsize);
							SetPerimeter1(GetNewFires(), count, xpt, ypt);
							SetFireChx(GetNewFires(), count, 0, 0);
						}
						SetNumPoints(GetNewFires(), count);
						SetInout(GetNewFires(), 1);
						IncNewFires(1);
						BoundingBox();
						fscanf(ignitionfile, "%s", TestEnd);
					}
					else
					{
						fclose(ignitionfile);
						IgnitionFileExists = true;
						break;
					}
				}
			}
		}

		if ((ignitionfile = fopen(lines, "r")) != NULL)
		{
			fscanf(ignitionfile, "%s", TestEnd);
			if (feof(ignitionfile))
			{
				fclose(ignitionfile);
				ignitionfile = 0;
			}
			else if (ignitionfile)
			{
				while (strcmp(TestEnd, "END"))
				{
					fposition1 = ftell(ignitionfile);
					count = 0;
					do
					{
						fscanf(ignitionfile, "%s", TestEnd);
						if (!strcmp(TestEnd, "END"))
							break;
						fscanf(ignitionfile, "%lf", &CenterY);
						count++;
					}
					while (strcmp(TestEnd, "END"));
					fseek(ignitionfile, fposition1, SEEK_SET);
					if (count == 2)
					{
						AllocPerimeter1(GetNewFires(), 5);		// 1 more for bounding box
						fscanf(ignitionfile, "%lf %lf", &CenterX, &CenterY);
						fscanf(ignitionfile, "%lf %lf", &xpt, &ypt);
						CenterX -= ((long) (NewHeader.WestUtm / 1000)) * 1000;
						CenterY -= ((long) (NewHeader.SouthUtm / 1000)) * 1000;
						xpt -= ((long) (NewHeader.WestUtm / 1000)) * 1000;
						ypt -= ((long) (NewHeader.SouthUtm / 1000)) * 1000;
						SetPerimeter1(GetNewFires(), 0, CenterX, CenterY);
						SetFireChx(GetNewFires(), 0, -1.0, 0.0);	// 100% of equilibrium spread
						SetPerimeter1(GetNewFires(), 2, xpt, ypt);
						SetFireChx(GetNewFires(), 2, -1.0, 0.0);	// 100% of equilibrium spread
						double xdiff, ydiff, midx, midy;//, oppslope;
						xdiff = CenterX - xpt;
						ydiff = CenterY - ypt;
						midx = CenterX - xdiff / 2.0;   	//-xdiff/100.0;
						midy = CenterY - ydiff / 2.0;   	//-xdiff/100.0*oppslope;
						SetPerimeter1(GetNewFires(), 1, midx, midy);
						SetFireChx(GetNewFires(), 1, -1.0, 0.0);	// 100% of equilibrium spread
						SetPerimeter1(GetNewFires(), 3, midx, midy);
						SetFireChx(GetNewFires(), 3, -1.0, 0.0);	// 100% of equilibrium spread
						count = 3;
					}
					else
					{
						AllocPerimeter1(GetNewFires(), 2 * count - 1);		// 1 more for bounding box
						//count1=0;
						count2 = 2 * count - 3;
						for (count1 = 0; count1 < count; count1++)
						{
							fscanf(ignitionfile, "%lf %lf", &CenterX, &CenterY);
							CenterX -= ((long) (NewHeader.WestUtm / 1000)) * 1000;
							CenterY -= ((long) (NewHeader.SouthUtm / 1000)) * 1000;
							SetPerimeter1(GetNewFires(), count1, CenterX,
								CenterY);
							SetFireChx(GetNewFires(), count1, -1.0, 0.0);	// 100% of equilibrium spread
							if (count1 > 0 && count1 < count - 1)   			   // write points bacward in array
							{
								SetPerimeter1(GetNewFires(), count2, CenterX,
									CenterY);
								SetFireChx(GetNewFires(), count2, -1.0, 0.0);	// 100% of equilibrium spread
								--count2;
							}
						}
					}
					fscanf(ignitionfile, "%s", TestEnd);
					SetNumPoints(GetNewFires(), 2 * count - 2);
					SetInout(GetNewFires(), 1);
					IncNewFires(1);
					BoundingBox();							// find bounding box for line source
					fscanf(ignitionfile, "%s", TestEnd);
				}
				fclose(ignitionfile);
				IgnitionFileExists = true;
			}
		}

		if ((ignitionfile = fopen(polys, "r")) != NULL)
		{
			fscanf(ignitionfile, "%s", TestEnd);
			if (feof(ignitionfile))
			{
				fclose(ignitionfile);
				ignitionfile = 0;
				//break;
			}
			else if (ignitionfile)
			{
				while (strcmp(TestEnd, "END"))
				{
					fposition1 = ftell(ignitionfile);
					count = 0;
					do
					{
						fscanf(ignitionfile, "%s", TestEnd);
						if (!strcmp(TestEnd, "END"))
							break;
						fscanf(ignitionfile, "%lf", &CenterY);
						count++;
					}
					while (strcmp(TestEnd, "END"));
					fseek(ignitionfile, fposition1, SEEK_SET);
					count1 = count;
					AllocPerimeter1(GetNewFires(), count1 + 1);		// 1 more for bounding box
					for (count = 0; count < count1; count++)
					{
						fscanf(ignitionfile, "%lf %lf", &CenterX, &CenterY);
						CenterX -= ((long) (NewHeader.WestUtm / 1000)) * 1000;
						CenterY -= ((long) (NewHeader.SouthUtm / 1000)) * 1000;
						SetPerimeter1(GetNewFires(), count, CenterX, CenterY);
						SetFireChx(GetNewFires(), count, -1.0, 0.0);	// 100% of equilibrium spread
					}
					fscanf(ignitionfile, "%s", TestEnd);
					SetNumPoints(GetNewFires(), count1);
					SetInout(GetNewFires(), 1);
					IncNewFires(1);
					if (arp() < 0)
						ReversePoints(1);
					BoundingBox();							// find bounding box for line source
					fscanf(ignitionfile, "%s", TestEnd);
				}
				fclose(ignitionfile);
				IgnitionFileExists = true;
			}
		}
	}
	fclose(ignitions);

	return IgnitionFileExists;
}

double arp()
/* CALCULATES AREA AND PERIMETER AS A PLANIMETER (WITH TRIANGLES) */
{
	/*	long count, count1=0, FireNum=GetNewFires()-1, numx;
	double xpt1, ypt1, xpt2, ypt2, aangle, zangle, DiffAngle;
	double newarea, area=0.0;
	numx=GetNumPoints(FireNum);
	if(numx<3)
	   return area;
	poly.startx=GetPerimeter1Value(FireNum, 0, 0);
	poly.starty=GetPerimeter1Value(FireNum, 0, 1);
	while(count1<numx)
	{  count1++;
		xpt1=GetPerimeter1Value(FireNum, 1, 0);//poly.perimget(1, 0, FireNum);
	   ypt1=GetPerimeter1Value(FireNum, 1, 1);//poly.perimget(1, 1, FireNum);
		zangle=poly.direction(xpt1, ypt1);
	   if(zangle!=999.9)
		  break;
	}
	count1++;
	for(count=count1;count<numx;count++)
	{	xpt2=GetPerimeter1Value(FireNum, count, 0);
		ypt2=GetPerimeter1Value(FireNum, count, 1);
		newarea=.5*(poly.startx*ypt1-xpt1*poly.starty+xpt1*ypt2-xpt2*ypt1+xpt2*poly.starty-poly.startx*ypt2);
		newarea=fabs(newarea);
		aangle=poly.direction(xpt2, ypt2);
		if(aangle!=999.9)
		{   DiffAngle=aangle-zangle;
			if(DiffAngle>PI)
			   DiffAngle=-(2.0*PI-DiffAngle);
			else if(DiffAngle<-PI)
			   DiffAngle=2.0*PI+DiffAngle;
			if(DiffAngle>0.0)
			   area-=newarea;
			else if(DiffAngle<0.0)
			   area+=newarea;
		   zangle=aangle;
		}
		xpt1=xpt2;
		ypt1=ypt2;
	}
	return area;
		 */
	return 0; //delete when uncommenting
}


void ReversePoints(int TYPE)
{
	/*
		long j, AFire=GetNewFires()-1;
		long count, BFire=GetNewFires();
		double fxc, fyc, fxc2, fyc2, RosI, RosI2;
		long halfstop=GetNumPoints(AFire)/2;				// truncated number of points
		switch(TYPE)
		{	case 0:  		// transfer points in reverse to next array, +1 fires
				AllocPerimeter1(BFire, GetNumPoints(AFire)); 	// allocate new array
				for(count=0; count<GetNumPoints(AFire); count++)
				{	fxc=GetPerimeter1Value(AFire, count, 0);
					fyc=GetPerimeter1Value(AFire, count, 1);
					RosI=GetPerimeter1Value(AFire, count, 2);
					j=GetNumPoints(AFire)-(count+1);
					SetPerimeter1(BFire, j, fxc, fyc);  		 // reverse points
					SetFireChx(BFire, j, RosI, 0);  			 // in next available array
				}   									   // set bounding box
				SetPerimeter1(BFire, count, GetPerimeter1Value(AFire, count, 0), GetPerimeter1Value(AFire, count, 1));  		 // reverse points
				SetFireChx(BFire, count, GetPerimeter1Value(AFire, count, 2), GetPerimeter1Value(AFire, count, 3)); 				 // in next available array
				SetNumPoints(BFire, count);		// same number of points in fire
				SetInout(BFire,1);  	  		// ID fire as outward burning
				IncNewFires(1);
				break;
			case 1:		// reverse points in same array
				for(count=0; count<halfstop; count++)
				{    j=GetNumPoints(AFire)-count-1;
					fxc=GetPerimeter1Value(AFire, count, 0);
					fyc=GetPerimeter1Value(AFire, count, 1);
					RosI=GetPerimeter1Value(AFire, count, 2);
					fxc2=GetPerimeter1Value(AFire, j, 0);
					fyc2=GetPerimeter1Value(AFire, j, 1);
					RosI2=GetPerimeter1Value(AFire, j, 2);
					SetPerimeter1(AFire, count, fxc2, fyc2);
					SetFireChx(AFire, count, RosI2, 0);
					SetPerimeter1(AFire, j, fxc, fyc);
					SetFireChx(AFire, j, RosI, 0);
				}
				break;
		}
			*/
}


void BoundingBox()
{
	/*
		double xpt, ypt, Xlo, Xhi, Ylo, Yhi;
		long NumFire=GetNewFires()-1;
		long NumPoint=GetNumPoints(NumFire);

		Xlo=Xhi=GetPerimeter1Value(NumFire, 0, 0);
		Ylo=Yhi=GetPerimeter1Value(NumFire, 0, 1);
		for(int i=1; i<NumPoint; i++)
		{	xpt=GetPerimeter1Value(NumFire, i, 0);
			ypt=GetPerimeter1Value(NumFire, i, 1);
			if(xpt<Xlo) Xlo=xpt;
			else
			{	if(xpt>Xhi)
					Xhi=xpt;
			}
			if(ypt<Ylo) Ylo=ypt;
			else
			{	if(ypt>Yhi)
				Yhi=ypt;
			}
		}
		SetPerimeter1(NumFire, NumPoint, Xlo, Xhi);
		SetFireChx(NumFire, NumPoint, Ylo, Yhi);
			*/
}


void CheckMessageLoop()
{
	// catches and interprets incoming WINDOWS messages
//	MSG msg;

/*	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
*/
}


long GridInterval(long Interval)
{
	// get & set Grid Interval
	if (Interval >= 0)
		OverlayGridInterval = Interval;

	return OverlayGridInterval;
}

/*
COLORREF GridColor(long Colr)
{// get & set Grid Color
	if(Colr>=0)
	{	switch(Colr)
		{	case 0: OverlayGridColor=GetPaletteColor(255, 255, 255); break;
			case 1: OverlayGridColor=GetPaletteColor(192, 192, 192); break;
			case 2: OverlayGridColor=GetPaletteColor(128, 128, 128); break;
			case 3: OverlayGridColor=GetPaletteColor(0, 0, 0); break;
			default:OverlayGridColor=GetPaletteColor(255, 255, 255); break;
		}
	}

	return OverlayGridColor;
}
*/

long TopoShadeDirection(long Direction)
{
	// get & set Light Source Direction
	if (Direction >= 0)
		LightSourceDirection = Direction;

	return LightSourceDirection;
}


double pow2(double base)
{
	// replaces pow(X,Y) where Y==2
	return base * base;
}

void SetFullTint(double tint)
{
	FullTint = tint;
}

double GetFullTint()
{
	return FullTint;
}

void SetTint(double Tint)
{
	// sets minimum display units for landcape window
	tint = Tint;
}


double GetTint()
{
	// retrieves minimum display unit for landscappe window
	return tint;
}


void SetMaxWindowDim(long Dim)
{
	// sets dimension of landscape window
	MaxWindowDim = Dim;
}


long GetMaxWindowDim()
{
	// gets dimension of landscape window
	return MaxWindowDim;
}

bool ViewPortChanged(long TrueFalse)
{
	if (TrueFalse >= 0)
		ViewPort_Changed = TrueFalse;

	return ViewPort_Changed;
}

bool ViewPortStatus(long TrueFalse)
{
	if (TrueFalse >= 0)
	{
		ViewPort_Status = TrueFalse;
		if (TrueFalse == 0)
		{
			ViewPortNorth = Header.hinorth;
			ViewPortSouth = Header.lonorth;
			ViewPortEast = Header.hieast;
			ViewPortWest = Header.loeast;
		}
	}

	return ViewPort_Status;
}

void SetLandscapeViewPort(double north, double south, double east, double west)
{
	double cols, rows, f, ic, ir;

	ViewPortNorth = north;
	ViewPortSouth = south;
	ViewPortEast = east;
	ViewPortWest = west;
	rows = ((ViewPortNorth - ViewPortSouth) / GetCellResolutionY());
	f = modf(rows, &ir);
	NumViewNorth = (long) ir;
	if (f > 0.50)
		NumViewNorth++;
	cols = ((ViewPortEast - ViewPortWest) / GetCellResolutionX());
	f = modf(cols, &ic);
	NumViewEast = (long) ic;
	if (f > 0.5)
		NumViewEast++;
}

void GetLandscapeViewPort(double* north, double* south, double* east,
	double* west)
{
	*north = ViewPortNorth;
	*south = ViewPortSouth;
	*east = ViewPortEast;
	*west = ViewPortWest;
}


double GetViewNorth()
{
	return ViewPortNorth;
}

double GetViewSouth()
{
	return ViewPortSouth;
}

double GetViewEast()
{
	return ViewPortEast;
}

double GetViewWest()
{
	return ViewPortWest;
}

long GetNumViewEast()
{
	return NumViewEast;
}

long GetNumViewNorth()
{
	return NumViewNorth;
}


char* GetCurrentPath()
{
	if (!UseCurrentPath)
		memset(CurrentPath, 0x0, 256 * sizeof(char));
	else
		//GetCurrentDirectory(256, CurrentPath);
                getcwd(CurrentPath,512);
	return CurrentPath;
}


bool CanUseCurrentPath(long YesNo)
{
	if (YesNo > -1)
		UseCurrentPath = (bool) YesNo;

	return UseCurrentPath;
}


//------------------------------------------------------------------------------
//
//
//
//------------------------------------------------------------------------------


long AdjustIgnitionSpreadRates(long YesNoValue)
{
	if (YesNoValue > -1)
		AdjustIgSpreadRates = YesNoValue;

	return AdjustIgSpreadRates;
}


//------------------------------------------------------------------------------
//
//
//
//------------------------------------------------------------------------------


CanopyCharacteristics::CanopyCharacteristics()
{
	DefaultHeight = Height = 15.0;  		 // need default for changing Variables in LCP
	DefaultBase = CrownBase = 4.0;
	DefaultDensity = BulkDensity = 0.20;
	Diameter = 20.0;
	FoliarMC = 100;
	Tolerance = 2;
	Species = 1;
}


void SetCanopyChx(double Height, double CrownBase, double BulkDensity,
	double Diameter, long FoliarMoisture, long Tolerance, long Species)
{
	CanopyChx.Height = CanopyChx.DefaultHeight = Height;
	CanopyChx.CrownBase = CanopyChx.DefaultBase = CrownBase;
	CanopyChx.BulkDensity = CanopyChx.DefaultDensity = BulkDensity;
	CanopyChx.Diameter = Diameter;
	CanopyChx.FoliarMC = FoliarMoisture;
	CanopyChx.Tolerance = Tolerance;
	CanopyChx.Species = Species;
}


void GetDefaultCrownChx(double* Height, double* Base, double* Density)
{
	*Height = CanopyChx.DefaultHeight;
	*Base = CanopyChx.DefaultBase;
	*Density = CanopyChx.DefaultDensity;
}


double GetDefaultCrownHeight()
{
	return CanopyChx.Height;
}


double GetDefaultCrownBase()
{
	return CanopyChx.CrownBase;
}


double GetDefaultCrownBD(short cover)
{
	if (CrownDensityLinkToCC)
		return CanopyChx.BulkDensity * ((double) cover) / 100.0;

	return CanopyChx.BulkDensity;
}


double GetAverageDBH()
{
	return CanopyChx.Diameter;
}


double GetFoliarMC()
{
	return CanopyChx.FoliarMC;
}


long GetTolerance()
{
	return CanopyChx.Tolerance;
}


long GetCanopySpecies()
{
	return CanopyChx.Species;
}


int GetNumFuelChanges()
{
	return 0;//FuelMod.NumChanges;
}


void SetNumFuelChanges(int Num)
{
	/*if(Num==0)						 // reset all changes if Num is 0
	{	for(Num=0; Num<9; Num++)
			FuelMod.OrderToFuel[Num]=0;
		Num=0;
	}
	FuelMod.NumChanges=Num;
	*/
}


void IncNumFuelChanges()
{
	//FuelMod.NumChanges++;
}


long FuelModelUnits = 0;

long AccessFuelModelUnits(long Val)
{
	if (Val > -1)
		FuelModelUnits = Val;

	return FuelModelUnits;
}


void SetTopoShading(bool YesNo)
{
	TOPOSHADING = YesNo;
}


bool TopoShading()
{
	return TOPOSHADING;
}


long GetInputMode()
{
	// gets input mode to the landscape window

	return LandscapeInputMode;
}


void SetInputMode(long ModeCode, bool Pending)
{
	// sets input model to the landscape window
	LandscapeInputMode = ModeCode;
	if (Pending)
		LandscapeInputMode += 100;
}

/*
COLORREF GetFuelColor(int Number)
{
	if(Number==98 || Number==-2)
		Number=51;
	else if(Number==99 || Number==-1)
		Number=0;
	if(Number<52 && Number>-1)
	{    COLORREF colr=FuelMod.FCOLOR[Number];
		if(ValidPalette)
			return GetPaletteColor(GetRValue(colr),GetGValue(colr),GetBValue(colr));

		return colr;
	}

	return RGB(0, 0, 0);
}


COLORREF GetChangedColor(int Number)
{
	COLORREF colr=FuelMod.CCOLOR[Number];
	if(ValidPalette)
		return GetPaletteColor(GetRValue(colr),GetGValue(colr),GetBValue(colr));
	else
		return colr;
}


void SetFuelColor(int Number, COLORREF colr)
{
	if(Number>51 || Number==-2)
		Number=51;
	if(Number==-1)
		Number=0;
	FuelMod.FCOLOR[Number]=colr;
}


void SetChangedColor(int Number, COLORREF colr)
{
	FuelMod.CCOLOR[Number]=colr;
}


void SetOrderToFuel(int Number, int Order)
{
	FuelMod.OrderToFuel[Number]=Order;
}


int GetOrderToFuel(int Number)
{
	return FuelMod.OrderToFuel[Number];
}
*/

void SetXORArrows(bool TorF)
{
	XORArrows = TorF;
}


bool GetXORArrows()
{
	return XORArrows;
}


void GetMaxDims(double* maxx, double* maxy)
{
	*maxx = MaxX;
	*maxy = MaxY;
}


void SetMaxDims(double maxx, double maxy)
{
	MaxX = maxx;
	MaxY = maxy;
}


void GetRatios(double* xratio, double* yratio)
{
	*xratio = xrat;
	*yratio = yrat;
}


void SetRatios(double xratio, double yratio)
{
	xrat = xratio;
	yrat = yratio;
}


//TWindow * GetFarsiteWindow()
/*TFarsiteWindow * GetFarsiteWindow()
{
	return FarsiteWindow;
}
*/


/*TThreeDWindow *Get3dWindow()
{
	return ThreeDWindow;
}
*/


//void SetFarsiteWindow(TWindow* FWindow)
/*void SetFarsiteWindow(TFarsiteWindow* FWindow)
{
	FarsiteWindow=FWindow;
}
*/


/*void Set3dWindow(TThreeDWindow *window)
{
	ThreeDWindow=window;
}


HBITMAP GetBitmap(short Number)
{
	HBITMAP bm = 0;

	switch (Number)
	{
	case 1:
		bm = hBitmap1; break;
	case 2:
		bm = hBitmap2; break;
	case 3:
		bm = hBitmap3; break;
	}

	return bm;
}


void ResetBitmap(short Number)
{
	switch (Number)
	{
	case 1:
		if (hBitmap1)
			DeleteObject(hBitmap1);
		hBitmap1 = 0;
		break;
	case 2:
		if (hBitmap2)
			DeleteObject(hBitmap2);
		hBitmap2 = 0;
		break;
	case 3:
		if (hBitmap3)
			DeleteObject(hBitmap3);
		hBitmap3 = 0;
		break;
	}
}

void SetBitmap(short Number, HBITMAP hBitmap)
{
	switch (Number)
	{
	case 1:
		hBitmap1 = hBitmap; break;
	case 2:
		hBitmap2 = hBitmap; break;
	case 3:
		hBitmap3 = hBitmap; break;
	}
}*/

int GetFuelsAltered()
{
	return FuelsAltered;
}

void SetFuelsAltered(int YesNo)
{
	FuelsAltered = YesNo;
}


FuelConversions::FuelConversions()
{
	long i;

	for (i = 0;
		i < 257;
		i++)   		  		// could also read default file here
		Type[i] = i;
	Type[99] = -1;
	Type[98] = -2;
	Type[97] = -3;
	Type[96] = -4;
	Type[95] = -5;
	Type[94] = -6;
	Type[93] = -7;
	Type[92] = -8;
	Type[91] = -9;
}


int GetFuelConversion(int fuel)
{
	// retrieve fuel model conversions
	int cnv = -1;

	if (fuel >= 0 && fuel < 257)	// check fuel for valid array range
	{
		cnv = fuelconversion.Type[fuel];   // get new fuel
		if (cnv < 1)  // if not
		{
			if (cnv < -9)
				cnv = -1;
			//if(cnv!=0 && cnv!=-1 && cnv!=-2)
			//	cnv=-1;    // return rock of other than -1 or -2
		}
		else if (!IsNewFuelReserved(cnv)) //NewFuel>13 && NewFuel<51)    // check to see if custom model required
		{
			if (newfuels[cnv].number == 0)   // check to see if cust mod here
				cnv = -1;     // return rock if no fuel model
		}
		else if (cnv > 256)  // if new fuel too high
			cnv = -1;
	}

	return cnv;
}


int SetFuelConversion(int From, int To)
{
	// set fuel model conversions
	if (From >= 0 && From < 257 && To < 257 && To >= 0)
	{
		if (To > 90 && To < 100)
			To = To - 100;		// make all negative for the 90's, indicate unburnable
		else if (To == 0)
			To = -1;
		fuelconversion.Type[From] = To;
	}
	else
		return false;

	return true;
}


bool ChangeInputMode(long YesNo)
{
	// true if new change, false if not, -1 returns existing value
	if (YesNo >= 0)
		InputChanged = YesNo;

	return InputChanged;
}


void SetZoomReplayDelay(long MilliSeconds)
{
	if (MilliSeconds > 1000)
		MilliSeconds = 1000;
	if (MilliSeconds < 0)
		MilliSeconds = 0;
	ZoomReplayDelay = MilliSeconds;
}


long GetZoomReplayDelay()
{
	return ZoomReplayDelay;
}

long GetTheme_DistanceUnits()
{
	return Header.GridUnits;
}


short GetTheme_Units(short DataTheme)
{
	short units;

	switch (DataTheme)
	{
	case 0:
		units = Header.EUnits; break;
	case 1:
		units = Header.SUnits; break;
	case 2:
		units = Header.AUnits; break;
	case 3:
		units = Header.FOptions; break;
	case 4:
		units = Header.CUnits; break;
	case 5:
		units = Header.HUnits; break;
	case 6:
		units = Header.BUnits; break;
	case 7:
		units = Header.PUnits; break;
	case 8:
		units = Header.DUnits; break;
	case 9:
		units = Header.WOptions; break;
	}

	return units;
}

short GetTheme_NumCategories(short DataTheme)
{
	long cats;

	switch (DataTheme)
	{
	case 0:
		cats = Header.numelev; break;
	case 1:
		cats = Header.numslope; break;
	case 2:
		cats = Header.numaspect; break;
	case 3:
		cats = Header.numfuel; break;
	case 4:
		cats = Header.numcover; break;
	case 5:
		cats = Header.numheight; break;
	case 6:
		cats = Header.numbase; break;
	case 7:
		cats = Header.numdensity; break;
	case 8:
		cats = Header.numduff; break;
	case 9:
		cats = Header.numwoody; break;
	}

	return (short) cats;
}

long GetTheme_HiValue(short DataTheme)
{
	long hi;

	switch (DataTheme)
	{
	case 0:
		hi = Header.hielev; break;
	case 1:
		hi = Header.hislope; break;
	case 2:
		hi = Header.hiaspect; break;
	case 3:
		hi = Header.hifuel; break;
	case 4:
		hi = Header.hicover; break;
	case 5:
		hi = Header.hiheight; break;
	case 6:
		hi = Header.hibase; break;
	case 7:
		hi = Header.hidensity; break;
	case 8:
		hi = Header.hiduff; break;
	case 9:
		hi = Header.hiwoody; break;
	}

	return hi;
}

long GetTheme_LoValue(short DataTheme)
{
	long lo;

	switch (DataTheme)
	{
	case 0:
		lo = Header.loelev; break;
	case 1:
		lo = Header.loslope; break;
	case 2:
		lo = Header.loaspect; break;
	case 3:
		lo = Header.lofuel; break;
	case 4:
		lo = Header.locover; break;
	case 5:
		lo = Header.loheight; break;
	case 6:
		lo = Header.lobase; break;
	case 7:
		lo = Header.lodensity; break;
	case 8:
		lo = Header.loduff; break;
	case 9:
		lo = Header.lowoody; break;
	}

	return lo;
}


char* GetTheme_FileName(short DataTheme)
{
	//getThemeFileName_name

	switch (DataTheme)
	{
	case 0:
		strcpy(getThemeFileName_name, Header.ElevFile); break;
	case 1:
		strcpy(getThemeFileName_name, Header.SlopeFile); break;
	case 2:
		strcpy(getThemeFileName_name, Header.AspectFile); break;
	case 3:
		strcpy(getThemeFileName_name, Header.FuelFile); break;
	case 4:
		strcpy(getThemeFileName_name, Header.CoverFile); break;
	case 5:
		strcpy(getThemeFileName_name, Header.HeightFile); break;
	case 6:
		strcpy(getThemeFileName_name, Header.BaseFile); break;
	case 7:
		strcpy(getThemeFileName_name, Header.DensityFile); break;
	case 8:
		strcpy(getThemeFileName_name, Header.DuffFile); break;
	case 9:
		strcpy(getThemeFileName_name, Header.WoodyFile); break;
	}

	return getThemeFileName_name; 
}


bool LinkDensityWithCrownCover(long TrueFalse)
{
	if (TrueFalse >= 0)
		CrownDensityLinkToCC = TrueFalse;

	return CrownDensityLinkToCC;
};


long HaveCrownFuels()
{
	return Header.CrownFuels - 20;		// subtract 10 to ID file as version 2.x
}

long HaveGroundFuels()
{
	return Header.GroundFuels - 20;
}


double GetCellResolutionX()
{
	if (Header.GridUnits == 2)
		return Header.XResol * 1000.0;   // to kilometers

	return Header.XResol;
}


double GetCellResolutionY()
{
	if (Header.GridUnits == 2)
		return Header.YResol * 1000.0;

	return Header.YResol;
}


double MetricResolutionConvert()
{
	if (Header.GridUnits == 1)
		return 3.280839895; 	// metric conversion to meters
	else
		return 1.0;
}


int CheckCellResUnits()
{
	return Header.GridUnits;
}


void SetCustFuelModelID(bool True_False)
{
	HAVE_CUST_MODELS = True_False;
}


void SetConvFuelModelID(bool True_False)
{
	HAVE_CONV_MODELS = True_False;
}


bool NeedCustFuelModels()
{
	return NEED_CUST_MODELS;
}


bool NeedConvFuelModels()
{
	return NEED_CONV_MODELS;
}


bool HaveCustomFuelModels()
{
	return HAVE_CUST_MODELS;
}


bool HaveFuelConversions()
{
	return HAVE_CONV_MODELS;
}


void SetRastFormat(long Type)
{
	RastFormat = Type;
}


long GetRastFormat()
{
	return RastFormat;
}


void SetVectFormat(long Type)
{
	VectFormat = Type;
}


long GetVectFormat()
{
	return VectFormat;
}


long CanSetRasterResolution(long YesNo)
{
	if (YesNo > -1)
		CanSetRastRes = YesNo;

	return CanSetRastRes;
}


long SetRasterExtentToViewport(long YesNo)
{
	if (YesNo > -1)
		RasterVPExtent = YesNo;

	return RasterVPExtent;
}


void SetRastRes(double XResol, double YResol)
{
	// raster output resolution
	RasterCellResolutionX = XResol;
	RasterCellResolutionY = YResol;
}


void GetRastRes(double* XResol, double* YResol)
{
	// raster output resolution
	*XResol = RasterCellResolutionX;
	*YResol = RasterCellResolutionY;
}


void SetVectorFileName(const char* FileName)
{
	memset(VectFName, 0x0, sizeof(VectFName));
	sprintf(VectFName, "%s", FileName);
	FILE* newfile;

	newfile = fopen(VectFName, "w");
	if (newfile == NULL)
	{
		//SetFileAttributes(VectFName, FILE_ATTRIBUTE_NORMAL);
		chmod(VectFName,S_IRWXO);
		newfile = fopen(VectFName, "w");
	}
	fclose(newfile);
}


char* GetVectorFileName()
{
	return VectFName;
}


void SetRasterFileName(const char* FileName)
{
	memset(RasterArrivalTime, 0x0, sizeof(RasterArrivalTime));
	memset(RasterFireIntensity, 0x0, sizeof(RasterFireIntensity));
	memset(RasterFlameLength, 0x0, sizeof(RasterFlameLength));
	memset(RasterSpreadRate, 0x0, sizeof(RasterSpreadRate));
	memset(RasterHeatPerArea, 0x0, sizeof(RasterHeatPerArea));
	memset(RasterCrownFire, 0x0, sizeof(RasterCrownFire));
	memset(RasterFireDirection, 0x0, sizeof(RasterFireDirection));
	memset(RasterReactionIntensity, 0x0, sizeof(RasterReactionIntensity));
	memset(RastFName, 0x0, sizeof(RastFName));
	sprintf(RasterArrivalTime, "%s%s", FileName, ".toa");
	sprintf(RasterFireIntensity, "%s%s", FileName, ".fli");
	sprintf(RasterFlameLength, "%s%s", FileName, ".fml");
	sprintf(RasterSpreadRate, "%s%s", FileName, ".ros");
	sprintf(RasterHeatPerArea, "%s%s", FileName, ".hpa");
	sprintf(RasterCrownFire, "%s%s", FileName, ".cfr");
	sprintf(RasterFireDirection, "%s%s", FileName, ".sdr");
	sprintf(RasterReactionIntensity, "%s%s", FileName, ".rci");
	sprintf(RastFName, "%s%s", FileName, ".rst");
}


char* GetRasterFileName(long Type)
{
	if (Type == 0)
		return RastFName;
	if (Type == RAST_ARRIVALTIME)
		return RasterArrivalTime;
	if (Type == RAST_FIREINTENSITY)
		return RasterFireIntensity;
	if (Type == RAST_FLAMELENGTH)
		return RasterFlameLength;
	if (Type == RAST_SPREADRATE)
		return RasterSpreadRate;
	if (Type == RAST_HEATPERAREA)
		return RasterHeatPerArea;
	if (Type == RAST_CROWNFIRE)
		return RasterCrownFire;
	if (Type == RAST_FIREDIRECTION)
		return RasterFireDirection;
	if (Type == RAST_REACTIONINTENSITY)
		return RasterReactionIntensity;

	return NULL;
}


void SetFileOutputOptions(long FileType, bool YesNo)
{
	switch (FileType)
	{
	case RAST_ARRIVALTIME:
		rast_arrivaltime = YesNo;
		break;
	case RAST_FIREINTENSITY:
		rast_fireintensity = YesNo;
		break;
	case RAST_SPREADRATE:
		rast_spreadrate = YesNo;
		break;
	case RAST_FLAMELENGTH:
		rast_flamelength = YesNo;
		break;
	case RAST_HEATPERAREA:
		rast_heatperarea = YesNo;
		break;
	case RAST_CROWNFIRE:
		rast_crownfire = YesNo;
		break;
	case RAST_FIREDIRECTION:
		rast_firedirection = YesNo;
		break;
	case RAST_REACTIONINTENSITY:
		rast_reactionintensity = YesNo;
		break;
	}
}


bool GetFileOutputOptions(long FileType)
{
	bool YesNo = false;

	switch (FileType)
	{
	case RAST_ARRIVALTIME:
		YesNo = rast_arrivaltime;
		break;
	case RAST_FIREINTENSITY:
		YesNo = rast_fireintensity;
		break;
	case RAST_SPREADRATE:
		YesNo = rast_spreadrate;
		break;
	case RAST_FLAMELENGTH:
		YesNo = rast_flamelength;
		break;
	case RAST_HEATPERAREA:
		YesNo = rast_heatperarea;
		break;
	case RAST_CROWNFIRE:
		YesNo = rast_crownfire;
		break;
	case RAST_FIREDIRECTION:
		YesNo = rast_firedirection;
		break;
	case RAST_REACTIONINTENSITY:
		YesNo = rast_reactionintensity;
		break;
	}

	return YesNo;
}


void SetRastMake(bool YesNo)
{
	RastMake = YesNo;
}


bool GetRastMake()
{
	return RastMake;
}

static long DisplayUnits = 0;
static long OutputUnits = 0;

long AccessDisplayUnits(long val)
{
	if (val >= 0)
		DisplayUnits = val;

	return DisplayUnits;
}


long AccessOutputUnits(long val)
{
	if (val >= 0)
		OutputUnits = val;

	return OutputUnits;
}




void SetVectMake(bool YesNo)
{
	VectMake = YesNo;
}


bool GetVectMake()
{
	return VectMake;
}

void SetShapeMake(bool YesNo)
{
	ShapeMake = YesNo;
}


bool GetShapeMake()
{
	return ShapeMake;
}

long ExcludeBarriersFromVectorFiles(long YesNo)
{
	if (YesNo >= 0)
		ShapeBarriersSeparately = YesNo;

	return ShapeBarriersSeparately;
}


void SetVectVisOnly(bool YesNo)
{
	VISONLY = YesNo;
}


bool GetVectVisOnly()
{
	return VISONLY;
}


void SetShapeFileChx(const char* ShapeName, long VisOnly, long Polygons,
	long BarriersSep)
{
	memset(ShapeFileName, 0x0, sizeof(ShapeFileName));
	strcpy(ShapeFileName, ShapeName);
	ShapeVisibleStepsOnly = VisOnly;
	ShapePolygonsNotLines = Polygons;
	ShapeBarriersSeparately = BarriersSep;
}


char* GetShapeFileChx(long* VisOnly, long* Polygons, long* BarriersSep)
{
	*VisOnly = ShapeVisibleStepsOnly;
	*Polygons = ShapePolygonsNotLines;
	*BarriersSep = ShapeBarriersSeparately;

	return ShapeFileName;
}

/*
//------------------------------------------------------------------------------
//  Raster Overlay Stuff
//------------------------------------------------------------------------------

static double rW, rE, rN, rS, rCellSizeX, rCellSizeY, rMaxVal, rMinVal;
static long   rRows, rCols;
static double *RasterOverlay=0;

bool	CreateRasterOverlay(char *FileName, double *max, double *min)
{
	long i, j;
	 char TestRead[30];
	 double value;
	 double rMaxVal, rMinVal;
	 double rN, rS, rE, rW, rCellSizeX, rCellSizeY;
	 FILE *Rast;

	 if((Rast=fopen(FileName, "r"))==NULL)
	 	return false;

	 if(RasterOverlay)
	 	free(RasterOverlay);
	 RasterOverlay=0;

	rMaxVal=1e-300;
	 rMinVal=1e300;
	 fscanf(Rast, "%s", TestRead);
	 if(!strcmp(TestRead, "north:")) // GRASS File
	{    fscanf(Rast, "%lf", &rN);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%lf", &rS);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%lf", &rE);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%lf", &rW);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%ld", &rRows);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%ld", &rCols);
		rCellSizeY=(rN-rS)/(double) rRows;
		rCellSizeX=(rE-rW)/(double) rCols;
	}
	else if(!strcmp(strupr(TestRead), "NCOLS")) 	 // ARC grid file
	{    fscanf(Rast, "%ld", &rCols);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%ld", &rRows);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%lf", &rW);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%lf", &rS);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%lf", &rCellSizeX);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%s", TestRead);
		rN=rS+rCellSizeX*(double) rRows;
		rE=rW+rCellSizeX*(double) rCols;
		rCellSizeY=rCellSizeX;
	}

	 if((RasterOverlay=(double *) calloc(rRows*rCols, sizeof(double)))==NULL)
	 	return false;

	for(i=0; i<rRows; i++)
	 {	for(j=0; j<rCols; j++)
	 	{	fscanf(Rast, "%lf", &value);
			   RasterOverlay[i*rCols+j]=value;
			   if(value>=0.0)
			   {	if(value<rMinVal)
			   		rMinVal=value;
			   	if(value>rMaxVal)
			   		rMaxVal=value;
			   }
		  }
	 }
	 fclose(Rast);

	 *max=rMaxVal;
	 *min=rMinVal;

	 rW=ConvertUtmToEastingOffset(rW);
	 rE=ConvertUtmToEastingOffset(rE);
	 rN=ConvertUtmToNorthingOffset(rN);
	 rS=ConvertUtmToNorthingOffset(rS);

	 return true;
}


void DeleteRasterOverlay()
{
	 if(RasterOverlay)
	 	free(RasterOverlay);
	 RasterOverlay=0;
}

double QueryRasterOverlay(double xcoord, double ycoord)
{
	 if(RasterOverlay==0)
	 	return -2;
	 if(xcoord<rW || xcoord>rE)
	 	return -3;
	 if(ycoord<rS || ycoord>rN)
	 	return -4;

	 double xpt, ypt;
	 long easti, northi, posit;

	xpt=(xcoord-rW)/rCellSizeX;
	ypt=(ycoord-rS)/rCellSizeY;
	easti=((long) xpt);
	northi=((long) ypt);
	northi=rRows-northi-1;
	if(northi<0)
		northi=0;
	posit=(northi*rCols+easti);

	return RasterOverlay[posit];
}


bool	CheckRasterOverlay()
{
 	if(RasterOverlay)
	 	return true;

	 return false;
}


*/
//------------------------------------------------------------------------------
//  Vis Perim stuff
//------------------------------------------------------------------------------

static long const MaxCount = 500;
static long i, MemCount = 0;
static long MemPerim[MaxCount*2];
//static HANDLE hVisperimSemaphore = 0;
//static long prevct;


void CopyOutVisPerimFile(char* NewVisFile)
{
	//CopyFile(VisPerim, NewVisFile, false);
	fcopy(VisPerim, NewVisFile);
}


void CopyInVisPerimFile(char* NewVisFile)
{
	//WaitForSingleObject(hVisperimSemaphore, INFINITE);
	if (strlen(VisPerim) > 0)
	{
		if ((access(VisPerim, F_OK)) != -1)
		{
			//SetFileAttributes(VisPerim, FILE_ATTRIBUTE_NORMAL);
			chmod(VisPerim,S_IRWXO);
			remove(VisPerim);
			memset(VisPerim, 0x0, sizeof(VisPerim));
		}
	}
	getcwd(VisPerim, 255);
	//	strcat(VisPerim, "\\visperim.vsp");
	char RNum[16] = "";
	sprintf(RNum, "%d", rand() % 1000);
	strcat(VisPerim, "\\visperim");
	strcat(VisPerim, RNum);
	strcat(VisPerim, ".vsp");
	if ((access(NewVisFile, F_OK)) != -1)
	{
		//SetFileAttributes(NewVisFile, FILE_ATTRIBUTE_NORMAL);
		chmod(NewVisFile,S_IRWXO);
		//CopyFile(NewVisFile, VisPerim, false);
		fcopy(NewVisFile, VisPerim );
	}
	MemCount = 0;
	//ReleaseSemaphore(hVisperimSemaphore, 1, &prevct);
}


void ResetVisPerimFile()
{
	//	system("del c:\visperim.vsp");
	//WaitForSingleObject(hVisperimSemaphore, INFINITE);
	if ((access(VisPerim, F_OK)) != -1)
	{
		//SetFileAttributes(VisPerim, FILE_ATTRIBUTE_NORMAL);
		chmod(VisPerim,S_IRWXO);
		remove(VisPerim);
	}
	memset(VisPerim, 0x0, sizeof VisPerim);
	getcwd(VisPerim, 255);
	//	strcat(VisPerim, "\\visperim.vsp");
	char RNum[16] = "";
	sprintf(RNum, "%d", rand() % 1000);
	strcat(VisPerim, "\\visperim");
	strcat(VisPerim, RNum);
	strcat(VisPerim, ".vsp");
	if ((access(VisPerim, F_OK)) != -1)
	{
		//SetFileAttributes(VisPerim, FILE_ATTRIBUTE_NORMAL);
		chmod(VisPerim,S_IRWXO);
		remove(VisPerim);
	}
	MemCount = 0;
	//ReleaseSemaphore(hVisperimSemaphore, 1, &prevct);
	//	VP=fopen(VisPerim, "w");
	//	fclose(VP);
	//	DeleteFile(VisPerim);
}


long GetVisPerimFile(long* xpt, long* ypt)
{
	if (VP)
	{
		if (fscanf(VP, "%ld %ld", xpt, ypt) == EOF)    // end of file
			return -1;
		else
		{
			if (*xpt < 0)
				return 0;					   // end of fire polygon
			else
				return 1;   					 // successful read
		}
	}

	return -1;
}


void SetVisPerimFile(long xpt, long ypt)
{
	if (!VP)
		return;

	MemPerim[MemCount * 2] = xpt;
	MemPerim[MemCount * 2 + 1] = ypt;
	MemCount++;
	if (MemCount > MaxCount - 1)
	{
		for (i = 0; i < MemCount; i++)
			fprintf(VP, "%ld %ld\n", MemPerim[i * 2], MemPerim[i * 2 + 1]);
		MemCount = 0;
	}
	//if(VP)
	//	fprintf(VP, "%ld %ld\n", xpt, ypt);
}


void SetVisPerimPoint(long xpt, long ypt, long firenum)
{
	// Writes "point" to file nad ends fire
	if (!VP)
		return;

	if (MemCount < MaxCount - 2)
	{
		MemPerim[MemCount * 2] = xpt;
		MemPerim[MemCount * 2 + 1] = ypt;
		MemCount++;
		MemPerim[MemCount * 2] = xpt + 1;
		MemPerim[MemCount * 2 + 1] = ypt + 1;
		MemCount++;
		MemPerim[MemCount * 2] = -firenum;
		MemPerim[MemCount * 2 + 1] = -1;
		MemCount++;
	}
	else
	{
		for (i = 0; i < MemCount; i++)
			fprintf(VP, "%ld %ld\n", MemPerim[i * 2], MemPerim[i * 2 + 1]);
		MemCount = 0;
		fprintf(VP, "%ld %ld\n", xpt, ypt);
		fprintf(VP, "%ld %ld\n", xpt + 1, ypt + 1);
		fprintf(VP, "%ld %ld\n", -firenum, -1L);
	}
}


long OpenVisPerimFile(int readfromstart)
{
	long FilePos = 0;

	//WaitForSingleObject(hVisperimSemaphore, INFINITE);
	if (readfromstart)
	{
		if ((VP = fopen(VisPerim, "r")) != NULL)
			fseek(VP, 0, SEEK_SET);
	}
	else
	{
		if ((VP = fopen(VisPerim, "r")) != NULL)
		{
			fseek(VP, 0, SEEK_END);
			FilePos = ftell(VP);//fgetpos(VP, &FilePos);
			fclose(VP);
		}
		VP = fopen(VisPerim, "a");
		if (VP == NULL)
		{
			//SetFileAttributes(VisPerim, FILE_ATTRIBUTE_NORMAL);
			chmod(VisPerim,S_IRWXO);
			VP = fopen(VisPerim, "a");
		}
	}

	return FilePos;
}


void OpenVisPerimFileAtPosition(long FilePosition)
{
	//WaitForSingleObject(hVisperimSemaphore, INFINITE);
	if ((VP = fopen(VisPerim, "r")) != NULL)
		fseek(VP, FilePosition, SEEK_SET);
	else
		VP = 0;
}


void OpenCopyVisPerimFile(char* FileName)
{
	//WaitForSingleObject(hVisperimSemaphore, INFINITE);
	if ((VP = fopen(FileName, "r")) != NULL)
		rewind(VP);
	else
		VP = 0;
}


long CloseVisPerimFile(int writeend, long firenum)
{
	long FSize = 0;

	if (VP)
	{
		for (i = 0; i < MemCount; i++)
			fprintf(VP, "%ld %ld\n", MemPerim[i * 2], MemPerim[i * 2 + 1]);
		MemCount = 0;
		if (writeend)
		{
			fprintf(VP, "%ld %ld\n", -firenum, -1L);
			FSize = 1;
		}
		fclose(VP);
	}
	//ReleaseSemaphore(hVisperimSemaphore, 1, &prevct);

	return FSize;
}


/*bool CreateVisperimSemaphore()
{
	CloseVisperimSemaphore();
	char Name[128] = "";
	char TimeID[128] = "";
	SYSTEMTIME st;
	GetSystemTime(&st);
	sprintf(TimeID, "%ld%ld%ld%ld%ld%ld%ld", st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	sprintf(Name, "%s%s", "VPSEMAPHORE", TimeID);
	hVisperimSemaphore = CreateSemaphore(NULL, 1, 1, Name);
	if (hVisperimSemaphore == 0)
		return false;

	return true;
}

void CloseVisperimSemaphore()
{
	if (hVisperimSemaphore)
		CloseHandle(hVisperimSemaphore);
	hVisperimSemaphore = 0;
}
*/

//--------------------------------------------------------------------------------
//
//	view 3-D RASTER & VECTOR file information
//
//--------------------------------------------------------------------------------


void SetViewRasterThemeInfo(char* filename, long hi, long lo)
{
	strcpy(ThreeDRasterFile, filename);
	ThreeDRasterHi = hi;
	ThreeDRasterLo = lo;
}

char* GetViewRasterThemeInfo(long* hi, long* lo)
{
	*hi = ThreeDRasterHi;
	*lo = ThreeDRasterLo;

	return ThreeDRasterFile;
}

bool Draw3DVectorFile(long YesNo)
{
	if (YesNo >= 0)
		View3DVectorFile = YesNo;

	return View3DVectorFile;
}


void SetViewVectorThemeInfo(char* filename, COLORREF colr, long FileType)
{
	strcpy(ThreeDVectorFile, filename);
	VectorColor = colr;
	VectorFileType = FileType;
}


char* GetViewVectorThemeInfo(COLORREF* colr)
{
	*colr = VectorColor;

	return ThreeDVectorFile;
}


long GetViewVectorFileType()
{
	return VectorFileType;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  Attached Vectors
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

VectorStorage* CurVect = 0;
VectorStorage* FirstVect = 0;
VectorStorage* NextVect = 0;
VectorStorage* LastVect = 0;

void SetNumVectorThemes(long number)
{
	NumVectorThemes = number;
}


long GetNumVectorThemes()
{
	return NumVectorThemes;
}


long AllocVectorTheme()
{
	if (FirstVect == 0)
	{
		if ((FirstVect = new VectorStorage[1]) == NULL)
			return -1;
//		ZeroMemory(FirstVect, sizeof(VectorStorage));
               	memset(FirstVect,0x0, sizeof(VectorStorage));
		CurVect = FirstVect;
		CurVect->last = 0;
		CurVect->next = 0;
	}
	else
	{
		long i;

		CurVect = FirstVect;
		for (i = 0; i < NumVectorThemes; i++)
		{
			LastVect = CurVect;
			NextVect = (VectorStorage *) CurVect->next;
			CurVect = NextVect;
		}
		if ((CurVect = new VectorStorage[1]) == NULL)
			return -1;
//		ZeroMemory(CurVect, sizeof(VectorStorage));
		memset(CurVect,0x0, sizeof(VectorStorage));
		CurVect->last = (VectorStorage *) LastVect;
		if (LastVect)
			LastVect->next = (VectorStorage *) CurVect;
		CurVect->next = 0;
	}
	NumVectorThemes++;

	return NumVectorThemes - 1;
}


bool SetVectorOnOff(long FileNum, long OnOff)
{
	long i;

	CurVect = FirstVect;
	for (i = 0; i < NumVectorThemes; i++)
	{
		NextVect = (VectorStorage *) CurVect->next;
		if (i == FileNum)
			break;
		CurVect = NextVect;
	}
	if (OnOff >= 0)
		CurVect->theme.OnOff = (bool) OnOff;

	return CurVect->theme.OnOff;
}


VectorTheme* GetVectorTheme(long FileNum)
{
	long i;

	CurVect = FirstVect;
	for (i = 0; i < NumVectorThemes; i++)
	{
		NextVect = (VectorStorage *) CurVect->next;
		if (i == FileNum)
			break;
		CurVect = NextVect;
	}

	return &CurVect->theme;
}


void SetVectorTheme(long Num, VectorTheme* Theme)
{
	long i;

	CurVect = FirstVect;
	for (i = 0; i < NumVectorThemes; i++)
	{
		NextVect = (VectorStorage *) CurVect->next;
		if (i == Num)
			break;
		CurVect = NextVect;
	}
	//	memcpy(&CurVect->theme, Theme, sizeof(VectorTheme));
	memcpy(&CurVect->theme, Theme, sizeof(VectorTheme));
}


VectorTheme* SetNewVectorTheme(VectorTheme* Theme)
{
	long i;

	LastVect = 0;
	CurVect = FirstVect;
	for (i = 0; i < NumVectorThemes; i++)
	{
		LastVect = CurVect;
		NextVect = (VectorStorage *) CurVect->next;
		CurVect = NextVect;
	}
	if ((CurVect = new VectorStorage[1]) == NULL)
		return NULL;
//	ZeroMemory(CurVect, sizeof(VectorStorage));
	memset(CurVect,0x0, sizeof(VectorStorage));
	CurVect->last = (VectorStorage *) LastVect;
	if (LastVect)
		LastVect->next = (VectorStorage *) CurVect;
	CurVect->next = 0;
	memcpy(&CurVect->theme, Theme, sizeof(VectorTheme));
	if (FirstVect == 0)
		FirstVect = CurVect;
	NumVectorThemes++;

	return &CurVect->theme;
}


void FreeVectorTheme(long FileNum)
{
	long i;
	VectorStorage* TempVect;

	CurVect = FirstVect;
	for (i = 0; i < NumVectorThemes; i++)
	{
		NextVect = (VectorStorage *) CurVect->next;
		if (i == FileNum)
			break;
		CurVect = NextVect;
	}
	TempVect = CurVect;
	LastVect = (VectorStorage *) TempVect->last;
	CurVect = (VectorStorage *) TempVect->next;
	if (CurVect)
		CurVect->last = (VectorStorage *) LastVect;
	delete[]TempVect;
	if (FileNum == 0)
		FirstVect = CurVect;
	else if (LastVect)
		LastVect->next = (VectorStorage *) CurVect;
	NumVectorThemes--;
}


void FreeAllVectorThemes()
{
	CurVect = FirstVect;
	for (i = 0; i < NumVectorThemes; i++)
	{
		NextVect = (VectorStorage *) CurVect->next;
		if (CurVect)
			delete[] CurVect;
		CurVect = NextVect;
	}
	if (CurVect)
		delete[] CurVect;
	FirstVect = 0;
	CurVect = 0;
	NextVect = 0;
	NumVectorThemes = 0;
}

//--------------------------------------------------------------------------------
//
//	manage weather/wind stream data
//
//--------------------------------------------------------------------------------

bool CalcFirstLastStreamData()
{
	bool BadRange = false;
	long sn;
	unsigned long  FirstWtr, FirstWnd, LastWtr, LastWnd;
	double ipart, fract;

	for (sn = 0; sn < GetNumStations(); sn++)
	{
		if (AtmosphereGridExists() < 2)
		{
			if (!wtrdt[sn] || (!wddt[sn] && AtmosphereGridExists() == 0))
			{
				continue;
			}
		}

		FirstWtr = (unsigned long)((GetJulianDays(FirstMonth[sn].wtr) + FirstDay[sn].wtr) * 1440.0);
		fract = modf((double) FirstHour[sn].wnd / 100.0, &ipart);
		FirstWnd = (unsigned long)((GetJulianDays(FirstMonth[sn].wnd) + FirstDay[sn].wnd) * 1440.0 +
			ipart * 60.0 +
			fract);
		LastWtr = (unsigned long)((GetJulianDays(LastMonth[sn].wtr) + LastDay[sn].wtr) * 1440.0);
		fract = modf((double) LastHour[sn].wnd / 100.0, &ipart);
		LastWnd = (unsigned long)((GetJulianDays(LastMonth[sn].wnd) + LastDay[sn].wnd) * 1440.0 +
			ipart * 60.0 +
			fract);

		if (FirstWtr > LastWtr)
			LastWtr += (unsigned long)(365.0 * 1440.0);
		if (FirstWnd > LastWnd)
			LastWnd += (unsigned long)(365.0 * 1440.0);

		if (FirstWnd <= FirstWtr)
		{
			if (LastWnd <= FirstWtr)
				BadRange = true;
		}
		else if (FirstWnd > FirstWtr)
		{
			if (FirstWnd > LastWtr)
				BadRange = true;
		}
		if (BadRange)
		{
			FirstMonth[sn].all = 0;
			FirstDay[sn].all = 0;
			FirstHour[sn].all = 0;

			return false;
		}

		if (FirstMonth[sn].wtr > FirstMonth[sn].wnd)
		{
			FirstMonth[sn].all = FirstMonth[sn].wtr;
			FirstDay[sn].all = FirstDay[sn].wtr;
			FirstHour[sn].all = FirstHour[sn].wtr;
		}
		else if (FirstMonth[sn].wtr < FirstMonth[sn].wnd)
		{
			FirstMonth[sn].all = FirstMonth[sn].wnd;
			FirstDay[sn].all = FirstDay[sn].wnd;
			FirstHour[sn].all = FirstHour[sn].wnd;
		}
		else
		{
			FirstMonth[sn].all = FirstMonth[sn].wtr;
			if (FirstDay[sn].wtr > FirstDay[sn].wnd)
			{
				FirstDay[sn].all = FirstDay[sn].wtr;
				FirstHour[sn].all = FirstHour[sn].wtr;
			}
			else if (FirstDay[sn].wtr < FirstDay[sn].wnd)
			{
				FirstDay[sn].all = FirstDay[sn].wnd;
				FirstHour[sn].all = FirstHour[sn].wnd;
			}
			else
			{
				FirstDay[sn].all = FirstDay[sn].wnd;
				if (FirstHour[sn].wtr > FirstHour[sn].wnd)
					FirstHour[sn].all = FirstHour[sn].wtr;
				else
					FirstHour[sn].all = FirstHour[sn].wnd;
			}
		}
		if (LastMonth[sn].wtr < LastMonth[sn].wnd)
		{
			LastMonth[sn].all = LastMonth[sn].wtr;
			LastDay[sn].all = LastDay[sn].wtr;
			LastHour[sn].all = LastHour[sn].wtr;
		}
		else if (LastMonth[sn].wtr > LastMonth[sn].wnd)
		{
			LastMonth[sn].all = LastMonth[sn].wnd;
			LastDay[sn].all = LastDay[sn].wnd;
			LastHour[sn].all = LastHour[sn].wnd;
		}
		else
		{
			LastMonth[sn].all = LastMonth[sn].wtr;
			if (LastDay[sn].wtr < LastDay[sn].wnd)
			{
				LastDay[sn].all = LastDay[sn].wtr;
				LastHour[sn].all = LastHour[sn].wtr;
			}
			else if (LastDay[sn].wtr > LastDay[sn].wnd)
			{
				LastDay[sn].all = LastDay[sn].wnd;
				LastHour[sn].all = LastHour[sn].wnd;
			}
			else
			{
				LastDay[sn].all = LastDay[sn].wnd;
				if (LastHour[sn].wtr < LastHour[sn].wnd)
					LastHour[sn].all = LastHour[sn].wtr;
				else
					LastHour[sn].all = LastHour[sn].wnd;
			}
		}
	}

	return true;
}



//--------------------------------------------------------------------------------
//
//	Manage Atmosphere Grid with regular Weather/Wind Stream Data
//
//--------------------------------------------------------------------------------


void SetCorrectStreamNumbers()
{
	long i;

	if (AtmosphereGridExists() == 1)		// if wind grid only
	{
		for (i = 0; i < 5; i++)
			FreeWindData(i);
		if (NumWeatherStations > 0)
			NumWindStations = NumWeatherStations;
		else
			NumWindStations = 1;
		for (i = 0; i < NumWindStations; i++)
		{
			FirstMonth[i].wnd = AtmGrid->GetAtmMonth(0);
			LastMonth[i].wnd = AtmGrid->GetAtmMonth(AtmGrid->GetTimeIntervals() -
											1);
			FirstDay[i].wnd = AtmGrid->GetAtmDay(0);
			LastDay[i].wnd = AtmGrid->GetAtmDay(AtmGrid->GetTimeIntervals() -
										1);
			FirstHour[i].wnd = AtmGrid->GetAtmHour(0);
			LastHour[i].wnd = AtmGrid->GetAtmHour(AtmGrid->GetTimeIntervals() -
										1);
		}
	}
	else if (AtmosphereGridExists() == 2)	// if weather and wind grids
	{
		for (i = 0; i < 5; i++)
		{
			FreeWindData(i);
			FreeWeatherData(i);
		}
		i = 0;
		NumWindStations = 1;
		NumWeatherStations = 1;
		FirstMonth[i].wnd = AtmGrid->GetAtmMonth(0);
		LastMonth[i].wnd = AtmGrid->GetAtmMonth(AtmGrid->GetTimeIntervals() -
										1);
		FirstDay[i].wnd = AtmGrid->GetAtmDay(0);
		LastDay[i].wnd = AtmGrid->GetAtmDay(AtmGrid->GetTimeIntervals() - 1);
		FirstHour[i].wnd = AtmGrid->GetAtmHour(0);
		LastHour[i].wnd = AtmGrid->GetAtmHour(AtmGrid->GetTimeIntervals() - 1);
		FirstMonth[i].wtr = AtmGrid->GetAtmMonth(0);
		LastMonth[i].wtr = AtmGrid->GetAtmMonth(AtmGrid->GetTimeIntervals() -
										1);
		FirstDay[i].wtr = AtmGrid->GetAtmDay(0);
		LastDay[i].wtr = AtmGrid->GetAtmDay(AtmGrid->GetTimeIntervals() - 1);
		FirstHour[i].wtr = AtmGrid->GetAtmHour(0);
		LastHour[i].wtr = AtmGrid->GetAtmHour(AtmGrid->GetTimeIntervals() - 1);
	}
}



//--------------------------------------------------------------------------------
//
//	wind data structures
//
//--------------------------------------------------------------------------------

long GetOpenWindStation()
{
	long i;

	for (i = 0; i < 5; i++)
	{
		if (!wddt[i])
			break;
	}

	return i;
}


long AllocWindData(long StatNum, long NumObs)
{
	long StationNumber = StatNum;

	if (wddt[StationNumber])
	{
		delete[] wddt[StationNumber];
		wddt[StationNumber] = 0;
		MaxWindObs[StatNum] = 0;
	}
	else
	{
		StationNumber = GetOpenWindStation();
		if (StationNumber < 5)
			NumWindStations = StationNumber + 1;
	}
	if (StationNumber<5 && NumObs>0)
	{
		size_t nmemb = MaxWindObs[StationNumber] = NumObs*2;			// alloc 2* number needed
		if ((wddt[StationNumber] = new WindData[nmemb]) == NULL)
			StationNumber = -1;
	}
	else
		StationNumber = -1;

	return StationNumber;
}


void FreeWindData(long StationNumber)
{
	if (wddt[StationNumber])
	{
		delete[] wddt[StationNumber];//free(wddt[StationNumber]);
		NumWindStations--;
		MaxWindObs[StationNumber] = 0;
	}
	wddt[StationNumber] = 0;
	FirstMonth[StationNumber].wnd = 0;
	LastMonth[StationNumber].wnd = 0;
	FirstDay[StationNumber].wnd = 0;
	LastDay[StationNumber].wnd = 0;
	FirstHour[StationNumber].wnd = 0;
	LastHour[StationNumber].wnd = 0;
}


long SetWindData(long StationNumber, long NumObs, long month, long day,
	long hour, double windspd, long winddir, long cloudy)
{
	if (NumObs < MaxWindObs[StationNumber])
	{
		wddt[StationNumber][NumObs].mo = month;
		wddt[StationNumber][NumObs].dy = day;
		wddt[StationNumber][NumObs].hr = hour;
		wddt[StationNumber][NumObs].ws = windspd;
		wddt[StationNumber][NumObs].wd = winddir;
		wddt[StationNumber][NumObs].cl = cloudy;

		if (month == 13)
		{
			FirstMonth[StationNumber].wnd = wddt[StationNumber][0].mo;
			LastMonth[StationNumber].wnd = wddt[StationNumber][NumObs - 1].mo;
			FirstDay[StationNumber].wnd = wddt[StationNumber][0].dy;
			LastDay[StationNumber].wnd = wddt[StationNumber][NumObs - 1].dy;
			FirstHour[StationNumber].wnd = wddt[StationNumber][0].hr;
			LastHour[StationNumber].wnd = wddt[StationNumber][NumObs - 1].hr;
		}

		return 1;
	}

	return 0;
}

long GetWindMonth(long StationNumber, long NumObs)
{
	if (NumObs > MaxWindObs[StationNumber] - 1)
		NumObs = MaxWindObs[StationNumber] - 1;

	return wddt[StationNumber][NumObs].mo;
}

long GetWindDay(long StationNumber, long NumObs)
{
	if (NumObs > MaxWindObs[StationNumber] - 1)
		NumObs = MaxWindObs[StationNumber] - 1;

	return wddt[StationNumber][NumObs].dy;
}

long GetWindHour(long StationNumber, long NumObs)
{
	if (NumObs > MaxWindObs[StationNumber] - 1)
		NumObs = MaxWindObs[StationNumber] - 1;

	return wddt[StationNumber][NumObs].hr;
}

double GetWindSpeed(long StationNumber, long NumObs)
{
	if (NumObs > MaxWindObs[StationNumber] - 1)
		NumObs = MaxWindObs[StationNumber] - 1;

	return wddt[StationNumber][NumObs].ws;
}

long GetWindDir(long StationNumber, long NumObs)
{
	if (NumObs > MaxWindObs[StationNumber] - 1)
		NumObs = MaxWindObs[StationNumber] - 1;

	return wddt[StationNumber][NumObs].wd;
}

long GetWindCloud(long StationNumber, long NumObs)
{
	if (NumObs > MaxWindObs[StationNumber] - 1)
		NumObs = MaxWindObs[StationNumber] - 1;

	return wddt[StationNumber][NumObs].cl;
}

long GetMaxWindObs(long StationNumber)
{
	if (StationNumber > 4)
		return -1;

	return MaxWindObs[StationNumber] - 1;
}

//--------------------------------------------------------------------------------
//
//	weather data structures
//
//--------------------------------------------------------------------------------

long GetOpenWeatherStation()
{
	long i;

	for (i = 0; i < 5; i++)
	{
		if (!wtrdt[i])
			break;
	}

	return i;
}


long GetNumStations()
{
	if (NumWeatherStations > NumWindStations)
		return NumWindStations;

	return NumWeatherStations;
}


long AllocWeatherData(long StatNum, long NumObs)
{
	long StationNumber = StatNum;

	if (wtrdt[StationNumber])
	{
		delete[] wtrdt[StationNumber];//free(wtrdt[StationNumber]);
		wtrdt[StationNumber] = 0;
		MaxWeatherObs[StatNum] = 0;
	}
	else
	{
		StationNumber = GetOpenWeatherStation();
		if (StationNumber < 5)
			NumWeatherStations = StationNumber + 1;
	}
	if (StationNumber<5 && NumObs>0)
	{
		size_t nmemb = MaxWeatherObs[StationNumber] = NumObs + 20;		// alloc 20 more than needed
		if ((wtrdt[StationNumber] = new WeatherData[nmemb]) == NULL)
			StationNumber = -1;
	}
	else
		StationNumber = -1;

	return StationNumber;
}


void FreeWeatherData(long StationNumber)
{
	if (wtrdt[StationNumber])
	{
		delete[] wtrdt[StationNumber];
		MaxWeatherObs[StationNumber] = 0;
		NumWeatherStations--;
	}
	wtrdt[StationNumber] = 0;
	FirstMonth[StationNumber].wtr = 0;
	LastMonth[StationNumber].wtr = 0;
	FirstDay[StationNumber].wtr = 0;
	LastDay[StationNumber].wtr = 0;
	FirstHour[StationNumber].wtr = 0;
	LastHour[StationNumber].wtr = 0;
}


long SetWeatherData(long StationNumber, long NumObs, long month, long day,
	double rain, long time1, long time2, double temp1, double temp2,
	long humid1, long humid2, double elevation, long tr1, long tr2)
{
	if (NumObs < MaxWeatherObs[StationNumber])
	{
		wtrdt[StationNumber][NumObs].mo = month;
		wtrdt[StationNumber][NumObs].dy = day;
		wtrdt[StationNumber][NumObs].rn = rain;
		wtrdt[StationNumber][NumObs].t1 = time1;
		wtrdt[StationNumber][NumObs].t2 = time2;
		wtrdt[StationNumber][NumObs].T1 = temp1;
		wtrdt[StationNumber][NumObs].T2 = temp2;
		wtrdt[StationNumber][NumObs].H1 = humid1;
		wtrdt[StationNumber][NumObs].H2 = humid2;
		wtrdt[StationNumber][NumObs].el = elevation;
		wtrdt[StationNumber][NumObs].tr1 = tr1;
		wtrdt[StationNumber][NumObs].tr2 = tr2;

		if (month == 13)
		{
			FirstMonth[StationNumber].wtr = wtrdt[StationNumber][0].mo;
			LastMonth[StationNumber].wtr = wtrdt[StationNumber][NumObs - 1].mo;
			FirstDay[StationNumber].wtr = wtrdt[StationNumber][0].dy;
			LastDay[StationNumber].wtr = wtrdt[StationNumber][NumObs - 1].dy;
			FirstHour[StationNumber].wtr = wtrdt[StationNumber][0].t1;
			LastHour[StationNumber].wtr = wtrdt[StationNumber][NumObs - 1].t2;
		}

		EnvtChanged[0][StationNumber] = true;   // 1hr fuels
		EnvtChanged[1][StationNumber] = true;   // 10hr fuels
		EnvtChanged[2][StationNumber] = true;   // 100hr fuels
		EnvtChanged[3][StationNumber] = true;   // 1000hr fuels
		return 1;
	}

	return 0;
}


long GetWeatherMonth(long StationNumber, long NumObs)
{
	if (NumObs > MaxWeatherObs[StationNumber] - 1)
		NumObs = MaxWeatherObs[StationNumber] - 1;
	return wtrdt[StationNumber][NumObs].mo;
}

long GetWeatherDay(long StationNumber, long NumObs)
{
	if (NumObs > MaxWeatherObs[StationNumber] - 1)
		NumObs = MaxWeatherObs[StationNumber] - 1;

	return wtrdt[StationNumber][NumObs].dy;
}

double GetWeatherRain(long StationNumber, long NumObs)
{
	if (NumObs > MaxWeatherObs[StationNumber] - 1)
		NumObs = MaxWeatherObs[StationNumber] - 1;

	return wtrdt[StationNumber][NumObs].rn;
}

long GetWeatherTime1(long StationNumber, long NumObs)
{
	if (NumObs > MaxWeatherObs[StationNumber] - 1)
		NumObs = MaxWeatherObs[StationNumber] - 1;

	return wtrdt[StationNumber][NumObs].t1;
}

long GetWeatherTime2(long StationNumber, long NumObs)
{
	if (NumObs > MaxWeatherObs[StationNumber] - 1)
		NumObs = MaxWeatherObs[StationNumber] - 1;

	return wtrdt[StationNumber][NumObs].t2;
}

double GetWeatherTemp1(long StationNumber, long NumObs)
{
	if (NumObs > MaxWeatherObs[StationNumber] - 1)
		NumObs = MaxWeatherObs[StationNumber] - 1;

	return wtrdt[StationNumber][NumObs].T1;
}

double GetWeatherTemp2(long StationNumber, long NumObs)
{
	if (NumObs > MaxWeatherObs[StationNumber] - 1)
		NumObs = MaxWeatherObs[StationNumber] - 1;

	return wtrdt[StationNumber][NumObs].T2;
}

long GetWeatherHumid1(long StationNumber, long NumObs)
{
	if (NumObs > MaxWeatherObs[StationNumber] - 1)
		NumObs = MaxWeatherObs[StationNumber] - 1;

	return wtrdt[StationNumber][NumObs].H1;
}

long GetWeatherHumid2(long StationNumber, long NumObs)
{
	if (NumObs > MaxWeatherObs[StationNumber] - 1)
		NumObs = MaxWeatherObs[StationNumber] - 1;

	return wtrdt[StationNumber][NumObs].H2;
}

double GetWeatherElev(long StationNumber, long NumObs)
{
	if (NumObs > MaxWeatherObs[StationNumber] - 1)
		NumObs = MaxWeatherObs[StationNumber] - 1;

	return wtrdt[StationNumber][NumObs].el;
}


void GetWeatherRainTimes(long StationNumber, long NumObs, long* tr1, long* tr2)
{
	if (NumObs > MaxWeatherObs[StationNumber] - 1)
		NumObs = MaxWeatherObs[StationNumber] - 1;

	*tr1 = wtrdt[StationNumber][NumObs].tr1;
	*tr2 = wtrdt[StationNumber][NumObs].tr2;
}


long GetMaxWeatherObs(long StationNumber)
{
	if (StationNumber > 4)
		return -1;

	return MaxWeatherObs[StationNumber] - 1;
}


bool PreCalcMoistures(long YesNo)
{
	if (YesNo > -1)
		PreCalcFuelMoistures = (bool) YesNo;

	return PreCalcFuelMoistures;
}


long GetMoistCalcInterval(long FM_SIZECLASS, long CATEGORY)
{
	return MoistCalcInterval[FM_SIZECLASS][CATEGORY];
}


void SetMoistCalcInterval(long FM_SIZECLASS, long CATEGORY, long Val)
{
	switch (CATEGORY)
	{
	case FM_INTERVAL_TIME:
		switch (FM_SIZECLASS)
		{
		case 1:
			if (Val < 60)
				Val = 60;
			if (Val > 240)
				Val = 240;
			break;
		case 2:
			if (Val < 60)
				Val = 600;
			if (Val > 240)
				Val = 240;
			break;
		case 3:
			if (Val < 60)
				Val = 6000;
			if (Val > 240)
				Val = 240;
			break;
		}
		break;
	case FM_INTERVAL_ELEV:
		if (Val < 10)
			Val = 10;
		break;
	case FM_INTERVAL_SLOPE:
		if (Val < 5)
			Val = 5;
		break;
	case FM_INTERVAL_ASP:
		if (Val < 5)
			Val = 5;
		break;
	case FM_INTERVAL_COV:
		if (Val < 5)
			Val = 5;
		break;
	}
	MoistCalcInterval[FM_SIZECLASS][CATEGORY] = Val;
}


//------------------------------------------------------------------------------
//
//  Weather/Wind units
//
//------------------------------------------------------------------------------

long WeatherUnits[5] =
{
	-1, -1, -1, -1, -1
};
long WindUnits[5] =
{
	-1, -1, -1, -1, -1
};

void SetWindUnits(long StationNumber, long Units)
{
	WindUnits[StationNumber] = Units;
}

void SetWeatherUnits(long StationNumber, long Units)
{
	WeatherUnits[StationNumber] = Units;
}

long GetWindUnits(long StationNumber)
{
	return WindUnits[StationNumber];
}

long GetWeatherUnits(long StationNumber)
{
	return WeatherUnits[StationNumber];
}




//------------------------------------------------------------------------------
//
//  Weather/Wind station grid functions 						   s
//
//------------------------------------------------------------------------------

StationGrid::StationGrid()
{
	Grid = 0;
}


StationGrid::~StationGrid()
{
	if (Grid)
		delete[] Grid;
}


void AllocStationGrid(long XDim, long YDim)
{
	long x, y, Num;

	FreeStationGrid();
	size_t nmemb = 2 * XDim* YDim;
	grid.XDim = XDim;
	grid.YDim = YDim;
	grid.Width = ((double) (GetHiEast() - GetLoEast())) / ((double) XDim);
	grid.Height = ((double) (GetHiNorth() - GetLoNorth())) / ((double) YDim);
	grid.Grid = new long[nmemb];
	for (y = 0; y < YDim; y++)
	{
		for (x = 0; x < XDim; x++)  // initialize grid to station 1
		{
			Num = x + y * grid.XDim;
			grid.Grid[Num] = 1;
		}
	}
}


long FreeStationGrid()
{
	if (grid.Grid)
	{
		delete[] grid.Grid;
		grid.Grid = 0;

		return 1;
	}
	grid.Grid = 0;

	return 0;
}


long GetStationNumber(double xpt, double ypt)
{
	long XCell = (long)((xpt - GetLoEast()) / grid.Width);
	long YCell = (long)((ypt - GetLoNorth()) / grid.Height);
	long CellNum = XCell + YCell* grid.XDim;

	if (grid.Grid[CellNum] > NumWeatherStations)   // check to see if data exist
		return 0;   		  // for a given weather station
	if (grid.Grid[CellNum] > NumWindStations)
		return 0;

	return grid.Grid[CellNum];
}


long SetStationNumber(long XPos, long YPos, long StationNumber)
{
	long CellNumber = XPos + YPos* grid.XDim;
	if (StationNumber <= NumWeatherStations &&
		StationNumber <= NumWindStations &&
		StationNumber > 0)
		grid.Grid[CellNumber] = StationNumber;
	else
		return 0;

	return 1;
}


long GridInitialized()
{
	if (grid.Grid)
		return 1;

	return 0;
}


void SetGridEastDimension(long XDim)
{
	grid.XDim = XDim;
}


void SetGridNorthDimension(long YDim)
{
	grid.YDim = YDim;
}

long GetGridEastDimension()
{
	return grid.XDim;
}


long GetGridNorthDimension()
{
	return grid.YDim;
}


void SetGridNorthOffset(double offset)
{
	NorthGridOffset = offset;
}

void SetGridEastOffset(double offset)
{
	EastGridOffset = offset;
}

double GetGridNorthOffset()
{
	return NorthGridOffset;
}

double GetGridEastOffset()
{
	return EastGridOffset;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

LandscapeTheme* lcptheme = 0;

LandscapeTheme* GetLandscapeTheme()
{
	return lcptheme;
}

//------------------------------------------------------------------------------
//
//	Landscape Theme functions
//
//------------------------------------------------------------------------------


LandscapeTheme::LandscapeTheme(bool Analyze) : GridTheme()
{
	long i;
	//	memcpy(Name, GetLandFileName(), sizeof(Name));
	memcpy(Name, GetLandFileName(), sizeof(Name));

	for (i = 0; i < 10; i++)
//		ZeroMemory(&AllCats[i], 100 * sizeof(long));
		memset(&AllCats[i],0x0, 100 * sizeof(long));

	if (Analyze)
		AnalyzeStats();
	else
		ReadStats();
	CopyStats(F_DATA);
	CreateRamp();
	WantNewRamp = false;
}


void LandscapeTheme::ReadStats()
{
	NumAllCats[0] = Header.numelev;
	NumAllCats[1] = Header.numslope;
	NumAllCats[2] = Header.numaspect;
	NumAllCats[3] = Header.numfuel;
	NumAllCats[4] = Header.numcover;
	NumAllCats[5] = Header.numheight;
	NumAllCats[6] = Header.numbase;
	NumAllCats[7] = Header.numdensity;
	NumAllCats[8] = Header.numduff;
	NumAllCats[9] = Header.numwoody;
	memcpy(&AllCats[0], Header.elevs, 100 * sizeof(long));
	memcpy(&AllCats[1], Header.slopes, 100 * sizeof(long));
	memcpy(&AllCats[2], Header.aspects, 100 * sizeof(long));
	memcpy(&AllCats[3], Header.fuels, 100 * sizeof(long));
	memcpy(&AllCats[4], Header.covers, 100 * sizeof(long));
	memcpy(&AllCats[5], Header.heights, 100 * sizeof(long));
	memcpy(&AllCats[6], Header.bases, 100 * sizeof(long));
	memcpy(&AllCats[7], Header.densities, 100 * sizeof(long));
	memcpy(&AllCats[8], Header.duffs, 100 * sizeof(long));
	memcpy(&AllCats[9], Header.woodies, 100 * sizeof(long));
	maxval[0] = Header.hielev;
	minval[0] = Header.loelev;
	maxval[1] = Header.hislope;
	minval[1] = Header.loslope;
	maxval[2] = Header.hiaspect;
	minval[2] = Header.loaspect;
	maxval[3] = Header.hifuel;
	minval[3] = Header.lofuel;
	maxval[4] = Header.hicover;
	minval[4] = Header.locover;
	maxval[5] = Header.hiheight;
	minval[5] = Header.loheight;
	maxval[6] = Header.hibase;
	minval[6] = Header.lobase;
	maxval[7] = Header.hidensity;
	minval[7] = Header.lodensity;
	maxval[8] = Header.hiduff;
	minval[8] = Header.loduff;
	maxval[9] = Header.hiwoody;
	minval[9] = Header.lowoody;
	Continuous = 0;
}


void LandscapeTheme::AnalyzeStats()
{
	long i, j;

	for (i = 0; i < 10; i++)
	{
		for (j = 0; j < 100; j++)
			AllCats[i][j] = -2;
	}

	FillCats();
	SortCats();
	Header.numelev = NumAllCats[0];
	Header.numslope = NumAllCats[1];
	Header.numaspect = NumAllCats[2];
	Header.numfuel = NumAllCats[3];
	Header.numcover = NumAllCats[4];
	Header.numheight = NumAllCats[5];
	Header.numbase = NumAllCats[6];
	Header.numdensity = NumAllCats[7];
	Header.numduff = NumAllCats[8];
	Header.numwoody = NumAllCats[9];
	memcpy(Header.elevs, &AllCats[0], 100 * sizeof(long));
	memcpy(Header.slopes, &AllCats[1], 100 * sizeof(long));
	memcpy(Header.aspects, &AllCats[2], 100 * sizeof(long));
	memcpy(Header.fuels, &AllCats[3], 100 * sizeof(long));
	memcpy(Header.covers, &AllCats[4], 100 * sizeof(long));
	memcpy(Header.heights, &AllCats[5], 100 * sizeof(long));
	memcpy(Header.bases, &AllCats[6], 100 * sizeof(long));
	memcpy(Header.densities, &AllCats[7], 100 * sizeof(long));
	memcpy(Header.duffs, &AllCats[8], 100 * sizeof(long));
	memcpy(Header.woodies, &AllCats[9], 100 * sizeof(long));
	Header.hielev = (long)maxval[0];
	Header.loelev = (long)minval[0];
	Header.hislope = (long)maxval[1];
	Header.loslope = (long)minval[1];
	Header.hiaspect = (long)maxval[2];
	Header.loaspect = (long)minval[2];
	Header.hifuel = (long)maxval[3];
	Header.lofuel = (long)minval[3];
	Header.hicover = (long)maxval[4];
	Header.locover = (long)minval[4];
	Header.hiheight = (long)maxval[5];
	Header.loheight = (long)minval[5];
	Header.hibase = (long)maxval[6];
	Header.lobase = (long)minval[6];
	Header.hidensity = (long)maxval[7];
	Header.lodensity = (long)minval[7];
	Header.hiduff = (long)maxval[8];
	Header.loduff = (long)minval[8];
	Header.hiwoody = (long)maxval[9];
	Header.lowoody = (long)minval[9];
	Continuous = 0;
}

void LandscapeTheme::CopyStats(long layer)
{
	memcpy(Cats, AllCats[layer], 99 * sizeof(long));
	NumCats = NumAllCats[layer];
	MaxVal = maxval[layer];
	MinVal = minval[layer];
	if (NumCats > 0)
		CatsOK = true;
	else
		CatsOK = false;
}


void LandscapeTheme::FillCats()
{
	long i, j, k, m, pos;
	double x, y, resx, resy;
	celldata cell;
	crowndata cfuel;
	grounddata gfuel;

	resx = GetCellResolutionX();
	resy = GetCellResolutionY();
	for (m = 0; m < 10; m++)
	{
		maxval[m] = -1e100;
		minval[m] = 1e100;
		NumAllCats[m] = 0;
		for (k = 0; k < 100; k++)
			AllCats[m][k] = -1;
	}
	for (i = 0; i < GetNumNorth(); i++)
	{
		y = GetHiNorth() - i * resy - resy / 2.0;
		for (j = 0; j < GetNumEast(); j++)
		{
			x = GetLoEast() + j * resx + resx / 2.0;
			CellData(x, y, cell, cfuel, gfuel, &pos);
			AllCats[0][NumAllCats[0]] = cell.e;
			AllCats[1][NumAllCats[1]] = cell.s;
			AllCats[2][NumAllCats[2]] = cell.a;
			AllCats[3][NumAllCats[3]] = cell.f;
			AllCats[4][NumAllCats[4]] = cell.c;
			if (HaveCrownFuels())
			{
				if (cfuel.h >= 0)
					AllCats[5][NumAllCats[5]] = cfuel.h;///10.0;
				if (cfuel.b >= 0)
					AllCats[6][NumAllCats[6]] = cfuel.b;///10;
				if (cfuel.p >= 0)
					AllCats[7][NumAllCats[7]] = cfuel.p;
			}
			if (HaveGroundFuels())
			{
				if (gfuel.d >= 0)
					AllCats[8][NumAllCats[8]] = gfuel.d;
				if (gfuel.w >= 0)
					AllCats[9][NumAllCats[9]] = gfuel.w;
			}
			for (m = 0; m < 10; m++)
			{
				if (maxval[m] < AllCats[m][NumAllCats[m]])
					maxval[m] = AllCats[m][NumAllCats[m]];
				if (AllCats[m][NumAllCats[m]] >= 0)
				{
					if (minval[m] > AllCats[m][NumAllCats[m]])
						minval[m] = AllCats[m][NumAllCats[m]];
				}
			}
			for (m = 0; m < 10; m++)
			{
				if (NumAllCats[m] > 98)
					continue;
				for (k = 0; k < NumAllCats[m]; k++)
				{
					if (AllCats[m][NumAllCats[m]] == AllCats[m][k])
						break;
				}
				if (k == NumAllCats[m])
					NumAllCats[m]++;
			}
		}
	}
	for (m = 0; m < 10; m++)
	{
		if (NumAllCats[m] > 98)
			NumAllCats[m] = -1;
	}
}


void LandscapeTheme::SortCats()
{
	long i, j, m;
	long SwapCats[101];

	for (m = 0; m < 10; m++)
	{
		if (NumAllCats[m] < 0)
			continue;
		memcpy(SwapCats, AllCats[m], 100 * sizeof(long));
		for (i = 0; i < NumAllCats[m] - 1; i++)
		{
			for (j = i + 1; j < NumAllCats[m]; j++)
			{
				if (SwapCats[j] < SwapCats[i])
				{
					SwapCats[100] = SwapCats[i];
					SwapCats[i] = SwapCats[j];
					SwapCats[j] = SwapCats[100];
				}
			}
		}
		AllCats[m][0] = 0;
		for (i = 0; i < NumAllCats[m]; i++)
			AllCats[m][i + 1] = SwapCats[i];
		minval[m] = AllCats[m][1];
		if (minval[m] < 0)
			minval[m] = 0;
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
RasterTheme::RasterTheme() : GridTheme()
{
	map = 0;
	Priority = 0;
	CatsOK = false;
}

RasterTheme::~RasterTheme()
{
	if (map)
		delete[] map;
	map = 0;
}

bool RasterTheme::SetTheme(char* FileName)
{
	long i, j;
	char TestRead[30];
	double value;
	FILE* Rast;

	if ((Rast = fopen(FileName, "r")) == NULL)
		return false;

	if (map)
		delete[] map;
	map = 0;
	strcpy(Name, FileName);

	rMaxVal = 1e-300;
	rMinVal = 1e300;
	fscanf(Rast, "%s", TestRead);
	if (!strcmp(TestRead, "north:")) // GRASS File
	{
		fscanf(Rast, "%lf", &rN);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%lf", &rS);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%lf", &rE);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%lf", &rW);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%ld", &rRows);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%ld", &rCols);
		rCellSizeY = (rN - rS) / (double) rRows;
		rCellSizeX = (rE - rW) / (double) rCols;
	}
	else if (!strcmp(strupr(TestRead), "NCOLS"))	  // ARC grid file
	{
		fscanf(Rast, "%ld", &rCols);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%ld", &rRows);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%lf", &rW);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%lf", &rS);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%lf", &rCellSizeX);
		fscanf(Rast, "%s", TestRead);
		fscanf(Rast, "%s", TestRead);
		rN = rS + rCellSizeX * (double) rRows;
		rE = rW + rCellSizeX * (double) rCols;
		rCellSizeY = rCellSizeX;
	}

	if ((map = new double[rRows * rCols]) == NULL)
		return false;
//	ZeroMemory(map, rRows * rCols * sizeof(double));
	memset(map,0x0, rRows * rCols * sizeof(double));

	for (i = 0; i < rRows; i++)
	{
		for (j = 0; j < rCols; j++)
		{
			fscanf(Rast, "%lf", &value);
			map[i * rCols + j] = value;
			if (value >= 0.0)
			{
				if (value < rMinVal)
					rMinVal = value;
				if (value > rMaxVal)
					rMaxVal = value;
			}
		}
	}
	fclose(Rast);

	if (CheckCellResUnits() == 2)
	{
		//rE*=1000.0;
		//rW*=1000.0;
		//rN*=1000.0;
		//rS*=1000.0;
		rCellSizeX *= 1000.0;
		rCellSizeY *= 1000.0;
	}

	rW = ConvertUtmToEastingOffset(rW);
	rE = ConvertUtmToEastingOffset(rE);
	rN = ConvertUtmToNorthingOffset(rN);
	rS = ConvertUtmToNorthingOffset(rS);

	Continuous = 1;
	FillCats();
	SortCats();
	//ramp = 0;
	RedVal = 50;
	GreenVal = 0;
	BlueVal = 0;
	VarVal = 0;
	NumColors = 12;
	MaxBrite = 255;
	OnOff = true;
	WantNewRamp = true;
	WantNewColor = false;
	CreateRamp();

	return true;
}


void RasterTheme::FillCats()
{
	NumCats = 0;
	long i, j, k, m;
	double Int;

	for (i = 0; i < rRows; i++)
	{
		for (j = 0; j < rCols; j++)
		{
			m = i * rCols + j;
			modf(map[m], &Int);
			if (Int >= 0.0)
			{
				Cats[NumCats] = (long)ceil(map[m]);
				for (k = 0; k < NumCats; k++)
				{
					if (Cats[NumCats] == Cats[k])
						break;
				}
				if (k == NumCats)
					NumCats++;
			}
			if (NumCats > 99)
				break;
		}
		if (NumCats > 99)
		{
			NumCats = -1;

			break;
		}
	}
	MaxVal = rMaxVal;   		// base class copies
	MinVal = rMinVal;
}


void RasterTheme::SortCats()
{
	if (NumCats == -1)
		return;
	else
		CatsOK = 1;
	long i, j, SwapCats[101];

	memcpy(SwapCats, Cats, 100 * sizeof(long));
	for (i = 0; i < NumCats - 1; i++)
	{
		for (j = i + 1; j < NumCats; j++)
		{
			if (SwapCats[j] < SwapCats[i])
			{
				SwapCats[100] = SwapCats[i];
				SwapCats[i] = SwapCats[j];
				SwapCats[j] = SwapCats[100];
			}
		}
	}
	Cats[0] = 0;
	for (i = 0; i < NumCats; i++)
		Cats[i + 1] = SwapCats[i];
}


static long ThemeID = 0;
static long NumThemes = 0;

Themes* FirstTheme = 0;
Themes* CurrentTheme;
Themes* NextTheme;
Themes* LastTheme;


double QueryRasterMap(Themes* theme, double xcoord, double ycoord)
{
	if (theme->theme->map == NULL) //theme->theme->MapAlloc==false) //*
		return -2;
	if (xcoord<(theme->theme->rW) || (xcoord>theme->theme->rE))
		return -3;
	if (ycoord<(theme->theme->rS) || (ycoord>theme->theme->rN))
		return -4;

	double xpt, ypt;
	long easti, northi, posit;

	xpt = (xcoord - (theme->theme->rW)) / (theme->theme->rCellSizeX);
	ypt = (ycoord - (theme->theme->rS)) / (theme->theme->rCellSizeY);
	easti = ((long) xpt);
	northi = ((long) ypt);
	northi = theme->theme->rRows - northi - 1;
	if (northi < 0)
		northi = 0;
	posit = northi * (theme->theme->rCols) + easti;

	return theme->theme->map[posit];
}


long GetNumRasterThemes()
{
	return NumThemes;
}

long SetNumRasterThemes(long Num)
{
	NumThemes = Num;
	if (NumThemes == 0)
		ThemeID = 0;	// reset Unique Theme ID

	return NumThemes;
}

Themes* GetRasterTheme(long Num)
{
	long i;

	CurrentTheme = FirstTheme;
	for (i = 0; i < NumThemes; i++)
	{
		NextTheme = (Themes *) CurrentTheme->next;
		if (CurrentTheme->ID == Num)
			break;
		CurrentTheme = NextTheme;
	}

	return CurrentTheme;
}

Themes* AllocRasterTheme()
{
	if (FirstTheme == 0)
	{
		if ((FirstTheme = new Themes) == NULL)
			return NULL;

		CurrentTheme = FirstTheme;
		CurrentTheme->last = 0;
		CurrentTheme->next = 0;
		LastTheme = 0;
	}
	else
	{
		long i;

		CurrentTheme = FirstTheme;
		for (i = 0; i < NumThemes; i++)
		{
			LastTheme = CurrentTheme;
			NextTheme = (Themes *) CurrentTheme->next;
			CurrentTheme = NextTheme;
		}
		CurrentTheme = new Themes;
		CurrentTheme->next = 0;
		CurrentTheme->last = (Themes *) LastTheme;
		if (LastTheme)
			LastTheme->next = (Themes *) CurrentTheme;
	}
	NumThemes++;
	CurrentTheme->Changed = 1;
	CurrentTheme->ID = ThemeID++;
	CurrentTheme->Selected = true;
	CurrentTheme->theme = new RasterTheme();//(RasterTheme *) calloc(1, sizeof(RasterTheme));

	return CurrentTheme;
}

void FreeAllRasterThemes()
{
	long i;

	CurrentTheme = FirstTheme;
	for (i = 0; i < NumThemes; i++)
	{
		NextTheme = (Themes *) CurrentTheme->next;
		delete CurrentTheme->theme;
		delete CurrentTheme;//GlobalFree(CurrentTheme);//free(CurrentTheme);
		CurrentTheme = NextTheme;
	}
	FirstTheme = 0;
	ThemeID = 0;
	NumThemes = 0;
}

void FreeRasterTheme(long Num)
{
	long i;

	CurrentTheme = FirstTheme;
	for (i = 0; i < NumThemes; i++)
	{
		NextTheme = (Themes *) CurrentTheme->next;
		if (CurrentTheme->ID == Num)
			break;
		CurrentTheme = NextTheme;
	}
	if (CurrentTheme->last)
	{
		LastTheme = (Themes *) CurrentTheme->last;
		LastTheme->next = (Themes *) NextTheme;
	}
	delete CurrentTheme->theme;
	delete CurrentTheme;//GlobalFree(CurrentTheme);//free(CurrentTheme);
	if (Num == 0 && NextTheme != NULL)
	{
		FirstTheme = NextTheme;
		FirstTheme->last = 0;
	}
	//     else
	//     	FirstTheme=LastTheme;
	NumThemes--;
	ThemeID--;
	if (NumThemes == 0)
		FirstTheme = 0;
	CurrentTheme = FirstTheme;  				// reorder the themes
	for (i = 0; i < NumThemes; i++)
	{
		NextTheme = (Themes *) CurrentTheme->next;
		if (CurrentTheme->ID > Num)
			CurrentTheme->ID--;
		CurrentTheme = NextTheme;
	}
}



//------------------------------------------------------------------------------
//
//	Landscape header and cell positioning functions
//
//------------------------------------------------------------------------------

static long NumVals;
static bool CantAllocLCP = false;

void ReadHeader2()
{
	fseek(landfile, 0, SEEK_SET);
	fread(&Header2, sizeof(headdata2), 1, landfile);
	// do this incase a version 1.0 file has gotten through
	Header.loeast = ConvertUtmToEastingOffset(Header.WestUtm);
	Header.hieast = ConvertUtmToEastingOffset(Header.EastUtm);
	Header.lonorth = ConvertUtmToNorthingOffset(Header.SouthUtm);
	Header.hinorth = ConvertUtmToNorthingOffset(Header.NorthUtm);
	//	celldata cell;
	//	fseek(landfile, sizeof(cell), SEEK_CUR);
	//	OldFilePosition=ftell(landfile);		   // position of file at end of first record
	//	OldFilePosition=0;

	if (Header2.FOptions == 1 || Header2.FOptions == 3)
		NEED_CUST_MODELS = true;
	else
		NEED_CUST_MODELS = false;
	if (Header2.FOptions == 2 || Header2.FOptions == 3)
		NEED_CONV_MODELS = true;
	else
		NEED_CONV_MODELS = false;
	HAVE_CUST_MODELS = false;
	HAVE_CONV_MODELS = false;
	// set raster resolution
	RasterCellResolutionX = (Header2.EastUtm - Header2.WestUtm) /
		(double) Header2.numeast;
	RasterCellResolutionY = (Header2.NorthUtm - Header2.SouthUtm) /
		(double) Header2.numnorth;
	ViewPortNorth = RasterCellResolutionY * (double) Header2.numnorth +
		Header2.lonorth;
	ViewPortSouth = Header2.lonorth;
	ViewPortEast = RasterCellResolutionX * (double) Header2.numeast +
		Header2.loeast;
	ViewPortWest = Header2.loeast;
	//	NumViewNorth=(ViewPortNorth-ViewPortSouth)/Header.YResol;
	//	NumViewEast=(ViewPortEast-ViewPortWest)/Header.XResol;
	double rows, cols;
	rows = (ViewPortNorth - ViewPortSouth) / Header2.YResol;
	NumViewNorth = (long)rows;
	if (modf(rows, &rows) > 0.5)
		NumViewNorth++;
	cols = (ViewPortEast - ViewPortWest) / Header2.XResol;
	NumViewEast = (long)cols;
	if (modf(cols, &cols) > 0.5)
		NumViewEast++;

	if (HaveCrownFuels())
		NumVals = 8;
	else
		NumVals = 5;
	CantAllocLCP = false;

	if (lcptheme)
	{
		delete lcptheme;
		lcptheme = 0;
	}
	lcptheme = new LandscapeTheme(true);
}


void ReadHeader()
{
	fseek(landfile, 0, SEEK_SET);

	//     unsigned int sizeh=sizeof(Header);
	//	fread(&Header, sizeh, 1, landfile);
	int32_t longtemp;
	int16_t shortemp;
	long i,j;
	
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.CrownFuels = (long) longtemp;
	//printf("longtemp:%d\nHeader.CrownFuels:%d\n",longtemp,Header.CrownFuels);
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.GroundFuels =  (long) longtemp;
	//printf("longtemp:%d\nHeader.GroundFuels:%d\n",longtemp,Header.GroundFuels);
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.latitude =  (long) longtemp;
	//printf("longtemp:%d\nHeader.latitude:%d\n",longtemp,Header.latitude);
	fread(&Header.loeast, sizeof(double), 1, landfile);
	fread(&Header.hieast, sizeof(double), 1, landfile);
	fread(&Header.lonorth, sizeof(double), 1, landfile);
	fread(&Header.hinorth, sizeof(double), 1, landfile);
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.loelev =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.hielev =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.numelev =  (long) longtemp;
	for (i=0;i<100;i++){
	  fread(&longtemp, sizeof(int32_t), 1, landfile);
	  Header.elevs[i] =  (long) longtemp;
	}
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.loslope =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.hislope =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.numslope =  (long) longtemp;
	for (i=0;i<100;i++){
	  fread(&longtemp, sizeof(int32_t), 1, landfile);
	  Header.slopes[i] =  (long) longtemp;
	}
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.loaspect =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.hiaspect =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.numaspect =  (long) longtemp;
	for (i=0;i<100;i++){
	  fread(&longtemp, sizeof(int32_t), 1, landfile);
	  Header.aspects[i] =  (long) longtemp;
	} 
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.lofuel =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.hifuel =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.numfuel =  (long) longtemp;
	for (i=0;i<100;i++){
	  fread(&longtemp, sizeof(int32_t), 1, landfile);
	  Header.fuels[i] =  (long) longtemp;
	}
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.locover =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.hicover =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.numcover =  (long) longtemp;
	for (i=0;i<100;i++){
	  fread(&longtemp, sizeof(int32_t), 1, landfile);
	  Header.covers[i] =  (long) longtemp;
	}
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.loheight =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.hiheight =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.numheight =  (long) longtemp;
	for (i=0;i<100;i++){
	  fread(&longtemp, sizeof(int32_t), 1, landfile);
	  Header.heights[i] =  (long) longtemp;
	}
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.lobase =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.hibase =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.numbase =  (long) longtemp;
	for (i=0;i<100;i++){
	  fread(&longtemp, sizeof(int32_t), 1, landfile);
	  Header.bases[i] =  (long) longtemp;
	}
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.lodensity =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.hidensity =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.numdensity =  (long) longtemp;
	for (i=0;i<100;i++){
	  fread(&longtemp, sizeof(int32_t), 1, landfile);
	  Header.densities[i] =  (long) longtemp;
	}
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.loduff =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.hiduff =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.numduff =  (long) longtemp;
	for (i=0;i<100;i++){
	  fread(&longtemp, sizeof(int32_t), 1, landfile);
	  Header.duffs[i] =  (long) longtemp;
	}
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.lowoody =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.hiwoody =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.numwoody =  (long) longtemp;
	for (i=0;i<100;i++){
	  fread(&longtemp, sizeof(int32_t), 1, landfile);
	  Header.woodies[i] =  (long) longtemp;
	}
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.numeast =  (long) longtemp;
	fread(&longtemp, sizeof(int32_t), 1, landfile);
	Header.numnorth =  (long) longtemp;
	fread(&Header.EastUtm, sizeof(double), 1, landfile);
	fread(&Header.WestUtm, sizeof(double), 1, landfile);
	fread(&Header.NorthUtm, sizeof(double), 1, landfile);
	fread(&Header.SouthUtm, sizeof(double), 1, landfile);
	fread(&Header.GridUnits, sizeof(int32_t), 1, landfile);
	Header.GridUnits =  (long) longtemp;
	fread(&Header.XResol, sizeof(double), 1, landfile);
	fread(&Header.YResol, sizeof(double), 1, landfile);
	fread(&shortemp, sizeof(int16_t), 1, landfile);
	Header.GridUnits = (short) shortemp;
	fread(&shortemp, sizeof(int16_t), 1, landfile);
	Header.SUnits = (short) shortemp;
	fread(&shortemp, sizeof(int16_t), 1, landfile);
	Header.AUnits = (short) shortemp;
	fread(&shortemp, sizeof(int16_t), 1, landfile);
	Header.FOptions = (short) shortemp;
	fread(&shortemp, sizeof(int16_t), 1, landfile);
	Header.CUnits = (short) shortemp;
	fread(&shortemp, sizeof(int16_t), 1, landfile);
	Header.HUnits = (short) shortemp;
	fread(&shortemp, sizeof(int16_t), 1, landfile);
	Header.BUnits = (short) shortemp;
	fread(&shortemp, sizeof(int16_t), 1, landfile);
	Header.PUnits = (short) shortemp;
	fread(&shortemp, sizeof(int16_t), 1, landfile);
	Header.DUnits = (short) shortemp;
	fread(&shortemp, sizeof(int16_t), 1, landfile);
	Header.WOptions = (short) shortemp;
	fread(Header.ElevFile, sizeof(char), 256, landfile);
	fread(Header.SlopeFile, sizeof(char), 256, landfile);
	fread(Header.AspectFile, sizeof(char), 256, landfile);
	fread(Header.FuelFile, sizeof(char), 256, landfile);
	fread(Header.CoverFile, sizeof(char), 256, landfile);
	fread(Header.HeightFile, sizeof(char), 256, landfile);
	fread(Header.BaseFile, sizeof(char), 256, landfile);
	fread(Header.DensityFile, sizeof(char), 256, landfile);
	fread(Header.DuffFile, sizeof(char), 256, landfile);
	fread(Header.WoodyFile, sizeof(char), 256, landfile);
	fread(Header.Description, sizeof(char), 512, landfile);
	// do this in case a version 1.0 file has gotten through
	Header.loeast = ConvertUtmToEastingOffset(Header.WestUtm);
	Header.hieast = ConvertUtmToEastingOffset(Header.EastUtm);
	Header.lonorth = ConvertUtmToNorthingOffset(Header.SouthUtm);
	Header.hinorth = ConvertUtmToNorthingOffset(Header.NorthUtm);
	if (Header.FOptions == 1 || Header.FOptions == 3)
		NEED_CUST_MODELS = true;
	else
		NEED_CUST_MODELS = false;
	if (Header.FOptions == 2 || Header.FOptions == 3)
		NEED_CONV_MODELS = true;
	else
		NEED_CONV_MODELS = false;
	//HAVE_CUST_MODELS=false;
	//HAVE_CONV_MODELS=false;
	// set raster resolution
	RasterCellResolutionX = (Header.EastUtm - Header.WestUtm) /
		(double) Header.numeast;
	RasterCellResolutionY = (Header.NorthUtm - Header.SouthUtm) /
		(double) Header.numnorth;
	ViewPortNorth = RasterCellResolutionY * (double) Header.numnorth +
		Header.lonorth;
	ViewPortSouth = Header.lonorth;
	ViewPortEast = RasterCellResolutionX * (double) Header.numeast +
		Header.loeast;
	ViewPortWest = Header.loeast;
	//	NumViewNorth=(ViewPortNorth-ViewPortSouth)/Header.YResol;
	//	NumViewEast=(ViewPortEast-ViewPortWest)/Header.XResol;
	double rows, cols;
	rows = (ViewPortNorth - ViewPortSouth) / Header.YResol;
	NumViewNorth = (long)rows;
	if (modf(rows, &rows) > 0.5)
		NumViewNorth++;
	cols = (ViewPortEast - ViewPortWest) / Header.XResol;
	NumViewEast = (long)cols;
	if (modf(cols, &cols) > 0.5)
		NumViewEast++;

	if (HaveCrownFuels())
	{
		if (HaveGroundFuels())
			NumVals = 10;
		else
			NumVals = 8;
	}
	else
	{
		if (HaveGroundFuels())
			NumVals = 7;
		else
			NumVals = 5;
	}
	CantAllocLCP = false;

	if (lcptheme)
	{
		delete lcptheme;
		lcptheme = 0;
	}
	lcptheme = new LandscapeTheme(false);

	if (landscape == 0)
	{
		if (CantAllocLCP == false)
		{
			//long i;
			double NumAlloc;

			fseek(landfile, headsize, SEEK_SET);
			//     		if((landscape=(short *) calloc(Header.numnorth*Header.numeast, NumVals*sizeof(short)))!=NULL)
			NumAlloc = (double)
				(Header.numnorth * Header.numeast * NumVals * sizeof(short));
			if (NumAlloc > 2147483647)
			{
				CantAllocLCP = true;

				return;
			}

			try
			{
				landscape = new short[Header.numnorth * Header.numeast * NumVals];
			}
			catch (...)
			{
				landscape = 0;
			}
			if (landscape != NULL)
			{
				//ZeroMemory(landscape,Header.numnorth * Header.numeast * NumVals * sizeof(short));
                                memset(landscape,0x0,Header.numnorth * Header.numeast * NumVals * sizeof(short));
				
				//for (i = 0; i < Header.numnorth; i++){
				for (i = 0; i < (Header.numnorth*(NumVals * Header.numeast)); i=i+(NumVals * Header.numeast)){
				  for (j = 0;j < (NumVals * Header.numeast);j++){
				    //fread(&landscape[i * NumVals * Header.numeast],sizeof(int16_t), NumVals * Header.numeast, landfile);
				    fread(&shortemp,sizeof(int16_t), 1, landfile);
				    landscape[i+j] = (short) shortemp;
				  }
				}
				fseek(landfile, headsize, SEEK_SET);
				//     		 	OldFilePosition=0;     // thread local
				CantAllocLCP = false;
			}
			else
				CantAllocLCP = true;
		}
	}
	//	long p;
	//   CellData(Header.loeast, Header.hinorth, &p);
}


size_t GetHeadSize()
{
	return headsize;
}


void SetFilePosition(long position)
{
	OldFilePosition = position;
}


bool SetCellData(double x, double y, celldata* cell, crowndata* crown,
	grounddata* ground)
{
	short ldata[10];
	long posit;

	posit = GetCellPosition(x, y);

	if (!CantAllocLCP)
	{
		memcpy(ldata, &landscape[posit * NumVals], NumVals * sizeof(short));

		if (cell != NULL)
			memcpy(ldata, cell, 5 * sizeof(short));
		if (crown != NULL)
			memcpy(&ldata[5], crown, 3 * sizeof(short));
		if (ground != NULL)
			memcpy(&ldata[8], ground, 2 * sizeof(short));

		memcpy(&landscape[posit * NumVals], ldata, NumVals * sizeof(short));
	}
	else
	{
		celldata tcell;

		CloseEditLandFile();
		OpenEditLandFile();
		fseek(GetLandFile(), GetHeadSize(), SEEK_SET);
		SetFilePosition(0);

		if (Header.CrownFuels == 20)
		{
			if (Header.GroundFuels == 20)
			{
				fseek(landfile, (posit - OldFilePosition) * sizeof(celldata),
					SEEK_CUR);
				if (cell != NULL)
				{
					fseek(landfile,
						-(posit - OldFilePosition) * sizeof(celldata),
						SEEK_CUR);
					fwrite(cell, sizeof(celldata), 1, landfile);
				}
			}
			else
			{
				fseek(landfile,
					(posit - OldFilePosition) * (sizeof(celldata) +
					sizeof(grounddata)),
					SEEK_CUR);
				if (cell != NULL && ground != NULL)
				{
					fwrite(cell, sizeof(celldata), 1, landfile);
					fwrite(ground, sizeof(grounddata), 1, landfile);
				}
				else if (cell != NULL)
					fwrite(cell, sizeof(celldata), 1, landfile);
				else if (ground != NULL)
				{
					fread(&tcell, sizeof(celldata), 1, landfile);
					fwrite(ground, sizeof(grounddata), 1, landfile);
				}
			}
		}
		else
		{
			if (Header.GroundFuels == 20)		// none
			{
				fseek(landfile,
					(posit - OldFilePosition) * (sizeof(celldata) +
					sizeof(crowndata)),
					SEEK_CUR);
				if (cell != NULL && crown != NULL)
				{
					fwrite(cell, sizeof(celldata), 1, landfile);
					fwrite(crown, sizeof(crowndata), 1, landfile);
				}
				else if (cell != NULL)
					fwrite(cell, sizeof(celldata), 1, landfile);
				else if (crown != NULL)
				{
					fread(&tcell, sizeof(celldata), 1, landfile);
					fwrite(crown, sizeof(crowndata), 1, landfile);
				}
			}
			else
			{
				fseek(landfile,
					(posit - OldFilePosition) * (sizeof(celldata) +
					sizeof(crowndata) +
					sizeof(grounddata)),
					SEEK_CUR);
				if (cell != NULL && crown != NULL && ground != NULL)
				{
					fwrite(cell, sizeof(celldata), 1, landfile);
					fwrite(crown, sizeof(crowndata), 1, landfile);
					fwrite(ground, sizeof(grounddata), 1, landfile);
				}
				else if (cell != NULL && crown != NULL)
				{
					fwrite(cell, sizeof(celldata), 1, landfile);
					fwrite(crown, sizeof(crowndata), 1, landfile);
				}
				else if (cell != NULL && ground != NULL)
				{
					fwrite(cell, sizeof(celldata), 1, landfile);
					fread(&tcell, sizeof(crowndata), 1, landfile);
					fwrite(ground, sizeof(grounddata), 1, landfile);
				}
				else if (crown != NULL && ground != NULL)
				{
					fread(&tcell, sizeof(celldata), 1, landfile);
					fwrite(crown, sizeof(crowndata), 1, landfile);
					fwrite(ground, sizeof(grounddata), 1, landfile);
				}
				else if (cell != NULL)
					fwrite(cell, sizeof(celldata), 1, landfile);
				else if (crown != NULL)
				{
					fread(&tcell, sizeof(celldata), 1, landfile);
					fwrite(crown, sizeof(crowndata), 1, landfile);
				}
				else if (ground != NULL)
				{
					fread(&tcell, sizeof(celldata), 1, landfile);
					fread(&tcell, sizeof(crowndata), 1, landfile);
					fwrite(ground, sizeof(grounddata), 1, landfile);
				}
			}
		}

		CloseEditLandFile();
		ReOpenLandFile();    // doesent alloc memory
	}

	return true;
}


void GetCellDataFromMemory(long posit, celldata& cell, crowndata& cfuel,
	grounddata& gfuel)
{
	short ldata[10];

	memcpy(ldata, &landscape[posit * NumVals], NumVals * sizeof(short));
	switch (NumVals)
	{
	case 5:
		// only 5 basic themes
		memcpy(&cell, ldata, NumVals * sizeof(short));
		break;
	case 7:
		// 5 basic and duff and woody
		memcpy(&cell, ldata, 5 * sizeof(short));
		memcpy(&gfuel, &ldata[5], 2 * sizeof(short));
		break;
	case 8:
		// 5 basic and crown fuels
		memcpy(&cell, ldata, 5 * sizeof(short));
		memcpy(&cfuel, &ldata[5], 3 * sizeof(short));
		break;
	case 10:
		// 5 basic, crown fuels, and duff and woody
		memcpy(&cell, ldata, 5 * sizeof(short));
		memcpy(&cfuel, &ldata[5], 3 * sizeof(short));
		memcpy(&gfuel, &ldata[8], 2 * sizeof(short));
		break;
	}
}

/*
void SetBinaryCellData(unsigned long row, unsigned long col, unsigned long num, short *data)
{
	 long Posit;

	 Posit=(row*Header.numeast+col);
	 if(Posit+num>=Header.numeast*Header.numnorth)
	 	num=Header.numeast*Header.numnorth-Posit-1;
	 Posit*=NumVals;
	 memcpy(&landscape[Posit], data, NumVals*num*sizeof(short));
}
*/

/*
bool GetBinaryCellData(unsigned long row, unsigned long col, unsigned long num, short *data)
{
	 if(landscape==0)
	 {    if(CantAllocLCP==false)
	 	{	long i;

	 		fseek(landfile, headsize, SEEK_SET);
	 		if((landscape=new short[Header.numnorth*Header.numeast*NumVals])!=NULL)
		  	{    ZeroMemory(landscape, Header.numnorth*Header.numeast*NumVals*sizeof(short));
			   	for(i=0; i<Header.numnorth; i++)
		  			fread(&landscape[i*NumVals*Header.numeast], sizeof(short),
							NumVals*Header.numeast, landfile);
		  		fseek(landfile, headsize, SEEK_SET);
	 		 	OldFilePosition=0;
			   	CantAllocLCP=false;
		  	}
		  	else
		  		CantAllocLCP=true;
		  }
	 }
	 if(CantAllocLCP)
	 	return false;

	long Posit;

	 if(row<0)
	 	row=0;
	 else if(row>Header.numnorth-1)
	 	row=Header.numnorth-1;
	 if(col<0)
	 	col=0;
	 else if(col>Header.numeast-1)
	 	col=Header.numeast-1;

	 Posit=(row*Header.numeast+col);
	 if(Posit+num>=Header.numeast*Header.numnorth)
	 	num=Header.numeast*Header.numnorth-Posit-1;
	 Posit*=NumVals;
	 memcpy(data, &landscape[Posit], NumVals*num*sizeof(short));

	 return true;
}

*/

bool SaveLCPMemoryToFile(char* NewName)
{
	FILE* otpfile;
	long i, j, posit;

	otpfile = fopen(NewName, "wb");
	fwrite(&Header, sizeof(headdata), 1, otpfile);
	for (i = 0; i < Header.numnorth; i++)
	{
		for (j = 0; j < Header.numeast; j++)
		{
			posit = (i * Header.numeast + j) * NumVals;
			fwrite(&landscape[posit], NumVals * sizeof(short), 1, otpfile);
		}
	}
	fclose(otpfile);

	return true;
}


bool WriteAsciiLCPTheme(long ThemeNum, long Type, char* Name)
{
	FILE* otpfile;
	double mult = 1.0;
	long i, j, ThemeLoc = ThemeNum;
	short val[10];
	unsigned long loc;

	otpfile = fopen(Name, "w");
	printf("Test2\n");
	switch (Type)
	{
	case 1:
		// grid
		fprintf(otpfile, "%s %ld\n", "NCOLS", Header.numeast);
		fprintf(otpfile, "%s %ld\n", "NROWS", Header.numnorth);
		fprintf(otpfile, "%s %lf\n", "XLLCORNER", Header.WestUtm);//*MetersToKm);
		fprintf(otpfile, "%s %lf\n", "YLLCORNER", Header.SouthUtm);//*MetersToKm);
		fprintf(otpfile, "%s %lf\n", "CELLSIZE", Header.XResol);//*MetersToKm
		fprintf(otpfile, "%s %ld\n", "NODATA_VALUE", -1L);
		break;
	case 2:
		// grass
		fprintf(otpfile, "%s    %lf\n", "north:", Header.NorthUtm);//*MetersToKm);
		fprintf(otpfile, "%s    %lf\n", "south:", Header.SouthUtm);//*MetersToKm);
		fprintf(otpfile, "%s     %lf\n", "east:", Header.EastUtm);//*MetersToKm);
		fprintf(otpfile, "%s     %lf\n", "west:", Header.WestUtm);//*MetersToKm);
		fprintf(otpfile, "%s     %ld\n", "rows:", Header.numnorth);
		fprintf(otpfile, "%s     %ld\n", "cols:", Header.numeast);
		break;
	}
	if (NumVals == 7)
		ThemeLoc -= 3;

	if (landscape == 0)
	{
		fseek(landfile, headsize, SEEK_SET);
		SetFilePosition(0);
	}

	if (ThemeNum == 5 || ThemeNum == 6)	// crown height and base height
	{
		switch (ThemeNum)
		{
		case 5:
			if (Header.HUnits == 1 || Header.HUnits == 2)
				mult = 10.0;
			break;
		case 6:
			if (Header.BUnits == 1 || Header.BUnits == 2)
				mult = 10.0;
			break;
		}

		for (i = 0; i < Header.numnorth; i++)
		{
			for (j = 0; j < Header.numeast; j++)
			{
				if (landscape)
				{
					loc = (i * Header.numeast + j) * NumVals + ThemeLoc;
					fprintf(otpfile, "%4.1lf ", landscape[loc] / mult);
				}
				else
				{
					fread(val, NumVals * sizeof(short), 1, landfile);
					fprintf(otpfile, "%4.1lf ", (double) val[ThemeNum] / mult);
				}
			}
			fprintf(otpfile, "\n");
		}
		fclose(otpfile);
	}
	else if (ThemeNum == 7)	// bulk density
	{
		if (Header.PUnits == 1)
			mult = 100.0;
		else if (Header.PUnits == 2)
			mult = 1000.0;
		for (i = 0; i < Header.numnorth; i++)
		{
			for (j = 0; j < Header.numeast; j++)
			{
				if (landscape)
				{
					loc = (i * Header.numeast + j) * NumVals + ThemeLoc;
					fprintf(otpfile, "%1.4lf ", landscape[loc] / mult);
				}
				else
				{
					fread(val, NumVals * sizeof(short), 1, landfile);
					fprintf(otpfile, "%4.1lf ", (double) val[ThemeNum] / mult);
				}
			}
			fprintf(otpfile, "\n");
		}
		fclose(otpfile);
	}
	else
	{
		for (i = 0; i < Header.numnorth; i++)
		{
			for (j = 0; j < Header.numeast; j++)
			{
				if (landscape)
				{
					loc = (i * Header.numeast + j) * NumVals + ThemeLoc;
					fprintf(otpfile, "%d ", landscape[loc]);
				}
				else
				{
					fread(val, NumVals * sizeof(short), 1, landfile);
					fprintf(otpfile, "%4.1lf ", (double) val[ThemeNum] / mult);
				}
			}
			fprintf(otpfile, "\n");
		}
		fclose(otpfile);
	}

	if (landscape == 0)
	{
		fseek(landfile, headsize, SEEK_SET);
		SetFilePosition(0);
	}

	return true;
}


celldata CellData(double east, double north, celldata& cell, crowndata& cfuel,
	grounddata& gfuel, long* posit)
{
	long Position;
	long j;
	int16_t shortemp;
	if (landscape == 0)
	{
		if (CantAllocLCP == false)
		{
			long i;
			double AllocSize;
			printf("headsize:%d\n",headsize);
			fseek(landfile, headsize, SEEK_SET);
			AllocSize = Header.numnorth * Header.numeast * NumVals;
			if (AllocSize > 2147483647)
			{
				CantAllocLCP = true;
			}
			else
			{
				try
				{
					landscape = new short[Header.numnorth * Header.numeast * NumVals];
				}
				catch (...)
				{
					CantAllocLCP = true;
					landscape = 0;
				}
				if (landscape != NULL)
				{
					//ZeroMemory(landscape,Header.numnorth * Header.numeast * NumVals * sizeof(short));
                                        memset(landscape,0x0,Header.numnorth * Header.numeast * NumVals * sizeof(short));
					for (i = 0; i < Header.numnorth; i++){
						//fread(&landscape[i * NumVals * Header.numeast],sizeof(short), NumVals * Header.numeast, landfile);
					  for (j = 0 ;j < NumVals * Header.numeast;j++)
					  fread(&shortemp,sizeof(int16_t), 1, landfile);
					  landscape[i * j] = (short) shortemp;
					}
					fseek(landfile, headsize, SEEK_SET);
					//     		 	OldFilePosition=0;     // thread local
					CantAllocLCP = false;
				}
				else
					CantAllocLCP = true;
			}
		}
	}

	Position = GetCellPosition(east, north);
	if (!CantAllocLCP)
	{
		GetCellDataFromMemory(Position, cell, cfuel, gfuel);

		return cell;
	}

	if (Header.CrownFuels == 20)
	{
		if (Header.GroundFuels == 20)
		{
			fseek(landfile, (Position - OldFilePosition) * sizeof(celldata),
				SEEK_CUR);
			fread(&cell, sizeof(celldata), 1, landfile);
		}
		else
		{
			fseek(landfile,
				(Position - OldFilePosition) * (sizeof(celldata) +
				sizeof(grounddata)),
				SEEK_CUR);
			fread(&cell, sizeof(celldata), 1, landfile);
			fread(&gfuel, sizeof(grounddata), 1, landfile);
		}
	}
	else
	{
		if (Header.GroundFuels == 20)		// none
		{
			fseek(landfile,
				(Position - OldFilePosition) * (sizeof(celldata) +
				sizeof(crowndata)),
				SEEK_CUR);
			fread(&cell, sizeof(celldata), 1, landfile);
			fread(&cfuel, sizeof(crowndata), 1, landfile);
		}
		else
		{
			fseek(landfile,
				(Position - OldFilePosition) * (sizeof(celldata) +
				sizeof(crowndata) +
				sizeof(grounddata)),
				SEEK_CUR);
			fread(&cell, sizeof(celldata), 1, landfile);
			fread(&cfuel, sizeof(crowndata), 1, landfile);
			fread(&gfuel, sizeof(grounddata), 1, landfile);
		}
		if (cfuel.h > 0)
		{
			CanopyChx.Height = (double) cfuel.h / 10.0;
			if (Header.HUnits == 2)
				CanopyChx.Height /= 3.280839;
		}
		else
			CanopyChx.Height = CanopyChx.DefaultHeight;
		if (cfuel.b > 0)
		{
			CanopyChx.CrownBase = (double) cfuel.b / 10.0;
			if (Header.BUnits == 2)
				CanopyChx.CrownBase /= 3.280839;
		}
		else
			CanopyChx.CrownBase = CanopyChx.DefaultBase;
		if (cfuel.p > 0)
		{
			if (Header.PUnits == 1)
				CanopyChx.BulkDensity = ((double) cfuel.p) / 100.0;
			else if (Header.PUnits == 2)
				CanopyChx.BulkDensity = ((double) cfuel.p / 1000.0) * 16.01845;
		}
		else
			CanopyChx.BulkDensity = CanopyChx.DefaultDensity;
	}

	OldFilePosition = Position + 1;
	if (posit != NULL)
		*posit = Position;

	return cell;
}


long GetCellPosition(double east, double north)
{
	double xpt = (east - Header.loeast) / GetCellResolutionX();
	double ypt = (north - Header.lonorth) / GetCellResolutionY();
	long easti = ((long) xpt);
	long northi = ((long) ypt);
	northi = Header.numnorth - northi - 1;
	if (northi < 0)
		northi = 0;
	long posit = (northi* Header.numeast + easti);

	return posit;
}


void SetVisibleTheme(long ThemeNumber)
{
	VisibleThemeNumber = ThemeNumber;
}

long GetVisibleTheme()
{
	if (!HaveCrownFuels() && VisibleThemeNumber > 4)
		VisibleThemeNumber = 3;

	return VisibleThemeNumber;
}


//--------------------------------------------------------

headdata GetHeaderInformation()
{
	return Header;
}


void SetHeader(headdata* data)
{
	memcpy(&Header, data, sizeof(headdata));
}


long GetNumEast()
{
	return Header.numeast;
}


long GetNumNorth()
{
	return Header.numnorth;
}


double GetWestUtm()
{
	return Header.WestUtm;
}

double GetEastUtm()
{
	return Header.EastUtm;
}

double GetSouthUtm()
{
	return Header.SouthUtm;
}

double GetNorthUtm()
{
	return Header.NorthUtm;
}

double GetLoEast()
{
	return Header.loeast;
}
double GetHiEast()
{
	return Header.hieast;
}

double GetLoNorth()
{
	return Header.lonorth;
}

double GetHiNorth()
{
	return Header.hinorth;
}

double ConvertEastingOffsetToUtm(double input)
{
	double MetersToKm = 1.0;
	double ipart;

	if (Header.GridUnits == 2)
		MetersToKm = 0.001;

	modf(Header.WestUtm / 1000.0, &ipart);

	return (input + ipart * 1000.0) * MetersToKm;
}

double ConvertNorthingOffsetToUtm(double input)
{
	double MetersToKm = 1.0;
	double ipart;

	if (Header.GridUnits == 2)
		MetersToKm = 0.001;

	modf(Header.SouthUtm / 1000.0, &ipart);

	return (input + ipart * 1000.0) * MetersToKm;
}

double ConvertUtmToEastingOffset(double input)
{
	double KmToMeters = 1.0;
	double ipart;

	if (Header.GridUnits == 2)
		KmToMeters = 1000.0;

	modf(Header.WestUtm / 1000.0, &ipart);

	return input * KmToMeters - ipart * 1000.0;
}

double ConvertUtmToNorthingOffset(double input)
{
	double KmToMeters = 1.0;
	double ipart;

	if (Header.GridUnits == 2)
		KmToMeters = 1000.0;

	modf(Header.SouthUtm / 1000.0, &ipart);

	return input * KmToMeters - ipart * 1000.0;
}


char* GetHeaderDescription()
{
	return Header.Description;
}

//-----------------------------------------------------------------------------

long GetLoElev()
{
	return Header.loelev;
}

long GetHiElev()
{
	return Header.hielev;
}


long GetConditMonth()
{
	return conditmonth;
}


long GetConditDay()
{
	return conditday;
}


long GetConditMinDeficit()
{
	if (CondPeriod == false)
		return 0;

	long CondMin;
	long StartMin;

	CondMin = (GetJulianDays(conditmonth) + conditday) * 1440;// 00:01 on this day
	StartMin = (GetJulianDays(startmonth) + startday) * 1440 +
		(starthour / 100 * 60) +
		startmin;
	if (StartMin < CondMin)
		StartMin += 365 * 1440;

	return StartMin - CondMin;
}


void SetConditMonth(long input)
{
	conditmonth = input;
}


void SetConditDay(long input)
{
	conditday = input;
}


long GetLatitude()
{
	return Header.latitude;
}

long GetStartMonth()
{
	return startmonth;
}

long GetStartDay()
{
	return startday;
}

long GetStartHour()
{
	return starthour;
}


long GetStartMin()
{
	return startmin;
}

long GetStartDate()
{
	return startdate;
}


long GetMaxMonth()
{
	long maxmonth = LastMonth[0].all;
	long numstations = GetNumStations();

	for (long i = 0; i < numstations; i++)
	{
		if (LastMonth[i].all < maxmonth)
			maxmonth = LastMonth[i].all;
	}

	return maxmonth;
}

long GetMaxDay()
{
	long i;
	long maxday = LastDay[0].all;
	long numstations = GetNumStations();
	long maxmonth = GetMaxMonth();

	for (i = 0; i < numstations; i++)
	{
		if (LastMonth[i].all == maxmonth && maxday > LastDay[i].all)
			maxday = LastDay[i].all;
	}

	return maxday;
}

long GetMaxHour()
{
	long i;
	long maxhour = LastHour[0].all;
	long maxmonth = GetMaxMonth();
	long maxday = GetMaxDay();
	long numstations = GetNumStations();

	for (i = 0; i < numstations; i++)
	{
		if (LastMonth[i].all == maxmonth && LastDay[i].all == maxday)
		{
			if (LastHour[i].all < maxhour)
				maxhour = LastHour[i].all;
		}
	}

	return maxhour;
}


long GetMinMonth()
{
	long i;
	long minmonth = FirstMonth[0].all;
	long NStat = GetNumStations();

	for (i = 0; i < NStat; i++)
	{
		if (FirstMonth[i].all > minmonth)
			minmonth = FirstMonth[i].all;
	}

	return minmonth;
}


long GetMinDay()
{
	long i;
	long minday = FirstDay[0].all;
	long NStat = GetNumStations();
	long minmonth = GetMinMonth();

	for (i = 0; i < NStat; i++)
	{
		if (FirstMonth[i].all == minmonth)
		{
			if (FirstDay[i].all > minday)
				minday = FirstDay[i].all;
		}
	}

	return minday;
}


long GetMinHour()
{
	long i;
	long minhour = FirstHour[0].all;
	long NStat = GetNumStations();
	long minmonth = GetMinMonth();
	long minday = GetMinDay();

	for (i = 0; i < NStat; i++)
	{
		if (LastMonth[i].all == minmonth && LastDay[i].all == minday)
		{
			if (FirstHour[i].all > minhour)
				minhour = FirstHour[i].all;
		}
	}

	return minhour;
}


//-------

long IgnitionResetAtRestart(long YesNo)
{
	if (YesNo >= 0)
		IgnitionReset = YesNo;

	return IgnitionReset;
}

long RotateSensitiveIgnitions(long YesNo)
{
	if (YesNo >= 0)
		RotateIgnitions = YesNo;

	return RotateIgnitions;
}


long ShowFiresAsGrown(long YesNo)
{
	if (YesNo >= 0)
		ShowVectsAsGrown = YesNo;

	return ShowVectsAsGrown;
}


long DurationResetAtRestart(long YesNo)
{
	if (YesNo >= 0)
		DurationReset = YesNo;

	return DurationReset;
}


bool PreserveInactiveEnclaves(long YesNo)
{
	if (YesNo >= 0)
		InactiveEnclaves = (bool) YesNo;

	return InactiveEnclaves;
}


void SetStartMonth(long input)
{
	startmonth = input;
}

void SetStartDay(long input)
{
	startday = input;
}

void SetStartHour(long input)
{
	starthour = input;
}

void SetStartMin(long input)
{
	startmin = input;
}

void SetStartDate(long input)
{
	startdate = input;
}

long GetEndMonth()
{
	return endmonth;
}

long GetEndDay()
{
	return endday;
}

long GetEndHour()
{
	return endhour;
}

long GetEndDate()
{
	return enddate;
}

long GetEndMin()
{
	return endmin;
}

void SetEndMonth(long input)
{
	endmonth = input;
}

void SetEndDay(long input)
{
	endday = input;
}

void SetEndHour(long input)
{
	endhour = input;
}

void SetEndMin(long input)
{
	endmin = input;
}

void SetEndDate(long input)
{
	enddate = input;
}

double GetSimulationDuration()
{
	return SimulationDuration;
}

void SetSimulationDuration(double simdur)
{
	SimulationDuration = simdur;
}


bool UseConditioningPeriod(long YesNo)
{
	if (YesNo > -1)
		CondPeriod = (bool) YesNo;

	return CondPeriod;
}


bool EnvironmentChanged(long YesNo, long StationNumber, long FuelSize)
{
	if (FuelSize > 3)
		FuelSize = 3;
	if (YesNo > -1)
		EnvtChanged[FuelSize][StationNumber] = (bool) YesNo;

	return EnvtChanged[FuelSize][StationNumber];
}


FILE* GetLandFile()
{
	return landfile;
}


bool OpenLandFile()
{
	//	SetCanopyChx(15.0, 4.0, 0.2, 30.0, 100, 2, 1);
	if (landfile)
		fclose(landfile);
	if (landscape)
	{
		delete[] landscape;//GlobalFree(landscape);//free(landscape);
		landscape = 0;
		CantAllocLCP = false;
	}
	if ((landfile = fopen(LandFName, "rb")) == NULL)
	{
		landfile = 0;

		return false;
	}

	return true;
}


bool ReOpenLandFile()
{
	if (landfile)
		fclose(landfile);
	//if(landscape)
	//{	GlobalFree(landscape);//free(landscape);
	//     landscape=0;
	//	CantAllocLCP=false;
	//}
	if ((landfile = fopen(LandFName, "rb")) == NULL)
	{
		landfile = 0;

		return false;
	}
	fseek(landfile, headsize, SEEK_SET);
	OldFilePosition = 0;

	return true;
}


bool OpenEditLandFile()
{
	if (landfile)
		fclose(landfile);
	//if(landscape)
	//{	GlobalFree(landscape);//free(landscape);
	//     landscape=0;
	//}
	CantAllocLCP = true;
	if ((landfile = fopen(LandFName, "r+b")) != NULL)
		return true;
	else
	{
		//SetFileAttributes(LandFName, FILE_ATTRIBUTE_NORMAL);
		chmod(LandFName,S_IRWXO);
		landfile = fopen(LandFName, "r+b");
	}

	return true;
}


void CloseLandFile()
{
	if (landscape)
	{
		delete[] landscape;//GlobalFree(landscape);//free(landscape);
		landscape = 0;
	}
	CantAllocLCP = false;
	if (landfile)
		fclose(landfile);
	if (lcptheme)
	{
		delete lcptheme;
		lcptheme = 0;
	}
	memset(LandFName, 0x0, sizeof LandFName);
	landfile = 0;
	CanopyChx.Height = CanopyChx.DefaultHeight;
	CanopyChx.CrownBase = CanopyChx.DefaultBase;
	CanopyChx.BulkDensity = CanopyChx.DefaultDensity;
}

void CloseEditLandFile()
{
	if (landfile)
		fclose(landfile);
	CantAllocLCP = false;
	landfile = 0;
	//     lcptheme->ReadStats();
}


bool TestForLCPVersion1()
{
	long TestCrownFuelValue;

	rewind(landfile);
	fread(&TestCrownFuelValue, sizeof(long), 1, landfile);
	if (TestCrownFuelValue < 10)	 // v1 LCP file had crown fuel indicator as 0 or 1
		return true;			// v2 LCP file has crown fuel indicator as 10 or 11

	return false;
}


bool TestForLCPVersion2()
{
	long TestCrownFuelValue;

	rewind(landfile);
	fread(&TestCrownFuelValue, sizeof(long), 1, landfile);
	if (TestCrownFuelValue > 9 && TestCrownFuelValue < 20)     // v1 LCP file had crown fuel indicator as 0 or 1
		return true;								// v2 LCP file has crown fuel indicator as 10 or 11

	return false;
}


bool ConvertLCPFileToVersion2()
{
	size_t FileNameLength;
	long i, j;
	char OldLandFName[256];
	FILE* oldlandfile;
	celldata cell;
	crowndata cfuel;

	memset(OldLandFName, 0x0, sizeof(OldLandFName));
	FileNameLength = strlen(LandFName);
	strncpy(OldLandFName, LandFName, FileNameLength - 4);
	strcat(OldLandFName, ".LCP1");
	fclose(landfile);
	if (rename(LandFName, OldLandFName)!=-1)
	{
		//-------Change Header Info  ---------------------------	 `
		oldlandfile = fopen(OldLandFName, "rb");
		fread(&OldHeader, sizeof(oldheaddata), 1, oldlandfile);
		Header2.CrownFuels = OldHeader.CrownFuels + 10;
		Header2.latitude = OldHeader.latitude;
		Header2.loelev = OldHeader.loelev;
		Header2.hielev = OldHeader.hielev;
		Header2.numeast = OldHeader.numeast;
		Header2.numnorth = OldHeader.numnorth;
		Header2.EastUtm = OldHeader.EastUtm;
		Header2.WestUtm = OldHeader.WestUtm;
		Header2.NorthUtm = OldHeader.NorthUtm;
		Header2.SouthUtm = OldHeader.SouthUtm;
		Header2.loeast = ConvertUtmToEastingOffset(Header.WestUtm);
		Header2.hieast = ConvertUtmToEastingOffset(Header.EastUtm);
		Header2.lonorth = ConvertUtmToNorthingOffset(Header.SouthUtm);
		Header2.hinorth = ConvertUtmToNorthingOffset(Header.NorthUtm);
		Header2.GridUnits = OldHeader.GridUnits;
		Header2.XResol = OldHeader.XResol;
		Header2.YResol = OldHeader.YResol;
		Header2.EUnits = OldHeader.EUnits;
		Header2.SUnits = OldHeader.SUnits;
		Header2.AUnits = OldHeader.AUnits;
		Header2.FOptions = OldHeader.FOptions;
		Header2.CUnits = OldHeader.CUnits;
		Header2.HUnits = OldHeader.HUnits;
		Header2.BUnits = OldHeader.BUnits;
		Header2.PUnits = OldHeader.PUnits;

		if (landfile)
			fclose(landfile);
		landfile = fopen(LandFName, "wb");
		if (landfile == NULL)
		{
			//SetFileAttributes(LandFName, FILE_ATTRIBUTE_NORMAL);
			chmod(LandFName,S_IRWXO);
			landfile = fopen(LandFName, "wb");
		}
		fwrite(&Header2, sizeof(headdata2), 1, landfile);

		for (i = 0; i < Header2.numnorth; i++)
		{
			for (j = 0; j < Header2.numeast; j++)
			{
				fseek(oldlandfile, 2 * sizeof(long), SEEK_CUR);
				fread(&cell, sizeof(celldata), 1, oldlandfile);
				fwrite(&cell, sizeof(celldata), 1, landfile);
				if (Header.CrownFuels == 11)
				{
					fread(&cfuel, sizeof(crowndata), 1, oldlandfile);
					cfuel.h = (short) (cfuel.h * 10);	// increase precision to tenths
					cfuel.b = (short) (cfuel.b * 10);
					if (Header.PUnits == 2)
						cfuel.p = (short) (cfuel.p * 10);
					fwrite(&cfuel, sizeof(crowndata), 1, landfile);
				}
			}
		}
		fclose(oldlandfile);
		fclose(landfile);
		if ((landfile = fopen(LandFName, "rb")) == NULL)
		{
			landfile = 0;

			return false;
		}

		return true;
	}
	else
		return false;
}


bool ConvertLCPFile2ToVersion4()
{
	size_t FileNameLength;
	long i, j;
	char OldLandFName[256];
	char BkupLandFName[256] = "";
	FILE* oldlandfile;
	celldata cell;
	crowndata cfuel;

	strcpy(BkupLandFName, LandFName);
	memset(OldLandFName, 0x0, sizeof(OldLandFName));
	FileNameLength = strlen(LandFName);
	strncpy(OldLandFName, LandFName, FileNameLength - 4);
	strcat(OldLandFName, ".LCP3");
	fclose(landfile);
	if (rename(LandFName, OldLandFName)!=-1)
	{
		//-------Change Header Info  ---------------------------	 `
		oldlandfile = fopen(OldLandFName, "rb");
		fread(&Header2, sizeof(headdata2), 1, oldlandfile);
		Header.CrownFuels = Header2.CrownFuels + 10;
		Header.latitude = Header2.latitude;
		Header.loelev = Header2.loelev;
		Header.hielev = Header2.hielev;
		Header.numeast = Header2.numeast;
		Header.numnorth = Header2.numnorth;
		Header.EastUtm = Header2.EastUtm;
		Header.WestUtm = Header2.WestUtm;
		Header.NorthUtm = Header2.NorthUtm;
		Header.SouthUtm = Header2.SouthUtm;
		Header.loeast = ConvertUtmToEastingOffset(Header.WestUtm);
		Header.hieast = ConvertUtmToEastingOffset(Header.EastUtm);
		Header.lonorth = ConvertUtmToNorthingOffset(Header.SouthUtm);
		Header.hinorth = ConvertUtmToNorthingOffset(Header.NorthUtm);
		//Header.loeast=Header2.loeast;
		//Header.hieast=Header2.hieast;
		//Header.lonorth=Header2.lonorth;
		//Header.hinorth=Header2.hinorth;
		Header.GridUnits = Header2.GridUnits;
		Header.XResol = Header2.XResol;
		Header.YResol = Header2.YResol;
		Header.EUnits = Header2.EUnits;
		Header.SUnits = Header2.SUnits;
		Header.AUnits = Header2.AUnits;
		Header.FOptions = Header2.FOptions;
		Header.CUnits = Header2.CUnits;
		Header.HUnits = Header2.HUnits;
		Header.BUnits = Header2.BUnits;
		Header.PUnits = Header2.PUnits;
		// stuff not found in Version 2.0
		Header.DUnits = 0;
		Header.WOptions = 0;
		Header.GroundFuels = 20;	// none
		memset(Header.ElevFile, 0x0, sizeof(Header.ElevFile));
		memset(Header.SlopeFile, 0x0, sizeof(Header.ElevFile));
		memset(Header.AspectFile, 0x0, sizeof(Header.ElevFile));
		memset(Header.FuelFile, 0x0, sizeof(Header.ElevFile));
		memset(Header.CoverFile, 0x0, sizeof(Header.ElevFile));
		memset(Header.HeightFile, 0x0, sizeof(Header.ElevFile));
		memset(Header.BaseFile, 0x0, sizeof(Header.ElevFile));
		memset(Header.DensityFile, 0x0, sizeof(Header.ElevFile));
		memset(Header.DuffFile, 0x0, sizeof(Header.ElevFile));
		memset(Header.WoodyFile, 0x0, sizeof(Header.ElevFile));
		memset(Header.Description, 0x0, sizeof(Header.Description));

		landfile = fopen(LandFName, "wb");
		if (landfile == NULL)
		{
			//SetFileAttributes(LandFName, FILE_ATTRIBUTE_NORMAL);
			chmod(LandFName,S_IRWXO);
			landfile = fopen(LandFName, "wb");
		}
		fwrite(&Header, sizeof(headdata), 1, landfile);

		for (i = 0; i < Header.numnorth; i++)
		{
			for (j = 0; j < Header.numeast; j++)
			{
				fread(&cell, sizeof(celldata), 1, oldlandfile);
				fwrite(&cell, sizeof(celldata), 1, landfile);
				if (Header.CrownFuels == 21)
				{
					fread(&cfuel, sizeof(crowndata), 1, oldlandfile);
					fwrite(&cfuel, sizeof(crowndata), 1, landfile);
				}
			}
		}
		fclose(oldlandfile);
		CloseLandFile();

		strcpy(LandFName, BkupLandFName);
		if (!OpenLandFile())
			return false;

		ReadHeader();
		lcptheme->AnalyzeStats();
		Header.numelev = lcptheme->NumAllCats[0];
		Header.numslope = lcptheme->NumAllCats[1];
		Header.numaspect = lcptheme->NumAllCats[2];
		Header.numfuel = lcptheme->NumAllCats[3];
		Header.numcover = lcptheme->NumAllCats[4];
		Header.numheight = lcptheme->NumAllCats[5];
		Header.numbase = lcptheme->NumAllCats[6];
		Header.numdensity = lcptheme->NumAllCats[7];
		Header.numduff = 0;//lcptheme->NumAllCats[8];
		Header.numwoody = 0;//lcptheme->NumAllCats[9];
		memcpy(Header.elevs, lcptheme->AllCats[0], 100 * sizeof(long));
		memcpy(Header.slopes, lcptheme->AllCats[1], 100 * sizeof(long));
		memcpy(Header.aspects, lcptheme->AllCats[2], 100 * sizeof(long));
		memcpy(Header.fuels, lcptheme->AllCats[3], 100 * sizeof(long));
		memcpy(Header.covers, lcptheme->AllCats[4], 100 * sizeof(long));
		memcpy(Header.heights, lcptheme->AllCats[5], 100 * sizeof(long));
		memcpy(Header.bases, lcptheme->AllCats[6], 100 * sizeof(long));
		memcpy(Header.densities, lcptheme->AllCats[7], 100 * sizeof(long));
//		ZeroMemory(Header.duffs, 100 * sizeof(long));
		memset(Header.duffs,0x0, 100 * sizeof(long));
//		ZeroMemory(Header.woodies, 100 * sizeof(long));
		memset(Header.woodies,0x0, 100 * sizeof(long));
		//memcpy(Header.duffs, lcptheme->AllCats[8], 100*sizeof(long));
		//memcpy(Header.woodies, lcptheme->AllCats[9], 100*sizeof(long));
		Header.loslope = (long)lcptheme->minval[1];
		Header.hislope = (long)lcptheme->maxval[1];
		Header.loaspect = (long)lcptheme->minval[2];
		Header.hiaspect = (long)lcptheme->maxval[2];
		Header.lofuel = (long)lcptheme->minval[3];
		Header.hifuel = (long)lcptheme->maxval[3];
		Header.locover = (long)lcptheme->minval[4];
		Header.hicover = (long)lcptheme->maxval[4];
		Header.loheight = (long)lcptheme->minval[5];
		Header.hiheight = (long)lcptheme->maxval[5];
		Header.lobase = (long)lcptheme->minval[6];
		Header.hibase = (long)lcptheme->maxval[6];
		Header.lodensity = (long)lcptheme->minval[7];
		Header.hidensity = (long)lcptheme->maxval[7];
		Header.loduff = 0;//lcptheme->minval[8];
		Header.hiduff = 0;//lcptheme->maxval[8];
		Header.lowoody = 0;//lcptheme->minval[9];
		Header.hiwoody = 0;//lcptheme->maxval[9];
		CloseLandFile();

		strcpy(LandFName, BkupLandFName);
		landfile = fopen(LandFName, "rb+");
		fwrite(&Header, sizeof(headdata), 1, landfile);
		fclose(landfile);
		OpenLandFile();

		return true;
	}
	else
		return false;
}


char* GetLandFileName()
{
	return LandFName;
}

void SetLandFileName(const char* FileName)
{
	memset(LandFName, 0x0, sizeof(LandFName));
	sprintf(LandFName, "%s", FileName);
}



//-------------------------------------------------------------------------
//	Palette Functions
//-------------------------------------------------------------------------
char StoredColorFile[256] = "";

char* GetStoredColorFile()
{
	return StoredColorFile;
}

void GetStoredColorFile(char* Filename)
{
	strcpy(StoredColorFile, Filename);
}


/*
bool InitializePalette()
{
	ValidPalette=false;
//	return false;

	TScreenDC screenDC;

	if(screenDC.GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
	{    BYTE Red, Green, Blue, n=0;
		BYTE cr, cg, cb;

		FarPal=(LOGPALETTE*) new char[sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)* 216];
		FarPal->palVersion=0x300;
		FarPal->palNumEntries=216;
		for(Red=0; Red<6; Red++)
		{    cr=(BYTE)(Red*51);//31+100;
			for(Green=0; Green<6; Green++)
			{    cg=(BYTE) (Green*51);//31+100;
				for(Blue=0; Blue<6; Blue++)
				{    cb=(BYTE) (Blue*51);//31+100;
					SetPaletteColor(n, cr, cg, cb);
					n++;
				}
			}
		}
		ValidPalette=true;
		hLogPal=CreatePalette(FarPal);
		hMemPal=CreatePalette(FarPal);

		return true;
	}
	hMemPal=0;
	hLogPal=0;
	ValidPalette=false;

	return false;
}


LOGPALETTE* GetPalette()
{
	if(FarPal)
	 	return FarPal;

	 return NULL;
}


void DestroyPalette()
{
	ValidPalette=false;
	if(FarPal)
	{	delete[] FarPal;
		  FarPal=0;
	 }
	if(hLogPal)
		DeleteObject(hLogPal);
	if(hMemPal)
		DeleteObject(hMemPal);
}


bool AccessHDCPalette(HDC hDC, int ONOFF)
{
	if(ValidPalette)
	{    if(ONOFF>0)
		{    switch(ONOFF)
			{	case 1:
					PrevPalette1=SelectPalette(hDC, hLogPal, 0);
					RealizePalette(hDC);
					break;
				case 2:
					PrevPalette2=SelectPalette(hDC, hMemPal, 0);
					break;
			}
		}
		else
		{    switch(ONOFF)
			{	case -1:  SelectPalette(hDC, PrevPalette1, 0);
						break;
				case -2:  RealizePalette(hDC);
						SelectPalette(hDC, PrevPalette2, 0);
						break;
			}
		}

		return true;
	}

	return false;
}


bool AccessTDCPalette(TDC& hDC, int ONOFF)
{
	if(ValidPalette)
	{	if(ONOFF==1)
		{	TPalette Palette(FarPal);
			hDC.SelectObject(Palette, false);
			hDC.RealizePalette();
		}
		else
			hDC.RestorePalette();

		return true;
	}

	return false;
}


bool HavePalette()
{
	return ValidPalette;
}


void SetPaletteColor(int Entry, BYTE Red, BYTE Green, BYTE Blue)
{
	FarPal->palPalEntry[Entry].peRed=Red;
	FarPal->palPalEntry[Entry].peGreen=Green;
	FarPal->palPalEntry[Entry].peBlue=Blue;
	FarPal->palPalEntry[Entry].peFlags=PC_RESERVED;
}


COLORREF GetPaletteColor(int Red, int Green, int Blue)
{
	if(ValidPalette)
		return PALETTERGB(Red, Green, Blue);
	else
		return RGB(Red, Green, Blue);
}
*/

/*
ThemeColors::ThemeColors()
{
	long i, j;
	ENum=100;
	SNum=18;
	if(Header.AUnits==0)
		ANum=25;
	else
		ANum=120;
	CNum=20;
	HNum=25;
	BNum=25;
	DNum=30;

	 Elevation=new COLORREF[ENum];
	 Slope=new COLORREF[SNum];
	 Aspect=new COLORREF[ANum];
	 Cover=new COLORREF[CNum];
	 Height=new COLORREF[HNum];
	 Base=new COLORREF[BNum];
	 Density=new COLORREF[DNum];

	MinVal=0;
	for(i=0; i<7; i++)
	{	switch(i)
		{	case 0:
				MaxVal=ENum;
				IntValue=(MaxVal-MinVal)/350.0;
				if(IntValue==0)
					IntValue=1.0;
				for(j=0; j<MaxVal; j++)
					Elevation[j]=ColorFromSpectrum(j+1);
				break;
			case 1:
				MaxVal=SNum;
				IntValue=(MaxVal-MinVal)/350.0;
				if(IntValue==0)
					IntValue=1.0;
				for(j=0; j<MaxVal; j++)
					Slope[j]=ColorFromSpectrum(j+1);
				break;
			case 2:
				MaxVal=ANum;
				IntValue=(MaxVal-MinVal)/350.0;
				if(IntValue==0)
					IntValue=1.0;
				for(j=0; j<MaxVal; j++)
					Aspect[j]=ColorFromSpectrum(j+1);
				break;
			case 3:
				MaxVal=CNum;
				IntValue=(MaxVal-MinVal)/350.0;
				if(IntValue==0)
					IntValue=1.0;
				for(j=0; j<MaxVal; j++)
					Cover[j]=ColorFromSpectrum(j+1);
				break;
			case 4:
				MaxVal=HNum;
				IntValue=(MaxVal-MinVal)/350.0;
				if(IntValue==0)
					IntValue=1.0;
				for(j=0; j<MaxVal; j++)
					Height[j]=ColorFromSpectrum(j+1);
				break;
			case 5:
				MaxVal=BNum;
				IntValue=(MaxVal-MinVal)/350.0;
				if(IntValue==0)
					IntValue=1.0;
				for(j=0; j<MaxVal; j++)
					Base[j]=ColorFromSpectrum(j+1);
				break;
			case 6:
				MaxVal=DNum;
				IntValue=(MaxVal-MinVal)/350.0;
				if(IntValue==0)
					IntValue=1.0;
				for(j=0; j<MaxVal; j++)
					Density[j]=ColorFromSpectrum(j+1);
				break;

		}
	}
}


COLORREF ThemeColors::ColorFromSpectrum(long ColorVal)
{
	COLORREF Colr=RGB(255,255,255);
	BYTE Red, Green, Blue;
	double RelValue, ColrCat, SubColr;

	if(ColorVal>=0.0)
	{    RelValue=(ColorVal-MinVal)/IntValue;
		SubColr=modf(RelValue/50.0, &ColrCat);
		  if(ColrCat>6)
		  {	ColrCat=6;
		  	SubColr=49.0/50.0;
		  }
		switch(long(ColrCat))
		{	case 0:
				Red=1; Green=1; Blue=1;
				break;
			case 1:
				Red=1; Green=0; Blue=1;
				break;
			case 2:
				Red=0; Green=0; Blue=1;
				break;
			case 3:
				Red=0; Green=1; Blue=1;
				break;
			case 4:
				Red=0; Green=1; Blue=0;
				break;
			case 5:
				Red=1; Green=1; Blue=0;
				break;
			case 6:
				Red=1; Green=0; Blue=0;
				break;
		}
		SubColr*=155;			// SubColr*=255
		SubColr+=100;   		 // increase between 100 and 255
		Colr=GetPaletteColor(Red*SubColr, Green*SubColr, Blue*SubColr);
	}

	return Colr;
}


ThemeColors::~ThemeColors()
{
	delete[] Elevation;
	 delete[] Slope;
	 delete[] Aspect;
	 delete[] Cover;
	 delete[] Height;
	 delete[] Base;
	 delete[] Density;
}


COLORREF GetThemeColor(long VisTheme, long Attrib)
{
	COLORREF colr;

	if(VisTheme<0)
		VisTheme=VisibleThemeNumber;

	switch(VisTheme)
	{	case 0: Attrib=(long) ((((double)(Attrib-Header.loelev))/
					((double)(Header.hielev-Header.loelev)))*100);
			   if(Attrib>99)
				  	Attrib=99;
				  if(Attrib<0)
				  	Attrib=0;
			   colr=themecolors.Elevation[Attrib]; break;
		case 1: if(Header.SUnits>0)   // percent
				Attrib/=7;
			   else 	   		// degrees
				Attrib/=6;
			   if(Attrib>17) Attrib=17;
			   if(Attrib<0)
				Attrib=0;
			   colr=themecolors.Slope[Attrib]; break;
		case 2: if(Header.AUnits>0)
			   {	Attrib=(long) ((double) Attrib)/14.4;
				   	if(Attrib>119)
						Attrib=119;
				  }
		  	   else if(Attrib>24)
				  	Attrib=24;
			   if(Attrib<0)
				Attrib=0;
			   colr=themecolors.Aspect[Attrib]; break;
		case 3: if(Attrib<-2)
				Attrib=0;
			   colr=GetFuelColor(Attrib);
			   break;
		case 4: if(Header.CUnits>0)    // not categories
				Attrib/=5;
			   if(Attrib>19)
				Attrib=19;
			   if(Attrib<0)
				Attrib=0;
			   colr=themecolors.Cover[Attrib]; break;
		case 5: //if(Header.HUnits>1)
			   //	Attrib/=8;
			   //else
				Attrib/=30;
			   if(Attrib>24)
				Attrib=24;
			   if(Attrib<0)
				Attrib=0;
			   colr=themecolors.Height[Attrib]; break;
		case 6: //if(Header.BUnits>1)
			   //	Attrib/=8;
			   //else
				Attrib/=30;
			   if(Attrib>24)
				Attrib=24;
			   if(Attrib<0)
				Attrib=0;
			   colr=themecolors.Base[Attrib]; break;
		case 7: //if(Header.PUnits==2)
			   //	Attrib/=16.018;
			   //Attrib/=100;
			   Attrib-=5;
			   if(Attrib>29)
				Attrib=29;
			   if(Attrib<0)
				Attrib=0;
			   colr=themecolors.Density[Attrib]; break;
	}

	return colr;
}


void SetThemeColor(long VisTheme, long Attrib, COLORREF colr)
{
	if(VisTheme<0)
		VisTheme=VisibleThemeNumber;
	switch(VisTheme)
	{	case 0: //Attrib=(long) (((double)(Attrib-Header.loelev))/
			   //		((double)(Header.hielev-Header.loelev)))*25;
				  if(Attrib>99)
				  	Attrib=99;
			   themecolors.Elevation[Attrib]=colr; break;
		case 1: if(Header.SUnits>0)   // percent
				Attrib/=7;
			   else 	   		// degrees
				Attrib/=6;
			   if(Attrib>17) Attrib=17;
			   themecolors.Slope[Attrib]=colr; break;
		case 2: if(Header.AUnits>0)
			   {	Attrib=(long) ((double) Attrib)/14.4;
					if(Attrib>119)
						Attrib=119;
				  }
				  else if(Attrib>24)
				  	Attrib=24;
				  if(Attrib<0)
				  	Attrib=0;
			   themecolors.Aspect[Attrib]=colr; break;
		case 3: SetFuelColor(Attrib, colr); break;
		case 4: if(Header.CUnits>0)    // not categories
				Attrib/=5;
			   if(Attrib>19)
				Attrib=19;
			   themecolors.Cover[Attrib]=colr; break;
		case 5: //if(Header.HUnits>1)
			   //	Attrib/=8;
			   //else
				Attrib/=3;
			   if(Attrib>24)
				Attrib=24;
			   themecolors.Height[Attrib]=colr; break;
		case 6: //if(Header.BUnits>1)
			   //	Attrib/=8;
			   //else
				Attrib/=3;
			   if(Attrib>24)
				Attrib=24;
			   themecolors.Base[Attrib]=colr; break;
		case 7: //if(Header.PUnits==2)
			   //	Attrib/=16.018;
			   Attrib/=100;
			   Attrib-=5;
			   if(Attrib>29)
				Attrib=29;
			   themecolors.Density[Attrib]=colr; break;
	}
}

*/

//-------------------------------------------------------------------------------
//		 Access to Fire Display Functions
//-------------------------------------------------------------------------------

double MinFireChx(double Type)
{
	// max display fire chx
	if (Type >= 0.0)
		FireMinChx = Type;

	return FireMinChx;
}


double MaxFireChx(double Type)
{
	// max display fire chx
	if (Type >= 0.0)
		FireMaxChx = Type;

	return FireMaxChx;
}


long NumFireInterval(long Num)
{
	// num of fire intervals displayed
	if (Num > 0)
		FireNumInterval = Num;

	return FireNumInterval;
}


long OutputFireParam(long Param)
{
	// select fire chx for display
	if (Param >= 0)
		FireOutputParam = Param;

	return FireOutputParam;
}


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//
//	Atmosphere Grid Global Functions
//
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------


bool SetAtmosphereGrid(long NumGrids)
{
	if (NumGrids == 0)
	{
		/*
		if(AtmGrid->AtmGridWTR)
		{	NumWeatherStations=0;    // reset number of weather stations
			NumWindStations=0;  	 // reset number of wind stations
		}
		else if(AtmGrid->AtmGridWND)
			NumWindStations=0;  	 // reset number of wind stations
		*/
		if (AtmGrid)
			delete AtmGrid;
		AtmGrid = 0;
		NorthGridOffset = 0.0;
		EastGridOffset = 0.0;

		return false;
	}
	if (AtmGrid == 0)
		AtmGrid = new AtmosphereGrid(NumGrids);
	else
	{
		delete AtmGrid;
		AtmGrid = new AtmosphereGrid(NumGrids);
	}

	return true;
}


AtmosphereGrid* GetAtmosphereGrid()
{
	return AtmGrid;
}

long AtmosphereGridExists()
{
	if (AtmGrid)
	{
		if (AtmGrid->AtmGridWTR)
			return 2;
		else if (AtmGrid->AtmGridWND)
			return 1;
	}

	return 0;
}


//-------------------------------------------------------------------------------
//	BurnPeriod Data and Functions
//-------------------------------------------------------------------------------

static double DownTime = 0.0;
static long NumAbsoluteData = 0;
static long NumRelativeData = 0;
static long LastAccess = -1;
static AbsoluteBurnPeriod* abp = 0;
static RelativeBurnPeriod* rbp = 0;


bool AllocBurnPeriod(long Num)
{
	FreeBurnPeriod();

	//long i, FirstDay, StartDay, EndDay, NumDays, LastDay;
	long i,FirstDay, NumDays, LastDay;
	//long mo, dy, JDay, maxdy;
	long mo, dy, JDay;

	FirstDay = GetJulianDays(GetMinMonth());
	FirstDay += GetMinDay();
	LastDay = GetJulianDays(GetMaxMonth());
	LastDay += GetMaxDay();
	if (FirstDay < LastDay)
		NumDays = LastDay - FirstDay;
	else
		NumDays = 365 - FirstDay + LastDay;
	if (Num < NumDays)
		Num = NumDays;

	abp = new AbsoluteBurnPeriod[Num];
	if (abp == NULL)
		return false;
	memset(abp, 0x0, Num * sizeof(AbsoluteBurnPeriod));
	NumAbsoluteData = Num;

	mo = GetMinMonth();
	dy = GetMinDay();
	for (i = 0; i < NumDays; i++)
	{
		abp[i].Month = mo;
		abp[i].Day = dy;
		abp[i].Start = 0;
		abp[i].End = 2400;
		dy++;
		JDay = dy + GetJulianDays(mo);
		if (mo < 12)
		{
			if (JDay > GetJulianDays(mo + 1))
			{
				dy = 1;
				mo++;
			}
		}
		else
		{
			if (JDay > 365)
			{
				dy = 1;
				mo = 1;
			}
		}
	}

	return true;
}


void FreeBurnPeriod()
{
	NumRelativeData = 0;
	NumAbsoluteData = 0;
	if (abp)
		delete[] abp;	//free(abp);
	abp = 0;
	if (rbp)
		delete[] rbp;	//free(rbp);
	rbp = 0;
	DownTime = 0.0;
	LastAccess = -1;
}


void SetBurnPeriod(long num, long mo, long dy, long start, long end)
{
	if (abp)
	{
		for (i = 0; i < NumAbsoluteData; i++)
		{
			num = i;
			if (abp[i].Month == mo && abp[i].Day == dy)
				break;
		}
		abp[num].Month = mo;
		abp[num].Day = dy;
		abp[num].Start = start;
		abp[num].End = end;
	}
}


bool GetBurnPeriod(long num, long* mo, long* dy, long* start, long* end)
{
	if (abp)
	{
		*mo = abp[num].Month;
		*dy = abp[num].Day;
		*start = abp[num].Start;
		*end = abp[num].End;
	}
	else
	{
		*mo = *dy = *start = *end = 0;

		return false;
	}

	return true;
}


long GetNumBurnPeriods()
{
	return NumAbsoluteData;
}


bool InquireInBurnPeriod(double SimTime)
{
	if (!rbp)
		return true;

	for (i = LastAccess + 1; i < NumRelativeData; i++)
	{
		if (SimTime >= rbp[i].Start)
		{
			if (SimTime <= rbp[i].End)
			{
				LastAccess = i - 1;

				return true;
			}
		}
		else
			break;
	}

	return false;
}

long GetJulianDays(long Month)
{
	long days;
	switch (Month)
	{
	case 1:
		days = 0; break;			// cumulative days to begin of month
	case 2:
		days = 31; break;   		// except ignores leapyear, but who cares anyway,
	case 3:
		days = 59; break;
	case 4:
		days = 90; break;
	case 5:
		days = 120; break;
	case 6:
		days = 151; break;
	case 7:
		days = 181; break;
	case 8:
		days = 212; break;
	case 9:
		days = 243; break;
	case 10:
		days = 273; break;
	case 11:
		days = 304; break;
	case 12:
		days = 334; break;
	default:
		days = 0; break;
	}

	return days;
}


/**
 * \brief Converts a time expressed in "component form" (ie: month, day, hour,
 *   minutes) to the same time expressed in minutes from the beginning of 
 *   the simulation.  
 * 
 * This algorithm assumes that the arguments have the following units: 
 * 
 * \arg mo "month of year" number (1-12)
 * \arg dy "day of month" number (1-31)
 * \arg hr "hour of day times 100" number (0000 - 2400)
 * \arg min "minute of hour" number (0-59)
 * 
 * This algorithm works by converting the simulation start time to "minutes
 * since the start of the year" for a reference.  Then the provided time is 
 * also converted to "minutes since the start of the year."  The "simulation
 * time" is then computed by a simple subtraction of the start time from the
 * provided time.  
 * 
 * Because there is no provision for specifying the year of any of the 
 * times, certain assumptions must be made.  For instance, if the "month" 
 * of the provided time is less than the month of the simulation start, 
 * the algorithm assumes that the provided time refers to a time in the 
 * next year. 
 */
double ConvertActualTimeToSimtime(long mo, long dy, long hr, long mn,
	bool FromCondit)
{
	double SimTime, RefStart;

	if (!FromCondit)
	{
		if (GetStartDate() == 0)
		{
			SimTime = -1.0;
			return SimTime;
		}
		RefStart = (GetJulianDays(startmonth) + startday) * 1440.0 +
			starthour /
			100.0 * 60.0 +
			startmin;
	}
	else
	{
		if (conditmonth == 0 && conditday == 0)
		{
			SimTime = -1.0;
			return SimTime;
		}
		RefStart = (GetJulianDays(conditmonth) + conditday) * 1440.0;
	}

	if (!FromCondit)
	{
		if (mo >= startmonth)
			SimTime = (GetJulianDays(mo) + dy) * 1440.0 +
				hr /
				100.0 * 60.0 +
				mn;
		else
			SimTime = (GetJulianDays(mo) + dy + 365.0) * 1440.0 +
				hr /
				100.0 * 60.0 +
				mn;
	}
	else
	{
		if (mo >= conditmonth)
			SimTime = (GetJulianDays(mo) + dy) * 1440.0 +
				hr /
				100.0 * 60.0 +
				mn;
		else
			SimTime = (GetJulianDays(mo) + dy + 365.0) * 1440.0 +
				hr /
				100.0 * 60.0 +
				mn;
	}
	SimTime -= RefStart;

	return SimTime;
}


void ConvertSimtimeToActualTime(double SimTime, long* mo, long* dy, long* hr,
	long* mn, bool FromCondit)
{
	long months, days, hours, mins, ndays;
	double fday, fhour;

	fday = (SimTime / 1440.0);	// days from minutes
	days = (long) fday;
	fhour = ((fday - days) * 24.0);  // hours from days
	hours = (long) fhour;
	mins = (long)((fhour - hours) * 60);	// minutes from hours

	if (!FromCondit)
	{
		hours += GetStartHour() / 100;
		mins += GetStartMin();
	}
	if (mins > 60)
	{
		mins -= 60;
		hours++;
	}
	if (hours >= 24)
	{
		hours -= 24;
		days++;
	}
	hours *= 100;
	if (!FromCondit)
	{
		if (GetStartDate() + days > 365)
		{
			days = GetStartDate() + days - 365;
			for (i = 1; i < 12; i++)
			{
				if (days < GetJulianDays(i))
					break;
			}
			days -= GetJulianDays(i - 1);
			months = i - 1;
		}
		else
		{
			for (i = 1; i < 12; i++)
			{
				if (GetStartMonth() + i > 12)
					ndays = 365 + GetJulianDays(i);
				else
					ndays = GetJulianDays(GetStartMonth() + i);
				if (days + GetStartDate() <= ndays)
					break;
			}
			if (GetStartMonth() + i - 1 > 12)
				ndays = 365 + GetJulianDays(i - 1);
			else
				ndays = GetJulianDays(GetStartMonth() + i - 1);
			days -= (ndays - GetStartDate());//(GetJulianDays(GetStartMonth()+i-1)-GetStartDate());
			months = GetStartMonth() + (i - 1);
			if (months > 12)
				months -= 12;
		}
	}
	else
	{
		long ConDate = GetJulianDays(conditmonth) + conditday;

		if (ConDate + days > 365)
		{
			days = ConDate + days - 365;
			for (i = 1; i < 12; i++)
			{
				if (days < GetJulianDays(i))
					break;
			}
			days -= GetJulianDays(i - 1);
			months = i - 1;
		}
		else
		{
			for (i = 1; i < 12; i++)
			{
				if (conditmonth + i > 12)
					ndays = 365 + GetJulianDays(i);
				else
					ndays = GetJulianDays(conditmonth + i);
				if (days + ConDate < ndays)//GetJulianDays(conditmonth+i))
					break;
			}
			if (conditmonth + i > 12)
				ndays = 365 + GetJulianDays(i - 1);
			else
				ndays = GetJulianDays(conditmonth + i - 1);
			days -= (ndays - ConDate);//(GetJulianDays(conditmonth+i-1)-ConDate);
			months = conditmonth + (i - 1);
			if (months > 12)
				months -= 12;
		}
	}

	*mo = months;
	*dy = days;
	*hr = hours;
	*mn = mins;
}


bool CheckBurnPeriod(long mo1, long dy1, long mo2, long dy2)
{
	if (abp == 0)
		return true;

	long BPStart, BPEnd;
	long DurStart, DurEnd;

	BPStart = GetJulianDays(abp[0].Month);
	BPStart += abp[0].Day;
	BPEnd = GetJulianDays(abp[NumAbsoluteData - 1].Month);
	BPEnd += abp[NumAbsoluteData - 1].Day;

	DurStart = GetJulianDays(mo1);//(startmonth);   // comments remove references to stored start and end times
	DurStart += dy1;//startday;
	DurEnd = GetJulianDays(mo2);//(endmonth);
	DurEnd += dy2;//endday;

	if (DurEnd < DurStart)	// spans new year
	{
		if (DurStart < BPStart)
			return false;
		if (DurEnd > BPEnd)
			return false;
	}
	else
	{
		if (DurStart < BPStart)
			return false;
		if (DurEnd > BPEnd)
			return false;
	}

	return true;
}


void ConvertAbsoluteToRelativeBurnPeriod()
{
	long i, j, days, Day, Month, NumAlloc;
	double Add, Begin, End, Start, hrs, mins;
	RelativeBurnPeriod* trbp;

	if (abp == 0)
		return;
	if (rbp)
		delete[] rbp;//GlobalFree(rbp);//free(rbp);

	Start = GetJulianDays(startmonth);
	Start += startday - 1;
	End = GetJulianDays(endmonth);
	End += (long)endday;
	if (End < Start)
		NumRelativeData = (long)(End + 365 - Start);
	else
		NumRelativeData = (long)(End - Start);

	NumAlloc = NumAbsoluteData;
	if (NumRelativeData > NumAbsoluteData)
		NumAlloc = NumRelativeData;

	rbp = new RelativeBurnPeriod[NumAlloc];
	memset(rbp, 0x0, NumAlloc * sizeof(RelativeBurnPeriod));
	trbp = new RelativeBurnPeriod[NumAlloc];
	memset(trbp, 0x0, NumAlloc * sizeof(RelativeBurnPeriod));

	if (NumRelativeData > NumAbsoluteData)
	{
		AbsoluteBurnPeriod* tabp = new AbsoluteBurnPeriod[NumRelativeData];
		memcpy(tabp, abp, NumAbsoluteData * sizeof(AbsoluteBurnPeriod));
		delete[] abp;
		abp = new AbsoluteBurnPeriod[NumRelativeData];
		memcpy(abp, tabp, NumAbsoluteData * sizeof(AbsoluteBurnPeriod));
		delete[] tabp;

		Month = abp[NumAbsoluteData - 1].Month;
		Day = abp[NumAbsoluteData - 1].Day;
		for (i = NumAbsoluteData; i < NumRelativeData; i++)
		{
			Day++;
			if (Month < 12)
			{
				if (Day > GetJulianDays(Month + 1) - GetJulianDays(Month))
				{
					Day = 1;
					Month++;
				}
			}
			else
			{
				if (Day > 31)
				{
					Day = 1;
					Month = 1;
				}
			}
			abp[i].Month = Month;
			abp[i].Day = Day;
			abp[i].Start = 0;
			abp[i].End = 2400;
		}
		NumAbsoluteData = NumRelativeData;
	}

	rbp[0].Start = 0.0;//starthour*0.6;
	rbp[0].End = 1440 - starthour * 0.6;
	for (i = 1;
		i < NumRelativeData;
		i++)   		 // initialize all data to minutes
	{
		rbp[i].Start = rbp[i - 1].End;
		rbp[i].End = 1440 * (i + 1) - starthour * 0.6;
	}

	days = GetJulianDays(abp[0].Month);
	days += abp[0].Day - 1;
	End = Start = days * 1440;
	mins = modf(abp[0].Start / 100.0, &hrs);
	Start += (hrs * 60.0 + mins);
	mins = modf(abp[0].End / 100.0, &hrs);
	End += (hrs * 60.0 + mins);
	End -= Start;

	trbp[0].Start = 0;
	trbp[0].End = End;

	for (i = 1; i < NumAbsoluteData; i++)
	{
		days = GetJulianDays(abp[i].Month);
		if (abp[i].Month < abp[0].Month)
			days += 365;
		days += abp[i].Day - 1;
		End = Begin = days * 1440;
		mins = modf(abp[i].Start / 100.0, &hrs);
		Begin += (hrs * 60.0 + mins);
		mins = modf(abp[i].End / 100.0, &hrs);
		End += (hrs * 60.0 + mins);
		Begin -= Start;
		End -= Start;

		trbp[i].Start = Begin;
		trbp[i].End = End;
	}

	days = GetJulianDays(startmonth);
	days += startday - 1;
	if (startmonth < abp[0].Month)
		days += 365;
	Begin = days * 1440;
	mins = modf(starthour / 100.0, &hrs);
	Begin += (hrs * 60.0 + mins);   		 // beginning of simulation time

	if (trbp[NumAbsoluteData - 1].End + Start < Begin)	  // if after new year
		Add = Begin - (Start + (365 * 1440)); // start of data af
	else
		Add = Start - Begin;   // start of data earlier than begin of simulation

	for (i = 0; i < NumAbsoluteData; i++)
	{
		trbp[i].Start += Add;
		trbp[i].End += Add;
	}

	long FirstDay, StartDay, EndDay, NumDays;// LastDay;

	FirstDay = GetJulianDays(GetMinMonth());
	FirstDay += GetMinDay();
	StartDay = GetJulianDays(GetStartMonth());
	StartDay += GetStartDay();
	EndDay = GetJulianDays(GetEndMonth());
	EndDay += GetEndDay();
	if (StartDay < FirstDay)
		StartDay += 365;
	if (EndDay < FirstDay)
		EndDay += 365;

	NumDays = EndDay - StartDay;

	for (i = 0; i <= NumDays; i++)
	{
		j = i + StartDay - FirstDay;
		if (i < NumAlloc && j < NumAlloc)
			rbp[i] = trbp[j];
	}

	/*
	for(i=0; i<NumAbsoluteData; i++)
	{	for(j=0; j<NumRelativeData; j++)
		{    if(trbp[i].Start>=rbp[j].Start && trbp[i].End<=rbp[j].End)
		 		rbp[j]=trbp[i];
		 }
	}
	*/

	LastAccess = -1;
	//   DownTime=0.0;
	delete[] trbp;//GlobalFree(trbp);//free(trbp);
}

void AddDownTime(double time)
{
	if (time < 0)
		DownTime = 0.0;
	else
		DownTime += time;
}

double GetDownTime()
{
	return DownTime;
}


//-------------------------------------------------------------------------------
// Fuel Moisture Data and Functions
//-------------------------------------------------------------------------------

static InitialFuelMoisture fm[257];

bool SetInitialFuelMoistures(long Model, long t1, long t10, long t100,
	long tlh, long tlw)
{
	if (Model > 256 || Model < 1)
		return false;

	fm[Model].TL1 = t1;
	fm[Model].TL10 = t10;
	fm[Model].TL100 = t100;
	fm[Model].TLLH = tlh;
	fm[Model].TLLW = tlw;

	if (t1 > 1 && t10 > 1)
		fm[Model].FuelMoistureIsHere = true;
	else
		fm[Model].FuelMoistureIsHere = false;

	long i, j;
	for (i = 0; i < 3; i++) 	  // only up to 100 hr fuels [3]
	{
		for (j = 0; j < 5; j++)
			EnvtChanged[i][j] = true;
	}

	return fm[Model].FuelMoistureIsHere;
}

bool GetInitialFuelMoistures(long Model, long* t1, long* t10, long* t100,
	long* tlh, long* tlw)
{
	if (Model > 256 || Model < 1)
		return false;

	if (fm[Model].FuelMoistureIsHere)
	{
		*t1 = fm[Model].TL1;
		*t10 = fm[Model].TL10;
		*t100 = fm[Model].TL100;
		*tlh = fm[Model].TLLH;
		*tlw = fm[Model].TLLW;

		return true;
	}

	return false;
}

long GetInitialFuelMoisture(long Model, long FuelClass)
{
	if (Model > 256 || Model < 1)
		return 2;

	long mx;

	switch (FuelClass)
	{
	case 0:
		mx = fm[Model].TL1; break;
	case 1:
		mx = fm[Model].TL10; break;
	case 2:
		mx = fm[Model].TL100; break;
	case 3:
		mx = fm[Model].TLLH; break;
	case 4:
		mx = fm[Model].TLLW; break;
	}

	return mx;
}

bool InitialFuelMoistureIsHere(long Model)
{
	if (Model > 256 || Model < 1)
		return false;

	return fm[Model].FuelMoistureIsHere;
}


//-------------------------------------------------------------------------------
//	Coarse Woody Data and Functions
//-------------------------------------------------------------------------------

CoarseWoody coarsewoody[MAXNUM_COARSEWOODY_MODELS];
static double WeightLossErrorTol = 1.0;	// Mg/ha
CoarseWoody NFFLWoody[256];
CoarseWoody tempwoody;
static long CombineOption = CWD_COMBINE_ABSENT;


CoarseWoody::CoarseWoody()
{
	wd = 0;
	NumClasses = 0;
	Units = -1;
	TotalWeight = 0.0;
	Depth = 0.0;
}


bool AllocCoarseWoody(long ModelNumber, long NumClasses)
{
	if (coarsewoody[ModelNumber - 1].wd)
		FreeCoarseWoody(ModelNumber);
	if ((coarsewoody[ModelNumber - 1].wd = new WoodyData[NumClasses]) == NULL) //(WoodyData *) GlobalAlloc(GMEM_FIXED |  GMEM_ZEROINIT, NumClasses*sizeof(WoodyData)))==NULL)
		return false;
//	ZeroMemory(coarsewoody[ModelNumber - 1].wd, NumClasses * sizeof(WoodyData));
	memset(coarsewoody[ModelNumber - 1].wd,0x0, NumClasses * sizeof(WoodyData));
	coarsewoody[ModelNumber - 1].NumClasses = NumClasses;
	coarsewoody[ModelNumber - 1].TotalWeight = 0.0;
//	ZeroMemory(coarsewoody[ModelNumber - 1].Description, 64 * sizeof(char));
	memset(coarsewoody[ModelNumber - 1].Description,0x0, 64 * sizeof(char));

	return true;
}


void FreeCoarseWoody(long ModelNumber)
{
	if (coarsewoody[ModelNumber - 1].wd)
		delete[] coarsewoody[ModelNumber - 1].wd;//GlobalFree(coarsewoody[ModelNumber-1].wd);//free(coarsewoody[ModelNumber-1].wd);
	coarsewoody[ModelNumber - 1].NumClasses = 0;
	coarsewoody[ModelNumber - 1].wd = 0;
	coarsewoody[ModelNumber - 1].Units = -1;
	coarsewoody[ModelNumber - 1].TotalWeight = 0.0;
	coarsewoody[ModelNumber - 1].Depth = 0.0;
}

void FreeAllCoarseWoody()
{
	long i;

	for (i = 1; i < MAXNUM_COARSEWOODY_MODELS + 1; i++)
		FreeCoarseWoody(i);
	for (i = 0; i < 256; i++)
	{
		if (NFFLWoody[i].wd)
			delete[] NFFLWoody[i].wd;//GlobalFree(NFFLWoody[i].wd);//free(NFFLWoody[i].wd);
		NFFLWoody[i].NumClasses = 0;
		NFFLWoody[i].wd = 0;
	}
	if (tempwoody.wd)
		delete[] tempwoody.wd;//GlobalFree(tempwoody.wd);//free(tempwoody.wd);
}

bool SetWoodyData(long ModelNumber, long ClassNumber, WoodyData* wd,
	long units)
{
	if (ModelNumber > MAXNUM_COARSEWOODY_MODELS - 1)
		return false;

	if (coarsewoody[ModelNumber - 1].NumClasses <= ClassNumber)
		return false;

	//long i;
	double Total = 0.0;

	//coarsewoody[ModelNumber-1].TotalWeight=0.0;
	memcpy(&coarsewoody[ModelNumber - 1].wd[ClassNumber], wd,
		sizeof(WoodyData));
	coarsewoody[ModelNumber - 1].Units = units;
	//for(i=0; i<=ClassNumber; i++)
	Total = coarsewoody[ModelNumber - 1].TotalWeight + wd[0].Load;//coarsewoody[ModelNumber-1].wd[i];
	coarsewoody[ModelNumber - 1].TotalWeight = Total;
	SetNFFLWoody();
	EnvtChanged[3][0] = true;
	EnvtChanged[3][1] = true;
	EnvtChanged[3][2] = true;
	EnvtChanged[3][3] = true;
	EnvtChanged[3][4] = true;

	return true;
}

bool SetWoodyDataDepth(long ModelNumber, double depth, const char* Description)
{
	if (ModelNumber > MAXNUM_COARSEWOODY_MODELS - 1)
		return false;

	coarsewoody[ModelNumber - 1].Depth = depth;
	memcpy(coarsewoody[ModelNumber - 1].Description, Description,
		64 * sizeof(char));

	return true;
}


bool SetNFFLWoody()
{
	//long i, j, k;
	long i, k;

	if (NFFLWoody[0].wd)
		return true;

	for (i = 0; i < 256; i++)
	{
		if (NFFLWoody[i].wd)
			delete[] NFFLWoody[i].wd;
		NFFLWoody[i].NumClasses = 0;
		NFFLWoody[i].wd = 0;
	}

	for (i = 0; i < 256; i++)
	{
		if (newfuels[i + 1].number > 0)
		{
			NFFLWoody[i].NumClasses = 0;
			if (newfuels[i + 1].h1 > 0.0)
				NFFLWoody[i].NumClasses++;
			if (newfuels[i + 1].h10 > 0.0)
				NFFLWoody[i].NumClasses++;
			if (newfuels[i + 1].h100 > 0.0)
				NFFLWoody[i].NumClasses++;
			if (newfuels[i + 1].lh > 0.0)
				NFFLWoody[i].NumClasses++;
			if (newfuels[i + 1].lw > 0.0)
				NFFLWoody[i].NumClasses++;
			if ((NFFLWoody[i].wd = new WoodyData[NFFLWoody[i].NumClasses]) ==
				NULL)
				return false;
			//ZeroMemory(NFFLWoody[i].wd,NFFLWoody[i].NumClasses * sizeof(WoodyData));
                       	memset(NFFLWoody[i].wd,0x0,NFFLWoody[i].NumClasses * sizeof(WoodyData));
			k = 0;
			if (newfuels[i + 1].h1 > 0.0)
			{
				NFFLWoody[i].wd[k].Load = newfuels[i + 1].h1;
				NFFLWoody[i].wd[k++].Load = newfuels[i + 1].sav1;
			}
			if (newfuels[i + 1].h10 > 0.0)
			{
				NFFLWoody[i].wd[k].Load = newfuels[i + 1].h10;
				NFFLWoody[i].wd[k++].Load = 109.0;
			}
			if (newfuels[i + 1].h100 > 0.0)
			{
				NFFLWoody[i].wd[k].Load = newfuels[i + 1].h100;
				NFFLWoody[i].wd[k++].Load = 30.0;
			}
			if (newfuels[i + 1].lh > 0.0)
			{
				NFFLWoody[i].wd[k].Load = newfuels[i + 1].lh;
				NFFLWoody[i].wd[k++].Load = newfuels[i + 1].savlh;
			}
			if (newfuels[i + 1].lw > 0.0)
			{
				NFFLWoody[i].wd[k].Load = newfuels[i + 1].lw;
				NFFLWoody[i].wd[k++].Load = newfuels[i + 1].savlw;
			}
		}
		else
		{
			NFFLWoody[i].NumClasses = 0;
			NFFLWoody[i].Depth = 0.0;
			NFFLWoody[i].TotalWeight = 0.0;
			NFFLWoody[i].Units = 1;
			NFFLWoody[i].wd = 0;
		}
	}


	//	if((tempwoody.wd=(WoodyData *) GlobalAlloc(GMEM_FIXED |  GMEM_ZEROINIT, 20*sizeof(WoodyData)))==NULL)
	//     	return false;

	/*
	// fuel model 1
		NFFLWoody[i].NumClasses=3;
		 NFFLWoody[i].Depth=1.0;
		 NFFLWoody[i].Units=1;
	//     if((NFFLWoody[i].wd=(WoodyData *) calloc(NFFLWoody[i].NumClasses, sizeof(WoodyData)))==NULL)
		 if((NFFLWoody[i].wd=new WoodyData[NFFLWoody[i].NumClasses])==NULL)
		 	return false;
//		 ZeroMemory(NFFLWoody[i].wd, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 memset(NFFLWoody[i].wd,0x0, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 NFFLWoody[i].wd[0].SurfaceAreaToVolume=3500.0;
		 NFFLWoody[i].wd[0].Load=0.74;
		 NFFLWoody[i].wd[1].SurfaceAreaToVolume=109.0;
		 NFFLWoody[i].wd[1].Load=0.0;
		 NFFLWoody[i].wd[2].SurfaceAreaToVolume=30.0;
		 NFFLWoody[i++].wd[2].Load=0.0;
	// fuel model 2
		NFFLWoody[i].NumClasses=4;
		 NFFLWoody[i].Depth=1.0;
		 NFFLWoody[i].Units=1;
	//     if((NFFLWoody[1].wd=(WoodyData *) calloc(NFFLWoody[i].NumClasses, sizeof(WoodyData)))==NULL)
		 if((NFFLWoody[1].wd=new WoodyData[NFFLWoody[i].NumClasses])==NULL)
		 	return false;
	//     if((NFFLWoody[1].wd=(WoodyData *) GlobalAlloc(GMEM_FIXED |  GMEM_ZEROINIT, NFFLWoody[i].NumClasses*sizeof(WoodyData)))==NULL)
//		 ZeroMemory(NFFLWoody[1].wd, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 memset(NFFLWoody[1].wd,0x0, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 NFFLWoody[i].wd[0].SurfaceAreaToVolume=3000.0;
		 NFFLWoody[i].wd[0].Load=2.00;
		 NFFLWoody[i].wd[1].SurfaceAreaToVolume=109.0;
		 NFFLWoody[i].wd[1].Load=1.00;
		 NFFLWoody[i].wd[2].SurfaceAreaToVolume=30.0;
		 NFFLWoody[i].wd[2].Load=0.50;
		 NFFLWoody[i].wd[3].SurfaceAreaToVolume=1500.0;
		 NFFLWoody[i++].wd[3].Load=0.50;
	// fuel model 3
		NFFLWoody[i].NumClasses=3;
		 NFFLWoody[i].Depth=2.5;
		 NFFLWoody[i].Units=1;
	//     if((NFFLWoody[2].wd=(WoodyData *) calloc(NFFLWoody[i].NumClasses, sizeof(WoodyData)))==NULL)
	//     if((NFFLWoody[2].wd=(WoodyData *) GlobalAlloc(GMEM_FIXED |  GMEM_ZEROINIT, NFFLWoody[i].NumClasses*sizeof(WoodyData)))==NULL)
		 if((NFFLWoody[2].wd=new WoodyData[NFFLWoody[i].NumClasses])==NULL)
		 	return false;
//		 ZeroMemory(NFFLWoody[2].wd, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 memset(NFFLWoody[2].wd,0x0, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 NFFLWoody[i].wd[0].SurfaceAreaToVolume=1500.0;
		 NFFLWoody[i].wd[0].Load=3.01;
		 NFFLWoody[i].wd[1].SurfaceAreaToVolume=109.0;
		 NFFLWoody[i].wd[1].Load=0.0;
		 NFFLWoody[i].wd[2].SurfaceAreaToVolume=30.0;
		 NFFLWoody[i++].wd[2].Load=0.0;
	// fuel model 4
		NFFLWoody[i].NumClasses=4;
		 NFFLWoody[i].Depth=6.0;
		 NFFLWoody[i].Units=1;
	//     if((NFFLWoody[3].wd=(WoodyData *) calloc(NFFLWoody[i].NumClasses, sizeof(WoodyData)))==NULL)
	//     if((NFFLWoody[3].wd=(WoodyData *) GlobalAlloc(GMEM_FIXED |  GMEM_ZEROINIT, NFFLWoody[i].NumClasses*sizeof(WoodyData)))==NULL)
	//     	return false;
		 if((NFFLWoody[3].wd=new WoodyData[NFFLWoody[i].NumClasses])==NULL)
		 	return false;
//		 ZeroMemory(NFFLWoody[3].wd, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 memset(NFFLWoody[3].wd,0x0, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 NFFLWoody[i].wd[0].SurfaceAreaToVolume=2000.0;
		 NFFLWoody[i].wd[0].Load=5.01;
		 NFFLWoody[i].wd[1].SurfaceAreaToVolume=109.0;
		 NFFLWoody[i].wd[1].Load=4.01;
		 NFFLWoody[i].wd[2].SurfaceAreaToVolume=30.0;
		 NFFLWoody[i].wd[2].Load=2.00;
		 NFFLWoody[i].wd[3].SurfaceAreaToVolume=1500.0;
		 NFFLWoody[i++].wd[3].Load=5.01;
	// fuel model 5
		NFFLWoody[i].NumClasses=4;
		 NFFLWoody[i].Depth=2.0;
		 NFFLWoody[i].Units=1;
	//     if((NFFLWoody[4].wd=(WoodyData *) calloc(NFFLWoody[i].NumClasses, sizeof(WoodyData)))==NULL)
	//     if((NFFLWoody[4].wd=(WoodyData *) GlobalAlloc(GMEM_FIXED |  GMEM_ZEROINIT, NFFLWoody[i].NumClasses*sizeof(WoodyData)))==NULL)
	//     	return false;
		 if((NFFLWoody[4].wd=new WoodyData[NFFLWoody[i].NumClasses])==NULL)
		 	return false;
//		 ZeroMemory(NFFLWoody[4].wd, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 memset(NFFLWoody[4].wd,0x0, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 NFFLWoody[i].wd[0].SurfaceAreaToVolume=2000.0;
		 NFFLWoody[i].wd[0].Load=1.00;
		 NFFLWoody[i].wd[1].SurfaceAreaToVolume=109.0;
		 NFFLWoody[i].wd[1].Load=0.5;
		 NFFLWoody[i].wd[2].SurfaceAreaToVolume=30.0;
		 NFFLWoody[i].wd[2].Load=0.00;
		 NFFLWoody[i].wd[3].SurfaceAreaToVolume=1500.0;
		 NFFLWoody[i++].wd[3].Load=2.00;
	// fuel model 6
		NFFLWoody[i].NumClasses=3;
		 NFFLWoody[i].Depth=2.5;
		 NFFLWoody[i].Units=1;
	//     if((NFFLWoody[5].wd=(WoodyData *) calloc(NFFLWoody[i].NumClasses, sizeof(WoodyData)))==NULL)
	//     if((NFFLWoody[5].wd=(WoodyData *) GlobalAlloc(GMEM_FIXED |  GMEM_ZEROINIT, NFFLWoody[i].NumClasses*sizeof(WoodyData)))==NULL)
	//     	return false;
		 if((NFFLWoody[5].wd=new WoodyData[NFFLWoody[i].NumClasses])==NULL)
		 	return false;
//		 ZeroMemory(NFFLWoody[5].wd, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 memset(NFFLWoody[5].wd,0x0, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 NFFLWoody[i].wd[0].SurfaceAreaToVolume=1750.0;
		 NFFLWoody[i].wd[0].Load=1.50;
		 NFFLWoody[i].wd[1].SurfaceAreaToVolume=109.0;
		 NFFLWoody[i].wd[1].Load=2.5;
		 NFFLWoody[i].wd[2].SurfaceAreaToVolume=30.0;
		 NFFLWoody[i++].wd[2].Load=2.0;
	// fuel model 7
		NFFLWoody[i].NumClasses=4;
		 NFFLWoody[i].Depth=2.5;
		 NFFLWoody[i].Units=1;
	//     if((NFFLWoody[6].wd=(WoodyData *) calloc(NFFLWoody[i].NumClasses, sizeof(WoodyData)))==NULL)
	//     if((NFFLWoody[6].wd=(WoodyData *) GlobalAlloc(GMEM_FIXED |  GMEM_ZEROINIT, NFFLWoody[i].NumClasses*sizeof(WoodyData)))==NULL)
	//     	return false;
		 if((NFFLWoody[6].wd=new WoodyData[NFFLWoody[i].NumClasses])==NULL)
		 	return false;
//		 ZeroMemory(NFFLWoody[6].wd, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 memset(NFFLWoody[6].wd,0x0, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 NFFLWoody[i].wd[0].SurfaceAreaToVolume=1750.0;
		 NFFLWoody[i].wd[0].Load=1.13;
		 NFFLWoody[i].wd[1].SurfaceAreaToVolume=109.0;
		 NFFLWoody[i].wd[1].Load=1.87;
		 NFFLWoody[i].wd[2].SurfaceAreaToVolume=30.0;
		 NFFLWoody[i].wd[2].Load=1.5;
		 NFFLWoody[i].wd[3].SurfaceAreaToVolume=1500.0;
		 NFFLWoody[i++].wd[3].Load=0.37;
	// fuel model 8
		NFFLWoody[i].NumClasses=3;
		 NFFLWoody[i].Depth=0.2;
		 NFFLWoody[i].Units=1;
	//     if((NFFLWoody[7].wd=(WoodyData *) calloc(NFFLWoody[i].NumClasses, sizeof(WoodyData)))==NULL)
	//     if((NFFLWoody[7].wd=(WoodyData *) GlobalAlloc(GMEM_FIXED |  GMEM_ZEROINIT, NFFLWoody[i].NumClasses*sizeof(WoodyData)))==NULL)
	//     	return false;
		 if((NFFLWoody[7].wd=new WoodyData[NFFLWoody[i].NumClasses])==NULL)
		 	return false;
//		 ZeroMemory(NFFLWoody[7].wd, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 memset(NFFLWoody[7].wd,0x0, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 NFFLWoody[i].wd[0].SurfaceAreaToVolume=2000.0;
		 NFFLWoody[i].wd[0].Load=1.50;
		 NFFLWoody[i].wd[1].SurfaceAreaToVolume=109.0;
		 NFFLWoody[i].wd[1].Load=1.00;
		 NFFLWoody[i].wd[2].SurfaceAreaToVolume=30.0;
		 NFFLWoody[i++].wd[2].Load=2.5;
	// fuel model 9
		NFFLWoody[i].NumClasses=3;
		 NFFLWoody[i].Depth=0.2;
		 NFFLWoody[i].Units=1;
	//     if((NFFLWoody[8].wd=(WoodyData *) calloc(NFFLWoody[i].NumClasses, sizeof(WoodyData)))==NULL)
	//     if((NFFLWoody[8].wd=(WoodyData *) GlobalAlloc(GMEM_FIXED |  GMEM_ZEROINIT, NFFLWoody[i].NumClasses*sizeof(WoodyData)))==NULL)
	//     	return false;
		 if((NFFLWoody[8].wd=new WoodyData[NFFLWoody[i].NumClasses])==NULL)
		 	return false;
//		 ZeroMemory(NFFLWoody[8].wd, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 memset(NFFLWoody[8].wd,0x0, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 NFFLWoody[i].wd[0].SurfaceAreaToVolume=2500.0;
		 NFFLWoody[i].wd[0].Load=2.92;
		 NFFLWoody[i].wd[1].SurfaceAreaToVolume=109.0;
		 NFFLWoody[i].wd[1].Load=0.41;
		 NFFLWoody[i].wd[2].SurfaceAreaToVolume=30.0;
		 NFFLWoody[i++].wd[2].Load=0.15;
	// fuel model 10
		NFFLWoody[i].NumClasses=4;
		 NFFLWoody[i].Depth=1.0;
		 NFFLWoody[i].Units=1;
	//     if((NFFLWoody[9].wd=(WoodyData *) calloc(NFFLWoody[i].NumClasses, sizeof(WoodyData)))==NULL)
	//     if((NFFLWoody[9].wd=(WoodyData *) GlobalAlloc(GMEM_FIXED |  GMEM_ZEROINIT, NFFLWoody[i].NumClasses*sizeof(WoodyData)))==NULL)
	//     	return false;
		 if((NFFLWoody[9].wd=new WoodyData[NFFLWoody[i].NumClasses])==NULL)
		 	return false;
//		 ZeroMemory(NFFLWoody[9].wd, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 memset(NFFLWoody[9].wd,0x0, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 NFFLWoody[i].wd[0].SurfaceAreaToVolume=2000.0;
		 NFFLWoody[i].wd[0].Load=3.01;
		 NFFLWoody[i].wd[1].SurfaceAreaToVolume=109.0;
		 NFFLWoody[i].wd[1].Load=2.0;
		 NFFLWoody[i].wd[2].SurfaceAreaToVolume=30.0;
		 NFFLWoody[i].wd[2].Load=5.01;
		 NFFLWoody[i].wd[3].SurfaceAreaToVolume=1500.0;
		 NFFLWoody[i++].wd[3].Load=2.00;
	// fuel model 11
		NFFLWoody[i].NumClasses=3;
		 NFFLWoody[i].Depth=1.0;
		 NFFLWoody[i].Units=1;
	//     if((NFFLWoody[10].wd=(WoodyData *) calloc(NFFLWoody[i].NumClasses, sizeof(WoodyData)))==NULL)
	//     if((NFFLWoody[10].wd=(WoodyData *) GlobalAlloc(GMEM_FIXED |  GMEM_ZEROINIT, NFFLWoody[i].NumClasses*sizeof(WoodyData)))==NULL)
	//     	return false;
		 if((NFFLWoody[10].wd=new WoodyData[NFFLWoody[i].NumClasses])==NULL)
		 	return false;
//		 ZeroMemory(NFFLWoody[10].wd, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 memset(NFFLWoody[10].wd,0x0, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 NFFLWoody[i].wd[0].SurfaceAreaToVolume=1500.0;
		 NFFLWoody[i].wd[0].Load=1.50;
		 NFFLWoody[i].wd[1].SurfaceAreaToVolume=109.0;
		 NFFLWoody[i].wd[1].Load=4.51;
		 NFFLWoody[i].wd[2].SurfaceAreaToVolume=30.0;
		 NFFLWoody[i++].wd[2].Load=5.51;
	// fuel model 12
		NFFLWoody[i].NumClasses=3;
		 NFFLWoody[i].Depth=2.3;
		 NFFLWoody[i].Units=1;
	//     if((NFFLWoody[11].wd=(WoodyData *) calloc(NFFLWoody[i].NumClasses, sizeof(WoodyData)))==NULL)
	//     if((NFFLWoody[11].wd=(WoodyData *) GlobalAlloc(GMEM_FIXED |  GMEM_ZEROINIT, NFFLWoody[i].NumClasses*sizeof(WoodyData)))==NULL)
	//     	return false;
		 if((NFFLWoody[11].wd=new WoodyData[NFFLWoody[i].NumClasses])==NULL)
		 	return false;
//		 ZeroMemory(NFFLWoody[11].wd, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 memset(NFFLWoody[11].wd,0x0, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 NFFLWoody[i].wd[0].SurfaceAreaToVolume=1500.0;
		 NFFLWoody[i].wd[0].Load=4.01;
		 NFFLWoody[i].wd[1].SurfaceAreaToVolume=109.0;
		 NFFLWoody[i].wd[1].Load=14.03;
		 NFFLWoody[i].wd[2].SurfaceAreaToVolume=30.0;
		 NFFLWoody[i++].wd[2].Load=16.53;
	// fuel model 13
		NFFLWoody[i].NumClasses=3;
		 NFFLWoody[i].Depth=3.0;
		 NFFLWoody[i].Units=1;
	//     if((NFFLWoody[12].wd=(WoodyData *) calloc(NFFLWoody[i].NumClasses, sizeof(WoodyData)))==NULL)
	//     if((NFFLWoody[12].wd=(WoodyData *) GlobalAlloc(GMEM_FIXED |  GMEM_ZEROINIT, NFFLWoody[i].NumClasses*sizeof(WoodyData)))==NULL)
	//     	return false;
		 if((NFFLWoody[12].wd=new WoodyData[NFFLWoody[i].NumClasses])==NULL)
		 	return false;
//		 ZeroMemory(NFFLWoody[12].wd, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 memset(NFFLWoody[12].wd,0x0, NFFLWoody[i].NumClasses*sizeof(WoodyData));
		 NFFLWoody[i].wd[0].SurfaceAreaToVolume=1500.0;
		 NFFLWoody[i].wd[0].Load=7.01;
		 NFFLWoody[i].wd[1].SurfaceAreaToVolume=109.0;
		 NFFLWoody[i].wd[1].Load=23.04;
		 NFFLWoody[i].wd[2].SurfaceAreaToVolume=30.0;
		 NFFLWoody[i].wd[2].Load=28.05;
		 long j;
		 // convert all to metric
		 for(i=0; i<13; i++)
		 {	NFFLWoody[i].Depth/=3.280839895;
		 	for(j=0; j<NFFLWoody[i].NumClasses; j++)
			  {	NFFLWoody[i].wd[j].SurfaceAreaToVolume*=3.280839895;
			  	NFFLWoody[i].wd[j].Load*=0.224169061;
				   NFFLWoody[i].wd[j].HeatContent=8000.0*2.32599;
				   NFFLWoody[i].wd[j].Density=513.0;
				   NFFLWoody[i].TotalWeight+=NFFLWoody[i].wd[j].Load;
			  }
		 }
	*/
	return true;
}


double Get1000HrMoisture(long ModelNumber, bool Average)
{
     long i, Num;
	double AvgMx, sav1000;

	Num=0;
     AvgMx=0.0;
     sav1000=4.0/0.0106;	// 4 inch diameter
	for(i=0; i<coarsewoody[ModelNumber-1].NumClasses; i++)
     {	if(coarsewoody[ModelNumber-1].wd[i].SurfaceAreaToVolume>sav1000)
     		continue;
     	AvgMx+=coarsewoody[ModelNumber-1].wd[i].FuelMoisture;
          Num++;
     	if(!Average)          // pick first one
          	break;
     }
     if(Num>0)
	     AvgMx/=(double) Num;
     else
     	AvgMx=0.0;

	return AvgMx;
}


double GetWoodyFuelMoisture(long ModelNumber, long SizeClass)
{
	if (ModelNumber > MAXNUM_COARSEWOODY_MODELS)
		return 0.0;

	if (coarsewoody[ModelNumber - 1].NumClasses < SizeClass)
		return 0.0;

	return coarsewoody[ModelNumber - 1].wd[SizeClass].FuelMoisture;
}


long GetWoodyDataUnits(long ModelNumber, char* Description)
{
	memcpy(Description, coarsewoody[ModelNumber - 1].Description,
		64 * sizeof(char));

	return coarsewoody[ModelNumber - 1].Units;
}


void GetWoodyData(long WoodyModelNumber, long SurfModelNumber,
	long* NumClasses, WoodyData* woody, double* depth, double* load)
{
	bool Combine = false;

	*NumClasses = 0;
	*load = 0.0;


	switch (WoodyCombineOptions(GETVAL))
	{
	case CWD_COMBINE_NEVER:
		Combine = false; break;
	case CWD_COMBINE_ALWAYS:
		Combine = true; break;
	case CWD_COMBINE_ABSENT:
		if (WoodyModelNumber < 1)
			Combine = true;
		else if (coarsewoody[WoodyModelNumber - 1].wd == 0)
			Combine = true;
		else
			Combine = false;
		break;
	}

	if (Combine)
	{
		// gather surface fuel model data
		if (SurfModelNumber > 0 && SurfModelNumber < 14)														 // alloc the max
		{
			*NumClasses = NFFLWoody[SurfModelNumber - 1].NumClasses;//tempwoody.NumClasses=
			*depth = NFFLWoody[SurfModelNumber - 1].Depth; //tempwoody.Depth=
			*load = NFFLWoody[SurfModelNumber - 1].TotalWeight;//tempwoody.TotalWeight=
			memcpy(woody, NFFLWoody[SurfModelNumber - 1].wd,
				NFFLWoody[SurfModelNumber - 1].NumClasses * sizeof(WoodyData));
		}
		else if (SurfModelNumber > 0)
		{
			//double t1, t10, t100, tLH, tLW, s1, sLH, sLW, hd, hl, d, xm;
			double  s1, hd, hl;
			long i = 0, j;
			NewFuel nf;
			memset(&nf, 0x0, sizeof(NewFuel));

			GetNewFuel(SurfModelNumber, &nf);
			*depth = nf.depth;
			*load = 0.0;
			if (nf.h1 > 0.0)
			{
				woody[i].Load = nf.h1;//*0.224169061;
				woody[i].SurfaceAreaToVolume = s1;//*3.280839895;
				woody[i++].HeatContent = hd;//*2.32599;
			}
			if (nf.h10 > 0.0)
			{
				woody[i].Load = nf.h10;
				woody[i].SurfaceAreaToVolume = 109.0;//*3.280839895;
				woody[i++].HeatContent = hd;//*2.32599;
			}
			if (nf.h100 > 0.0)
			{
				woody[i].Load = nf.h100;
				woody[i].SurfaceAreaToVolume = 30;//*3.280839895;
				woody[i++].HeatContent = hd;//*2.32599;
			}
			if (nf.lw > 0.0)
			{
				woody[i].Load = nf.lw;
				woody[i].SurfaceAreaToVolume = nf.savlw;//*3.280839895;
				woody[i++].HeatContent = hl;//*2.32599;
			}
			if (nf.lh > 0.0)
			{
				woody[i].Load = nf.lh;
				woody[i].SurfaceAreaToVolume = nf.savlh;//*3.280839895;
				woody[i++].HeatContent = hl;//*2.32599;
			}
			for (j = 0; j < i; j++)
			{
				woody[j].Density = 513.0;
				*load += woody[j].Load;
			}
			*NumClasses = tempwoody.NumClasses = i;
		}
	}
	// patch into coarsewoody model data if present
	if (WoodyModelNumber > 0)
	{
		if (coarsewoody[WoodyModelNumber - 1].wd)
		{
			memcpy(&woody[*NumClasses],
				coarsewoody[WoodyModelNumber - 1].wd,
				coarsewoody[WoodyModelNumber - 1].NumClasses * sizeof(WoodyData));
			*NumClasses += coarsewoody[WoodyModelNumber - 1].NumClasses;
			*depth = coarsewoody[WoodyModelNumber - 1].Depth;
			*load += coarsewoody[WoodyModelNumber - 1].TotalWeight;
		}
	}
}


void GetCurrentFuelMoistures(long fuelmod, long woodymod, double* mxin,
	double* mxout, long NumMx)
{
	bool Combine;
	long i, NumClasses = 0;

//	ZeroMemory(mxout, 20 * sizeof(double));
	memset(mxout,0x0, 20 * sizeof(double));

	switch (WoodyCombineOptions(GETVAL))
	{
	case CWD_COMBINE_NEVER:
		Combine = false; break;
	case CWD_COMBINE_ALWAYS:
		Combine = true; break;
	case CWD_COMBINE_ABSENT:
		if (woodymod < 1)
			Combine = true;
		else if (coarsewoody[woodymod - 1].wd == 0)
			Combine = true;
		else
			Combine = false;
		break;
	}

	if (Combine)
	{
		if (fuelmod < 14)
		{
			switch (fuelmod)
			{
			case 0:
				break;
			case 1:
				memcpy(mxout, mxin, 3 * sizeof(double));
				NumClasses = 3;
				break;
			case 2:
				memcpy(mxout, mxin, 3 * sizeof(double));
				mxout[3] = mxin[5];
				NumClasses = 4;
				break;
			case 3:
				memcpy(mxout, mxin, 3 * sizeof(double));
				NumClasses = 3;
				break;
			case 4:
				memcpy(mxout, mxin, 3 * sizeof(double));
				mxout[3] = mxin[5];
				NumClasses = 4;
				break;
			case 5:
				memcpy(mxout, mxin, 3 * sizeof(double));
				mxout[3] = mxin[5];
				NumClasses = 4;
				break;
			case 6:
				memcpy(mxout, mxin, 3 * sizeof(double));
				NumClasses = 3;
				break;
			case 7:
				memcpy(mxout, mxin, 4 * sizeof(double));
				NumClasses = 4;
				break;
			case 8:
				memcpy(mxout, mxin, 3 * sizeof(double));
				NumClasses = 3;
				break;
			case 9:
				memcpy(mxout, mxin, 3 * sizeof(double));
				NumClasses = 3;
				break;
			case 10:
				memcpy(mxout, mxin, 3 * sizeof(double));
				mxout[3] = mxin[5];
				NumClasses = 4;
				break;
			case 11:
				memcpy(mxout, mxin, 3 * sizeof(double));
				NumClasses = 3;
				break;
			case 12:
				memcpy(mxout, mxin, 3 * sizeof(double));
				NumClasses = 3;
				break;
			case 13:
				memcpy(mxout, mxin, 3 * sizeof(double));
				NumClasses = 3;
				break;
			}
		}
		else
		{
			//if(FuelMod.TL1[fuelmod-14]>0.0)   	 // always have 1, 10, 100 hr fuels
			mxout[NumClasses++] = mxin[0];
			//if(FuelMod.TL10[fuelmod-14]>0.0)
			mxout[NumClasses++] = mxin[1];
			//if(FuelMod.TL100[fuelmod-14]>0.0)
			mxout[NumClasses++] = mxin[2];
			if (newfuels[fuelmod].lh > 0.0)//FuelMod.TLLiveH[fuelmod-14]>0.0)
				mxout[NumClasses++] = mxin[6];
			if (newfuels[fuelmod].lw > 0.0)//FuelMod.TLLiveW[fuelmod-14]>0.0)
				mxout[NumClasses++] = mxin[5];
		}
	}
	if (woodymod > 0)
	{
		for (i = 0; i < coarsewoody[woodymod - 1].NumClasses; i++)
			mxout[NumClasses + i] = coarsewoody[woodymod - 1].wd[i].FuelMoisture;
		memcpy(&mxout[NumClasses], mxin, 3 * sizeof(double));
		mxout[3] = mxin[5];
	}
}


long WoodyCombineOptions(long Option)
{
	if (Option > 0)
		CombineOption = Option;

	return CombineOption;
}


double WeightLossErrorTolerance(double value)
{
	if (value > 0.0)
		WeightLossErrorTol = value;

	return WeightLossErrorTol;
}

//----------------------------------------------------------------
//
//----------------------------------------------------------------

static long MaxThreads = 1;
static bool CanModify = true;

long GetMaxThreads()
{
	return MaxThreads;
}


void SetMaxThreads(long numthreads)
{
	if (numthreads > 0 && numthreads < 65)
		MaxThreads = numthreads;
}


bool CanModifyInputs(long YesNo)
{
	if (YesNo >= 0)
		CanModify = (bool) YesNo;

	return CanModify;
}


bool CanAssignProcessor()
{
	/*
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	if (osvi.dwPlatformId != VER_PLATFORM_WIN32s &&
		osvi.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS)
		return true;

	return false;
	*/
	return true;
}


//------------------------------------------------------------------------
//
//
//
//------------------------------------------------------------------------


void ResetNewFuels()
{
	memset(newfuels, 0x0, 256 * sizeof(NewFuel));

	InitializeNewFuel();

	long i;
	for (i = 0; i < 257; i++)
	{
		if (newfuels[i].number)
			newfuels[i].number *= -1;	// indicate permanence
	}

	HAVE_CUST_MODELS = false;
}


bool SetNewFuel(NewFuel* newfuel)
{
	long FuelNum;

	if (newfuel == 0)
		return false;

	FuelNum = newfuel->number;
	if (FuelNum > 256 || FuelNum < 0)
		return false;

	newfuel->number = 0;
	if (newfuel->h1 > 0.0)
		newfuel->number = FuelNum;
	else if (newfuel->h10 > 0.0)
		newfuel->number = FuelNum;
	else if (newfuel->h100 > 0.0)
		newfuel->number = FuelNum;
	else if (newfuel->lh > 0.0)
		newfuel->number = FuelNum;
	else if (newfuel->lw > 0.0)
		newfuel->number = FuelNum;

	memcpy(&newfuels[FuelNum], newfuel, sizeof(NewFuel));

	return true;
}


bool GetNewFuel(long number, NewFuel* newfuel)
{
	if (number < 0)
		return false;

	if (newfuels[number].number == 0)
		return false;

	if (newfuel != 0)
	{
		memcpy(newfuel, &newfuels[number], sizeof(NewFuel));
		newfuel->number = labs(newfuel->number);	// return absolute value of number
	}

	return true;
}


bool IsNewFuelReserved(long number)
{
	if (number < 0)
		return false;

	if (newfuels[number].number < 0)
		return true;

	return false;
}


double GetFuelDepth(int Number)
{
	if (Number < 0)
		return 0.0;

	if (newfuels[Number].number == 0)
		return 0.0;

	return newfuels[Number].depth;
}


void InitializeNewFuel()
{
	newfuels[1].number = 1;
	strcpy(newfuels[1].code, "FM1");
	newfuels[1].h1 = 0.740000;
	newfuels[1].h10 = 0.000000;
	newfuels[1].h100 = 0.000000;
	newfuels[1].lh = 0.000000;
	newfuels[1].lw = 0.000000;
	newfuels[1].dynamic = 0;
	newfuels[1].sav1 = 3500;
	newfuels[1].savlh = 1800;
	newfuels[1].savlw = 1500;
	newfuels[1].depth = 1.000000;
	newfuels[1].xmext = 0.120000;
	newfuels[1].heatd = 8000.000000;
	newfuels[1].heatl = 8000.000000;
	strcpy(newfuels[1].desc, "Short Grass");

	newfuels[2].number = 2;
	strcpy(newfuels[2].code, "FM2");
	newfuels[2].h1 = 2.000000;
	newfuels[2].h10 = 1.000000;
	newfuels[2].h100 = 0.500000;
	newfuels[2].lh = 0.000000;
	newfuels[2].lw = 0.500000;
	newfuels[2].dynamic = 0;
	newfuels[2].sav1 = 3000;
	newfuels[2].savlh = 1800;
	newfuels[2].savlw = 1500;
	newfuels[2].depth = 1.000000;
	newfuels[2].xmext = 0.150000;
	newfuels[2].heatd = 8000.000000;
	newfuels[2].heatl = 8000.000000;
	strcpy(newfuels[2].desc, "Timber Grass/Understory");

	newfuels[3].number = 3;
	strcpy(newfuels[3].code, "FM3");
	newfuels[3].h1 = 3.010000;
	newfuels[3].h10 = 0.000000;
	newfuels[3].h100 = 0.000000;
	newfuels[3].lh = 0.000000;
	newfuels[3].lw = 0.000000;
	newfuels[3].dynamic = 0;
	newfuels[3].sav1 = 1500;
	newfuels[3].savlh = 1800;
	newfuels[3].savlw = 1500;
	newfuels[3].depth = 2.500000;
	newfuels[3].xmext = 0.250000;
	newfuels[3].heatd = 8000.000000;
	newfuels[3].heatl = 8000.000000;
	strcpy(newfuels[3].desc, "Tall Grass");

	newfuels[4].number = 4;
	strcpy(newfuels[4].code, "FM4");
	newfuels[4].h1 = 5.010000;
	newfuels[4].h10 = 4.010000;
	newfuels[4].h100 = 2.000000;
	newfuels[4].lh = 0.000000;
	newfuels[4].lw = 5.010000;
	newfuels[4].dynamic = 0;
	newfuels[4].sav1 = 2000;
	newfuels[4].savlh = 1800;
	newfuels[4].savlw = 1500;
	newfuels[4].depth = 6.000000;
	newfuels[4].xmext = 0.200000;
	newfuels[4].heatd = 8000.000000;
	newfuels[4].heatl = 8000.000000;
	strcpy(newfuels[4].desc, "Chaparral");

	newfuels[5].number = 5;
	strcpy(newfuels[5].code, "FM5");
	newfuels[5].h1 = 1.000000;
	newfuels[5].h10 = 0.500000;
	newfuels[5].h100 = 0.000000;
	newfuels[5].lh = 0.000000;
	newfuels[5].lw = 2.000000;
	newfuels[5].dynamic = 0;
	newfuels[5].sav1 = 2000;
	newfuels[5].savlh = 1800;
	newfuels[5].savlw = 1500;
	newfuels[5].depth = 2.000000;
	newfuels[5].xmext = 0.200000;
	newfuels[5].heatd = 8000.000000;
	newfuels[5].heatl = 8000.000000;
	strcpy(newfuels[5].desc, "Short Brush");

	newfuels[6].number = 6;
	strcpy(newfuels[6].code, "FM6");
	newfuels[6].h1 = 1.500000;
	newfuels[6].h10 = 2.500000;
	newfuels[6].h100 = 2.000000;
	newfuels[6].lh = 0.000000;
	newfuels[6].lw = 0.000000;
	newfuels[6].dynamic = 0;
	newfuels[6].sav1 = 1750;
	newfuels[6].savlh = 1800;
	newfuels[6].savlw = 1500;
	newfuels[6].depth = 2.500000;
	newfuels[6].xmext = 0.250000;
	newfuels[6].heatd = 8000.000000;
	newfuels[6].heatl = 8000.000000;
	strcpy(newfuels[6].desc, "Dormant Brush");

	newfuels[7].number = 7;
	strcpy(newfuels[7].code, "FM7");
	newfuels[7].h1 = 1.130000;
	newfuels[7].h10 = 1.870000;
	newfuels[7].h100 = 1.500000;
	newfuels[7].lh = 0.000000;
	newfuels[7].lw = 0.370000;
	newfuels[7].dynamic = 0;
	newfuels[7].sav1 = 1550;
	newfuels[7].savlh = 1800;
	newfuels[7].savlw = 1500;
	newfuels[7].depth = 2.500000;
	newfuels[7].xmext = 0.400000;
	newfuels[7].heatd = 8000.000000;
	newfuels[7].heatl = 8000.000000;
	strcpy(newfuels[7].desc, "Southern Rough");

	newfuels[8].number = 8;
	strcpy(newfuels[8].code, "FM8");
	newfuels[8].h1 = 1.500000;
	newfuels[8].h10 = 1.000000;
	newfuels[8].h100 = 2.500000;
	newfuels[8].lh = 0.000000;
	newfuels[8].lw = 0.000000;
	newfuels[8].dynamic = 0;
	newfuels[8].sav1 = 2000;
	newfuels[8].savlh = 1800;
	newfuels[8].savlw = 1500;
	newfuels[8].depth = 0.200000;
	newfuels[8].xmext = 0.300000;
	newfuels[8].heatd = 8000.000000;
	newfuels[8].heatl = 8000.000000;
	strcpy(newfuels[8].desc, "Closed Timber Litter");

	newfuels[9].number = 9;
	strcpy(newfuels[9].code, "FM9");
	newfuels[9].h1 = 2.920000;
	newfuels[9].h10 = 0.410000;
	newfuels[9].h100 = 0.150000;
	newfuels[9].lh = 0.000000;
	newfuels[9].lw = 0.000000;
	newfuels[9].dynamic = 0;
	newfuels[9].sav1 = 2500;
	newfuels[9].savlh = 1800;
	newfuels[9].savlw = 1500;
	newfuels[9].depth = 0.200000;
	newfuels[9].xmext = 0.250000;
	newfuels[9].heatd = 8000.000000;
	newfuels[9].heatl = 8000.000000;
	strcpy(newfuels[9].desc, "Hardwood Litter");

	newfuels[10].number = 10;
	strcpy(newfuels[10].code, "FM10");
	newfuels[10].h1 = 3.010000;
	newfuels[10].h10 = 2.000000;
	newfuels[10].h100 = 5.010000;
	newfuels[10].lh = 0.000000;
	newfuels[10].lw = 2.000000;
	newfuels[10].dynamic = 0;
	newfuels[10].sav1 = 2000;
	newfuels[10].savlh = 1800;
	newfuels[10].savlw = 1500;
	newfuels[10].depth = 1.000000;
	newfuels[10].xmext = 0.250000;
	newfuels[10].heatd = 8000.000000;
	newfuels[10].heatl = 8000.000000;
	strcpy(newfuels[10].desc, "Timber Litter/Understory");

	newfuels[11].number = 11;
	strcpy(newfuels[11].code, "FM11");
	newfuels[11].h1 = 1.500000;
	newfuels[11].h10 = 4.510000;
	newfuels[11].h100 = 5.510000;
	newfuels[11].lh = 0.000000;
	newfuels[11].lw = 0.000000;
	newfuels[11].dynamic = 0;
	newfuels[11].sav1 = 1500;
	newfuels[11].savlh = 1800;
	newfuels[11].savlw = 1500;
	newfuels[11].depth = 1.000000;
	newfuels[11].xmext = 0.150000;
	newfuels[11].heatd = 8000.000000;
	newfuels[11].heatl = 8000.000000;
	strcpy(newfuels[11].desc, "Light Slash");

	newfuels[12].number = 12;
	strcpy(newfuels[12].code, "FM12");
	newfuels[12].h1 = 4.010000;
	newfuels[12].h10 = 14.030000;
	newfuels[12].h100 = 16.530000;
	newfuels[12].lh = 0.000000;
	newfuels[12].lw = 0.000000;
	newfuels[12].dynamic = 0;
	newfuels[12].sav1 = 1500;
	newfuels[12].savlh = 1800;
	newfuels[12].savlw = 1500;
	newfuels[12].depth = 2.300000;
	newfuels[12].xmext = 0.200000;
	newfuels[12].heatd = 8000.000000;
	newfuels[12].heatl = 8000.000000;
	strcpy(newfuels[12].desc, "Medium Slash");

	newfuels[13].number = 13;
	strcpy(newfuels[13].code, "FM13");
	newfuels[13].h1 = 7.010000;
	newfuels[13].h10 = 23.040000;
	newfuels[13].h100 = 28.050000;
	newfuels[13].lh = 0.000000;
	newfuels[13].lw = 0.000000;
	newfuels[13].dynamic = 0;
	newfuels[13].sav1 = 1500;
	newfuels[13].savlh = 1800;
	newfuels[13].savlw = 1500;
	newfuels[13].depth = 3.000000;
	newfuels[13].xmext = 0.250000;
	newfuels[13].heatd = 8000.000000;
	newfuels[13].heatl = 8000.000000;
	strcpy(newfuels[13].desc, "Heavy Slash");

	newfuels[99].number = 99;
	strcpy(newfuels[99].code, "NB1");
	strcpy(newfuels[99].desc, "Barren");

	newfuels[98].number = 98;
	strcpy(newfuels[98].code, "NB2");
	strcpy(newfuels[98].desc, "Water");

	newfuels[97].number = 97;
	strcpy(newfuels[97].code, "NB3");
	strcpy(newfuels[97].desc, "Snow or Ice");

	newfuels[96].number = 96;
	strcpy(newfuels[96].code, "NB4");
	strcpy(newfuels[96].desc, "Agricultural or Cropland");

	newfuels[95].number = 95;
	strcpy(newfuels[95].code, "NB5");
	strcpy(newfuels[95].desc, "Urban or Developed");

	newfuels[101].number = 101;
	strcpy(newfuels[101].code, "GR1");
	newfuels[101].h1 = 0.100000;
	newfuels[101].h10 = 0.000000;
	newfuels[101].h100 = 0.000000;
	newfuels[101].lh = 0.300000;
	newfuels[101].lw = 0.000000;
	newfuels[101].dynamic = 1;
	newfuels[101].sav1 = 2200;
	newfuels[101].savlh = 2000;
	newfuels[101].savlw = 1500;
	newfuels[101].depth = 0.400000;
	newfuels[101].xmext = 0.150000;
	newfuels[101].heatd = 8000.000000;
	newfuels[101].heatl = 8000.000000;
	strcpy(newfuels[101].desc, "Short, sparse, dry climate grass");

	newfuels[102].number = 102;
	strcpy(newfuels[102].code, "GR2");
	newfuels[102].h1 = 0.100000;
	newfuels[102].h10 = 0.000000;
	newfuels[102].h100 = 0.000000;
	newfuels[102].lh = 1.000000;
	newfuels[102].lw = 0.000000;
	newfuels[102].dynamic = 1;
	newfuels[102].sav1 = 2000;
	newfuels[102].savlh = 1800;
	newfuels[102].savlw = 1500;
	newfuels[102].depth = 1.000000;
	newfuels[102].xmext = 0.150000;
	newfuels[102].heatd = 8000.000000;
	newfuels[102].heatl = 8000.000000;
	strcpy(newfuels[102].desc, "Low load, dry climate grass");

	newfuels[103].number = 103;
	strcpy(newfuels[103].code, "GR3");
	newfuels[103].h1 = 0.100000;
	newfuels[103].h10 = 0.400000;
	newfuels[103].h100 = 0.000000;
	newfuels[103].lh = 1.500000;
	newfuels[103].lw = 0.000000;
	newfuels[103].dynamic = 1;
	newfuels[103].sav1 = 1500;
	newfuels[103].savlh = 1300;
	newfuels[103].savlw = 1500;
	newfuels[103].depth = 2.000000;
	newfuels[103].xmext = 0.300000;
	newfuels[103].heatd = 8000.000000;
	newfuels[103].heatl = 8000.000000;
	strcpy(newfuels[103].desc, "Low load, very coarse, humid climate grass");

	newfuels[104].number = 104;
	strcpy(newfuels[104].code, "GR4");
	newfuels[104].h1 = 0.250000;
	newfuels[104].h10 = 0.000000;
	newfuels[104].h100 = 0.000000;
	newfuels[104].lh = 1.900000;
	newfuels[104].lw = 0.000000;
	newfuels[104].dynamic = 1;
	newfuels[104].sav1 = 2000;
	newfuels[104].savlh = 1800;
	newfuels[104].savlw = 1500;
	newfuels[104].depth = 2.000000;
	newfuels[104].xmext = 0.150000;
	newfuels[104].heatd = 8000.000000;
	newfuels[104].heatl = 8000.000000;
	strcpy(newfuels[104].desc, "Moderate load, dry climate grass");

	newfuels[105].number = 105;
	strcpy(newfuels[105].code, "GR5");
	newfuels[105].h1 = 0.400000;
	newfuels[105].h10 = 0.000000;
	newfuels[105].h100 = 0.000000;
	newfuels[105].lh = 2.500000;
	newfuels[105].lw = 0.000000;
	newfuels[105].dynamic = 1;
	newfuels[105].sav1 = 1800;
	newfuels[105].savlh = 1600;
	newfuels[105].savlw = 1500;
	newfuels[105].depth = 1.500000;
	newfuels[105].xmext = 0.400000;
	newfuels[105].heatd = 8000.000000;
	newfuels[105].heatl = 8000.000000;
	strcpy(newfuels[105].desc, "Low load, humid climate grass");

	newfuels[106].number = 106;
	strcpy(newfuels[106].code, "GR6");
	newfuels[106].h1 = 0.100000;
	newfuels[106].h10 = 0.000000;
	newfuels[106].h100 = 0.000000;
	newfuels[106].lh = 3.400000;
	newfuels[106].lw = 0.000000;
	newfuels[106].dynamic = 1;
	newfuels[106].sav1 = 2200;
	newfuels[106].savlh = 2000;
	newfuels[106].savlw = 1500;
	newfuels[106].depth = 1.500000;
	newfuels[106].xmext = 0.400000;
	newfuels[106].heatd = 9000.000000;
	newfuels[106].heatl = 9000.000000;
	strcpy(newfuels[106].desc, "Moderate load, humid climate grass");

	newfuels[107].number = 107;
	strcpy(newfuels[107].code, "GR7");
	newfuels[107].h1 = 1.000000;
	newfuels[107].h10 = 0.000000;
	newfuels[107].h100 = 0.000000;
	newfuels[107].lh = 5.400000;
	newfuels[107].lw = 0.000000;
	newfuels[107].dynamic = 1;
	newfuels[107].sav1 = 2000;
	newfuels[107].savlh = 1800;
	newfuels[107].savlw = 1500;
	newfuels[107].depth = 3.000000;
	newfuels[107].xmext = 0.150000;
	newfuels[107].heatd = 8000.000000;
	newfuels[107].heatl = 8000.000000;
	strcpy(newfuels[107].desc, "High load, dry climate grass");

	newfuels[108].number = 108;
	strcpy(newfuels[108].code, "GR8");
	newfuels[108].h1 = 0.500000;
	newfuels[108].h10 = 1.000000;
	newfuels[108].h100 = 0.000000;
	newfuels[108].lh = 7.300000;
	newfuels[108].lw = 0.000000;
	newfuels[108].dynamic = 1;
	newfuels[108].sav1 = 1500;
	newfuels[108].savlh = 1300;
	newfuels[108].savlw = 1500;
	newfuels[108].depth = 4.000000;
	newfuels[108].xmext = 0.300000;
	newfuels[108].heatd = 8000.000000;
	newfuels[108].heatl = 8000.000000;
	strcpy(newfuels[108].desc, "High load, very coarse, humid climate grass");

	newfuels[109].number = 109;
	strcpy(newfuels[109].code, "GR9");
	newfuels[109].h1 = 1.000000;
	newfuels[109].h10 = 1.000000;
	newfuels[109].h100 = 0.000000;
	newfuels[109].lh = 9.000000;
	newfuels[109].lw = 0.000000;
	newfuels[109].dynamic = 1;
	newfuels[109].sav1 = 1800;
	newfuels[109].savlh = 1600;
	newfuels[109].savlw = 1500;
	newfuels[109].depth = 5.000000;
	newfuels[109].xmext = 0.400000;
	newfuels[109].heatd = 8000.000000;
	newfuels[109].heatl = 8000.000000;
	strcpy(newfuels[109].desc, "Very high load, humid climate grass");

	newfuels[121].number = 121;
	strcpy(newfuels[121].code, "GS1");
	newfuels[121].h1 = 0.200000;
	newfuels[121].h10 = 0.000000;
	newfuels[121].h100 = 0.000000;
	newfuels[121].lh = 0.500000;
	newfuels[121].lw = 0.650000;
	newfuels[121].dynamic = 1;
	newfuels[121].sav1 = 2000;
	newfuels[121].savlh = 1800;
	newfuels[121].savlw = 1800;
	newfuels[121].depth = 0.900000;
	newfuels[121].xmext = 0.150000;
	newfuels[121].heatd = 8000.000000;
	newfuels[121].heatl = 8000.000000;
	strcpy(newfuels[121].desc, "Low load, dry climate grass-shrub");

	newfuels[122].number = 122;
	strcpy(newfuels[122].code, "GS2");
	newfuels[122].h1 = 0.500000;
	newfuels[122].h10 = 0.500000;
	newfuels[122].h100 = 0.000000;
	newfuels[122].lh = 0.600000;
	newfuels[122].lw = 1.000000;
	newfuels[122].dynamic = 1;
	newfuels[122].sav1 = 2000;
	newfuels[122].savlh = 1800;
	newfuels[122].savlw = 1800;
	newfuels[122].depth = 1.500000;
	newfuels[122].xmext = 0.150000;
	newfuels[122].heatd = 8000.000000;
	newfuels[122].heatl = 8000.000000;
	strcpy(newfuels[122].desc, "Moderate load, dry climate grass-shrub");

	newfuels[123].number = 123;
	strcpy(newfuels[123].code, "GS3");
	newfuels[123].h1 = 0.300000;
	newfuels[123].h10 = 0.250000;
	newfuels[123].h100 = 0.000000;
	newfuels[123].lh = 1.450000;
	newfuels[123].lw = 1.250000;
	newfuels[123].dynamic = 1;
	newfuels[123].sav1 = 1800;
	newfuels[123].savlh = 1600;
	newfuels[123].savlw = 1600;
	newfuels[123].depth = 1.800000;
	newfuels[123].xmext = 0.400000;
	newfuels[123].heatd = 8000.000000;
	newfuels[123].heatl = 8000.000000;
	strcpy(newfuels[123].desc, "Moderate load, humid climate grass-shrub");

	newfuels[124].number = 124;
	strcpy(newfuels[124].code, "GS4");
	newfuels[124].h1 = 1.900000;
	newfuels[124].h10 = 0.300000;
	newfuels[124].h100 = 0.100000;
	newfuels[124].lh = 3.400000;
	newfuels[124].lw = 7.100000;
	newfuels[124].dynamic = 1;
	newfuels[124].sav1 = 1800;
	newfuels[124].savlh = 1600;
	newfuels[124].savlw = 1600;
	newfuels[124].depth = 2.100000;
	newfuels[124].xmext = 0.400000;
	newfuels[124].heatd = 8000.000000;
	newfuels[124].heatl = 8000.000000;
	strcpy(newfuels[124].desc, "High load, humid climate grass-shrub");

	newfuels[141].number = 141;
	strcpy(newfuels[141].code, "SH1");
	newfuels[141].h1 = 0.250000;
	newfuels[141].h10 = 0.250000;
	newfuels[141].h100 = 0.000000;
	newfuels[141].lh = 0.150000;
	newfuels[141].lw = 1.300000;
	newfuels[141].dynamic = 1;
	newfuels[141].sav1 = 2000;
	newfuels[141].savlh = 1800;
	newfuels[141].savlw = 1600;
	newfuels[141].depth = 1.000000;
	newfuels[141].xmext = 0.150000;
	newfuels[141].heatd = 8000.000000;
	newfuels[141].heatl = 8000.000000;
	strcpy(newfuels[141].desc, "Low load, dry climate shrub");

	newfuels[142].number = 142;
	strcpy(newfuels[142].code, "SH2");
	newfuels[142].h1 = 1.350000;
	newfuels[142].h10 = 2.400000;
	newfuels[142].h100 = 0.750000;
	newfuels[142].lh = 0.000000;
	newfuels[142].lw = 3.850000;
	newfuels[142].dynamic = 0;
	newfuels[142].sav1 = 2000;
	newfuels[142].savlh = 1800;
	newfuels[142].savlw = 1600;
	newfuels[142].depth = 1.000000;
	newfuels[142].xmext = 0.150000;
	newfuels[142].heatd = 8000.000000;
	newfuels[142].heatl = 8000.000000;
	strcpy(newfuels[142].desc, "Moderate load, dry climate shrub");

	newfuels[143].number = 143;
	strcpy(newfuels[143].code, "SH3");
	newfuels[143].h1 = 0.450000;
	newfuels[143].h10 = 3.000000;
	newfuels[143].h100 = 0.000000;
	newfuels[143].lh = 0.000000;
	newfuels[143].lw = 6.200000;
	newfuels[143].dynamic = 0;
	newfuels[143].sav1 = 1600;
	newfuels[143].savlh = 1800;
	newfuels[143].savlw = 1400;
	newfuels[143].depth = 2.400000;
	newfuels[143].xmext = 0.400000;
	newfuels[143].heatd = 8000.000000;
	newfuels[143].heatl = 8000.000000;
	strcpy(newfuels[143].desc, "Moderate load, humid climate shrub");

	newfuels[144].number = 144;
	strcpy(newfuels[144].code, "SH4");
	newfuels[144].h1 = 0.850000;
	newfuels[144].h10 = 1.150000;
	newfuels[144].h100 = 0.200000;
	newfuels[144].lh = 0.000000;
	newfuels[144].lw = 2.550000;
	newfuels[144].dynamic = 0;
	newfuels[144].sav1 = 2000;
	newfuels[144].savlh = 1800;
	newfuels[144].savlw = 1600;
	newfuels[144].depth = 3.000000;
	newfuels[144].xmext = 0.300000;
	newfuels[144].heatd = 8000.000000;
	newfuels[144].heatl = 8000.000000;
	strcpy(newfuels[144].desc, "Low load, humid climate timber-shrub");

	newfuels[145].number = 145;
	strcpy(newfuels[145].code, "SH5");
	newfuels[145].h1 = 3.600000;
	newfuels[145].h10 = 2.100000;
	newfuels[145].h100 = 0.000000;
	newfuels[145].lh = 0.000000;
	newfuels[145].lw = 2.900000;
	newfuels[145].dynamic = 0;
	newfuels[145].sav1 = 750;
	newfuels[145].savlh = 1800;
	newfuels[145].savlw = 1600;
	newfuels[145].depth = 6.000000;
	newfuels[145].xmext = 0.150000;
	newfuels[145].heatd = 8000.000000;
	newfuels[145].heatl = 8000.000000;
	strcpy(newfuels[145].desc, "High load, dry climate shrub");

	newfuels[146].number = 146;
	strcpy(newfuels[146].code, "SH6");
	newfuels[146].h1 = 2.900000;
	newfuels[146].h10 = 1.450000;
	newfuels[146].h100 = 0.000000;
	newfuels[146].lh = 0.000000;
	newfuels[146].lw = 1.400000;
	newfuels[146].dynamic = 0;
	newfuels[146].sav1 = 750;
	newfuels[146].savlh = 1800;
	newfuels[146].savlw = 1600;
	newfuels[146].depth = 2.000000;
	newfuels[146].xmext = 0.300000;
	newfuels[146].heatd = 8000.000000;
	newfuels[146].heatl = 8000.000000;
	strcpy(newfuels[146].desc, "Low load, humid climate shrub");

	newfuels[147].number = 147;
	strcpy(newfuels[147].code, "SH7");
	newfuels[147].h1 = 3.500000;
	newfuels[147].h10 = 5.300000;
	newfuels[147].h100 = 2.200000;
	newfuels[147].lh = 0.000000;
	newfuels[147].lw = 3.400000;
	newfuels[147].dynamic = 0;
	newfuels[147].sav1 = 750;
	newfuels[147].savlh = 1800;
	newfuels[147].savlw = 1600;
	newfuels[147].depth = 6.000000;
	newfuels[147].xmext = 0.150000;
	newfuels[147].heatd = 8000.000000;
	newfuels[147].heatl = 8000.000000;
	strcpy(newfuels[147].desc, "Very high load, dry climate shrub");

	newfuels[148].number = 148;
	strcpy(newfuels[148].code, "SH8");
	newfuels[148].h1 = 2.050000;
	newfuels[148].h10 = 3.400000;
	newfuels[148].h100 = 0.850000;
	newfuels[148].lh = 0.000000;
	newfuels[148].lw = 4.350000;
	newfuels[148].dynamic = 0;
	newfuels[148].sav1 = 750;
	newfuels[148].savlh = 1800;
	newfuels[148].savlw = 1600;
	newfuels[148].depth = 3.000000;
	newfuels[148].xmext = 0.400000;
	newfuels[148].heatd = 8000.000000;
	newfuels[148].heatl = 8000.000000;
	strcpy(newfuels[148].desc, "High load, humid climate shrub");

	newfuels[149].number = 149;
	strcpy(newfuels[149].code, "SH9");
	newfuels[149].h1 = 4.500000;
	newfuels[149].h10 = 2.450000;
	newfuels[149].h100 = 0.000000;
	newfuels[149].lh = 1.550000;
	newfuels[149].lw = 7.000000;
	newfuels[149].dynamic = 1;
	newfuels[149].sav1 = 750;
	newfuels[149].savlh = 1800;
	newfuels[149].savlw = 1500;
	newfuels[149].depth = 4.400000;
	newfuels[149].xmext = 0.400000;
	newfuels[149].heatd = 8000.000000;
	newfuels[149].heatl = 8000.000000;
	strcpy(newfuels[149].desc, "Very high load, humid climate shrub");

	newfuels[161].number = 161;
	strcpy(newfuels[161].code, "TU1");
	newfuels[161].h1 = 0.200000;
	newfuels[161].h10 = 0.900000;
	newfuels[161].h100 = 1.500000;
	newfuels[161].lh = 0.200000;
	newfuels[161].lw = 0.900000;
	newfuels[161].dynamic = 1;
	newfuels[161].sav1 = 2000;
	newfuels[161].savlh = 1800;
	newfuels[161].savlw = 1600;
	newfuels[161].depth = 0.600000;
	newfuels[161].xmext = 0.200000;
	newfuels[161].heatd = 8000.000000;
	newfuels[161].heatl = 8000.000000;
	strcpy(newfuels[161].desc, "Light load, dry climate timber-grass-shrub");

	newfuels[162].number = 162;
	strcpy(newfuels[162].code, "TU2");
	newfuels[162].h1 = 0.950000;
	newfuels[162].h10 = 1.800000;
	newfuels[162].h100 = 1.250000;
	newfuels[162].lh = 0.000000;
	newfuels[162].lw = 0.200000;
	newfuels[162].dynamic = 0;
	newfuels[162].sav1 = 2000;
	newfuels[162].savlh = 1800;
	newfuels[162].savlw = 1600;
	newfuels[162].depth = 1.000000;
	newfuels[162].xmext = 0.300000;
	newfuels[162].heatd = 8000.000000;
	newfuels[162].heatl = 8000.000000;
	strcpy(newfuels[162].desc, "Moderate load, humid climate timber-shrub");

	newfuels[163].number = 163;
	strcpy(newfuels[163].code, "TU3");
	newfuels[163].h1 = 1.100000;
	newfuels[163].h10 = 0.150000;
	newfuels[163].h100 = 0.250000;
	newfuels[163].lh = 0.650000;
	newfuels[163].lw = 1.100000;
	newfuels[163].dynamic = 1;
	newfuels[163].sav1 = 1800;
	newfuels[163].savlh = 1600;
	newfuels[163].savlw = 1400;
	newfuels[163].depth = 1.300000;
	newfuels[163].xmext = 0.300000;
	newfuels[163].heatd = 8000.000000;
	newfuels[163].heatl = 8000.000000;
	strcpy(newfuels[163].desc,
		"Moderate load, humid climate timber-grass-shrub");

	newfuels[164].number = 164;
	strcpy(newfuels[164].code, "TU4");
	newfuels[164].h1 = 4.500000;
	newfuels[164].h10 = 0.000000;
	newfuels[164].h100 = 0.000000;
	newfuels[164].lh = 0.000000;
	newfuels[164].lw = 2.000000;
	newfuels[164].dynamic = 0;
	newfuels[164].sav1 = 2300;
	newfuels[164].savlh = 1800;
	newfuels[164].savlw = 2000;
	newfuels[164].depth = 0.500000;
	newfuels[164].xmext = 0.120000;
	newfuels[164].heatd = 8000.000000;
	newfuels[164].heatl = 8000.000000;
	strcpy(newfuels[164].desc, "Dwarf conifer with understory");

	newfuels[165].number = 165;
	strcpy(newfuels[165].code, "TU5");
	newfuels[165].h1 = 4.000000;
	newfuels[165].h10 = 4.000000;
	newfuels[165].h100 = 3.000000;
	newfuels[165].lh = 0.000000;
	newfuels[165].lw = 3.000000;
	newfuels[165].dynamic = 0;
	newfuels[165].sav1 = 1500;
	newfuels[165].savlh = 1800;
	newfuels[165].savlw = 750;
	newfuels[165].depth = 1.000000;
	newfuels[165].xmext = 0.250000;
	newfuels[165].heatd = 8000.000000;
	newfuels[165].heatl = 8000.000000;
	strcpy(newfuels[165].desc, "Very high load, dry climate timber-shrub");

	newfuels[181].number = 181;
	strcpy(newfuels[181].code, "TL1");
	newfuels[181].h1 = 1.000000;
	newfuels[181].h10 = 2.200000;
	newfuels[181].h100 = 3.600000;
	newfuels[181].lh = 0.000000;
	newfuels[181].lw = 0.000000;
	newfuels[181].dynamic = 0;
	newfuels[181].sav1 = 2000;
	newfuels[181].savlh = 1800;
	newfuels[181].savlw = 1600;
	newfuels[181].depth = 0.200000;
	newfuels[181].xmext = 0.300000;
	newfuels[181].heatd = 8000.000000;
	newfuels[181].heatl = 8000.000000;
	strcpy(newfuels[181].desc, "Low load, compact conifer litter");

	newfuels[182].number = 182;
	strcpy(newfuels[182].code, "TL2");
	newfuels[182].h1 = 1.400000;
	newfuels[182].h10 = 2.300000;
	newfuels[182].h100 = 2.200000;
	newfuels[182].lh = 0.000000;
	newfuels[182].lw = 0.000000;
	newfuels[182].dynamic = 0;
	newfuels[182].sav1 = 2000;
	newfuels[182].savlh = 1800;
	newfuels[182].savlw = 1600;
	newfuels[182].depth = 0.200000;
	newfuels[182].xmext = 0.250000;
	newfuels[182].heatd = 8000.000000;
	newfuels[182].heatl = 8000.000000;
	strcpy(newfuels[182].desc, "Low load broadleaf litter");

	newfuels[183].number = 183;
	strcpy(newfuels[183].code, "TL3");
	newfuels[183].h1 = 0.500000;
	newfuels[183].h10 = 2.200000;
	newfuels[183].h100 = 2.800000;
	newfuels[183].lh = 0.000000;
	newfuels[183].lw = 0.000000;
	newfuels[183].dynamic = 0;
	newfuels[183].sav1 = 2000;
	newfuels[183].savlh = 1800;
	newfuels[183].savlw = 1600;
	newfuels[183].depth = 0.300000;
	newfuels[183].xmext = 0.200000;
	newfuels[183].heatd = 8000.000000;
	newfuels[183].heatl = 8000.000000;
	strcpy(newfuels[183].desc, "Moderate load confier litter");

	newfuels[184].number = 184;
	strcpy(newfuels[184].code, "TL4");
	newfuels[184].h1 = 0.500000;
	newfuels[184].h10 = 1.500000;
	newfuels[184].h100 = 4.200000;
	newfuels[184].lh = 0.000000;
	newfuels[184].lw = 0.000000;
	newfuels[184].dynamic = 0;
	newfuels[184].sav1 = 2000;
	newfuels[184].savlh = 1800;
	newfuels[184].savlw = 1600;
	newfuels[184].depth = 0.400000;
	newfuels[184].xmext = 0.250000;
	newfuels[184].heatd = 8000.000000;
	newfuels[184].heatl = 8000.000000;
	strcpy(newfuels[184].desc, "Small downed logs");

	newfuels[185].number = 185;
	strcpy(newfuels[185].code, "TL5");
	newfuels[185].h1 = 1.150000;
	newfuels[185].h10 = 2.500000;
	newfuels[185].h100 = 4.400000;
	newfuels[185].lh = 0.000000;
	newfuels[185].lw = 0.000000;
	newfuels[185].dynamic = 0;
	newfuels[185].sav1 = 2000;
	newfuels[185].savlh = 1800;
	newfuels[185].savlw = 1600;
	newfuels[185].depth = 0.600000;
	newfuels[185].xmext = 0.250000;
	newfuels[185].heatd = 8000.000000;
	newfuels[185].heatl = 8000.000000;
	strcpy(newfuels[185].desc, "High load conifer litter");

	newfuels[186].number = 186;
	strcpy(newfuels[186].code, "TL6");
	newfuels[186].h1 = 2.400000;
	newfuels[186].h10 = 1.200000;
	newfuels[186].h100 = 1.200000;
	newfuels[186].lh = 0.000000;
	newfuels[186].lw = 0.000000;
	newfuels[186].dynamic = 0;
	newfuels[186].sav1 = 2000;
	newfuels[186].savlh = 1800;
	newfuels[186].savlw = 1600;
	newfuels[186].depth = 0.300000;
	newfuels[186].xmext = 0.250000;
	newfuels[186].heatd = 8000.000000;
	newfuels[186].heatl = 8000.000000;
	strcpy(newfuels[186].desc, "High load broadleaf litter");

	newfuels[187].number = 187;
	strcpy(newfuels[187].code, "TL7");
	newfuels[187].h1 = 0.300000;
	newfuels[187].h10 = 1.400000;
	newfuels[187].h100 = 8.100000;
	newfuels[187].lh = 0.000000;
	newfuels[187].lw = 0.000000;
	newfuels[187].dynamic = 0;
	newfuels[187].sav1 = 2000;
	newfuels[187].savlh = 1800;
	newfuels[187].savlw = 1600;
	newfuels[187].depth = 0.400000;
	newfuels[187].xmext = 0.250000;
	newfuels[187].heatd = 8000.000000;
	newfuels[187].heatl = 8000.000000;
	strcpy(newfuels[187].desc, "Large downed logs");

	newfuels[188].number = 188;
	strcpy(newfuels[188].code, "TL8");
	newfuels[188].h1 = 5.800000;
	newfuels[188].h10 = 1.400000;
	newfuels[188].h100 = 1.100000;
	newfuels[188].lh = 0.000000;
	newfuels[188].lw = 0.000000;
	newfuels[188].dynamic = 0;
	newfuels[188].sav1 = 1800;
	newfuels[188].savlh = 1800;
	newfuels[188].savlw = 1600;
	newfuels[188].depth = 0.300000;
	newfuels[188].xmext = 0.350000;
	newfuels[188].heatd = 8000.000000;
	newfuels[188].heatl = 8000.000000;
	strcpy(newfuels[188].desc, "Long-needle litter");

	newfuels[189].number = 189;
	strcpy(newfuels[189].code, "TL9");
	newfuels[189].h1 = 6.650000;
	newfuels[189].h10 = 3.300000;
	newfuels[189].h100 = 4.150000;
	newfuels[189].lh = 0.000000;
	newfuels[189].lw = 0.000000;
	newfuels[189].dynamic = 0;
	newfuels[189].sav1 = 1800;
	newfuels[189].savlh = 1800;
	newfuels[189].savlw = 1600;
	newfuels[189].depth = 0.600000;
	newfuels[189].xmext = 0.350000;
	newfuels[189].heatd = 8000.000000;
	newfuels[189].heatl = 8000.000000;
	strcpy(newfuels[189].desc, "Very high load broadleaf litter");

	newfuels[201].number = 201;
	strcpy(newfuels[201].code, "SB1");
	newfuels[201].h1 = 1.500000;
	newfuels[201].h10 = 3.000000;
	newfuels[201].h100 = 11.000000;
	newfuels[201].lh = 0.000000;
	newfuels[201].lw = 0.000000;
	newfuels[201].dynamic = 0;
	newfuels[201].sav1 = 2000;
	newfuels[201].savlh = 1800;
	newfuels[201].savlw = 1600;
	newfuels[201].depth = 1.000000;
	newfuels[201].xmext = 0.250000;
	newfuels[201].heatd = 8000.000000;
	newfuels[201].heatl = 8000.000000;
	strcpy(newfuels[201].desc, "Low load activity fuel");

	newfuels[202].number = 202;
	strcpy(newfuels[202].code, "SB2");
	newfuels[202].h1 = 4.500000;
	newfuels[202].h10 = 4.250000;
	newfuels[202].h100 = 4.000000;
	newfuels[202].lh = 0.000000;
	newfuels[202].lw = 0.000000;
	newfuels[202].dynamic = 0;
	newfuels[202].sav1 = 2000;
	newfuels[202].savlh = 1800;
	newfuels[202].savlw = 1600;
	newfuels[202].depth = 1.000000;
	newfuels[202].xmext = 0.250000;
	newfuels[202].heatd = 8000.000000;
	newfuels[202].heatl = 8000.000000;
	strcpy(newfuels[202].desc, "Moderate load activity or low load blowdown");

	newfuels[203].number = 203;
	strcpy(newfuels[203].code, "SB3");
	newfuels[203].h1 = 5.500000;
	newfuels[203].h10 = 2.750000;
	newfuels[203].h100 = 3.000000;
	newfuels[203].lh = 0.000000;
	newfuels[203].lw = 0.000000;
	newfuels[203].dynamic = 0;
	newfuels[203].sav1 = 2000;
	newfuels[203].savlh = 1800;
	newfuels[203].savlw = 1600;
	newfuels[203].depth = 1.200000;
	newfuels[203].xmext = 0.250000;
	newfuels[203].heatd = 8000.000000;
	newfuels[203].heatl = 8000.000000;
	strcpy(newfuels[203].desc,
		"High load activity fuel or moderate load blowdown");

	newfuels[204].number = 204;
	strcpy(newfuels[204].code, "SB4");
	newfuels[204].h1 = 5.250000;
	newfuels[204].h10 = 3.500000;
	newfuels[204].h100 = 5.250000;
	newfuels[204].lh = 0.000000;
	newfuels[204].lw = 0.000000;
	newfuels[204].dynamic = 0;
	newfuels[204].sav1 = 2000;
	newfuels[204].savlh = 1800;
	newfuels[204].savlw = 1600;
	newfuels[204].depth = 2.700000;
	newfuels[204].xmext = 0.250000;
	newfuels[204].heatd = 8000.000000;
	newfuels[204].heatl = 8000.000000;
	strcpy(newfuels[204].desc, "High load blowdown");
}

void trim(char* totrim)
{
	for (int i = strlen(totrim) - 1; i > 0; i--)
	{
		switch (totrim[i])
		{
		case '\n':
		case '\r':
		case '\t':
		case ' ':
			totrim[i] = '\0';
			break;
		default:
			i = -1;
		}
	}
	bool found=false; 
	while(!found)
	{
		int j = 0;
		switch (totrim[0])
		{
		case '\n':
		case '\t':
		case ' ':
			for (j = 0; j < (int)strlen(totrim) - 1; j++)
				totrim[j] = totrim[j + 1];
			totrim[j] = '\0'; 
			break;
		default:
			found=true;
			break;
		}
	}
}









