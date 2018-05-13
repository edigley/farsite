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
#include "fsxwinpt.h"
#include <string.h>
#include <unistd.h>
#include "portablestrings.h"
#include "fcopy.h"
#include <sys/stat.h>

//#include "ftestrc.h"
//#include "fsxwhelp.h"
//#include "wwedits.h"
//#include "fsramp.h"
//#include "fswinpedits.h"
//#include <owl\picklist.h>
//#include <dir.h>

//extern bool VIEWLCP_ON, F_ON, THREED_ON;
//extern bool FARSITE_GO;
//extern bool GEN_ON, CMP_ON, CAN_ON;
//extern bool WWDEF_ON, WWGEN_ON, CanTryTerm, NEWSCREEN;
//bool CONV_ON = false, VIS_ON = false, CNST_ON = false;

#define NODATA -9999                          
#define IDYES 1                          


//------------------------------------------------------------------------------
//
//	FarInputs Struct Functions
//
//------------------------------------------------------------------------------


FarInputs::FarInputs()
{
	//Client=0;
	ResetData();
	SVGA_ON = false;
        inited=false;
        m_shapeFileInited=false;
        m_gridEastNorthSet=false;
        m_gridRowSet=false;
	m_BurnPeriodIsHere=false;
        m_dcount=0;
 	pathdiv[0] = '/';

}


void FarInputs::ResetData()
{
	memset(LandscapeFile, 0x0, sizeof(LandscapeFile));
	memset(BookmarkFile, 0x0, sizeof(BookmarkFile));
	memset(ProjectFile, 0x0, sizeof(ProjectFile));
	memset(AdjustmentFile, 0x0, sizeof(AdjustmentFile));
	memset(ConvertFile, 0x0, sizeof(ConvertFile));
	memset(FuelModelFile, 0x0, sizeof(FuelModelFile));
	memset(FuelMoistureFile, 0x0, sizeof(FuelMoistureFile));
	memset(BurnPeriod, 0x0, sizeof(BurnPeriod));
	memset(CoarseWoody, 0x0, sizeof(CoarseWoody));
	memset(MBLand, 0x0, sizeof(MBLand));
	memset(MBAdj, 0x0, sizeof(MBAdj));
	memset(MBMois, 0x0, sizeof(MBMois));
	memset(MBConv, 0x0, sizeof(MBConv));
	memset(MBFuelMods, 0x0, sizeof(MBFuelMods));
	memset(MBBpd, 0x0, sizeof(MBBpd));
	memset(MBCwd, 0x0, sizeof(MBBpd));
	for (long i = 0; i < 5; i++)
	{
		memset(WindFile[i], 0x0, sizeof(WindFile[i]));
		memset(WeatherFile[i], 0x0, sizeof(WeatherFile[i]));
		strcpy(MBWtr[i], "Load Weather");
		strcpy(MBWnd[i], "Load Winds");
	}
	LandID = false;
	WtrID = false;
	WndID = false;
	AdjID = false;
	FuelMoisID = false;
	BpdID = false;
	CwdID = false;
	SetCustFuelModelID(false);
	SetConvFuelModelID(false);
	//Changes.ResetChanges();
}


void FarInputs::InitializeLandscapeFile()
{
	bool cust = HaveCustomFuelModels();
	bool conv = HaveFuelConversions();

	//     ::ShowCursor(FALSE);
	//     HCURSOR wait=::LoadCursor(NULL, IDC_WAIT);
	//     ::SetCursor(wait);
	//     ::ShowCursor(TRUE);
	ReadHeader();
	//     ShowCursor(FALSE);
	//	HCURSOR arrow=::LoadCursor(NULL, IDC_ARROW);
	//     ::SetCursor(arrow);
	//     ::ShowCursor(TRUE);

	SetCustFuelModelID(cust);
	SetConvFuelModelID(conv);
	AllocStationGrid(1, 1);   	   // initialize station grid to 1
	LandID = true;
	/*	if(GetNumEast()>GetNumNorth())   //SETS PIXEL SPACING AT MINIMUM X/Y DIMENSION
			SetFullTint(8000.0/double (GetNumEast()));
		else
			SetFullTint(8000.0/double (GetNumNorth()));
		if(GetNumViewEast()>GetNumViewNorth())   //SETS PIXEL SPACING AT MINIMUM X/Y DIMENSION
		{    SetTint(8000.0/double (GetNumViewEast()));
			SetMaxWindowDim(GetTint()*GetNumViewEast());
		}
		else
		{	SetTint(8000.0/double (GetNumViewNorth()));
			SetMaxWindowDim(GetTint()*GetNumViewNorth());
		}
	*/
	char RevCopy[256];
	char pathdiv[] = "/";              		// search for path
	strcpy(RevCopy, GetLandFileName());	// copy file path string
	strrev(RevCopy);						// reverse file path string
	int length = strcspn(RevCopy, pathdiv);   // compute length without path
	memset(MBLand, 0x0, sizeof MBLand);
	strncpy(MBLand, RevCopy, length);		// copy length w/o path
	strrev(MBLand);						// reverse to get filename only
	//ViewPortStatus(0);
	//SetLandscapeViewPort(GetNorthUtm(), GetSouthUtm(), GetEastUtm(), GetWestUtm());
	//	WriteMessageBar(0);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

bool FarInputs::TestForAtmWeather()
{
	// test for gridded atmosphere inputs file type
	char TestString[256] = "";

	fscanf(CurrentFile, "%s", TestString);
	if (!strcmp("WEATHER_AND_WINDS", strupr(TestString)))
		return true;

	return false;
}


bool FarInputs::TestForAtmWinds()
{
	// test for gridded wind inputs file type
	char TestString[256] = "";

	fscanf(CurrentFile, "%s", TestString);
	if (!strcmp("WINDS_AND_CLOUDS", strupr(TestString)))
		return true;

	return false;
}


bool FarInputs::LoadWeatherData(long FileNum)
{
	long StationNumber;

	if (TestForAtmWeather())
	{
		printf(" FARSITE Version 4.0 cannot currently use gridded Temp, Humid, and Precip Files                	Can't Load Atmosphere Grid File, for WTR in FARSITE v4.0\n");		return false;

		if (LandID == false)
		{
			printf(" Load Landscape File First                 	Can't Load Atmosphere Grid File\n");
			return false;
		}
		if (FileNum > 0)
		{
			printf(" Grid Must Be Loaded As WTR File 1                 	Can't Load Atmosphere Grid File\n");
			return false;
		}
		EnvironmentChanged(1, 0, 0);
		EnvironmentChanged(1, 0, 1);
		EnvironmentChanged(1, 0, 2);
		EnvironmentChanged(1, 0, 3);
		SetAtmosphereGrid(6);
		fclose(CurrentFile);
		//		 ::ShowCursor(FALSE);
		//     	HCURSOR wait=::LoadCursor(NULL, IDC_WAIT);
		//		 ::SetCursor(wait);
		//     	::ShowCursor(TRUE);
		if (!GetAtmosphereGrid()->ReadInputTable(WeatherFile[FileNum]))
		{
			//ShowCursor(FALSE);
			//			HCURSOR arrow=::LoadCursor(NULL, IDC_ARROW);
			//			 ::SetCursor(arrow);
			//			::ShowCursor(TRUE);
			printf(" GetAtmosphereGrid()->ErrMsg            				Error, Bad Atmophere Grid File\n");			//fclose(CurrentFile);
			SetAtmosphereGrid(0);	// delete and reset Atmosphere Grid

			return false;
		}
		//		 ShowCursor(FALSE);
		//		HCURSOR arrow=::LoadCursor(NULL, IDC_ARROW);
		//		 ::SetCursor(arrow);
		//     	::ShowCursor(TRUE);
		SetSimulationDuration(0.0);//numaximum=0; // set duration to 0 to force resetting duration
		//for(long i=0; i<5; i++)	// removed - already done in next function
		//{	FreeWeatherData(i);
		//	FreeWindData(i);
		//}
		WndID = true;
		SetCorrectStreamNumbers();	// adjust number of weather streams to 1

		return true;
	}
	else
	{
		//for(long i=0; i<5; i++)
		//{	FreeWeatherData(i);
		//	FreeWindData(i);
		//}
		if (AtmosphereGridExists() == 2)
			SetAtmosphereGrid(0);
	}

	bool Metric = false, BadData = false;
	long i, fpos = 0, count = 0, num;
	double month, day, hmorn, haft, Hmax, Hmin;
	double ppt, Tmin, Tmax, elref, prevmo, prevday, tr1, tr2;
	char Line[256] = "", ErMsg[256] = "", head[256] = "";

	SetSimulationDuration(0.0);//numaximum=0; // set duration to 0 to force resetting duration
	if (AtmosphereGridExists() == 1 && FileNum > 0)
		SetCorrectStreamNumbers();     // make sure stream number match with wind grid
	rewind(CurrentFile);

	memset(Line, 0x0, sizeof(Line));
	fgets(Line, 255, CurrentFile);
	sscanf(Line, "%s", head);
	if (!strcmp(strlwr(head), "metric"))
	{
		Metric = true;
		fpos = ftell(CurrentFile);
	}
	else if (!strcmp(strlwr(head), "english"))
	{
		Metric = false;
		fpos = ftell(CurrentFile);
	}
	else						// no header, english default
		rewind(CurrentFile);

	prevmo = prevday = -1;
	while (!feof(CurrentFile))
	{
		memset(Line, 0x0, 256 * sizeof(char));
		fgets(Line, 255, CurrentFile);
		if (strlen(Line) == 0 || !strncmp(Line, "\n", 1))
		{
			if (count <= 0)
				count--;
			continue;
		}
		//if(feof(CurrentFile))
		//     break;
		num = sscanf(Line, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
				&month, &day, &ppt, &hmorn, &haft, &Tmin, &Tmax, &Hmax, &Hmin,
				&elref, &tr1, &tr2);
		if (num < 10)
		{
			sprintf(ErMsg, "%s %ld", "Weather Data Error, Line -> ", count);
			printf(" Too Few Fields in WTR Record  ErMsg\n");			fclose(CurrentFile);

			return false;
		}
		else if (num == 10)
			tr1 = tr2 = 0;
		count++;
		if (month<1 || month>12)
		{
			sprintf(ErMsg, "%s %ld", "Weather Data Error, Line -> ", count);
			printf("  Month out of range (1-12)  ErMsg\n");			BadData = true;
		}
		else if (day<1 || day>31)
		{
			sprintf(ErMsg, "%s %ld", "Weather Data Error Line -> ", count);
			printf(" Day out of range (1-31)  ErMsg\n");			BadData = true;
		}
		else if (hmorn<0 || haft>2359)
		{
			sprintf(ErMsg, "%s %ld", "Weather Data Error, Line -> ", count);
			printf(" Hour out of range (0-2359)  ErMsg\n");			BadData = true;
		}
		else if (Hmax > 99 || Hmin < 2)
		{
			sprintf(ErMsg, "%s %ld", "Weather Data Error, Line -> ", count);
			printf(" Humidity out of range (0-99 percent)  ErMsg\n");			BadData = true;
		}
		else if (elref < 0)
		{
			sprintf(ErMsg, "%s %ld", "Weather Data Error, Line -> ", count);
			printf(" Elevation Too Low (>0)  ErMsg\n");			BadData = true;
		}
		else if (prevmo == month && prevday > day)
		{
			sprintf(ErMsg, "%s %ld", "Weather Data Error, Line -> ", count);
			printf(" Days in descending order  ErMsg\n");			BadData = true;
		}
		else if (prevmo == month && fabs(prevday - day) > 1.0)
		{
			sprintf(ErMsg, "%s %ld", "Weather Data Error, Line -> ", count);
			printf(" Day appears to be missing  ErMsg\n");			BadData = true;
		}
		if (ppt > 0.0)
		{
			if (tr1<0 || tr2>2359)
			{
				sprintf(ErMsg, "%s %ld", "Weather Data Error, Line -> ", count);
				printf(" Rain Start or End Hour out of range (0-2359)  ErMsg\n");				BadData = true;
			}
			else if (tr1 >= tr2 && tr1 != 0)
			{
				sprintf(ErMsg, "%s %ld", "Weather Data Error, Line -> ", count);
				printf(" Rain Start Hour after Rain End Hour  ErMsg\n");				BadData = true;
			}
		}

		if (BadData)
		{
			fclose(CurrentFile);

			return false;
		}
		prevmo = month;
		prevday = day;
	}
	StationNumber = AllocWeatherData(FileNum, count);
	if (StationNumber >= 0)
	{
		fseek(CurrentFile, fpos, SEEK_SET);
		for (i = 0; i < count; i++)
		{
			memset(Line, 0x0, 256 * sizeof(char));
			fgets(Line, 255, CurrentFile);
			if (strlen(Line) == 0 || !strncmp(Line, "\n", 1))
			{
				i--;
				continue;
			}
			num = sscanf(Line,
					"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &month,
					&day, &ppt, &hmorn, &haft, &Tmin, &Tmax, &Hmax, &Hmin,
					&elref, &tr1, &tr2);
			if (ppt == 0.0)
			{
				tr1 = 0.0;
				tr2 = 0.0;
			}
			if (num < 10)
				break;
			if (Metric) 		 // convert to english
			{
				ppt *= 3.93;	 // convert to 1/100 inch
				Tmin *= 1.8;
				Tmin += 32.0;
				Tmax *= 1.8;
				Tmax += 32.0;
				elref *= 3.28;
				SetWeatherUnits(StationNumber, WW_UNITS_METRIC);
			}
			else
				SetWeatherUnits(StationNumber, WW_UNITS_ENGLISH);

			if (Hmax > 99)
				Hmax = 99;
			if (Hmin > 99)
				Hmin = 99;
			SetWeatherData(StationNumber, i, (long) month, (long) day, ppt,
				(long) hmorn, (long) haft, Tmin, Tmax, (long) Hmax,
				(long) Hmin, elref, (long) tr1, (long) tr2);
		}
		SetWeatherData(StationNumber, i, 13, (long) day, ppt, (long) hmorn,
			(long) haft, Tmin, Tmax, (long) Hmax, (long) Hmin, elref,
			(long) tr1, (long) tr2);
		fclose(CurrentFile);	 // 13 indicates end of weather inputs
	}
	else
	{
		SetWeatherUnits(StationNumber, WW_UNITS_UNDEFINED);

		return false;
	}

	//	ResetDuration();   // reset duration dialog for simulation because other
	return true;		// weather streams may be loaded, changing the available range of dates
}


//------------------------------------------------------------------------------
bool FarInputs::TestProjectFileVersion()
{
	FILE* pf;
	long HEAD;
	char header[8];

	memset(header, 0x0, sizeof(header));
	//if((pf=fopen(bookmark.ProjectFile, "r"))!=NULL)
	if ((pf = fopen(ProjectFile, "r")) != NULL)
	{
		rewind(pf);
		fscanf(pf, "%s", header);
		HEAD = atol(header);
		if (HEAD < 10)
		{
			printf("Project Will Be Converted to Version 4.0 and Old Project Renamed (.FP1) Project File Version 1x Detected\n");			if (true)
			{
				char OldProjFile[256], NewProjFile[256];
				memset(NewProjFile, 0x0, sizeof(NewProjFile));
				memset(OldProjFile, 0x0, sizeof(OldProjFile));
				//strncpy(OldProjFile, bookmark.ProjectFile, strlen(bookmark.ProjectFile)-4);
				strcpy(NewProjFile, ProjectFile);
				strncpy(OldProjFile, ProjectFile, strlen(ProjectFile) - 4);
				strcat(OldProjFile, ".FP1");
				//CopyFile(bookmark.ProjectFile, OldProjFile, false);
				fcopy(ProjectFile, OldProjFile);
				fclose(pf);

				if (OpenVersion1ProjectFile())
				{
					// rename landscape file inside FP1 file
					char lcpname[256], newname[256];
					memset(lcpname, 0x0, sizeof(lcpname));
					memset(newname, 0x0, sizeof(newname));
					pf = fopen(OldProjFile, "r+");
					if (pf == NULL)
					{
						//SetFileAttributes(OldProjFile, FILE_ATTRIBUTE_NORMAL);
						pf = fopen(OldProjFile, "r+");
					}
					rewind(pf);
					fscanf(pf, "%s", header);
					HEAD = ftell(pf) + 1;
					fscanf(pf, "%s", lcpname);
					strncpy(newname, lcpname, strlen(lcpname) - 4);
					strcat(newname, ".LCP1");
					fseek(pf, HEAD, SEEK_SET);
					fprintf(pf, "%s", newname);
					fclose(pf);

					strcpy(ProjectFile, NewProjFile);
					MakeProjectFile();
				}
				else
					return false;
			}
			else
			{
				fclose(pf);
				return false;
			}
		}
		else if (HEAD < 20)
		{
			printf("Project Will Be Converted to Version 4.0 and Old Project Renamed (.FP3) Project File Version 3x Detected\n");			if (IDYES == IDYES)
			{
				char OldProjFile[256], NewProjFile[256];
				memset(NewProjFile, 0x0, sizeof(NewProjFile));
				memset(OldProjFile, 0x0, sizeof(OldProjFile));
				//strncpy(OldProjFile, bookmark.ProjectFile, strlen(bookmark.ProjectFile)-4);
				strcpy(NewProjFile, ProjectFile);
				strncpy(OldProjFile, ProjectFile, strlen(ProjectFile) - 4);
				strcat(OldProjFile, ".FP3");
				//CopyFile(bookmark.ProjectFile, OldProjFile, false);
				fcopy(ProjectFile, OldProjFile);
				fclose(pf);

				if (OpenVersion2ProjectFile())
				{
					char lcpname[256], newname[256];
					memset(lcpname, 0x0, sizeof(lcpname));
					memset(newname, 0x0, sizeof(newname));
					pf = fopen(OldProjFile, "r+");
					if (pf == NULL)
					{
						//SetFileAttributes(OldProjFile, FILE_ATTRIBUTE_NORMAL);
						pf = fopen(OldProjFile, "r+");
					}
					rewind(pf);
					fscanf(pf, "%s", header);
					HEAD = ftell(pf) + 2;
					fscanf(pf, "%s", lcpname);
					strncpy(newname, lcpname, strlen(lcpname) - 4);
					strcat(newname, ".LC3");
					fseek(pf, HEAD, SEEK_SET);
					fprintf(pf, "%s", newname);
					fclose(pf);

					strcpy(ProjectFile, NewProjFile);
					MakeProjectFile();
				}
				else
					return false;
			}
			else
			{
				fclose(pf);

				return false;
			}
		}
		else
		{
			fclose(pf);
			if (!OpenVersion4ProjectFile())
			{
				return false;
			}
		}
	}
	else
		return false;

	return true;
}



bool FarInputs::LoadWindData(long FileNum)
{
	if (TestForAtmWinds())
	{
		if (LandID == false)
		{
			printf(" Load Landscape File First                 	Can't Load Atmosphere Grid File\n");
			return false;
		}
		if (FileNum > 0)
		{
			printf(" Grid Must Be Loaded As WND File 1                 	Can't Load Atmosphere Grid File\n");
			return false;
		}
		EnvironmentChanged(1, 0, 0);
		EnvironmentChanged(1, 0, 1);
		EnvironmentChanged(1, 0, 2);
		EnvironmentChanged(1, 0, 3);
		SetAtmosphereGrid(3);
		fclose(CurrentFile);
		//		 ::ShowCursor(FALSE);
		//     	HCURSOR wait=::LoadCursor(NULL, IDC_WAIT);
		//		 ::SetCursor(wait);
		//     	::ShowCursor(TRUE);
		if (!GetAtmosphereGrid()->ReadInputTable(WindFile[FileNum]))
		{
			//ShowCursor(FALSE);
			//			HCURSOR arrow=::LoadCursor(NULL, IDC_ARROW);
			//			 ::SetCursor(arrow);
			//			 ::ShowCursor(TRUE);
			printf(" GetAtmosphereGrid()->ErrMsg            				Error, Bad Atmophere Grid File\n");			//fclose(CurrentFile);
			SetAtmosphereGrid(0);	// delete and reset Atmosphere Grid

			return false;
		}
		//		 ShowCursor(FALSE);
		//		HCURSOR arrow=::LoadCursor(NULL, IDC_ARROW);
		//     	::SetCursor(arrow);
		//		 ::ShowCursor(TRUE);
		SetSimulationDuration(0.0);//numaximum=0; // set duration to 0 to force resetting duration
		//for(long i=0; i<5; i++)     // removed - already done in next function
		//	FreeWindData(i);
		SetCorrectStreamNumbers();	// adjust number of wind streams to
		// match weather streams

		return true;
	}
	else
	{
		//for(long i=0; i<5; i++)
		//	FreeWindData(i);
		if (AtmosphereGridExists())
		{
			EnvironmentChanged(1, 0, 0);
			EnvironmentChanged(1, 0, 1);
			EnvironmentChanged(1, 0, 2);
			EnvironmentChanged(1, 0, 3);
		}

		SetAtmosphereGrid(0);
	}

	long count = 0;
	double wss, month, wwwinddir, day, hhour, cloudcover;
	bool Metric = false, BadData = false;
	long i, fpos = 0, num;
	char Line[256] = "", ErMsg[256] = "", head[64] = "";

	SetSimulationDuration(0.0);//numaximum=0;	// for resetting duration , duration =0;
	rewind(CurrentFile);

	//fscanf(CurrentFile, "%s", UnitsString);
	memset(Line, 0x0, sizeof(Line));
	fgets(Line, 255, CurrentFile);
	sscanf(Line, "%s", head);
	if (!strcmp(strlwr(head), "metric"))
	{
		Metric = true;
		fpos = ftell(CurrentFile);
	}
	else if (!strcmp(strlwr(head), "english"))
	{
		Metric = false;
		fpos = ftell(CurrentFile);
	}
	else
		rewind(CurrentFile);

	while (!feof(CurrentFile))
	{
		memset(Line, 0x0, 256 * sizeof(char));
		fgets(Line, 255, CurrentFile);
		if (strlen(Line) == 0 || !strncmp(Line, "\n", 1))
		{
			if (count <= 0)
				count--;
			continue;
		}
		if (feof(CurrentFile))
			break;
		num = sscanf(Line, "%lf %lf %lf %lf %lf %lf", &month, &day, &hhour,
				&wss, &wwwinddir, &cloudcover);
		if (num < 6)
			break;
		count++;
		if (month<1 || month>12)
		{
			sprintf(ErMsg, "%s %ld", "Wind Data Error Line ->", count);
			printf(" Month Out of Range (1-12)  ErMsg\n");			BadData = true;
		}
		else if (day > 31 || day < 1)
		{
			sprintf(ErMsg, "%s %ld", "Wind Data Error Line ->", count);
			printf(" Day Out of Range (1-31)  ErMsg\n");			BadData = true;
		}
		else if (hhour<0 || hhour>2359)
		{
			sprintf(ErMsg, "%s %ld", "Wind Data Error Line ->", count);
			printf(" Hour Out of Range (0-2359)  ErMsg\n");			BadData = true;
		}
		else if (cloudcover > 100 || cloudcover < 0)
		{
			sprintf(ErMsg, "%s %ld", "Wind Data Error Line ->", count);
			printf(" Cloud Cover Out of Range (0-100)  ErMsg\n");			BadData = true;
		}
		if (BadData)
		{
			fclose(CurrentFile);

			return false;
		}
	}
	long StationNumber = AllocWindData(FileNum, count);
	if (StationNumber >= 0)
	{
		fseek(CurrentFile, fpos, SEEK_SET);
		for (i = 0; i < count; i++)
		{
			memset(Line, 0x0, 256 * sizeof(char));
			fgets(Line, 255, CurrentFile);
			if (strlen(Line) == 0 || !strncmp(Line, "\n", 1))
			{
				i--;
				continue;
			}
			num = sscanf(Line, "%lf %lf %lf %lf %lf %lf", &month, &day,
					&hhour, &wss, &wwwinddir, &cloudcover);
			if (num < 6)
				break;
			if (Metric)
			{
				wss *= 0.5402; //(0.62125/1.15);	 10m wind kph to 20ft wind mph
				SetWindUnits(StationNumber, WW_UNITS_METRIC);
			}
			else
				SetWindUnits(StationNumber, WW_UNITS_ENGLISH);
			SetWindData(StationNumber, i, (long) month, (long) day,
				(long) hhour, wss, (long) wwwinddir, (long) cloudcover);
		}
		SetWindData(StationNumber, i, 13, (long) day, (long) hhour, wss,
			(long) wwwinddir, (long) cloudcover); // signal end of wind data
		fclose(CurrentFile);
	}
	else
	{
		SetWindUnits(StationNumber, WW_UNITS_UNDEFINED);
		return false;
	}

	//ResetDuration();  	  // reset duration dialog for simulation because other
	return true;			// wind streams may be loaded, changing the available range of dates
}


//------------------------------------------------------------------------------


bool FarInputs::LoadAdjustmentFile()
{
	char CharTest[256] = "";
	long FT;
	double RRF;

	fscanf(CurrentFile, "%s", CharTest);
	FT = atol(CharTest);
	if (FT == 0 || FT > 256)
	{
		printf(" Adjustment File Must NOT Have Header Information, See Help File Format Error, Possible Zero Fuel Model# or Character Data in File\n");		fclose(CurrentFile);

		return false;
	}
	rewind(CurrentFile);
	while (!feof(CurrentFile))
	{
		fscanf(CurrentFile, "%ld", &FT);
		if (feof(CurrentFile))
			break;
		fscanf(CurrentFile, "%lf", &RRF);
		if (FT > 256)
		{
			printf(" Correct Model Number in Adjustment File Before Continuing  				Detected: Fuel Model Number > 256 \n");			fclose(CurrentFile);

			return false;
		}
		if (RRF > 20.0 || RRF < 0.0)
		{
			printf(" Adjustment Factor <0.0 or >20.0, Correct Before Continuing  				Detected: Bad Adjustment Factor\n");			fclose(CurrentFile);

			return false;
		}
		SetRosRed((int) FT, RRF);
	}
	AdjID = true;
	fclose(CurrentFile);

	return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


bool FarInputs::LoadFuelMoistureFile()
{
	char CharTest[256] = "";
	long i, ModNum, F1, F10, F100, FLW, FLH;

	for (i = 0; i < 257; i++)
		SetInitialFuelMoistures(i, 0, 0, 0, 0, 0);

	fscanf(CurrentFile, "%s", CharTest);
	ModNum = atol(CharTest);
	if (ModNum == 0 || ModNum > 99)
	{
		printf(" Moisture File Must NOT Have Header Information, See Help       	File Format Error, Possible Zero Fuel Model# or Character Data in File\n");		fclose(CurrentFile);

		return false;
	}
	rewind(CurrentFile);
	while (!feof(CurrentFile))
	{
		fscanf(CurrentFile, "%ld", &ModNum);
		if (feof(CurrentFile))
			break;
		fscanf(CurrentFile, "%ld %ld %ld %ld %ld", &F1, &F10, &F100, &FLH,
			&FLW);
		if (ModNum > 256)
		{
			printf(" Correct Fuel Moisture File Before Proceeding  					Error: Model Number>256\n");
			return false;
		}
		if (F1 < 0 || F10 < 0 || F100 < 0 || FLW < 0 || FLH < 0)
		{
			printf(" Correct Before Proceeding  					Error: Fuel Moisture <0\n");
			return false;
		}
		SetInitialFuelMoistures(ModNum, F1, F10, F100, FLH, FLW);
	}
	fclose(CurrentFile);
	FuelMoisID = true;

	return true;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


bool FarInputs::LoadCustomModelFile()
{
	SetCustFuelModelID(false);
	char* ptr, dynamic[32] = "", code[32] = "";
	char Line[256] = "", head[64] = "", ErrMsg[256] = "",
		BackupFile[256] = "", comment[256] = "";
	bool Metric = false, BadFile = false;
	long num, count, ModNum, FileFormat = 0;
	double s1, slh, slw;
	NewFuel newfuel, newfuelm;
	FILE* fout;

	memset(&newfuel, 0x0, sizeof(NewFuel));
	memset(&newfuelm, 0x0, sizeof(NewFuel));
	do
	{
		rewind(CurrentFile);
		memset(head, 0x0, 64 * sizeof(char));
		memset(Line, 0x0, sizeof(Line));
		fgets(Line, 255, CurrentFile);
		sscanf(Line, "%s", head);
		AccessFuelModelUnits(0);
		if (!strcmp(strlwr(head), "metric"))
		{
			Metric = true;
			AccessFuelModelUnits(1);
		}
		else if (strcmp(strlwr(head), "english"))   // no header in file
		{
			if (atol(head) > 256)
			{
				printf(" Bad Header in File  Custom Fuel Model File Error\n");				fclose(CurrentFile);

				return false;
			}
			memset(head, 0x0, 64 * sizeof(char));
			rewind(CurrentFile);
		}
		if (FileFormat > 0)
		{
			if (FileFormat == 1)
			{
				strcat(BackupFile, FuelModelFile);
				strcat(BackupFile, ".old");
				fcopy(FuelModelFile, BackupFile);
				fclose(CurrentFile);
				CurrentFile = fopen(BackupFile, "r");
				//if((access(FuelModelFile, 02))==-1)
				//{	SetFileAttributes(FuelModelFile, FILE_ATTRIBUTE_NORMAL);
				//		 DeleteFile(FuelModelFile);
				// }
				fout = fopen(FuelModelFile, "w");
				if (Metric)
					fprintf(fout, "METRIC\n");
				else
					fprintf(fout, "ENGLISH\n");
			}
			else
				fout = NULL;
			break;
		}

		memset(Line, 0x0, 256 * sizeof(char));
		fgets(Line, 255, CurrentFile);
		if (feof(CurrentFile))
			break;
		if (strlen(Line) == 0 || !strncmp(Line, "\n", 1))
			continue;
		num = sscanf(Line, "%ld %s %lf %lf %lf %lf %lf %s", &ModNum, head,
				&newfuel.h10, &newfuel.h100, &newfuel.lh, &newfuel.lw, &s1,
				comment);
		//if(!strcmp(strlwr(comment), "dynamic"))
		//  	FileFormat=2;
		//else if(!strcmp(strlwr(comment), "static"))
		//	FileFormat=2;
		//else
		if (strstr(strlwr(comment), "d"))
			FileFormat = 2;
		else if (strstr(strlwr(comment), "s"))
			FileFormat = 2;
		else
			FileFormat = 1;
	}
	while (FileFormat > 0);

	count = 0;
	while (!feof(CurrentFile))
	{
		memset(Line, 0x0, 256 * sizeof(char));
		fgets(Line, 255, CurrentFile);
		if (feof(CurrentFile))
			break;
		if (strlen(Line) == 0 || !strncmp(Line, "\n", 1))
			continue;
		memset(comment, 0x0, 256 * sizeof(char));
		if (FileFormat == 1)
		{
			num = sscanf(Line,
					"%ld %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %s",
					&ModNum, &newfuel.h1, &newfuel.h10, &newfuel.h100,
					&newfuel.lh, &newfuel.lw, &s1, &slh, &slw, &newfuel.depth,
					&newfuel.xmext, &newfuel.heatd, &newfuel.heatl, comment);
			if (num < 13)
				break;
			sprintf(newfuel.code, "FM%ld", ModNum);
			newfuel.dynamic = 0;
		}
		else
		{
			memset(code, 0x0, 32 * sizeof(char));
			num = sscanf(Line,
					"%ld %s %lf %lf %lf %lf %lf %s %lf %lf %lf %lf %lf %lf %lf %s",
					&ModNum, code, &newfuel.h1, &newfuel.h10, &newfuel.h100,
					&newfuel.lh, &newfuel.lw, dynamic, &s1, &slh, &slw,
					&newfuel.depth, &newfuel.xmext, &newfuel.heatd,
					&newfuel.heatl, comment);
			if (num < 15)
				break;
			//if(!strcmp(strupr(dynamic), "DYNAMIC"))
			//	newfuel.dynamic=1;
			if (strstr(strlwr(comment), "d"))
				newfuel.dynamic = 1;
			else
				newfuel.dynamic = 0;
			strncpy(newfuel.code, code, 4);
		}

		newfuel.sav1 = (long) s1;
		newfuel.savlh = (long) slh;
		newfuel.savlw = (long) slw;
		memset(newfuel.desc, 0x0, 256 * sizeof(char));
		if (strlen(comment) > 0)
		{
			//ptr = strstr(Line, comment);
			//strncpy(newfuel.desc, ptr, 64);
			strcpy(newfuel.desc, comment);
		}
		if (Metric) 	// convert to english
		{
			memcpy(&newfuelm, &newfuel, sizeof(NewFuel));
			newfuel.h1 /= 2.2417;
			newfuel.h10 /= 2.2417;
			newfuel.h100 /= 2.2417;
			newfuel.lh /= 2.2417;
			newfuel.lw /= 2.2417;
			newfuel.sav1 = (long)(s1 * 30.480060960);
			newfuel.savlh = (long)(slh * 30.480060960);
			newfuel.savlw = (long)(slw * 30.480060960);
			newfuel.depth /= 30.480060960;
			newfuel.heatd /= 2.324375;
			newfuel.heatl /= 2.324375;
		}
		count++;
		if (ModNum<14 || ModNum>256)
		{
			sprintf(ErrMsg, "Model Data Error Line => %ld", count);
			printf(" Fuel Model Number >256 or <14  ErrMsg\n");			BadFile = true;
		}
		else if (newfuel.xmext <= 0.0)
		{
			sprintf(ErrMsg, "Model Data Error Line => %ld", count);
			printf(" Extinction Moisture = 0  ErrMsg\n");			BadFile = true;
		}
		else if (newfuel.h1 + newfuel.h10 + newfuel.h100 <= 0.0)
		{
			sprintf(ErrMsg, "Model Data Error Line => %ld", count);
			printf(" Fuel Model Has No Dead Fuel  ErrMsg\n");			BadFile = true;
		}
		else if (newfuel.depth <= 0.0)
		{
			sprintf(ErrMsg, "Model Data Error Line => %ld", count);
			printf(" Depth = 0.0  ErrMsg\n");			BadFile = true;
		}
		else if (newfuel.heatl < 6000)
		{
			sprintf(ErrMsg, "Model Data Error Line => %ld", count);
			printf(" Live Heat Content Too Low  ErrMsg\n");			BadFile = true;
		}
		else if (newfuel.heatd < 4000)
		{
			sprintf(ErrMsg, "Model Data Error Line => %ld", count);
			printf(" Dead Heat Content Too Low  ErrMsg\n");			BadFile = true;
		}
		else if (newfuel.sav1 > 4000 ||
				newfuel.savlh > 4000 ||
				newfuel.savlw > 4000)
		{
			 	sprintf(ErrMsg, "Model Data Error Line => %ld", count);
			 	printf(" SAV Ratios Out of Range  ErrMsg\n");			 	BadFile = true;
		}

		if (BadFile)
		{
			fclose(CurrentFile);
			return false;
		}
		newfuel.xmext /= 100.0;
		newfuelm.xmext = newfuel.xmext;
		newfuel.number = newfuelm.number = ModNum;
		SetNewFuel(&newfuel);
		if (FileFormat == 1 && fout != NULL)
		{
			if (newfuel.dynamic == 0)
				sprintf(dynamic, "static");
			else
				sprintf(dynamic, "dynamic");
			if (Metric)
				fprintf(fout,
					"%ld %s %lf %lf %lf %lf %lf %s %ld %ld %ld %lf %ld %lf %lf %s\n",
					newfuelm.number, newfuelm.code, newfuelm.h1, newfuelm.h10,
					newfuelm.h100, newfuelm.lh, newfuelm.lw, dynamic,
					newfuelm.sav1, newfuelm.savlh, newfuelm.savlw,
					newfuelm.depth, (long)
					(newfuelm.xmext * 100.0),
					newfuelm.heatd, newfuelm.heatl, newfuelm.desc);
			else
				fprintf(fout,
					"%ld %s %lf %lf %lf %lf %lf %s %ld %ld %ld %lf %ld %lf %lf %s\n",
					newfuel.number, newfuel.code, newfuel.h1, newfuel.h10,
					newfuel.h100, newfuel.lh, newfuel.lw, dynamic,
					newfuel.sav1, newfuel.savlh, newfuel.savlw, newfuel.depth,
					(long)
					(newfuelm.xmext * 100.0), newfuel.heatd,
					newfuel.heatl, newfuel.desc);
		}
	}
	fclose(CurrentFile);
	if (fout)
	{
	//	fclose(fout);
		printf(" Existing File Renamed with .OLD extension Custom Fuel Model File Converted to new Format\n");	}

	SetCustFuelModelID(true);

	return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


bool FarInputs::LoadConversionFile()
{
	char Line[256] = "";
	long fueltype, fuelmodel;

	fscanf(CurrentFile, "%s", Line);
	fuelmodel = atol(Line);
	if (fuelmodel == 0 || fuelmodel > 256)
	{
		if (Line[0] != '0')
		{
			printf(" Conversion File Must NOT Have Header Information, See Help       			File Format Error, Possible Zero Fuel Model# or Character Data in File\n");			fclose(CurrentFile);

			return false;
		}
	}
	rewind(CurrentFile);
	while (!(feof(CurrentFile)))
	{
		fscanf(CurrentFile, "%ld", &fueltype);
		if (feof(CurrentFile))
			break;
		fscanf(CurrentFile, "%ld", &fuelmodel);
		if (!SetFuelConversion(fueltype, fuelmodel))
		{
			printf(" Conversion File Contains Invalid Data  				Error: Bad Conversion Data\n");			fueltype = -1;		// flag for bad file

			break;
		}
	}
	fclose(CurrentFile);
	if (fueltype < 0)
	{
		SetConvFuelModelID(false);

		return false;
	}
	else
		SetConvFuelModelID(true);

	return true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



bool FarInputs::OpenVersion1ProjectFile()
{
	char RevCopy[256];
	char CurrentDirectory[256] = "";
	char pathdiv[] = "/";              // search for path
	//char *projfile;
	char header[2];
	//char NewLandFName[256];
	long HEAD, length, NumWtr = 1, NumWnd = 1;
	long i, j;
	FILE* pf;
	COLORREF colr;

	getcwd(CurrentDirectory, 255);
	strcat(CurrentDirectory, pathdiv);
	//if((pf=fopen(bookmark.ProjectFile, "r"))!=NULL)
	if ((pf = fopen(ProjectFile, "r")) != NULL)
	{
		ResetData();
		fscanf(pf, "%s", header);
		HEAD = atol(header);
		if (version > 0)
		{
			for (i = 0; i < 5; i++)    // initialize strings for fileanmes
			{
				memset(WeatherFile[i], 0x0, sizeof WeatherFile[i]);
				memset(WindFile[i], 0x0, sizeof WindFile[i]);
				strcpy(MBWtr[i], "Load Weather");
				strcpy(MBWnd[i], "Load Winds");
				FreeWeatherData(i);
				FreeWindData(i);
			}
			fscanf(pf, "%s", LandscapeFile);
			fscanf(pf, "%li", &NumWtr);
			for (i = 0; i < NumWtr; i++)
				fscanf(pf, "%s", WeatherFile[i]);
			fscanf(pf, "%li", &NumWnd);
			for (i = 0; i < NumWnd; i++)
				fscanf(pf, "%s", WindFile[i]);
			if (LandID)
			{
				LandID = false;
				CloseLandFile();
			}
               /*
			if (VIEWLCP_ON)
			{
				if (!F_ON)// && !SIMULATE_GO)
				{
					VIEWLCP_ON = false;
					CanTryTerm = true;
					LandID = false;
					CloseLandFile();
					//GetFarsiteWindow()->CloseWindow();
					//SetFarsiteWindow(0);
					FreeStationGrid();	   	// reset data structures for model
				}
			}*/
			strcat(CurrentDirectory, LandscapeFile);
			memset(LandscapeFile, 0x0, sizeof(LandscapeFile));
			strcpy(LandscapeFile, CurrentDirectory);
			SetLandFileName(LandscapeFile);
			if (OpenLandFile())
			{
				if (TestForLCPVersion1())
				{
					int Response = IDYES;
					printf(" 						.LCP File Will Be Converted to v.2 Format, Old File Saved With Extension (LCP1)  						.LCP File Format v.1 Detected\n");					if (Response == IDYES)
					{
						if (!ConvertLCPFileToVersion2())
						{
							CloseLandFile();
							i = 0;
							printf(" 								Generate New .LCP File  								Error Reading .LCP File\n");						}
						else
							i = 1;
					}
					else
						i = 0;
				}
				else if (TestForLCPVersion2())
				{
					int Response = IDYES;
					printf(" 						.LCP File Will Be Converted to v.4 Format, Old File Saved With Extension (LCP2)  						.LCP File Format v.2 Detected\n");					if (Response == IDYES)
					{
						if (!ConvertLCPFile2ToVersion4())
						{
							CloseLandFile();
							i = 0;
							printf(" 								Generate New .LCP File  								Error Reading .LCP File\n");						}
						else
							i = 1;
					}
					else
						i = 0;
				}
				else
					i = 1;
				if (i == 1)
					InitializeLandscapeFile();
			}
			else
			{
				for (i = 0; i < 5; i++)    // initialize strings for fileanmes
				{
					memset(WeatherFile[i], 0x0, sizeof WeatherFile[i]);
					memset(WindFile[i], 0x0, sizeof WindFile[i]);
					strcpy(MBWtr[i], "Load Weather");
					strcpy(MBWnd[i], "Load Winds");
				}
				fclose(pf);
				printf(" GetLandFileName()  File Not Found\n");				SetLandFileName("");
				memset(ProjectFile, 0x0, sizeof(ProjectFile));
				memset(LandscapeFile, 0x0, sizeof(LandscapeFile));

				return false;
			}
			if (WtrID)
				WtrID = false;
			for (i = 0; i < NumWtr; i++)
			{
				//FreeWeatherData(i);
				if ((CurrentFile = fopen(WeatherFile[i], "r")) == NULL)
				{
					printf(" WeatherFile[i]  File Not Found\n");					memset(WeatherFile[i], 0x0, sizeof(WeatherFile[i]));
				}
				else
				{
					if (LoadWeatherData(i))
					{
						WtrID = true;
						strcpy(RevCopy, WeatherFile[i]);			// copy file path string
						strrev(RevCopy);					// reverse file path string
						length = strcspn(RevCopy, pathdiv);  // compute length without path
						memset(MBWtr[i], 0x0, sizeof MBWtr[i]);
						strncpy(MBWtr[i], RevCopy, length);	// copy length w/o path
						strrev(MBWtr[i]);				// reverse to get filename only
					}
				}
			}
			if (WndID)
				WndID = false;
			for (i = 0; i < NumWnd; i++)
			{
				//FreeWindData(i);
				if ((CurrentFile = fopen(WindFile[i], "r")) == NULL)
				{
					printf(" WindFile[i]  File Not Found\n");					memset(WindFile[i], 0x0, sizeof(WindFile[i]));
				}
				else
				{
					if (LoadWindData(i))
					{
						WndID = true;
						strcpy(RevCopy, WindFile[i]);			// copy file path string
						strrev(RevCopy);					// reverse file path string
						length = strcspn(RevCopy, pathdiv);  // compute length without path
						memset(MBWnd[i], 0x0, sizeof MBWnd[i]);
						strncpy(MBWnd[i], RevCopy, length);	// copy length w/o path
						strrev(MBWnd[i]);						// reverse to get filename only
					}
				}
			}
			if (version > 1)
			{
				char FileName1[256] = "";
				fscanf(pf, "%s", FileName1);
				if ((CurrentFile = fopen(FileName1, "r")) != NULL)
				{
					if (LoadAdjustmentFile())
					{
						strcpy(AdjustmentFile, FileName1);
						strcpy(RevCopy, FileName1);		// copy file path string
						strrev(RevCopy);				// reverse file path string
						length = strcspn(RevCopy, pathdiv); // compute length without path
						memset(MBAdj, 0x0, sizeof MBAdj);
						strncpy(MBAdj, RevCopy, length);  // copy length w/o path
						strrev(MBAdj);
					}
				}
				else
					printf(" Bad Adjustment File                                Can't Read File\n");				fscanf(pf, "%s", FileName1);
				if ((CurrentFile = fopen(FileName1, "r")) != NULL)
				{
					if (LoadFuelMoistureFile())
					{
						strcpy(FuelMoistureFile, FileName1);
						strcpy(RevCopy, FileName1);		// copy file path string
						strrev(RevCopy);				// reverse file path string
						length = strcspn(RevCopy, pathdiv); // compute length without path
						memset(MBMois, 0x0, sizeof MBMois);
						strncpy(MBMois, RevCopy, length);  // copy length w/o path
						strrev(MBMois);
					}
				}
				else
					printf(" Bad Fuel Moisture File                                Can't Read File\n");				if (version == 3 || version == 4)
				{
					fscanf(pf, "%s", FileName1);
					if ((CurrentFile = fopen(FileName1, "r")) != NULL)
					{
						strcpy(FuelModelFile, FileName1);
						if (LoadCustomModelFile())
						{
							strcpy(FuelModelFile, FileName1);
							strcpy(RevCopy, FileName1);		// copy file path string
							strrev(RevCopy);				// reverse file path string
							length = strcspn(RevCopy, pathdiv); // compute length without path
							memset(MBFuelMods, 0x0, sizeof MBFuelMods);
							strncpy(MBFuelMods, RevCopy, length);  // copy length w/o path
							strrev(MBFuelMods);
						}
					}
					else
						printf(" Bad Custom Fuel Model File  							Can't Read File\n");				}
				if (version == 4 || version == 5)
				{
					fscanf(pf, "%s", FileName1);
					if ((CurrentFile = fopen(FileName1, "r")) != NULL)
					{
						if (LoadConversionFile())
						{
							strcpy(ConvertFile, FileName1);
							strcpy(RevCopy, FileName1);		// copy file path string
							strrev(RevCopy);				// reverse file path string
							length = strcspn(RevCopy, pathdiv); // compute length without path
							memset(MBConv, 0x0, sizeof MBConv);
							strncpy(MBConv, RevCopy, length);  // copy length w/o path
							strrev(MBConv);
						}
					}
					else
						printf(" Bad Conversion File                                     Can't Read File\n");				}
			}
			long NumGEast, NumGNorth;   // retrieve and load weather grid information
			fscanf(pf, "%ld %ld", &NumGEast, &NumGNorth);
			AllocStationGrid(NumGEast, NumGNorth);
			for (i = 0; i < NumGNorth; i++)
			{
				for (j = 0; j < NumGEast; j++)
				{
					fscanf(pf, "%ld", &version);
					SetStationNumber(j, i, version);
				}
			}
			SetGridEastDimension(NumGEast);
			SetGridNorthDimension(NumGNorth);
			fscanf(pf, "%ld", &j);     // set color settings
			do
			{
				i = j;
				fscanf(pf, "%lu %ld", &colr, &j);
				//SetFuelColor(i+1, colr);
			}
			while (abs(j) != 99);
			if (j < 0)
			{
				//	InitializePalette(); // initialize super-VGA color palette
				SVGA_ON = true;
			}
			else
				SVGA_ON = false;
		}
		else
		{
			printf(" ProjectFile  !! Something is Wrong With This File !!:\n");			fclose(pf);

			return false;
		}
		fclose(pf);
	}
	else
	{
		printf(" ProjectFile  !! UNABLE to open file !!:\n");
		return false;
	}

	return true;
}


bool FarInputs::OpenVersion2ProjectFile()
{
	char RevCopy[256];
	char CurrentDirectory[256] = "";
	char pathdiv[] = "/";              // search for path
	//char *projfile;
	char header[3];
	//char NewLandFName[256];
	long length, NumWtr = 1, NumWnd = 1;
	long i, j;
	double north, south, east, west;
	FILE* pf;
	COLORREF colr;
	long HEAD;

	getcwd(CurrentDirectory, 255);
	strcat(CurrentDirectory, pathdiv);
	//if((pf=fopen(bookmark.ProjectFile, "r"))!=NULL)
	do
	{
		if ((pf = fopen(ProjectFile, "r")) == NULL)
		{
			printf(" ProjectFile  !! UNABLE to open file !!:\n");			//  			 //TOpenSaveDialog::TData FilenameData(OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
			//		"PROJECT FILES (*.FPJ)|*.FPJ|", 0, GetCurrentPath(), "*");
			//	if(TFileOpenDialog(Client, FilenameData, 0, "Select Project File").Execute()==IDOK)
			//strcpy(ProjectFile, FilenameData.FileName);
			//else
			//	break;
		}
	}
	while (!pf);

	if (pf)
	{
		strcpy(RevCopy, ProjectFile);
		ResetData();
		strcpy(ProjectFile, RevCopy);
		fscanf(pf, "%s", header);
		HEAD = atol(header);
		if (HEAD > 0)
		{
			for (i = 0; i < 5; i++)    // initialize strings for fileanmes
			{
				memset(WeatherFile[i], 0x0, sizeof WeatherFile[i]);
				memset(WindFile[i], 0x0, sizeof WindFile[i]);
				strcpy(MBWtr[i], "Load Weather");
				strcpy(MBWnd[i], "Load Winds");
				FreeWeatherData(i);
				FreeWindData(i);
			}
			fscanf(pf, "%s", LandscapeFile);//NewLandFName);
			fscanf(pf, "%li", &NumWtr);
			for (i = 0; i < NumWtr; i++)
				fscanf(pf, "%s", WeatherFile[i]);
			fscanf(pf, "%li", &NumWnd);
			for (i = 0; i < NumWnd; i++)
				fscanf(pf, "%s", WindFile[i]);
			if (LandID)
			{
				LandID = false;
				CloseLandFile();
			}
               /*
			if (VIEWLCP_ON)
			{
				if (!F_ON)// && !SIMULATE_GO)
				{
					VIEWLCP_ON = false;
					CanTryTerm = true;
					LandID = false;
					CloseLandFile();
					//					GetFarsiteWindow()->CloseWindow();
					//					SetFarsiteWindow(0);
					FreeStationGrid();	   	// reset data structures for model
				}
			}
               */
			strcpy(RevCopy, LandscapeFile);	// copy file path string
			strrev(RevCopy);						// reverse file path string
			length = strcspn(RevCopy, pathdiv);   // compute length without path
			memset(MBLand, 0x0, sizeof MBLand);
			strncpy(MBLand, RevCopy, length);		// copy length w/o path
			strrev(MBLand);						// reverse to get filename only
			strcat(CurrentDirectory, LandscapeFile);
			memset(LandscapeFile, 0x0, sizeof(LandscapeFile));
			strcpy(LandscapeFile, CurrentDirectory);
			SetLandFileName(LandscapeFile);//(NewLandFName);
			if (OpenLandFile())
			{
				if (TestForLCPVersion1())
				{
					printf(" .LCP File Will Be Converted to v.2 Format  .LCP File Format v.1 Detected\n");					if (!ConvertLCPFileToVersion2())
					{
						CloseLandFile();
						i = 0;
						printf(" Generate New .LCP File before Continuing  Error Reading .LCP File\n");					}
					else
						i = 1;
				}
				else if (TestForLCPVersion2())
				{
					printf(" .LCP File Will Be Converted to v.4 Format  .LCP File Format v.2 Detected\n");					if (!ConvertLCPFile2ToVersion4())
					{
						CloseLandFile();
						i = 0;
						printf(" Generate New .LCP File before continuing  Error Reading .LCP File\n");					}
					else
						i = 1;
				}
				else
					i = 1;
				if (i == 1)
					InitializeLandscapeFile();
			}
			else
			{
				for (i = 0; i < 5; i++)    // initialize strings for fileanmes
				{
					memset(WeatherFile[i], 0x0, sizeof WeatherFile[i]);
					memset(WindFile[i], 0x0, sizeof WindFile[i]);
					strcpy(MBWtr[i], "Load Weather");
					strcpy(MBWnd[i], "Load Winds");
				}
				fclose(pf);
				printf(" GetLandFileName()  File Not Found\n");				SetLandFileName("");
				memset(ProjectFile, 0x0, sizeof(ProjectFile));
				memset(LandscapeFile, 0x0, sizeof(LandscapeFile));

				return false;
			}
			if (WtrID)
				WtrID = false;
			for (i = 0; i < NumWtr; i++)
			{
				//FreeWeatherData(i);
				if ((CurrentFile = fopen(WeatherFile[i], "r")) == NULL)
				{
					printf(" WeatherFile[i]  File Not Found\n");					memset(WeatherFile[i], 0x0, sizeof(WeatherFile[i]));
				}
				else
				{
					if (LoadWeatherData(i))
					{
						WtrID = true;
						strcpy(RevCopy, WeatherFile[i]);			// copy file path string
						strrev(RevCopy);					// reverse file path string
						length = strcspn(RevCopy, pathdiv);  // compute length without path
						memset(MBWtr[i], 0x0, sizeof MBWtr[i]);
						strncpy(MBWtr[i], RevCopy, length);	// copy length w/o path
						strrev(MBWtr[i]);				// reverse to get filename only
					}
				}
			}
			if (WndID)
				WndID = false;
			for (i = 0; i < NumWnd; i++)
			{
				//FreeWindData(i);
				if ((CurrentFile = fopen(WindFile[i], "r")) == NULL)
				{
					printf(" WindFile[i]  File Not Found\n");					memset(WindFile[i], 0x0, sizeof(WindFile[i]));
				}
				else
				{
					if (LoadWindData(i))
					{
						WndID = true;
						strcpy(RevCopy, WindFile[i]);			// copy file path string
						strrev(RevCopy);					// reverse file path string
						length = strcspn(RevCopy, pathdiv);  // compute length without path
						memset(MBWnd[i], 0x0, sizeof MBWnd[i]);
						strncpy(MBWnd[i], RevCopy, length);	// copy length w/o path
						strrev(MBWnd[i]);						// reverse to get filename only
					}
				}
			}
			if (HEAD > 11)
			{
				char FileName1[256] = "";
				fscanf(pf, "%s", FileName1);
				if ((CurrentFile = fopen(FileName1, "r")) != NULL)
				{
					if (LoadAdjustmentFile())
					{
						strcpy(AdjustmentFile, FileName1);
						strcpy(RevCopy, FileName1);		// copy file path string
						strrev(RevCopy);				// reverse file path string
						length = strcspn(RevCopy, pathdiv); // compute length without path
						memset(MBAdj, 0x0, sizeof MBAdj);
						strncpy(MBAdj, RevCopy, length);  // copy length w/o path
						strrev(MBAdj);
					}
				}
				else
					printf(" Bad Adjustment File                                Can't Read File\n");				fscanf(pf, "%s", FileName1);
				if ((CurrentFile = fopen(FileName1, "r")) != NULL)
				{
					if (LoadFuelMoistureFile())
					{
						strcpy(FuelMoistureFile, FileName1);
						strcpy(RevCopy, FileName1);		// copy file path string
						strrev(RevCopy);				// reverse file path string
						length = strcspn(RevCopy, pathdiv); // compute length without path
						memset(MBMois, 0x0, sizeof MBMois);
						strncpy(MBMois, RevCopy, length);  // copy length w/o path
						strrev(MBMois);
					}
				}
				else
					printf(" Bad Fuel Moisture File                                Can't Read File\n");				if (HEAD == 13 || HEAD == 14 || HEAD == 17 || HEAD == 18)
				{
					fscanf(pf, "%s", FileName1);
					if ((CurrentFile = fopen(FileName1, "r")) != NULL)
					{
						strcpy(FuelModelFile, FileName1);
						if (LoadCustomModelFile())
						{
							strcpy(FuelModelFile, FileName1);
							strcpy(RevCopy, FileName1);		// copy file path string
							strrev(RevCopy);				// reverse file path string
							length = strcspn(RevCopy, pathdiv); // compute length without path
							memset(MBFuelMods, 0x0, sizeof MBFuelMods);
							strncpy(MBFuelMods, RevCopy, length);  // copy length w/o path
							strrev(MBFuelMods);
						}
					}
					else
						printf(" Bad Custom Fuel Model File  							Can't Read File\n");				}
				if (HEAD == 14 || HEAD == 15 || HEAD == 18 || HEAD == 19)
				{
					fscanf(pf, "%s", FileName1);
					if ((CurrentFile = fopen(FileName1, "r")) != NULL)
					{
						if (LoadConversionFile())
						{
							strcpy(ConvertFile, FileName1);
							strcpy(RevCopy, FileName1);		// copy file path string
							strrev(RevCopy);				// reverse file path string
							length = strcspn(RevCopy, pathdiv); // compute length without path
							memset(MBConv, 0x0, sizeof MBConv);
							strncpy(MBConv, RevCopy, length);  // copy length w/o path
							strrev(MBConv);
						}
					}
					else
						printf(" Bad Conversion File                                     Can't Read File\n");				}
				if (HEAD > 15)
				{
					fscanf(pf, "%s", FileName1);
					if ((CurrentFile = fopen(FileName1, "r")) != NULL)
					{
						if (LoadBurnPeriodFile())
						{
							strcpy(BurnPeriod, FileName1);
							strcpy(RevCopy, FileName1);		// copy file path string
							strrev(RevCopy);				// reverse file path string
							length = strcspn(RevCopy, pathdiv); // compute length without path
							memset(MBBpd, 0x0, sizeof(MBBpd));
							strncpy(MBBpd, RevCopy, length);  // copy length w/o path
							strrev(MBBpd);
						}
					}
					else
						printf(" Bad Burn Period File                                     Can't Read File\n");				}
			}
			long NumGEast, NumGNorth;   // retrieve and load weather grid information
			fscanf(pf, "%ld %ld", &NumGEast, &NumGNorth);
			AllocStationGrid(NumGEast, NumGNorth);
			for (i = 0; i < NumGNorth; i++)
			{
				for (j = 0; j < NumGEast; j++)
				{
					fscanf(pf, "%ld", &HEAD);
					SetStationNumber(j, i, HEAD);
				}
			}
			SetGridEastDimension(NumGEast);
			SetGridNorthDimension(NumGNorth);
			fscanf(pf, "%ld", &j);     // set color settings
			do
			{
				i = j;
				fscanf(pf, "%lu %ld", &colr, &j);
				//SetFuelColor(i, colr);
			}
			while (abs(j) != 99);
			if (j < 0)
			{
				//	InitializePalette(); // initialize super-VGA color palette
				SVGA_ON = true;
				//TransDisplayData.Dat.SVGA=BF_CHECKED;
			}
			else
				SVGA_ON = false;
			fscanf(pf, "%ld", &GridInterval);
			/*  			 if(CheckCellResUnits()==1)
					//  			 	::GridInterval(GridInterval*50);
								   else
					//  			 	::GridInterval(GridInterval*25);
								fscanf(pf, "%ld", &j);			// print number of stored vectors
					*/
			FreeAllVectorThemes();

			long NumTheme, Color;
			VectorTheme* theme;
			if (j > 0)
			{
				for (i = 0; i < j; i++)
				{
					NumTheme = AllocVectorTheme();
					if (NumTheme > -1)
					{
						theme = GetVectorTheme(NumTheme);
						fscanf(pf, "%s %ld %ld %lu %d %d\n", theme->FileName,
							&theme->FileType, &theme->Permanent,
							&Color, &theme->PenStyle, &theme->PenWidth);
						theme->OnOff = true;
					}
				}
			}
			if (!feof(pf))
			{
				fscanf(pf, "%lf", &north);
				fscanf(pf, "%lf", &south);
				fscanf(pf, "%lf", &east);
				fscanf(pf, "%lf", &west);
				/*SetLandscapeViewPort(north, south, east, west);
								if(GetNumViewEast()>GetNumViewNorth())   //SETS PIXEL SPACING AT MINIMUM X/Y DIMENSION
								{	SetTint(8000.0/(double) GetNumViewEast());
									SetMaxWindowDim(GetTint()*GetNumViewEast());
								}
								else
								{	SetTint(8000.0/(double) GetNumViewNorth());
									SetMaxWindowDim(GetTint()*GetNumViewNorth());
								}
								ViewPortStatus(true);
								ViewPortChanged(true);
													*/
			}
		}
		else
		{
			printf(" ProjectFile  !! Something is Wrong With This File !!:\n");			fclose(pf);

			return false;
		}
		fclose(pf);
	}
	else
	{
		printf(" ProjectFile  !! UNABLE to open file !!:\n");
		return false;
	}

	return true;
}


bool FarInputs::OpenVersion4ProjectFile()
{
	bool FirstTime = true, BurnPeriodIsHere = false;
	char RevCopy[256] = "", Name[256] = "", FileName1[256] = "";
	char CurrentDirectory[256] = "";
	//char AppendCurrentDirectory[256] = "";
	char pathdiv[] = "/";              // search for path
	char header[16] = "";
     //char *ptr;
	long HEAD, Val, length, NumWtr = 1, NumWnd = 1;
	long i, j;
	double north, south, east, west, val[7];
	FILE* pf;
	COLORREF colr;
	getNameFromFile(ProjectFile, Name, 255);
     //ptr=strstr(ProjectFile, Name);
     strncpy(CurrentDirectory, ProjectFile, strlen(ProjectFile)-strlen(Name));
     chdir(CurrentDirectory);
     //getcwd(CurrentDirectory, 255);
	//strcat(CurrentDirectory, pathdiv);
	//GetCurrentDirectory(256, CurrentDirectory);
	//if((pf=fopen(bookmark.ProjectFile, "r"))!=NULL)
	do
	{
		if ((pf = fopen(ProjectFile, "r")) == NULL)
		{
			if (FirstTime)
			{
				getNameFromFile(ProjectFile, Name, 255);
				strcpy(ProjectFile, Name);
				FirstTime = false;
			}
			else
			{
				printf(" ProjectFile  !! UNABLE to open file !!:\n");				//TOpenSaveDialog::TData FilenameData(OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
				// "PROJECT FILES (*.FPJ)|*.FPJ|", 0, GetCurrentPath(), "*");
				//if(TFileOpenDialog(Client, FilenameData, 0, "Select Project File").Execute()==IDOK)
				// strcpy(ProjectFile, FilenameData.FileName);
				//else
				// break;
			}
		}
	}
	while (!pf);

	if (pf)
	{
		strcpy(RevCopy, ProjectFile);
		ResetData();
		strcpy(ProjectFile, RevCopy);
		memset(header, 0x0, 16 * sizeof(char));
		fgets(header, 15, pf);//fscanf(pf, "%s", header);
		HEAD = atol(header);
		if (HEAD > 0)
		{
			for (i = 0; i < 5; i++)    // initialize strings for fileanmes
			{
				memset(WeatherFile[i], 0x0, sizeof WeatherFile[i]);
				memset(WindFile[i], 0x0, sizeof WindFile[i]);
				strcpy(MBWtr[i], "Load Weather");
				strcpy(MBWnd[i], "Load Winds");
				FreeWeatherData(i);
				FreeWindData(i);
			}
			memset(Name, 0x0, 256 * sizeof(char));
			fgets(Name, 255, pf);//fscanf(pf, "%s", LandscapeFile);//NewLandFName);
			sscanf(Name, "%s", LandscapeFile);
			memset(header, 0x0, 16 * sizeof(char));
			fgets(header, 15, pf);//fscanf(pf, "%d", &NumWtr);
			NumWtr = atol(header);
			for (i = 0; i < NumWtr; i++)
			{
				memset(Name, 0x0, 256 * sizeof(char));
				fgets(Name, 255, pf);//fscanf(pf, "%s", WeatherFile[i]);
				strncpy(WeatherFile[i], Name, strlen(Name) - 1);
			}
			memset(header, 0x0, 16 * sizeof(char));
			fgets(header, 15, pf);//fscanf(pf, "%d", &NumWnd);
			NumWnd = atol(header);
			for (i = 0; i < NumWnd; i++)
			{
				memset(Name, 0x0, 256 * sizeof(char));
				fgets(Name, 255, pf);//fscanf(pf, "%s", WindFile[i]);
				strncpy(WindFile[i], Name, strlen(Name) - 1);
			}
			if (LandID)
			{
				LandID = false;
				CloseLandFile();
			}
               /*
			if (VIEWLCP_ON)
			{
				if (!F_ON)// && !SIMULATE_GO)
				{
					VIEWLCP_ON = false;
					CanTryTerm = true;
					LandID = false;
					CloseLandFile();
					//					GetFarsiteWindow()->CloseWindow();
					//					SetFarsiteWindow(0);
					FreeStationGrid();	   	// reset data structures for model
				}
			}
			  			 if(THREED_ON)
								   {	Get3dWindow()->CloseWindow();
										Set3dWindow(0);
										THREED_ON=false;
								   }
					*/
			//getNameFromFile(LandscapeFile, MBLand, 64);//
			strcpy(MBLand,LandscapeFile);//
			//strcpy(RevCopy, LandscapeFile);	// copy file path string
			//strrev(RevCopy);						// reverse file path string
			//length=strcspn(RevCopy, pathdiv);   // compute length without path
			//memset(MBLand, 0x0, sizeof MBLand);
			//strncpy(MBLand, RevCopy, length);		// copy length w/o path
			//strrev(MBLand);						// reverse to get filename only
			strcat(CurrentDirectory, "/");   
			strcat(CurrentDirectory, LandscapeFile);
			memset(LandscapeFile, 0x0, sizeof(LandscapeFile));
			strcpy(LandscapeFile, CurrentDirectory);
			SetLandFileName(LandscapeFile);//(NewLandFName);
			if (OpenLandFile())
			{
				if (TestForLCPVersion1())
				{
					printf(" .LCP File Will Be Converted to v.2 Format  .LCP File Format v.1 Detected\n");					if (!ConvertLCPFileToVersion2())
					{
						CloseLandFile();
						i = 0;
						printf(" Generate New .LCP File before Continuing  Error Reading .LCP File\n");					}
					else
						i = 1;
				}
				else if (TestForLCPVersion2())
				{
					printf(" .LCP File Will Be Converted to v.4 Format  .LCP File Format v.2 Detected\n");					if (!ConvertLCPFile2ToVersion4())
					{
						CloseLandFile();
						i = 0;
						printf(" Generate New .LCP File before continuing  Error Reading .LCP File\n");					}
					else
						i = 1;
				}
				else
					i = 1;
				if (i == 1)
					InitializeLandscapeFile();
			}
			else
			{
				for (i = 0; i < 5; i++)    // initialize strings for fileanmes
				{
					memset(WeatherFile[i], 0x0, sizeof WeatherFile[i]);
					memset(WindFile[i], 0x0, sizeof WindFile[i]);
					strcpy(MBWtr[i], "Load Weather");
					strcpy(MBWnd[i], "Load Winds");
				}
				fclose(pf);
				printf(" GetLandFileName()  File Not Found\n");				SetLandFileName("");
				memset(ProjectFile, 0x0, sizeof(ProjectFile));
				memset(LandscapeFile, 0x0, sizeof(LandscapeFile));

				return false;
			}
			if (WtrID)
				WtrID = false;
			for (i = 0; i < NumWtr; i++)
			{
				//FreeWeatherData(i);
				trim(WeatherFile[i]);
				if ((CurrentFile = fopen(WeatherFile[i], "r")) == NULL)
				{
					printf("Weather File %s Not Found\n",WeatherFile[i]);					fflush(stdout);
					memset(WeatherFile[i], 0x0, sizeof(WeatherFile[i]));
					strcpy(MBWtr[i], "Load Weather");
				}
				else
				{
					if (LoadWeatherData(i))
					{
						WtrID = true;
						strcpy(RevCopy, WeatherFile[i]);			// copy file path string
						strrev(RevCopy);					// reverse file path string
						length = strcspn(RevCopy, pathdiv);  // compute length without path
						memset(MBWtr[i], 0x0, sizeof MBWtr[i]);
						strncpy(MBWtr[i], RevCopy, length);	// copy length w/o path
						strrev(MBWtr[i]);				// reverse to get filename only
					}
					else
						memset(&WeatherFile[i], 0x0, sizeof(WeatherFile[i]));
				}
			}
			if (WndID)
				WndID = false;
			for (i = 0; i < NumWnd; i++)
			{
				//FreeWindData(i);
				trim(WindFile[i]);
				if ((CurrentFile = fopen(WindFile[i], "r")) == NULL)
				{
					printf("WindFile %s Not Found\n",WindFile[i]);					memset(WindFile[i], 0x0, sizeof(WindFile[i]));
					strcpy(MBWnd[i], "Load Winds");
				}
				else
				{
					if (LoadWindData(i))
					{
						WndID = true;
						strcpy(RevCopy, WindFile[i]);			// copy file path string
						strrev(RevCopy);					// reverse file path string
						length = strcspn(RevCopy, pathdiv);  // compute length without path
						memset(MBWnd[i], 0x0, sizeof MBWnd[i]);
						strncpy(MBWnd[i], RevCopy, length);	// copy length w/o path
						strrev(MBWnd[i]);						// reverse to get filename only
					}
					else
						memset(&WindFile[i], 0x0, sizeof(WindFile[i]));
				}
			}
			if (HEAD > 50)
			{
				BurnPeriodIsHere = true;
				HEAD -= 10;
			}

			if (HEAD > 41)
			{
				memset(Name, 0x0, 256 * sizeof(char));
				fgets(Name, 255, pf);//fscanf(pf, "%s", FileName1);
				memset(FileName1, 0x0, 256 * sizeof(char));
				trim(Name);
				strncpy(FileName1, Name, strlen(Name));
				if ((CurrentFile = fopen(FileName1, "r")) != NULL)
				{
					if (LoadAdjustmentFile())
					{
						strcpy(AdjustmentFile, FileName1);
						strcpy(RevCopy, FileName1);		// copy file path string
						strrev(RevCopy);				// reverse file path string
						length = strcspn(RevCopy, pathdiv); // compute length without path
						memset(MBAdj, 0x0, sizeof MBAdj);
						strncpy(MBAdj, RevCopy, length);  // copy length w/o path
						strrev(MBAdj);
					}
					else
						memset(AdjustmentFile, 0x0, sizeof(AdjustmentFile));
				}
				else
				printf(" Adjustment File Not Found or Unreadable  Can't Read File\n");				memset(Name, 0x0, 256 * sizeof(char));
				fgets(Name, 255, pf);//fscanf(pf, "%s", FileName1);
				memset(FileName1, 0x0, 256 * sizeof(char));
				trim(Name);
				strncpy(FileName1, Name, strlen(Name));
				if ((CurrentFile = fopen(FileName1, "r")) != NULL)
				{
					if (LoadFuelMoistureFile())
					{
						strcpy(FuelMoistureFile, FileName1);
						strcpy(RevCopy, FileName1);		// copy file path string
						strrev(RevCopy);				// reverse file path string
						length = strcspn(RevCopy, pathdiv); // compute length without path
						memset(MBMois, 0x0, sizeof MBMois);
						strncpy(MBMois, RevCopy, length);  // copy length w/o path
						strrev(MBMois);
					}
					else
						memset(FuelMoistureFile, 0x0, sizeof(FuelMoistureFile));
				}
				else
					printf(" Fuel Moisture File Not Found or Unreadable  Can't Read File\n");					fflush(stdout);

				if (HEAD == 43 || HEAD == 44 || HEAD == 47 || HEAD == 48)
				{
					memset(Name, 0x0, 256 * sizeof(char));
					fgets(Name, 255, pf);//fscanf(pf, "%s", FileName1);
					memset(FileName1, 0x0, 256 * sizeof(char));
					trim(Name);
					strncpy(FileName1, Name, strlen(Name));
					if ((CurrentFile = fopen(FileName1, "r")) != NULL)
					{
						strcpy(FuelModelFile, FileName1);
						if (LoadCustomModelFile())
						{
							strcpy(FuelModelFile, FileName1);
							strcpy(RevCopy, FileName1);		// copy file path string
							strrev(RevCopy);				// reverse file path string
							length = strcspn(RevCopy, pathdiv); // compute length without path
							memset(MBFuelMods, 0x0, sizeof MBFuelMods);
							strncpy(MBFuelMods, RevCopy, length);  // copy length w/o path
							strrev(MBFuelMods);
						}
						else
							memset(FuelModelFile, 0x0, sizeof(FuelModelFile));
					}
					else
						printf(" Custom Fuel Model File Not Found or Unreadable  Can't Read File\n");						fflush(stdout);
				}
				if (HEAD == 44 || HEAD == 45 || HEAD == 48 || HEAD == 49)
				{
					memset(Name, 0x0, 256 * sizeof(char));
					fgets(Name, 255, pf);//fscanf(pf, "%s", FileName1);
					trim(Name);
					memset(FileName1, 0x0, 256 * sizeof(char));
					strncpy(FileName1, Name, strlen(Name));
					if ((CurrentFile = fopen(FileName1, "r")) != NULL)
					{
						if (LoadConversionFile())
						{
							strcpy(ConvertFile, FileName1);
							strcpy(RevCopy, FileName1);		// copy file path string
							strrev(RevCopy);				// reverse file path string
							length = strcspn(RevCopy, pathdiv); // compute length without path
							memset(MBConv, 0x0, sizeof MBConv);
							strncpy(MBConv, RevCopy, length);  // copy length w/o path
							strrev(MBConv);
						}
						else
							memset(ConvertFile, 0x0, sizeof(ConvertFile));
					}
					else
						printf(" Conversion File Not Found or Unreadable  Can't Read File\n");						fflush(stdout);
				}
				if (HEAD > 45)
				{
					memset(Name, 0x0, 256 * sizeof(char));
					fgets(Name, 255, pf);//fscanf(pf, "%s", FileName1);
					trim(Name);
					memset(FileName1, 0x0, 256 * sizeof(char));
					strncpy(FileName1, Name, strlen(Name));
					if ((CurrentFile = fopen(FileName1, "r")) != NULL)
					{
						if (LoadCoarseWoodyFile(FileName1))
						{
							strcpy(CoarseWoody, FileName1);
							strcpy(RevCopy, FileName1);		// copy file path string
							strrev(RevCopy);				// reverse file path string
							length = strcspn(RevCopy, pathdiv); // compute length without path
							memset(MBCwd, 0x0, sizeof(MBCwd));
							strncpy(MBCwd, RevCopy, length);  // copy length w/o path
							strrev(MBCwd);
						}
						else
						{
							printf(" FileName1  Coarse Woody File Not Loaded\n");							fflush(stdout);
							memset(CoarseWoody, 0x0, sizeof(CoarseWoody));
						}
					}
					else
						printf(" Coarse Woody File Not Found or Unreadable  Can't Read File\n");						fflush(stdout);
				}
				if (BurnPeriodIsHere)
				{
					memset(Name, 0x0, 256 * sizeof(char));
					fgets(Name, 255, pf);//fscanf(pf, "%s", FileName1);
					trim(Name);
					memset(FileName1, 0x0, 256 * sizeof(char));
					strncpy(FileName1, Name, strlen(Name));
					if ((CurrentFile = fopen(FileName1, "r")) != NULL)
					{
						if (LoadBurnPeriodFile())
						{
							strcpy(BurnPeriod, FileName1);
							strcpy(RevCopy, FileName1);		// copy file path string
							strrev(RevCopy);				// reverse file path string
							length = strcspn(RevCopy, pathdiv); // compute length without path
							memset(MBBpd, 0x0, sizeof(MBBpd));
							strncpy(MBBpd, RevCopy, length);  // copy length w/o path
							strrev(MBBpd);
						}
						else
							memset(BurnPeriod, 0x0, sizeof(BurnPeriod));
					}
					else
						printf(" Burn Period File %s Not Found or Unreadable  Can't Read File\n\n",FileName1);						fflush(stdout);
				}
			}
			long NumGEast, NumGNorth, NumAlloc;   // retrieve and load weather grid information
			char* GridRow = 0;
			memset(FileName1, 0x0, 256 * sizeof(char));
			fgets(FileName1, 255, pf);//fscanf(pf, "%ld %ld", &NumGEast, &NumGNorth);
			sscanf(FileName1, "%ld %ld", &NumGEast, &NumGNorth);
			AllocStationGrid(NumGEast, NumGNorth);
			NumAlloc = NumGEast * 3;
			if (NumAlloc < 512)
				NumAlloc = 512;
			GridRow = new char[NumAlloc];
			for (i = 0; i < NumGNorth; i++)
			{
				memset(GridRow, 0x0, NumAlloc * sizeof(char));
				fgets(GridRow, NumAlloc, pf);// 	fscanf(pf, "%ld", &HEAD);
				for (j = 0; j < NumGEast; j++)
				{
					sscanf(&GridRow[j * 2], "%ld", &Val);
					SetStationNumber(j, i, Val);
				}
			}
			SetGridEastDimension(NumGEast);
			SetGridNorthDimension(NumGNorth);
			delete[] GridRow;


			//fscanf(pf, "%ld", &j);	 // set color settings
			//do
			//{    i=j;
			//	fscanf(pf, "%lu %ld", &colr, &j);
			//	SetFuelColor(i, colr);
			//} while(abs(j)!=99);
			//if(j<0)
			//{	InitializePalette(); // initialize super-VGA color palette
			//	SVGA_ON=true;
			//TransDisplayData.Dat.SVGA=BF_CHECKED;
			//}
			//else

			memset(Name, 0x0, 256 * sizeof(char));
			fgets(Name, 255, pf);//fscanf(pf, "%s", Name);
			trim(Name);
			memset(FileName1, 0x0, 256 * sizeof(char));
			FILE* cf = NULL;
			if (strlen(Name) > 0)
			{
				strncpy(FileName1, Name, strlen(Name));
				cf = fopen(FileName1, "rb");
			}

			if (cf)
			{
				long num;//, pos, pos2;
				char krap[256], temp1[256] = "", temp2[256] = "";
				bool tempb;
				long templ;
				double tempd;
				long tempc[100];
				LandscapeTheme* grid;

				grid = GetLandscapeTheme();
				fread(krap, sizeof(char), 256, cf);
				strrev(krap);
				strrev(grid->Name);
				strncpy(temp1, krap, strlen(Name));
				strncpy(temp2, grid->Name, strlen(Name));
				strrev(temp1);
				strrev(temp2);
				strrev(krap);
				strrev(grid->Name);
				if (!strcmp(strlwr(temp1), strlwr(temp2)))//(strcmp(grid->Name, krap))
				{
					fread(&tempb, sizeof(bool), 1, cf);
					grid->LcpAscii = false; // tempb
					fread(&templ, sizeof(long), 1, cf);
					grid->Continuous = templ;
					fread(&templ, sizeof(long), 1, cf);
					grid->RedVal = templ;
					fread(&templ, sizeof(long), 1, cf);
					grid->GreenVal = templ;
					fread(&templ, sizeof(long), 1, cf);
					grid->BlueVal = templ;
					fread(&templ, sizeof(long), 1, cf);
					grid->VarVal = templ;
					fread(&templ, sizeof(long), 1, cf);
					grid->MaxBrite = templ;
					fread(&templ, sizeof(long), 1, cf);
					grid->ColorChange = templ;
					fread(&templ, sizeof(long), 1, cf);
					grid->CatsOK = templ;
					fread(tempc, sizeof(long), 100, cf);
					memcpy(grid->Cats, tempc, sizeof(tempc));
					fread(&tempd, sizeof(double), 1, cf);
					grid->MaxVal = tempd;
					fread(&tempd, sizeof(double), 1, cf);
					grid->MinVal = tempd;
					fread(&num, sizeof(long), 1, cf);
					if (num < 0 || grid->Continuous == 0)
					{
						grid->NumCats = abs(num);
						grid->NumColors = 0;
					}
					else
					{
						grid->NumColors = abs(num);
					}
					grid->CreateRamp();
					grid->WantNewRamp = false;
					//grid->ramp->SetRamp(abs(num), grid->RedVal,
					//				grid->GreenVal, grid->BlueVal,
					//				grid->VarVal, grid->MaxBrite);
					if (num > 0)
					{
						for (i = 0; i <= grid->NumColors; i++)
						{
							fread(&colr, sizeof(COLORREF), 1, cf);
							//grid->ramp->SetSpecificColor(i, GetRValue(colr),
							//				GetGValue(colr), GetBValue(colr));
						}
					}
					//GetFileTitle(Name, grid->ramp->ColorFile, 255);
					//memset(grid->ramp->ColorFile, 0x0,
					//	sizeof(grid->ramp->ColorFile));
					//strncpy(grid->ramp->ColorFile, Name, 255);
				}
				else
				{
					printf(" Random Colors will be used                                Color File Not Useable for This Landscape\n");				}
				fclose(cf);
			}

			SVGA_ON = false;
			memset(header, 0x0, 16 * sizeof(char));
			fgets(header, 15, pf);//fscanf(pf, "%ld", &GridInterval);
			GridInterval = atol(header);
			//  				::GridInterval(GridInterval);
			FreeAllVectorThemes();
			long NumTheme, Color;

			memset(header, 0x0, 16 * sizeof(char));
			fgets(header, 15, pf);//fscanf(pf, "%ld", &j);			// print number of stored vectors
			j = atol(header);
			VectorTheme Theme, * theme;
			if (j > 0)
			{
				for (i = 0; i < j; i++)
				{
					memset(FileName1, 0x0, 256 * sizeof(char));
					fgets(FileName1, 255, pf);
					sscanf(FileName1, "%s %ld %ld %lu %d %d %d %d\n",
						Theme.FileName, &Theme.FileType, &Theme.Permanent,
						&Color, &Theme.PenStyle, &Theme.PenWidth,
					 	&Theme.OnOff, &Theme.OnOff3d);
					if ((cf = fopen(Theme.FileName, "r")) != NULL)
					{
						memset(Name, 0x0, 256 * sizeof(char));
						getcwd(Name, 256);
						strcat(Name, "/");
						strcat(Name, Theme.FileName);
						strcpy(Theme.FileName, Name);
						NumTheme = AllocVectorTheme();
						if (NumTheme > -1)
						{
							theme = GetVectorTheme(NumTheme);
							memcpy(theme, &Theme, sizeof(VectorTheme));
						}
						fclose(cf);
					}
					else
						printf(" Theme.FileName  Vector File Not Found in Current Directory\n");				}
			}
			memset(header, 0x0, 16 * sizeof(char));
			fgets(header, 15, pf);//fscanf(pf, "%ld", &j);			// print number of stored vectors
			j = atol(header);//
			Themes* rtheme;
			if (j > 0)
			{
				for (i = 0; i < j; i++)
				{
					memset(Name, 0x0, 256 * sizeof(char));
					fgets(Name, 255, pf);//fscanf(pf, "%s\n", Name);
					memset(FileName1, 0x0, 256 * sizeof(char));
					strncpy(FileName1, Name, strlen(Name) - 1);
					if ((cf = fopen(FileName1, "r")) != NULL)
					{
						memset(Name, 0x0, 256 * sizeof(char));
						getcwd(Name, 256);
						strcat(Name, "/");
						strcat(Name, FileName1);
						strcpy(FileName1, Name);
						rtheme = AllocRasterTheme();
						if (NumTheme > -1)
						{
							//rtheme=GetRasterTheme(NumTheme);
							rtheme->theme->SetTheme(FileName1);
							rtheme->theme->WantNewRamp = true;
							rtheme->theme->WantNewColor = false;
							rtheme->theme->ColorChange = -1;
							rtheme->theme->OnOff = false;
							rtheme->theme->OnOff3d = false;
						}
						fclose(cf);
					}
					else
						printf(" Name  Raster File Not Found in Current Directory\n");				}
			}
			if (!feof(pf))
			{
				memset(FileName1, 0x0, 256 * sizeof(char));
				fgets(FileName1, 255, pf);//fscanf(pf, "%lf", &north);
				north = atof(FileName1);//
				memset(FileName1, 0x0, 256 * sizeof(char));
				fgets(FileName1, 255, pf);//fscanf(pf, "%lf", &south);
				south = atof(FileName1);//
				memset(FileName1, 0x0, 256 * sizeof(char));
				fgets(FileName1, 255, pf);//fscanf(pf, "%lf", &east);
				east = atof(FileName1);//
				memset(FileName1, 0x0, 256 * sizeof(char));
				fgets(FileName1, 255, pf);//fscanf(pf, "%lf", &west);
				west = atof(FileName1);//
				SetLandscapeViewPort(north, south, east, west);
				if (GetNumViewEast() > GetNumViewNorth())   /*SETS PIXEL SPACING AT MINIMUM X/Y DIMENSION*/
				{
					SetTint(8000.0 / (double) GetNumViewEast());
					SetMaxWindowDim((long)(GetTint() * GetNumViewEast()));
				}
				else
				{
					SetTint(8000.0 / (double) GetNumViewNorth());
					SetMaxWindowDim((long)(GetTint() * GetNumViewNorth()));
				}
				ViewPortStatus(true);
				ViewPortChanged(true);
			}
			if (!feof(pf))
			{
				memset(val, 0x0, 7 * sizeof(double));
				for (i = 0; i < 7; i++)
				{
					memset(FileName1, 0x0, 256 * sizeof(char));
					fgets(FileName1, 255, pf);
					val[i] = atof(FileName1);
					if (val[i] == 0.0)
						break;
				}
				if (val[6] > 0.0)
					SetCanopyChx(val[0], val[1], val[2], val[3],
						(long) val[4], (long) val[5], (long) val[6]);
				else
					SetCanopyChx(15.0, 4.0, 0.2, 20.0, 100, 2, 1);
			}
		}
		else
		{
			printf(" ProjectFile  !! Something is Wrong With This File !!:\n");			fclose(pf);

			return false;
		}
		fclose(pf);
	}
	else
	{
		printf(" ProjectFile  !! UNABLE to open file !!:\n");
		return false;
	}
	
	return true;
}

//initDir call this at the top of each of the following variable sets
//no name=value pair for this use the current directory off of 
//the settings file.  need to create a global to get the name off of
//look to see if ProjectFile might suffice

bool FarInputs::intializeDir(char* Name){
   char CurrentDirectory[256] = "";
   getNameFromFile(ProjectFile, Name, 255);
   //ptr=strstr(ProjectFile, Name);
   strncpy(CurrentDirectory, ProjectFile, strlen(ProjectFile)-strlen(Name));
   chdir(CurrentDirectory);
   //getcwd(CurrentDirectory, 255);
   //strcat(CurrentDirectory, pathdiv);
   //GetCurrentDirectory(256, CurrentDirectory);
   //if((pf=fopen(bookmark.ProjectFile, "r"))!=NULL)
   return true;
}
//end initDir

void FarInputs::initWindAndWeather(){
   if(inited)
	return;
   int i;
   for (i = 0; i < 5; i++)    // initialize strings for fileanmes
   {
		memset(WeatherFile[i], 0x0, sizeof WeatherFile[i]);
        memset(WindFile[i], 0x0, sizeof WindFile[i]);
        strcpy(MBWtr[i], "Load Weather");
        strcpy(MBWnd[i], "Load Winds");
        FreeWeatherData(i);
        FreeWindData(i);
   }
   inited=true;
}

bool FarInputs::setWeatherFile(int WeatherFileNum, const char* weatherFileName){
  initWindAndWeather();  
  char RevCopy[256] = "";
  int length;
  bool success = false ; 

  strncpy(WeatherFile[WeatherFileNum], weatherFileName, FILE_LEN);
  if ((CurrentFile = fopen(WeatherFile[WeatherFileNum], "r")) == NULL)
  {
	printf("Weather File %s Not Found\n",WeatherFile[WeatherFileNum]);
	fflush(stdout);
	memset(WeatherFile[WeatherFileNum], 0x0, sizeof(WeatherFile[WeatherFileNum]));
	strcpy(MBWtr[WeatherFileNum], "Load Weather");
  }
  else
  {
	if (LoadWeatherData(WeatherFileNum))
	{
		WtrID = true;
		// copy file path string
		strcpy(RevCopy, WeatherFile[WeatherFileNum]);
		// reverse file path string
		strrev(RevCopy);
		// compute length without path
		length = strcspn(RevCopy, pathdiv);
		memset(MBWtr[WeatherFileNum], 0x0, sizeof MBWtr[WeatherFileNum]);
		// copy length w/o path
		strncpy(MBWtr[WeatherFileNum], RevCopy, length);	
		// reverse to get filename only
		strrev(MBWtr[WeatherFileNum]);

        success = true ; 
	}
	else
		memset(&WeatherFile[WeatherFileNum], 0x0, sizeof(WeatherFile[WeatherFileNum]));
  }
  return success ;
}


bool FarInputs::setWindFile(int WindFileNum, const char* WindFileName){
  initWindAndWeather();  
  char RevCopy[256] = "";
  int length;
  bool success = false; 

  strncpy(WindFile[WindFileNum], WindFileName, FILE_LEN);

  if ((CurrentFile = fopen(WindFile[WindFileNum], "r")) == NULL)
  {
	printf("WindFile %s Not Found\n",WindFile[WindFileNum]);
	memset(WindFile[WindFileNum], 0x0, sizeof(WindFile[WindFileNum]));
	strcpy(MBWnd[WindFileNum], "Load Winds");
  }
  else
  {
	if (LoadWindData(WindFileNum))
	{
		WndID = true;
		// copy file path string
		strcpy(RevCopy, WindFile[WindFileNum]);
		// reverse file path string
		strrev(RevCopy);
  		// compute length without path
		length = strcspn(RevCopy, pathdiv);
		memset(MBWnd[WindFileNum], 0x0, sizeof MBWnd[WindFileNum]);
		// copy length w/o path
		strncpy(MBWnd[WindFileNum], RevCopy, length);
		// reverse to get filename only
		strrev(MBWnd[WindFileNum]);

        success = true ; 
	}
	else
		memset(&WindFile[WindFileNum], 0x0, sizeof(WindFile[WindFileNum]));
  }
  return success ;
}

/** please note this is different from the SetLandFileName
 *  this function calls SetLandFileName but does some decoration and 
 *  intialization of it first.
 */
bool FarInputs::setLandscapeFile(const char* value){
  int i=0;
  strncpy(LandscapeFile, value, FILE_LEN) ; 
  SetLandFileName(LandscapeFile);
  if (OpenLandFile())
  {
	if (TestForLCPVersion1())
	{
		printf(" .LCP File Will Be Converted to v.2 Format  .LCP File Format v.1 Detected\n");
		if (!ConvertLCPFileToVersion2())
		{
			CloseLandFile();
			i = 0;
			printf(" Generate New .LCP File before Continuing  Error Reading .LCP File\n");
		}
		else
			i = 1;
	}
	else if (TestForLCPVersion2())
	{
		printf(" .LCP File Will Be Converted to v.4 Format  .LCP File Format v.2 Detected\n");
		if (!ConvertLCPFile2ToVersion4())
		{
			CloseLandFile();
			i = 0;
			printf(" Generate New .LCP File before continuing  Error Reading .LCP File\n");
		}
		else
			i = 1;
	}
	else{
		i = 1;
	}
	if (i == 1){
	  
		InitializeLandscapeFile();
	}
  }
  else
  {
	  printf(" GetLandFileName()  File Not Found\n");
	  SetLandFileName("");
	  memset(ProjectFile, 0x0, sizeof(ProjectFile));
	  memset(LandscapeFile, 0x0, sizeof(LandscapeFile));
	  return false;
  }
  return true;
}

/** note that this has something to do with HEAD
 *  I don't know what HEAD is yet
 */
bool FarInputs::loadUpAdjustmentsFile(const char* adjFileName){
  bool success = false ;
  if (version > 50)
  {
	  m_BurnPeriodIsHere = true;
	  version -= 10;
  }
  if (version > 41)
  {

    char RevCopy[256] = "";
    int length;
	  if ((CurrentFile = fopen(adjFileName, "r")) != NULL)
	  {
			strncpy(AdjustmentFile, adjFileName, FILE_LEN);
		  if (LoadAdjustmentFile())
		  {
			  // copy file path string
			  strcpy(RevCopy, adjFileName);
			  // reverse file path string
			  strrev(RevCopy);
			  // compute length without path
			  length = strcspn(RevCopy, pathdiv);
			  memset(MBAdj, 0x0, sizeof MBAdj);
			  // copy length w/o path
			  strncpy(MBAdj, RevCopy, length);
			  strrev(MBAdj);

        success = true ; 
		  }
		  else
			  memset(AdjustmentFile, 0x0, sizeof(AdjustmentFile));
	  }
	  else
		  printf(" Adjustment File Not Found or Unreadable  Can't Read File\n");
   }
   return success;
}

/** note that this has something to do with HEAD
 *  I don't know what HEAD is yet
 *  This just assumes a valid version for the moisture file
 *  I think that is a pretty safe assumption being that this is in
 *  a whole new environment with a whole new file format
 */
bool FarInputs::loadUpMoistureFile(const char* moistureFile){
  char RevCopy[256] = "";
  int length;
  bool success = false ; 

	if ((CurrentFile = fopen(moistureFile, "r")) != NULL)
	{
	  strncpy(FuelMoistureFile, moistureFile, FILE_LEN);
		if (LoadFuelMoistureFile())
		{
			strcpy(RevCopy, moistureFile);		// copy file path string
			strrev(RevCopy);				// reverse file path string
			length = strcspn(RevCopy, pathdiv); // compute length without path
			memset(MBMois, 0x0, sizeof MBMois);
			strncpy(MBMois, RevCopy, length);  // copy length w/o path
			strrev(MBMois);

      success = true ;
		}
		else
			memset(FuelMoistureFile, 0x0, sizeof(FuelMoistureFile));
	}
	else
		printf(" Fuel Moisture File Not Found or Unreadable  Can't Read File\n");		
	fflush(stdout);
  return success ;
}

bool FarInputs::loadUpFuelModelFile(const char* moistureFile){
 bool success = false ; 
 if (version == 43 || version == 44 || version == 47 || version == 48)
  {
  char RevCopy[256] = "";
  int length;

	if ((CurrentFile = fopen(moistureFile, "r")) != NULL)
	{
		strncpy(FuelModelFile, moistureFile, FILE_LEN);
		if (LoadCustomModelFile())
		{
			strcpy(FuelModelFile, moistureFile);
			// copy file path string
			strcpy(RevCopy, moistureFile);
			// reverse file path string
			strrev(RevCopy);	
 			// compute length without path
			length = strcspn(RevCopy, pathdiv);
			memset(MBFuelMods, 0x0, sizeof MBFuelMods);
  			// copy length w/o path
			strncpy(MBFuelMods, RevCopy, length);
			strrev(MBFuelMods);
  
            success = true ; 
		}
		else
			memset(FuelModelFile, 0x0, sizeof(FuelModelFile));
	}
	else
		printf(" Custom Fuel Model File Not Found or Unreadable  Can't Read File\n");
	fflush(stdout);
  }
  return success;
}

bool FarInputs::loadUpConversionFile(const char* convertFile){
  char RevCopy[256] = "";
  int length;
  bool success = false; 

  // store filename in this object
  strncpy(ConvertFile, convertFile, FILE_LEN) ; 

  if (version == 44 || version == 45 || version == 48 || version == 49)
  {
	  if ((CurrentFile = fopen(ConvertFile, "r")) != NULL)
	  {
		  if (LoadConversionFile())
		  {
			  // copy file path string
			  strcpy(RevCopy, ConvertFile);
			  // reverse file path string
			  strrev(RevCopy);
			  // compute length without path
			  length = strcspn(RevCopy, pathdiv);
			  memset(MBConv, 0x0, sizeof MBConv);
			  // copy length w/o path
			  strncpy(MBConv, RevCopy, length);
			  strrev(MBConv);

              success = true ; 
		  }
	  }
	  else
	  	printf(" Conversion File Not Found or Unreadable  Can't Read File\n");
	  fflush(stdout);
  }
  return success ;
}


bool FarInputs::loadUpCoarseWoodyFile(const char* FileName1){
  char RevCopy[256] = "";
  int length;
  bool success = false ; 

  if (version > 45)
  {
	  if ((CurrentFile = fopen(CoarseWoody, "r")) != NULL)
	  {
	      strncpy(CoarseWoody, FileName1, FILE_LEN);

          // argument below is an Error message which is passed back!
		  if (LoadCoarseWoodyFile(RevCopy))
		  {
			  strcpy(RevCopy, FileName1);		// copy file path string
			  strrev(RevCopy);				// reverse file path string
			  length = strcspn(RevCopy, pathdiv); // compute length without path
			  memset(MBCwd, 0x0, sizeof(MBCwd));
			  strncpy(MBCwd, RevCopy, length);  // copy length w/o path
			  strrev(MBCwd);

              success = true ; 
		  }
		  else
		  {
			  printf("Coarse Woody File %s Not Loaded\n",FileName1);
			  fflush(stdout);
			  memset(CoarseWoody, 0x0, sizeof(CoarseWoody));
		  }
  	}
  	else
	  printf(" Coarse Woody File Not Found or Unreadable  Can't Read File\n");
  	fflush(stdout);
  }
  return success;
}


bool FarInputs::loadUpBurnPeriodFile(const char* FileName1){
  char RevCopy[256] = "";
  int length;
  bool success = false ; 

  if (m_BurnPeriodIsHere)
  {
	  if ((CurrentFile = fopen(FileName1, "r")) != NULL)
	  {
		  strncpy(BurnPeriod, FileName1, FILE_LEN);
		  if (LoadBurnPeriodFile())
		  {
			  strcpy(RevCopy, FileName1);		// copy file path string
			  strrev(RevCopy);				// reverse file path string
			  length = strcspn(RevCopy, pathdiv); // compute length without path
			  memset(MBBpd, 0x0, sizeof(MBBpd));
			  strncpy(MBBpd, RevCopy, length);  // copy length w/o path
			  strrev(MBBpd);
        
              success = true ; 
		  }
		  else
			  memset(BurnPeriod, 0x0, sizeof(BurnPeriod));
	  }
	  else
	    printf(" Burn Period File %s Not Found or Unreadable\n\n",
        FileName1);						
      fflush(stdout);
  }

  return success ;
}


void FarInputs::saveGridEastNorth(char* eastNorth){
	if(m_gridRowSet){
	   loadUpEastNorth(eastNorth,m_GridRow);
	}
	else{
	  strcpy(m_eastNorth,eastNorth);
	  m_gridEastNorthSet=true;
	}
}
void FarInputs::saveGridRow(char* GridRow){
	if(m_gridEastNorthSet){
	   loadUpEastNorth(m_eastNorth,GridRow);
	}
	else{
	  strcpy(m_GridRow,GridRow);
	  m_gridRowSet=true;
	}
}


bool FarInputs::loadUpEastNorth(char* eastNorth, char* GridRow){
	long NumGEast, NumGNorth, NumAlloc;   // retrieve and load weather grid information
	int i,j;
	long Val;
	//NumAlloc = atoi(GridRow);
	sscanf(eastNorth, "%ld %ld", &NumGEast, &NumGNorth);
	AllocStationGrid(NumGEast, NumGNorth);
	NumAlloc = NumGEast * 3;
	if (NumAlloc < 512)
		NumAlloc = 512;
	//GridRow = new char[NumAlloc];
	for (i = 0; i < NumGNorth; i++)
	{
		//memset(GridRow, 0x0, NumAlloc * sizeof(char));
		//fgets(GridRow, NumAlloc, pf);// 	fscanf(pf, "%ld", &HEAD);
		for (j = 0; j < NumGEast; j++)
		{
			sscanf(&GridRow[j * 2], "%ld", &Val);
			SetStationNumber(j, i, Val);
		}
	}
	SetGridEastDimension(NumGEast);
	SetGridNorthDimension(NumGNorth);
	//delete[] GridRow;
	return true;
}


bool FarInputs::loadUpLandscapeTheme(const char* FileName1){
  FILE* cf = NULL;
  char Name[256] = "";

  strncpy(Name, FileName1, 256);
  cf = fopen(FileName1, "rb");

  if (cf)
  {
	long num;//, pos, pos2;
	char krap[256], temp1[256] = "", temp2[256] = "";
	bool tempb;
	long templ;
	double tempd;
	long tempc[100];
	long i;
	LandscapeTheme* grid;
	COLORREF colr;

	grid = GetLandscapeTheme();
	fread(krap, sizeof(char), 256, cf);
	strrev(krap);
	strrev(grid->Name);
	strncpy(temp1, krap, strlen(Name));
	strncpy(temp2, grid->Name, strlen(Name));
	strrev(temp1);
	strrev(temp2);
	strrev(krap);
	strrev(grid->Name);
	if (!strcmp(strlwr(temp1), strlwr(temp2)))
	{
		fread(&tempb, sizeof(bool), 1, cf);
		grid->LcpAscii = false; // tempb
		fread(&templ, sizeof(long), 1, cf);
		grid->Continuous = templ;
		fread(&templ, sizeof(long), 1, cf);
		grid->RedVal = templ;
		fread(&templ, sizeof(long), 1, cf);
		grid->GreenVal = templ;
		fread(&templ, sizeof(long), 1, cf);
		grid->BlueVal = templ;
		fread(&templ, sizeof(long), 1, cf);
		grid->VarVal = templ;
		fread(&templ, sizeof(long), 1, cf);
		grid->MaxBrite = templ;
		fread(&templ, sizeof(long), 1, cf);
		grid->ColorChange = templ;
		fread(&templ, sizeof(long), 1, cf);
		grid->CatsOK = templ;
		fread(tempc, sizeof(long), 100, cf);
		memcpy(grid->Cats, tempc, sizeof(tempc));
		fread(&tempd, sizeof(double), 1, cf);
		grid->MaxVal = tempd;
		fread(&tempd, sizeof(double), 1, cf);
		grid->MinVal = tempd;
		fread(&num, sizeof(long), 1, cf);
		if (num < 0 || grid->Continuous == 0)
		{
			grid->NumCats = abs(num);
			grid->NumColors = 0;
		}
		else
		{
			grid->NumColors = abs(num);
		}
		grid->CreateRamp();
		grid->WantNewRamp = false;
		if (num > 0)
		{
			for (i = 0; i <= grid->NumColors; i++)
			{
				fread(&colr, sizeof(COLORREF), 1, cf);
			}
		}
	}
	else
	{
		printf(" Random Colors will be used Color File Not Useable for This Landscape\n");
	}
	fclose(cf);
  }
  return true;
}

void FarInputs::initShapeFileLoad(){
   if(!m_shapeFileInited){
        SVGA_ON = false;
	FreeAllVectorThemes();
	m_shapeFileInited=true;
   }	
}

/** this should get called for each shape file definition
 *  the shape file definition it defined in the scanf below
 */  
bool FarInputs::loadUpShapeFile(char* shapefile){
  char Name[256] = "";
  //long GridInterval, j;
  //sscanf(FileName1, "%ld %ld", &GridInterval, &j);
  initShapeFileLoad();
  FILE* cf = NULL;
  long NumTheme, Color;
  VectorTheme Theme, * theme;
  sscanf(shapefile, "%s %ld %ld %lu %d %d %d %d\n",
	Theme.FileName, &Theme.FileType, &Theme.Permanent,
	&Color, &Theme.PenStyle, &Theme.PenWidth,
 	&Theme.OnOff, &Theme.OnOff3d);
  if ((cf = fopen(Theme.FileName, "r")) != NULL)
  {
	memset(Name, 0x0, 256 * sizeof(char));
	getcwd(Name, 256);
	strcat(Name, "/");
	strcat(Name, Theme.FileName);
	strcpy(Theme.FileName, Name);
	NumTheme = AllocVectorTheme();
	if (NumTheme > -1)
	{
		theme = GetVectorTheme(NumTheme);
		memcpy(theme, &Theme, sizeof(VectorTheme));
	}
	fclose(cf);
  }
  else
	printf(" Theme.FileName  Vector File Not Found in Current Directory\n");
  return true;
}			

bool FarInputs::loadUpStoredVector(char* storedVectorFile){
  Themes* rtheme;
  FILE* cf = NULL;
  if ((cf = fopen(storedVectorFile, "r")) != NULL)
  {
	rtheme = AllocRasterTheme();
	//if (NumTheme > -1)
	//{
		//rtheme=GetRasterTheme(NumTheme);
		rtheme->theme->SetTheme(storedVectorFile);
		rtheme->theme->WantNewRamp = true;
		rtheme->theme->WantNewColor = false;
		rtheme->theme->ColorChange = -1;
		rtheme->theme->OnOff = false;
		rtheme->theme->OnOff3d = false;
	//}
	fclose(cf);
  }
  else
	printf(" Raster File %s Not Found in Current Directory\n",storedVectorFile);
  return true;
}

bool FarInputs::loadUpSetDirection(char* value,char direction){
  double north,south,east,west;
  switch(direction){
	case 'N':
		north = atof(value);
		m_dcount++;
		break;
	case 'S':
		south = atof(value);
		m_dcount++;
		break;
	case 'E':
		east = atof(value);
		m_dcount++;
		break;
	case 'W':
		west = atof(value);
		m_dcount++;
		break;
  }
  if(m_dcount==4){
	SetLandscapeViewPort(north, south, east, west);
        /*SETS PIXEL SPACING AT MINIMUM X/Y DIMENSION*/
	if (GetNumViewEast() > GetNumViewNorth())
	{
		SetTint(8000.0 / (double) GetNumViewEast());
		SetMaxWindowDim((long)(GetTint() * GetNumViewEast()));
	}
	else
	{
		SetTint(8000.0 / (double) GetNumViewNorth());
		SetMaxWindowDim((long)(GetTint() * GetNumViewNorth()));
	}
	ViewPortStatus(true);
	ViewPortChanged(true);
  }
  return true;
}


bool FarInputs::loadUpCanopyCheck(char* value){
  double val[7];
  memset(val, 0x0, 7 * sizeof(double));
  sscanf(value, "%lf %lf %lf %lf %lf %lf %lf", &val[0], 
	&val[1], &val[2],&val[3],&val[4],&val[5],&val[6]);
  if (val[6] > 0.0)
	SetCanopyChx(val[0], val[1], val[2], val[3],
		(long) val[4], (long) val[5], (long) val[6]);
  else
	SetCanopyChx(15.0, 4.0, 0.2, 20.0, 100, 2, 1);
  return true;
}


void FarInputs::MakeProjectFile()
{
	int temphead, head = 41;
	bool FirstTime = true;
	char Name[256] = "";
	long i, j, StationNumber;
	COLORREF colr;

	//CurrentFile=fopen(bookmark.ProjectFile, "w");
	CurrentFile = fopen(ProjectFile, "w");
	while (CurrentFile == NULL)
	{
		if (FirstTime)
		{
			getNameFromFile(ProjectFile, Name, 255);
			strcpy(ProjectFile, Name);
			FirstTime = false;
		}
		//  		else
		//  		   strcpy(ProjectFile, FilenameData.FileName);
		//  		{    TOpenSaveDialog::TData FilenameData(OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		//				"PROJECT FILES (*.FPJ)|*.FPJ|", 0, GetCurrentPath(), "*");
		//			if(TFileOpenDialog(Client, FilenameData, 0, "Select Project File").Execute()==IDOK)
		//  			 	strcpy(ProjectFile, FilenameData.FileName);
		//  			 else
		//  			 	break;
		//  		}
		//SetFileAttributes(ProjectFile, FILE_ATTRIBUTE_NORMAL);
		CurrentFile = fopen(ProjectFile, "w");
	};

	if (CurrentFile == NULL)
		return;

	if (AdjID && FuelMoisID)
	{
		head = 42;
		if (HaveCustomFuelModels())
		{
			head = 43;
			if (HaveFuelConversions())
				head = 44;
		}
		else
		{
			if (HaveFuelConversions())
				head = 45;
		}
	}
	temphead = head;
	if (CwdID)
		head += 4;
	if (BpdID)
		head += 10;
	switch (temphead)
	{
	case 41:
		fprintf(CurrentFile, "%d\n", head);
		fprintf(CurrentFile, "%s\n", MBLand);
		WriteWeatherWindProject();
		fprintf(CurrentFile, "\n");
		break;
	case 42:
		fprintf(CurrentFile, "%d\n", head);
		fprintf(CurrentFile, "%s\n", MBLand);
		WriteWeatherWindProject();
		fprintf(CurrentFile, "%s\n", MBAdj);
		fprintf(CurrentFile, "%s\n", MBMois);
		break;
	case 43:
		fprintf(CurrentFile, "%d\n", head);
		fprintf(CurrentFile, "%s\n", MBLand);
		WriteWeatherWindProject();
		fprintf(CurrentFile, "%s\n", MBAdj);
		fprintf(CurrentFile, "%s\n", MBMois);
		fprintf(CurrentFile, "%s\n", MBFuelMods);
		break;
	case 44:
		fprintf(CurrentFile, "%d\n", head);
		fprintf(CurrentFile, "%s\n", MBLand);
		WriteWeatherWindProject();
		fprintf(CurrentFile, "%s\n", MBAdj);
		fprintf(CurrentFile, "%s\n", MBMois);
		fprintf(CurrentFile, "%s\n", MBFuelMods);
		fprintf(CurrentFile, "%s\n", MBConv);
		break;
	case 45:
		fprintf(CurrentFile, "%d\n", head);
		fprintf(CurrentFile, "%s\n", MBLand);
		WriteWeatherWindProject();
		fprintf(CurrentFile, "%s\n", MBAdj);
		fprintf(CurrentFile, "%s\n", MBMois);
		fprintf(CurrentFile, "%s\n", MBConv);
		break;
	}
	if (CwdID)
		fprintf(CurrentFile, "%s\n", MBCwd);
	if (BpdID)
		fprintf(CurrentFile, "%s\n", MBBpd);
	if (GridInitialized())    // write station grid if initialized
	{
		double Width = (GetHiEast() - GetLoEast()) /
			(double) GetGridEastDimension();
		double Height = (GetHiNorth() - GetLoNorth()) /
			(double) GetGridNorthDimension();
		double xpt;
		double ypt = GetLoNorth() + Height / 2.0;
		fprintf(CurrentFile, "%ld %ld\n", GetGridEastDimension(),
			GetGridNorthDimension());
		for (i = 0; i < GetGridNorthDimension(); i++)
		{
			xpt = GetLoEast() + Width / 2.0;
			for (j = 0; j < GetGridEastDimension(); j++)
			{
				StationNumber = GetStationNumber(xpt, ypt);
				fprintf(CurrentFile, "%ld ", StationNumber);
				xpt += Width;
			}
			fprintf(CurrentFile, "\n");
			ypt += Height;
		}
	}
	else
	{
		fprintf(CurrentFile, "%i %i\n", 1, 1);
		fprintf(CurrentFile, "%i\n", 1);
	}

	//	for(i=0; i<52; i++) 		// write fuel model colors to project file
	///	{	colr=GetFuelColor(i);
	//		if(i<13 || colr>0)
	//			fprintf(CurrentFile, "%ld %lu\n", i, colr);
	//	}

	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//	Create a .CLR file that can also be read into the landscape
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	long num = 0;
	char RevCopy[256] = "";
	char pathdiv[] = "/";              		// search for path
	LandscapeTheme* grid = GetLandscapeTheme();

	strncpy(RevCopy, ProjectFile, strlen(ProjectFile) - 4);			// copy file path string
	strrev(RevCopy);										// reverse file path string
	int length = strcspn(RevCopy, pathdiv);   // compute length without path
	memset(Name, 0x0, 256 * sizeof(char));
	strncpy(Name, RevCopy, length);		// copy length w/o path
	strrev(Name);							// reverse to get filename only
	strcat(Name, ".CLR");

	//SetFileAttributes(Name, FILE_ATTRIBUTE_NORMAL);
	if (grid)
	{
		FILE* cf = fopen(Name, "wb");

		fwrite(grid->Name, sizeof(char), 256, cf);
		fwrite(&grid->LcpAscii, sizeof(bool), 1, cf);
		fwrite(&grid->Continuous, sizeof(long), 1, cf);
		fwrite(&grid->RedVal, sizeof(long), 1, cf);
		fwrite(&grid->GreenVal, sizeof(long), 1, cf);
		fwrite(&grid->BlueVal, sizeof(long), 1, cf);
		fwrite(&grid->VarVal, sizeof(long), 1, cf);
		fwrite(&grid->MaxBrite, sizeof(long), 1, cf);
		fwrite(&grid->ColorChange, sizeof(long), 1, cf);
		fwrite(&grid->CatsOK, sizeof(long), 1, cf);
		fwrite(grid->Cats, sizeof(long), 100, cf);
		fwrite(&grid->MaxVal, sizeof(double), 1, cf);
		fwrite(&grid->MinVal, sizeof(double), 1, cf);

		if (false) //grid->ramp)
		{
			//num = grid->ramp->GetNumColors();
			fwrite(&num, sizeof(long), 1, cf);

			for (i = 0; i <= num; i++)
			{
				//colr = grid->ramp->GetColor(i);
				fwrite(&colr, sizeof(COLORREF), 1, cf);
			}
		}
		else
		{
			num = grid->NumAllCats[GetVisibleTheme()];
			num *= -1;
			fwrite(&num, sizeof(long), 1, cf);
		}

		fclose(cf);
	}
	fprintf(CurrentFile, "%s\n", Name);
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------

	//if(SVGA_ON)
	//	fprintf(CurrentFile, "%ld\n", -99);    // end of fuel model colors
	//else
	//	fprintf(CurrentFile, "%ld\n", 99);
	//	fprintf(CurrentFile, "%ld\n", ::GridInterval(GETVAL));
	j = GetNumVectorThemes();
	fprintf(CurrentFile, "%ld\n", j);			// print number of stored vectors
	if (j > 0)
	{
		VectorTheme* vtheme;
		char RevCopy[256], VectName[256];
		char pathdiv[] = "/";              		// search for path
		int length;
          long Color=0;

		for (i = 0; i < j; i++)
		{
			vtheme = GetVectorTheme(i);
			strcpy(RevCopy, vtheme->FileName);	// copy file path string
			strrev(RevCopy);						// reverse file path string
			length = strcspn(RevCopy, pathdiv); 	  // compute length without path
			memset(VectName, 0x0, sizeof(VectName));
			strncpy(VectName, RevCopy, length);		// copy length w/o path
			strrev(VectName);						// reverse to get filename only
			fprintf(CurrentFile, "%s %ld %ld %lu %d %d %d %d\n", VectName,
				vtheme->FileType, vtheme->Permanent, Color,
				vtheme->PenStyle, vtheme->PenWidth, vtheme->OnOff,
				vtheme->OnOff3d);
		}
	}
	j = GetNumRasterThemes();
	fprintf(CurrentFile, "%ld\n", j);			// print number of stored vectors
	if (j > 0)
	{
		Themes* rtheme;
		char RevCopy[256], RastName[256];
		char pathdiv[] = "/";              		// search for path
		int length;

		for (i = 0; i < j; i++)
		{
			rtheme = GetRasterTheme(i);
			strcpy(RevCopy, rtheme->theme->Name);	// copy file path string
			strrev(RevCopy);						// reverse file path string
			length = strcspn(RevCopy, pathdiv); 	  // compute length without path
			memset(RastName, 0x0, sizeof(RastName));
			strncpy(RastName, RevCopy, length);		// copy length w/o path
			strrev(RastName);						// reverse to get filename only
			fprintf(CurrentFile, "%s\n", RastName);
		}
	}
	fprintf(CurrentFile, "%lf\n", GetViewNorth());
	fprintf(CurrentFile, "%lf\n", GetViewSouth());
	fprintf(CurrentFile, "%lf\n", GetViewEast());
	fprintf(CurrentFile, "%lf\n", GetViewWest());
	fprintf(CurrentFile, "%lf\n%lf\n%lf\n%lf\n%lf\n%ld\n%ld\n",
		GetDefaultCrownHeight(), GetDefaultCrownBase(),
		GetDefaultCrownBD(0), GetAverageDBH(), GetFoliarMC(),
		GetTolerance(), GetCanopySpecies());
	fclose(CurrentFile);
}


void FarInputs::WriteWeatherWindProject()
{
	//WRITES THE MULTIPLE WEATHER AND WIND FILES TO THE PROJECT FILE
	long i, NumWtr = 0, NumWnd = 0;
	while (strcmp(MBWtr[NumWtr], "Load Weather")) //(strlen(MBWtr[NumWtr])>0)
	{
		NumWtr++;
		if (NumWtr > 4)
			break;
	}
	while (strcmp(MBWnd[NumWnd], "Load Winds")) //(strlen(MBWnd[NumWnd])>0))
	{
		NumWnd++;
		if (NumWnd > 4)
			break;
	}
	fprintf(CurrentFile, "%ld\n", NumWtr);
	for (i = 0; i < NumWtr; i++)
		fprintf(CurrentFile, "%s\n", MBWtr[i]);
	fprintf(CurrentFile, "%ld\n", NumWnd);
	for (i = 0; i < NumWnd; i++)
		fprintf(CurrentFile, "%s\n", MBWnd[i]);
}


bool FarInputs::LoadBurnPeriodFile()
{
	bool result;
	char Line[256] = "";
	long num, i, Month, Day, Start1, End1;//, Start2, End2;
	long date, datemin, datemax;

	if (!CalcFirstLastStreamData())	// compute valid starting and ending dates
	{
		printf(" Edit or Reload WTR or WND files                 Error: Dates in Weather and Wind Files Don't Overlap\n");		fclose(CurrentFile);

		return false;
	}
	datemin = GetJulianDays(GetMinMonth()) + GetMinDay();
	datemax = GetJulianDays(GetMaxMonth()) + GetMaxDay();

	i = 0;
	result = true;
	do
	{
		fgets(Line, 255, CurrentFile);
		num = sscanf(Line, "%ld %ld %ld %ld", &Month, &Day, &Start1, &End1);//, &Start2, &End2);
		if (num <= 0)
			break;

		date = GetJulianDays(Month) + Day;
		if (GetMinMonth() > GetMaxMonth())
		{
			if (date > datemin && date < 366)
				result = true;
			else if (date<datemin && date>datemax)
				result = false;
			else
				result = true;
		}
		else
		{
			if (date < datemin)
				result = false;
			else if (date > datemax)
				result = false;
			else
				result = true;
		}

		if (result == false)
		{
			printf(" You Must Correct, Eliminate, or Create New BPD File  	               Error: Start or End Dates in WTR & WND Files Don't Coincide with BPD File\n");			fclose(CurrentFile);

			return false;
		}

		i++;
		memset(Line, 0x0, 256 * sizeof(char));
	}
	while (!feof(CurrentFile));

	AllocBurnPeriod(i);
	rewind(CurrentFile);
	i = 0;
	do
	{
		fgets(Line, 255, CurrentFile);
		num = sscanf(Line, "%ld %ld %ld %ld", &Month, &Day, &Start1, &End1);//, &Start2, &End2);
		if (num <= 0)
			break;
		SetBurnPeriod(i++, Month, Day, Start1, End1);
		memset(Line, 0x0, 256 * sizeof(char));
	}
	while (!feof(CurrentFile));

	BpdID = true;
	fclose(CurrentFile);

	return true;
}


bool FarInputs::LoadCoarseWoodyFile(char* ErrMsg)
{
	char Line[256] = "", unitstring[256] = "";
	char testload[256] = "", testdiam[256] = "", density[256] = "",
		testht[256] = "", testfm[256] = "";
	char testnum[256] = "", text[256] = "", testdepth[256] = "",
		desc[256] = "";
	long i, j, wunits, num = 0, scannum;
	double sav, load, ht, fm, depth, dense, diameter;
	double load_mult, ht_mult, depth_mult, dense_mult, diam_mult, sav_mult;
	long ErrNo = 0, LineNo = 1;
	WoodyData wd;
	long* NumClasses;

	NumClasses = new long[MAXNUM_COARSEWOODY_MODELS];
//	ZeroMemory(NumClasses, MAXNUM_COARSEWOODY_MODELS * sizeof(long));
	memset(NumClasses,0x0, MAXNUM_COARSEWOODY_MODELS * sizeof(long));

	fgets(unitstring, 255, CurrentFile);
	if (!strncmp(strupr(unitstring), "ENGLIS", 6))
	{
		wunits = 1;
		sav_mult = 3.280839895;	// 1/ft to 1/m
		diam_mult = 0.0254;		// inches to m
		ht_mult = 2.32599;  	   // btu/lb to J/kg
		load_mult = 0.224169061; 	// tons/ac to kg/m2
		depth_mult = 0.3048;		// feet to meters
		dense_mult = 16.0184633737; // lb/ft3 to kg/m3
	}
	else if (!strncmp(strupr(unitstring), "METRIC", 6))
	{
		wunits = 2;
		sav_mult = 1.0;			// 1/m to 1/m
		diam_mult = 0.01;		// cm to m
		ht_mult = 1.0;  	   	// J/kg to J/kg
		load_mult = 0.1;		 	// Mg/ha to kg/m2
		depth_mult = 1.0;		// meters to meters
		dense_mult = 1.0;
	}
	else
	{
		sprintf(ErrMsg, "Need 'ENGLISH' or 'METRIC' at top of file");
		fclose(CurrentFile);
		delete[] NumClasses;

		return false;
	}

	i = 0;
	do
	{
		memset(Line, 0x0, sizeof(Line));
		fgets(Line, 255, CurrentFile);
		if (feof(CurrentFile))
			break;
		sscanf(Line, "%s %s %s", text, testnum, desc);
		LineNo++;
		if (!strcmp(strupr(text), "MODEL"))
		{
			if (i > 0)
			{
				NumClasses[num - 1] = i;
				i = 0;
			}
			num = atol(testnum);
			if (num<1 || num>MAXNUM_COARSEWOODY_MODELS)
			{
				ErrNo = 1;
				break;
			}
			memset(Line, 0x0, sizeof(Line));
			fgets(Line, 255, CurrentFile);
			if (feof(CurrentFile))
				break;
			sscanf(Line, "%s %s", text, testdepth);
			if (strcmp(strupr(text), "DEPTH"))
			{
				ErrNo = 2;
				break;
			}
			else
			{
				depth = atof(testdepth) * depth_mult;
				if (depth<0.05 || depth>4.0)
				{
					ErrNo = 3;
					break;
				}
			}
			memset(Line, 0x0, sizeof(Line));
			fgets(Line, 255, CurrentFile);
		}
		scannum = sscanf(Line, "%s %s %s %s %s", testdiam, testload, testht,
					density, testfm);
		LineNo++;
		if (scannum <= 0)
			continue;
		else if (scannum != 5)
		{
			ErrNo = 9;
			break;
		}
		ht = atof(testht) * ht_mult;
		load = atof(testload) * load_mult;
		diameter = atof(testdiam) * diam_mult;  // result is m
		if (diameter > 1e-4)
			sav = 4.0 / diameter;
		else
			sav = 20000.0;
		fm = atof(testfm);
		if (num<1 || num>MAXNUM_COARSEWOODY_MODELS)
		{
			ErrNo = 1;
			break;
		}
		if (ht<14000.0 || ht>24000.0)
		{
			ErrNo = 4;
			break;
		}
		if (load<0.001 || load>60.0)
		{
			ErrNo = 5;
			break;
		}
		if (sav<2.0 || sav>10000.0)
		{
			ErrNo = 6;
			break;
		}
		if (fm<2.0 || fm>300.0)
		{
			ErrNo = 7;
			break;
		}
		if (!strncmp(strupr(density), "R", 1))
			dense = 300.0; //density;
		else if (!strncmp(strupr(density), "S", 1))
			dense = 513.0;//	kg/m3
		else
			dense = atof(density) * dense_mult;
		if (dense<200.0 || dense>1000.0)
		{
			ErrNo = 8;
			break;
		}

		memset(Line, 0x0, sizeof(Line));
		i++;
	}
	while (!feof(CurrentFile));
	NumClasses[num - 1] = i;
	if (ErrNo > 0)
	{
		fclose(CurrentFile);

		switch (ErrNo)
		{
		case 1:
			sprintf(ErrMsg, "Model Number out of Range: 1 - %i, Line %li",
				MAXNUM_COARSEWOODY_MODELS, LineNo);
			break;
		case 2:
			sprintf(ErrMsg, "Must Use 'DEPTH' specifier for model, Line %ld",
				LineNo);
			break;
		case 3:
			sprintf(ErrMsg,
				"Depth out of Range: 0.05-4.0m (0.2-12ft), Line %ld", LineNo);
			break;
		case 4:
			sprintf(ErrMsg,
				"Heat content out of Range: 14k-24k kJ/kg (5-14k BTU/lb), Line %ld",
				LineNo);
			break;
		case 5:
			sprintf(ErrMsg,
				"Loading out of Range: 0.1-600 Mg/ha (0.1-300 t/ac), Line %ld",
				LineNo);
			break;
		case 6:
			sprintf(ErrMsg,
				"Diameter out of Range: 0.04-200cm (0.015-78in), Line %ld",
				LineNo);
			break;
		case 7:
			sprintf(ErrMsg, "Fuel Moisture out of Range: 2-300 percent, Line %ld",
				LineNo);
			break;
		case 8:
			sprintf(ErrMsg,
				"Sound/Rotten Density out of Range: 200-1000 kg/m3 percent (13-62 lb/ft3), Line %ld",
				LineNo);
			break;
		case 9:
			sprintf(ErrMsg, "Wrong Number of Data Fields, Line %ld", LineNo);
			break;
		}
		delete[] NumClasses;

		return false;
	}

	FreeAllCoarseWoody();
	for (i = 0; i < MAXNUM_COARSEWOODY_MODELS; i++)
	{
		if (NumClasses[i] == 0)
			continue;
		else if (!AllocCoarseWoody(i + 1, NumClasses[i]))
		{
			sprintf(ErrMsg,
				"Can't Alloc CWD Memory, (This Ain't Good, Reboot and Pray)");
			fclose(CurrentFile);
			delete[] NumClasses;

			return false;
		}
	}

	rewind(CurrentFile);
	memset(Line, 0x0, sizeof(Line));
	fgets(Line, 255, CurrentFile);
	for (i = 0; i < MAXNUM_COARSEWOODY_MODELS; i++)
	{
		if (NumClasses[i] == 0)	// just use as counter here
			continue;
		memset(Line, 0x0, sizeof(Line));
		fgets(Line, 255, CurrentFile);
//		ZeroMemory(desc, 256 * sizeof(char));
		memset(desc,0x0, 256 * sizeof(char));
		sscanf(Line, "%s %s %s", text, testnum, desc);
		num = atol(testnum);
		memset(Line, 0x0, sizeof(Line));
		fgets(Line, 255, CurrentFile);
		sscanf(Line, "%s %s", text, testdepth);
		if (!strcmp(strupr(text), "DEPTH"))
			depth = atof(testdepth) * depth_mult;
		SetWoodyDataDepth(num, depth, desc);
		memset(Line, 0x0, sizeof(Line));
		for (j = 0; j < NumClasses[num - 1]; j++)
		{
			memset(Line, 0x0, sizeof(Line));
			fgets(Line, 255, CurrentFile);
			sscanf(Line, "%s %s %s %s %s", testdiam, testload, testht,
				density, testfm);
			diameter = atof(testdiam);
			if (wunits == 1)
				diameter /= 12.0; // inches to feet
			else
				diameter /= 100.0; // cm to m
			if (diameter > 1e-4)
				sav = 4.0 / diameter;  // 1/m or 1/ft
			else
				sav = 10000.0;

			wd.HeatContent = atof(testht) * ht_mult; // J/kg
			wd.Load = atof(testload) * load_mult;    // kg/m^2
			wd.SurfaceAreaToVolume = sav * sav_mult; // 1/m
			wd.FuelMoisture = atof(testfm);// / 100.0;  // fraction
			if (!strncmp(strupr(density), "R", 1))
				dense = 300.0 ; //density;
			else if (!strncmp(strupr(density), "S", 1))
				dense = 513.0 ;//	kg/m3
			else
				dense = atof(density) * dense_mult;// *dense_mult;
			wd.Density = dense;
			SetWoodyData(num, j, &wd, wunits);
			memset(Line, 0x0, sizeof(Line));
		}
	}
	CwdID = true;
	fclose(CurrentFile);
	delete[] NumClasses;

	return true;
}


bool FarInputs::SaveWtr(long Station)
{
	//if (Changes.WTR[Station] == false)
	//	return true;

	long i, Units;
	char Header[24] = "ENGLISH";
	long tr1, tr2;
	double TMult = 1.0, TAdd = 0.0, RMult = 1.0, EMult = 1.0;
	WeatherData wtr;

	Units = GetWeatherUnits(Station);
	if (Units)
	{
		sprintf(Header, "%s", "METRIC");
		TMult = 1.8;
		TAdd = 32;
		RMult = 3.93;
		EMult = 3.28;
	}

	CurrentFile = fopen(WeatherFile[Station], "w");
	if (CurrentFile == NULL)
	{
		//SetFileAttributes(WeatherFile[Station], FILE_ATTRIBUTE_NORMAL);
		CurrentFile = fopen(WeatherFile[Station], "w");
		if (CurrentFile == NULL)
			return false;
	}

	fprintf(CurrentFile, "%s\n", Header);
	for (i = 0; i < GetMaxWeatherObs(Station); i++)
	{
		if (GetWeatherMonth(Station, i) == 13)
			break;
		wtr.mo = GetWeatherMonth(Station, i);
		wtr.dy = GetWeatherDay(Station, i);
		wtr.rn = GetWeatherRain(Station, i) / RMult;
		wtr.t1 = GetWeatherTime1(Station, i);
		wtr.t2 = GetWeatherTime2(Station, i);
		wtr.T1 = (GetWeatherTemp1(Station, i) - TAdd) / TMult;
		wtr.T2 = (GetWeatherTemp2(Station, i) - TAdd) / TMult;
		wtr.H1 = GetWeatherHumid1(Station, i);
		wtr.H2 = GetWeatherHumid2(Station, i);
		wtr.el = GetWeatherElev(Station, i) / EMult;
		if (wtr.rn > 0)
		{
			GetWeatherRainTimes(Station, i, &tr1, &tr2);
			wtr.tr1 = tr1;
			wtr.tr2 = tr2;
		}
		else
			wtr.tr1 = wtr.tr2 = 0;
		fprintf(CurrentFile, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
			wtr.mo, wtr.dy, (long) wtr.rn, wtr.t1, wtr.t2, (long) wtr.T1,
			(long) wtr.T2, wtr.H1, wtr.H2, (long) wtr.el);
		if (wtr.rn == 0.0)
			fprintf(CurrentFile, "\n");
		else if (wtr.tr1 > 0 || wtr.tr2 > 0)
			fprintf(CurrentFile, " %ld %ld\n", wtr.tr1, wtr.tr2);
		else
			fprintf(CurrentFile, "\n");
	}
	fclose(CurrentFile);
	//Changes.WTR[Station] = false;

	return true;
}


bool FarInputs::SaveWnd(long Station)
{
	//if (Changes.WND[Station] == false)
	//	return true;

	char Header[24] = "ENGLISH";
	long i, Units;
	double WMult = 1.0;
	WindData wnd;

	CurrentFile = fopen(WindFile[Station], "w");
	if (CurrentFile == NULL)
	{
		//SetFileAttributes(WindFile[Station], FILE_ATTRIBUTE_NORMAL);
		CurrentFile = fopen(WindFile[Station], "w");
		if (CurrentFile == NULL)
			return false;
	}

	Units = GetWindUnits(Station);
	if (Units)
		sprintf(Header, "%s", "METRIC");

	fprintf(CurrentFile, "%s\n", Header);
	for (i = 0; i < GetMaxWindObs(Station); i++)
	{
		if (GetWindMonth(Station, i) == 13)
			break;
		wnd.mo = GetWindMonth(Station, i);
		wnd.dy = GetWindDay(Station, i);
		wnd.hr = GetWindHour(Station, i);
		wnd.ws = GetWindSpeed(Station, i) / WMult;
		wnd.wd = GetWindDir(Station, i);
		wnd.cl = GetWindCloud(Station, i);

		fprintf(CurrentFile, "%ld %ld %ld %ld %ld %ld\n", wnd.mo, wnd.dy,
			wnd.hr, (long) wnd.ws, wnd.wd, wnd.cl);
	}
	fclose(CurrentFile);
	//Changes.WND[Station] = false;

	return true;
}


bool FarInputs::SaveAdj()
{
	//if (Changes.ADJ == false)
	//	return true;

	long i;

	CurrentFile = fopen(AdjustmentFile, "w");
	if (CurrentFile == NULL)
	{
	//	SetFileAttributes(AdjustmentFile, FILE_ATTRIBUTE_NORMAL);
		CurrentFile = fopen(AdjustmentFile, "w");
		if (CurrentFile == NULL)
			return false;
	}
	for (i = 1; i < 257; i++)
		fprintf(CurrentFile, "%ld %lf\n", i, GetRosRed(i));
	fclose(CurrentFile);
	//Changes.ADJ = false;

	return true;
}


bool FarInputs::SaveFms()
{
	//if (Changes.FMS == false)
	//	return true;
	long i;
	long ad, bd, cd, dd, ed;

	CurrentFile = fopen(FuelMoistureFile, "w");
	if (CurrentFile == NULL)
	{
		//SetFileAttributes(FuelMoistureFile, FILE_ATTRIBUTE_NORMAL);
		CurrentFile = fopen(FuelMoistureFile, "w");
		if (CurrentFile == NULL)
			return false;
	}

	for (i = 1; i < 257; i++)
	{
		if (!GetInitialFuelMoistures(i, &ad, &bd, &cd, &dd, &ed))
			continue;
		fprintf(CurrentFile, "%ld %ld %ld %ld %ld %ld\n", i, ad, bd, cd, dd,
			ed);
	}
	fclose(CurrentFile);
	//Changes.FMS = false;

	return true;
}

bool FarInputs::SaveCnv()
{
	//if (Changes.CNV == false)
	//	return true;

	CurrentFile = fopen(ConvertFile, "w");
	if (CurrentFile == NULL)
	{
		//SetFileAttributes(ConvertFile, FILE_ATTRIBUTE_NORMAL);
		CurrentFile = fopen(ConvertFile, "w");
		if (CurrentFile == NULL)
			return false;
	}

	long i, test;

	for (i = 1; i < 257; i++)
	{
		if (i > 256 || i < 0)
			continue;
		test = GetFuelConversion(i);
		if (test == -1)
			test = 99;
		else if (test == -2)
			test = 98;
		fprintf(CurrentFile, "%ld %ld\n", i, test);
	}
	fclose(CurrentFile);
	//Changes.CNV = false;

	return true;
}



bool FarInputs::SaveFmd()
{
	//if (Changes.FMD == false)
	//	return true;

	CurrentFile = fopen(FuelModelFile, "w");
	if (CurrentFile == NULL)
	{
		//SetFileAttributes(FuelModelFile, FILE_ATTRIBUTE_NORMAL);
		CurrentFile = fopen(FuelModelFile, "w");
		if (CurrentFile == NULL)
			return false;
	}

	long i, Units;
	char UnitString[8] = "ENGLISH";
	double ConvertLoad = 1.0, ConvertSAV = 1.0, ConvertDepth = 1.0,
		ConvertHeat = 1.0;
	double temp;
	NewFuel newfuel;

	memset(&newfuel, 0x0, sizeof(NewFuel));
	Units = AccessFuelModelUnits(GETVAL);
	if (Units == 1)
	{
		sprintf(UnitString, "%s", "METRIC");
		ConvertLoad = 2.2417;
		ConvertSAV = 30.480060960;
		ConvertDepth = 30.480060960;
		ConvertHeat = 2.324375;
	}

	fprintf(CurrentFile, "%s\n", UnitString);
	for (i = 14; i < 257; i++)  // all English values
	{
		if (!GetNewFuel(i, &newfuel))
			continue;
		temp = (long) (newfuel.xmext * 100.0);
		fprintf(CurrentFile,
			"%ld %6.3lf %6.3lf %6.3lf %6.3lf %6.3lf %ld %ld %ld %6.3lf %ld %ld %ld %48s\n",
			i, newfuel.h1 * ConvertLoad, newfuel.h10 * ConvertLoad,
			newfuel.h100 * ConvertLoad, newfuel.lh * ConvertLoad,
			newfuel.lw * ConvertLoad, (long)
			(newfuel.sav1 / ConvertSAV),
			(long)
			(newfuel.savlh / ConvertSAV),
			(long)
			(newfuel.savlw / ConvertSAV), newfuel.depth * ConvertDepth,
			(long)
			(temp), (long)
			(newfuel.heatd * ConvertHeat),
			(long)
			(newfuel.heatl * ConvertHeat), newfuel.desc);
	}
	fclose(CurrentFile);
	//Changes.FMD = false;

	return true;
}


bool FarInputs::SaveBpd()
{
	//if (Changes.BPD == false)
	//	return true;

	CurrentFile = fopen(BurnPeriod, "w");
	if (CurrentFile == NULL)
	{
		//SetFileAttributes(BurnPeriod, FILE_ATTRIBUTE_NORMAL);
		CurrentFile = fopen(BurnPeriod, "w");
		if (CurrentFile == NULL)
			return false;
	}

	long i, num;
	AbsoluteBurnPeriod abp;

	num = GetNumBurnPeriods();
	for (i = 0; i < num; i++)
	{
		GetBurnPeriod(i, &abp.Month, &abp.Day, &abp.Start, &abp.End);
		fprintf(CurrentFile, "%ld %ld %ld %ld\n", abp.Month, abp.Day,
			abp.Start, abp.End);
	}
	fclose(CurrentFile);
	//Changes.BPD = false;

	return true;
}


bool FarInputs::SaveCwd()
{
	//if (Changes.CWD == false)
	//	return true;

	CurrentFile = fopen(CoarseWoody, "w");
	if (CurrentFile == NULL)
	{
		//SetFileAttributes(CoarseWoody, FILE_ATTRIBUTE_NORMAL);
		CurrentFile = fopen(CoarseWoody, "w");
		if (CurrentFile == NULL)
			return false;
	}

	long i, j, ModelUnits;
	long NumClasses;
	char Desc[256] = "", UnitString[54] = "";
	double depth, load;
	double sav_mult, load_mult, ht_mult, depth_mult, diam_mult, dense_mult;
	WoodyData wd[20];

	for (i = 0; i < MAXNUM_COARSEWOODY_MODELS; i++)
	{
		ModelUnits = GetWoodyDataUnits(i + 1, Desc);
		if (ModelUnits > -1)
			break;
	}
	if (ModelUnits == 1)
	{
		sprintf(UnitString, "%s", "ENGLISH");
		diam_mult = 12.0;
		sav_mult = 3.280839895;	// 1/ft to 1/m
		ht_mult = 2.32599;  	   // btu/lb to J/kg
		load_mult = 0.224169061; 	// tons/ac to kg/m2
		depth_mult = 0.3048;		// feet to meters
		dense_mult = 16.0184633737; // lb/ft3 to kg/m3
	}
	else
	{
		sprintf(UnitString, "%s", "METRIC");
		diam_mult = 100.0;
		sav_mult = 1.0;			// 1/m to 1/m
		ht_mult = 1.0;  	   	// J/kg to J/kg
		load_mult = 0.1;		 	// Mg/ha to kg/m2
		depth_mult = 1.0;		// meters to meters
		dense_mult = 1.0;
	}

	fprintf(CurrentFile, "%s\n", UnitString);
	for (i = 0; i < MAXNUM_COARSEWOODY_MODELS; i++)
	{
		GetWoodyData(i + 1, 0, &NumClasses, wd, &depth, &load);	// pass 0 to ignore surface fuels
		if (NumClasses == 0 || load <= 0.0)
			continue;
		ModelUnits = GetWoodyDataUnits(i + 1, Desc);
		load /= load_mult;
		depth /= depth_mult;
		for (j = 0; j < NumClasses; j++)
		{
			wd[j].SurfaceAreaToVolume /= sav_mult;
			wd[j].Load /= load_mult;
			wd[j].HeatContent /= ht_mult;
			wd[j].Density /= dense_mult;
		}
		fprintf(CurrentFile, "MODEL %ld %s\n", i + 1, Desc);
		fprintf(CurrentFile, "DEPTH %4.2lf\n", depth);
		for (j = 0; j < NumClasses; j++)
		{
			if (wd[j].Density == 300.0)
				fprintf(CurrentFile, "%6.3lf %5.2lf %5.0lf %s %ld\n",
					(4.0 * diam_mult) / wd[j].SurfaceAreaToVolume, wd[j].Load,
					wd[j].HeatContent, "R",
					(long) (wd[j].FuelMoisture * 100.0));
			else if (wd[j].Density == 513.0)
				fprintf(CurrentFile, "%6.3lf %5.2lf %5.0lf %s %ld\n",
					(4.0 * diam_mult) / wd[j].SurfaceAreaToVolume, wd[j].Load,
					wd[j].HeatContent, "S",
					(long) (wd[j].FuelMoisture * 100.0));
			else
				fprintf(CurrentFile, "%6.3lf %5.2lf %5.0lf %3.0lf %ld\n",
					(4.0 * diam_mult) / wd[j].SurfaceAreaToVolume, wd[j].Load,
					wd[j].HeatContent, wd[j].Density,
					(long) (wd[j].FuelMoisture * 100.0));
		}
	}
	//Changes.CWD = false;
	fclose(CurrentFile);

	return true;
}


//------------------------------------------------------------------------------
//
// 	FarInputs Dialog
//
//------------------------------------------------------------------------------
/*
TTransInputData::TTransInputData()
{
	ResetData();
}

void TTransInputData::ResetData()
{
	Lcp = false;
	Adj = false;
	Fms = false;
	Cnv = false;
	Fmd = false;
	Cwd = false;
	Bpd = false;
	Wtr1 = false;
	Wtr2 = false;
	Wtr3 = false;
	Wtr4 = false;
	Wtr5 = false;
	Wnd1 = false;
	Wnd2 = false;
	Wnd3 = false;
	Wnd4 = false;
	Wnd5 = false;
}
*/

