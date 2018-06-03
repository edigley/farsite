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
//==============================================================================
//
//	FARSITE.CPP	Header and Function file for FARSITE
//
//	FARSITE: Fire Area Simulator
//
//	Copyright 1994, 1995, 1996 Mark A. Finney
//	FARSITE is a Registered Trademark of Mark A. Finney
//
//	Mark A. Finney
//	Systems for Environmental Management
// 	PO BOX 8868, Missoula MT 59807
//
//
// 	Version 		4.1.0  Final
//	Date			12/21/2004
//
//==============================================================================

#include "farsite4.h"
#include <sys/stat.h>
#include <unistd.h>
#include <strings.h>
#include "portablestrings.h"
#include <iostream>
#include <omp.h>

#define SIMREQ_NULL				0
#define SIMREQ_INITIATETERMINATE 	1
#define SIMREQ_STARTRESTART 		2
#define SIMREQ_RESUMESUSPEND 		3
#define SIMREQ_RESET			4
#define IDYES 1

#define PI 3.1415926536

//-------------------------------------------------------------------------------
// version number as appears in "About" window
//-------------------------------------------------------------------------------
char VERSION[] = "Version 4.1.0batch";
//-------------------------------------------------------------------------------

char ElTime[12], CurTime[14];
bool F_ON = 0;
PFrontData pfdata;						// glogal current post frontal stuff
FILE* CurrentFile;						   // global file pointer, for landscape file

//------------------------------------------------------------------------------
//
//	Application Class
//
//------------------------------------------------------------------------------


TFarsiteInterface::~TFarsiteInterface()
{
}

TFarsiteInterface::TFarsiteInterface(const char* name, const char* farsitedirectory,
                                     const char* bookmark) //: TApplication(name)
{
    long i;

//	hSimThread = 0;
//	hWaitThread = 0;
//	hWaitEvent = hFarSimEvent = 0;
    NumSimThreads = 0;
    TerminateWait = WaitInProgress = false;
    SimRequest = SIMREQ_NULL;
    SIM_SUSPENDED = SIM_COMPLETED = false;
    SIMULATE_GO = false;
    FARSITE_GO = false;
    LEAVEPROCESS = false;
    NextFireAfterInterrupt = 0;
    OldFireAfterInterrupt = 0;
    VisPerimSize = 0;   			// size of disk copy of vector perimeters
    InitializeStrings();		  // for SuperVGA
    NumWData = 0; 				// initialize wind grid storage for xorpen
    WData = 0;
    gaat = 0;					// group air attack pointer
    //burn.env->ResetData();
    //WStat=0;
    //NumWindStats=0;
    InitializeRosRed();			// set rate of spread adjustments to 1.0;
    ReviseGroundAtkON = ReviseAirAtkON = false;
    //CM_SWITCHMESSAGE=::RegisterWindowMessage("SWITCH_INPUTMODE");
    TextFilterIndex = RasterFilterIndex = VectorFilterIndex = 0;

    memset(FarsiteDirectory, 0x0, sizeof(FarsiteDirectory));
    strcpy(FarsiteDirectory, farsitedirectory);
    memset(FMMFileName, 0x0, sizeof(FMMFileName));
    memset(FMMDescription, 0x0, sizeof(FMMDescription));
    memset(CrewFileName, 0x0, sizeof(CrewFileName));
    memset(BookMarkArg, 0x0, sizeof(BookMarkArg));
    //getcwd(FarsiteDirectory, 255);
    /*
    	for (i = 0; i < 10000; i++)
    	{
    		sprintf(IgFileName, "%signition%ld.ign", FarsiteDirectory, i);
    		if (access(IgFileName, W_OK) == -1)
    			break;
    	}
    	FILE *iout=fopen(IgFileName, "w");
    	fclose(iout);
    */
    //CloseHandle(CreateFile(IgFileName, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_NEW, NULL, NULL));
    CanModifyInputs(true);
    ResetNewFuels();
    SetModelParams() ;
}


void TFarsiteInterface::InitMainWindow()
{
    //CreateVisperimSemaphore();
    FlatOpenProject();
    return;
}


void TFarsiteInterface::InitializeStrings()
{
    for (long i = 0; i < 5; i++)	// initialize strings for fileanmes
    {
        memset(Inputs.WeatherFile[i], 0x0, sizeof Inputs.WeatherFile[i]);
        memset(Inputs.WindFile[i], 0x0, sizeof Inputs.WindFile[i]);
        strcpy(Inputs.MBWtr[i], "Load Weather");
        strcpy(Inputs.MBWnd[i], "Load Winds");
        //memset(MBWtr[i], 0x0, sizeof MBWtr[i]);
        //memmset(MBWnd[i], 0x0, sizeof MBWnd[i]);   // set message bars
    }
    strcpy(Inputs.MBLand, "Load Landscape");
    memset(MBStatus, 0x0, sizeof MBStatus);
    memset(MBMode, 0x0, sizeof MBStatus);
    sprintf(ElTime, "%02d %02d%s%02d", 0, 0, ":", 0);
}


void TFarsiteInterface::EvClose()
{
    //	//CmCancel();
}



bool TFarsiteInterface::CanClose()
{
    int j, i;

    //if(Inputs.LandID)    // changed now because of memory storage of lcp
    CloseLandFile();
    //  		EndSimThread();
    j = GetNumCrews();  		 // must use constant j because NumCrews changes in FreeCrew();
    for (i = j - 1; i > -1; i--)
        FreeCrew(0);
    j = GetNumAirCraft();   		  // must use constant j because NumAirCraft changes in FreeCrew();
    for (i = 0; i < j; i++)
        FreeAirCraft(i);
    //DDE.CloseServer();
    //CloseVisperimSemaphore();
    remove(IgFileName);

    return true;
}


//-----------------------------------------------------------------------------------------
//
// 		TFarsiteInterface function definitions
//
//-----------------------------------------------------------------------------------------



bool TFarsiteInterface::OpenLandscapeFile()
{
    if (Inputs.LandID == true)
    {
        strcpy(Inputs.MBLand, "Load Landscape");
        CloseLandFile();
        memset(Inputs.LandscapeFile, 0x0, sizeof(Inputs.LandscapeFile));
        //		TransInputData.Lcp = BF_UNCHECKED;
        //TransInputData.Lcp = false;
        Inputs.LandID = false;
    }


    //SetLandFileName(FilenameData.FileName);
    if (OpenLandFile())
    {
        if (TestForLCPVersion1())
        {
            int Response = IDYES;

            if (Response == IDYES)
            {
                if (!ConvertLCPFileToVersion2())
                {
                    CloseLandFile();
                    return false;
                }
                else
                {
                    Inputs.InitializeLandscapeFile();
                    //strcpy(Inputs.LandscapeFile, FilenameData.FileName);
                    //					TransInputData.Lcp = BF_CHECKED;
                    //TransInputData.Lcp = true;
                }
            }
            else
            {
                Inputs.LandID = false;
                CloseLandFile();
                FreeStationGrid();								// reset data structures for model
                strcpy(Inputs.MBLand, "Load Landscape");
                return false;
            }
        }
        else if (TestForLCPVersion2())
        {
            int Response = IDYES;
            if (Response == IDYES)
            {
                if (!ConvertLCPFile2ToVersion4())
                {
                    CloseLandFile();
                    return false;
                }
                else
                {
                    Inputs.InitializeLandscapeFile();
                    //strcpy(Inputs.LandscapeFile, FilenameData.FileName);
                    //					TransInputData.Lcp = BF_CHECKED;
                    //TransInputData.Lcp = true;
                }
            }
            else
            {
                Inputs.LandID = false;
                CloseLandFile();
                FreeStationGrid();								// reset data structures for model
                strcpy(Inputs.MBLand, "Load Landscape");
                return false;
            }
        }
        else
        {
            Inputs.InitializeLandscapeFile();
            //strcpy(Inputs.LandscapeFile, FilenameData.FileName);
            //			TransInputData.Lcp = BF_CHECKED;
            //TransInputData.Lcp = true;
        }
    }
    else
    {
        return false;
    }



    return true;
}

void TFarsiteInterface::SetInputsFromFile(char* settingsfilename)
{
    FILE* insettings;
    char Line[256];
    char* name;
    char* value;
    insettings = fopen(settingsfilename, "r");
    if(insettings)
    {
        Inputs.ResetData();
    }
    else
    {
        printf("cannot open %s",settingsfilename);
        return;
    }
    while (!feof(insettings))
    {
        memset(Line, 0x0, 256 * sizeof(char));
        fgets(Line, 255, insettings);
        if (strlen(Line) == 0 || !strncmp(Line, "\n", 1))
        {
            continue;
        }
        if (Line[0] == '#')
        {
            continue;
        }
        name = Line;
        value = strchr(Line, '=');
        if (value != NULL)
        {
            value[0]='\0';
            value++;
        }
        else
        {
            continue;
        }
        trim(name);
        trim(value);
        if (strlen(value) == 0)
        {
            continue;
        }
        processNameValue(name,value);

    }
    ResetDuration();
    FlatSimulateInitiateTerminate();
    //SetModelParams();
    //on these in the start data case and end date case
    SetEndDate(GetJulianDays(GetEndMonth()) + GetEndDay());
    SetStartDate(GetJulianDays(GetStartMonth()) + GetStartDay()+GetStartHour());
}
void TFarsiteInterface::processNameValue(char* name, char* value)
{
    char buff[64];
    char numset[] = ".1234567890";
    int i;
    if (strlen(name) == 0)
    {
        return;
    }
    if (strlen(value) == 0)
    {
        return;
    }
    if (strcmp(strlwr(name), "version") == 0)
    {
        Inputs.version=atoi(value);
    }
    if (strcmp(strlwr(name), "projectfilename") == 0)
    {
        strcpy(Inputs.ProjectFile, value);
        Inputs.TestProjectFileVersion();
    }
    if (strcmp(strlwr(name), "ignitionfile") == 0)
    {
        strcpy(Ignition.ifile, value);
    }
    if (strcmp(strlwr(name), "ignitionfiletype") == 0)
    {
        if (strcmp(strlwr(value), "polygon") == 0)
        {
            Ignition.SelectFileInputCmdL(1);
        }
        else if(strcmp(strlwr(value), "line") == 0)
        {
            Ignition.SelectFileInputCmdL(2);
        }
        else if(strcmp(strlwr(value), "point") == 0)
        {
            Ignition.SelectFileInputCmdL(3);
        }
        else
        {
            printf("Unknown ignition file type, must be either polygon, line, or point!");
        }
    }
    if (strcmp(strlwr(name), "landscapefile") == 0)
    {
        Inputs.setLandscapeFile(value);
    }
    if (strcmp(strlwr(name), "adjustmentfile") == 0)
    {
        Inputs.loadUpAdjustmentsFile(value);
    }
    if (strcmp(strlwr(name), "fuelmoisturefile") == 0)
    {
        Inputs.loadUpMoistureFile(value);
    }
    if (strcmp(strlwr(name), "coarsewoodyfile") == 0)
    {
        strcpy(Inputs.CoarseWoody, value);
        Inputs.loadUpCoarseWoodyFile(value);
    }
    if (strcmp(strlwr(name), "weatherfile0") == 0)
    {
        Inputs.setWeatherFile(0,value);
    }
    if (strcmp(strlwr(name), "weatherfile1") == 0)
    {
        Inputs.setWeatherFile(1,value);
    }
    if (strcmp(strlwr(name), "weatherfile2") == 0)
    {
        Inputs.setWeatherFile(2,value);
    }
    if (strcmp(strlwr(name), "weatherfile3") == 0)
    {
        Inputs.setWeatherFile(3,value);
    }
    if (strcmp(strlwr(name), "weatherfile4") == 0)
    {
        Inputs.setWeatherFile(4,value);
    }
    if (strcmp(strlwr(name), "windfile0") == 0)
    {
        Inputs.setWindFile(0,value);
    }
    if (strcmp(strlwr(name), "windfile1") == 0)
    {
        Inputs.setWindFile(1,value);
    }
    if (strcmp(strlwr(name), "windfile2") == 0)
    {
        Inputs.setWindFile(2,value);
    }
    if (strcmp(strlwr(name), "windfile3") == 0)
    {
        Inputs.setWindFile(3,value);
    }
    if (strcmp(strlwr(name), "windfile4") == 0)
    {
        Inputs.setWindFile(4,value);
    }
    if (strcmp(strlwr(name), "fuelmodelfile") == 0)
    {
        Inputs.loadUpFuelModelFile(value);
    }
    if (strcmp(strlwr(name), "convertfile") == 0)
    {
        Inputs.loadUpConversionFile(value);
    }
    if (strcmp(strlwr(name), "burnperiodfile") == 0)
    {
        Inputs.loadUpBurnPeriodFile(value);
    }
    if (strcmp(strlwr(name), "grideastnorth") == 0)
    {
        Inputs.saveGridEastNorth(value);
    }
    if (strcmp(strlwr(name), "gridrow") == 0)
    {
        Inputs.saveGridRow(value);
    }
    if (strcmp(strlwr(name), "landscapethemefile") == 0)
    {
        Inputs.loadUpLandscapeTheme(value);
    }
    if (strcmp(strlwr(name), "shapedeffile") == 0)
    {
        Inputs.loadUpShapeFile(value);
    }
    if (strcmp(strlwr(name), "storedvector") == 0)
    {
        Inputs.loadUpStoredVector(value);
    }
    if (strcmp(strlwr(name), "north") == 0)
    {
        Inputs.loadUpSetDirection(value,'N');
    }
    if (strcmp(strlwr(name), "south") == 0)
    {
        Inputs.loadUpSetDirection(value,'S');
    }
    if (strcmp(strlwr(name), "east") == 0)
    {
        Inputs.loadUpSetDirection(value,'E');
    }
    if (strcmp(strlwr(name), "west") == 0)
    {
        Inputs.loadUpSetDirection(value,'W');
    }
    if (strcmp(strlwr(name), "canopycheckvalues") == 0)
    {
        Inputs.loadUpCanopyCheck(value);
    }
    if (strcmp(strlwr(name), "useconditioningperiod") == 0)
    {
        if(strcmp(strlwr(value), "true") == 0)
        {
            UseConditioningPeriod(1);
        }
        else
        {
            UseConditioningPeriod(0);
        }
    }

    if (strcmp(strlwr(name), "conditmonth") == 0)
    {
        SetConditMonth(atoi(value));
    }
    if (strcmp(strlwr(name), "conditday") == 0)
    {
        SetConditDay(atoi(value));
    }
    if (strcmp(strlwr(name), "startmonth") == 0)
    {
        SetStartMonth(atoi(value));
    }
    if (strcmp(strlwr(name), "startday") == 0)
    {
        SetStartDay(atoi(value));
    }
    if (strcmp(strlwr(name), "starthour") == 0)
    {
        SetStartHour(atoi(value));
    }
    if (strcmp(strlwr(name), "startmin") == 0)
    {
        SetStartMin(atoi(value));
    }
    if (strcmp(strlwr(name), "endmonth") == 0)
    {
        SetEndMonth(atoi(value));
    }
    if (strcmp(strlwr(name), "endday") == 0)
    {
        SetEndDay(atoi(value));
    }
    if (strcmp(strlwr(name), "endhour") == 0)
    {
        SetEndHour(atoi(value));
    }
    if (strcmp(strlwr(name), "endmin") == 0)
    {
        SetEndMin(atoi(value));
    }
    if (strcmp(strlwr(name), "rastmake") == 0)
    {
        bool bValue = false;
        if (strcmp(strlwr(value), "true") == 0)
        {
            bValue = true;
        }
        SetRastMake(bValue);
    }
    if (strcmp(strlwr(name), "vectmake") == 0)
    {
        bool bValue = false;
        if (strcmp(strlwr(value), "true") == 0)
        {
            bValue = true;
        }
        SetVectMake(bValue);
    }
    if (strcmp(strlwr(name), "shapemake") == 0)
    {
        bool bValue = false;
        if (strcmp(strlwr(value), "true") == 0)
        {
            bValue = true;
        }
        SetShapeMake(bValue);
    }
    if (strcmp(strlwr(name), "shapefile") == 0)
    {
        SetShapeFileChx(value, 1, 0, 1);
    }
    if (strcmp(strlwr(name), "rasterfilename") == 0)
    {
        SetRasterFileName(value);
    }
    if (strcmp(strlwr(name), "vectorfilename") == 0)
    {
        SetVectorFileName(value);
    }
    if (strcmp(strupr(name), "RAST_ARRIVALTIME") == 0)
    {
        bool bValue = false;
        if (strcmp(strlwr(value), "true") == 0)
        {
            bValue = true;
        }
        SetFileOutputOptions(RAST_ARRIVALTIME, bValue);
    }
    if (strcmp(strupr(name), "RAST_FIREINTENSITY") == 0)
    {
        bool bValue = false;
        if (strcmp(strlwr(value), "true") == 0)
        {
            bValue = true;
        }
        SetFileOutputOptions(RAST_FIREINTENSITY, bValue);
    }
    if (strcmp(strupr(name), "RAST_SPREADRATE") == 0)
    {
        bool bValue = false;
        if (strcmp(strlwr(value), "true") == 0)
        {
            bValue = true;
        }
        SetFileOutputOptions(RAST_SPREADRATE, bValue);
    }
    if (strcmp(strupr(name), "RAST_FLAMELENGTH") == 0)
    {
        bool bValue = false;
        if (strcmp(strlwr(value), "true") == 0)
        {
            bValue = true;
        }
        SetFileOutputOptions(RAST_FLAMELENGTH, bValue);
    }
    if (strcmp(strupr(name), "RAST_HEATPERAREA") == 0)
    {
        bool bValue = false;
        if (strcmp(strlwr(value), "true") == 0)
        {
            bValue = true;
        }
        SetFileOutputOptions(RAST_HEATPERAREA, bValue);
    }
    if (strcmp(strupr(name), "RAST_CROWNFIRE") == 0)
    {
        bool bValue = false;
        if (strcmp(strlwr(value), "true") == 0)
        {
            bValue = true;
        }
        SetFileOutputOptions(RAST_CROWNFIRE, bValue);
    }
    if (strcmp(strupr(name), "RAST_FIREDIRECTION") == 0)
    {
        bool bValue = false;
        if (strcmp(strlwr(value), "true") == 0)
        {
            bValue = true;
        }
        SetFileOutputOptions(RAST_FIREDIRECTION, bValue);
    }
    if (strcmp(strupr(name), "RAST_REACTIONINTENSITY") == 0)
    {
        bool bValue = false;
        if (strcmp(strlwr(value), "true") == 0)
        {
            bValue = true;
        }
        SetFileOutputOptions(RAST_REACTIONINTENSITY, bValue);
    }
    // timestep is only valid from 1min to 59min or 1.00 hrs to 6.00 hrs
    if (strcmp(strlwr(name), "timestep") == 0)
    {
        memset(buff,0x0,64);
        i = strspn(value,numset);
        strncpy(buff,value,i);
        if(strchr(value,'m')==NULL)
        {
            SetActualTimeStep(atof(buff)*60);
        }
        else
        {
            SetActualTimeStep((atof(buff)));
        }
    }
    //visible step only valid from 1hrs to 4.17 days
    if (strcmp(strlwr(name), "visiblestep") == 0)
    {
        memset(buff,0x0,64);
        i = strspn(value,numset);
        strncpy(buff,value,i);
        if(strchr(value,'m')==NULL)
        {
            if(strchr(value,'h')==NULL)
            {
                //value is specified in days
                SetVisibleTimeStep(atof(buff)*60*24);
            }
            else
            {
                //value is specified in hrs
                SetVisibleTimeStep(atof(buff)*60);
            }  //end else
        }
        else
        {
            //value is specified in minutes
            SetVisibleTimeStep(atof(buff));
        } //end else
    } //end parsing visiblestep
    //secondary visible step only valid from 1hrs to 4.17 days
    if (strcmp(strlwr(name), "secondaryvisiblestep") == 0)
    {
        memset(buff,0x0,64);
        i = strspn(value,numset);
        strncpy(buff,value,i);
        if(strchr(value,'m')==NULL)
        {
            if(strchr(value,'h')==NULL)
            {
                //value is specified in days
                SetSecondaryVisibleTimeStep(atof(buff)*60*24);
            }
            else
            {
                //value is specified in hrs
                SetSecondaryVisibleTimeStep(atof(buff)*60);
            }  //end else
        }
        else
        {
            //value is specified in minutes
            SetSecondaryVisibleTimeStep(atof(buff));
        } //end else
    } //end parsing secondaryvisiblestep

    //perimeter resolution only valid from 1 to 500m default is 60m
    if (strcmp(strlwr(name), "perimeterresolution") == 0)
    {
        SetPerimRes(atof(value));
    }
    //distance resolution only valid from 1 to 500m default is 60m
    if (strcmp(strlwr(name), "distanceresolution") == 0)
    {
        SetDistRes(atof(value));
    }
    //model units either english or metric
    if (strcmp(strlwr(name), "units") == 0)
    {
    }
    //only valid value for the following is either true or false
    if (strcmp(strlwr(name), "enablecrownfire") == 0)
    {
        if(strcmp(strlwr(value), "true") == 0)
        {
            EnableCrowning(1);
        }
        else
        {
            EnableCrowning(0);
        }
    }
    if (strcmp(strlwr(name), "linkcrowndensityandcover") == 0)
    {
        if(strcmp(strlwr(value), "true") == 0)
        {
            LinkDensityWithCrownCover(1);
        }
        else
        {
            LinkDensityWithCrownCover(0);
        }
    }
    //only valid value for the following is either true or false
    if (strcmp(strlwr(name), "embersfromtorchingtrees") == 0)
    {
        if(strcmp(strlwr(value), "true") == 0)
        {
            EnableSpotting(1);
        }
        else
        {
            EnableSpotting(0);
        }
    }
    // can't find this in original Farsite4 code...
    if (strcmp(strlwr(name), "embersfromsurfacefires") == 0)
    {
    }
    if (strcmp(strlwr(name), "enablespotfiregrowth") == 0)
    {
        if(strcmp(strlwr(value), "true") == 0)
        {
            EnableSpotFireGrowth(1);
        }
        else
        {
            EnableSpotFireGrowth(0);
        }
    }
    //ignition frequency and ignition delay are only used
    //if enable spot fire growth is true
    //ignition frequency is from 0 to 100 percent
    if (strcmp(strlwr(name), "ignitionfrequency") == 0)
    {
        PercentIgnition(atof(value));
    }

    if (strcmp(strlwr(name), "ignitionROS") == 0)
    {
        AdjustIgnitionSpreadRates((long)atof(value));
    }

    //ignition delay 0 to 320 minutes
    if (strcmp(strlwr(name), "ignitiondelay") == 0)
    {
        IgnitionDelay(atof(value));
    }
    if (strcmp(strlwr(name),"nwnsbackingros") == 0)
    {
        bool bValue = false;
        if (strcmp(strlwr(value), "true") == 0)
        {
            bValue = true;
        }
        ConstantBackingSpreadRate(bValue);
    }
    if (strcmp(strlwr(name), "expansioncorrection") == 0)
    {
        if (strcmp(strlwr(value), "true") == 0)
        {
            CheckExpansions(1);
        }
        else
        {
            CheckExpansions(0);
        }
    }
    //distance checking is either fire level =1 or sim level =0
    if (strcmp(strlwr(name), "distancechecking") == 0)
    {
        if (strcmp(strlwr(value), "simlevel") == 0)
        {
            DistanceCheckMethod(0);
        }
        else if (strcmp(strlwr(value), "firelevel") == 0)
        {
            DistanceCheckMethod(1);
        }
        else
        {
            printf("Invalid Distance Checking! Method must be either Simlevel or Firelevel.\n");
        }
    }
    //fire accleration is either on or off
    if (strcmp(strlwr(name), "fireacceleration") == 0)
    {
        bool bValue = false;
        if (strcmp(strlwr(value), "on") == 0)
        {
            bValue = true;
        }
        SetAccelerationON(bValue);
    }
    if (strcmp(strlwr(name), "fireacclerationloadfile") == 0)
    {
    }
    if (strcmp(strlwr(name), "fireacclerationsavefile") == 0)
    {
    }
    //fire fuel is either model or types
    if (strcmp(strlwr(name), "firefuel") == 0)
    {
    }
    //fire model value may be between 1-50
    if (strcmp(strlwr(name), "firemodelvalue") == 0)
    {
    }
    //fire fuel value may be one of grass
    //shrub, timber, slash, default
    if (strcmp(strlwr(name), "firefuelvalue") == 0)
    {
    }
    if (strcmp(strlwr(name), "accelerationpointvalueA") == 0)
    {
    }
    if (strcmp(strlwr(name), "accelerationpointvalueB") == 0)
    {
    }
    if (strcmp(strlwr(name), "accelerationlinevalueA") == 0)
    {
    }
    if (strcmp(strlwr(name), "accelerationlinevalueB") == 0)
    {
    }
    //acceleration transition value is between 10-500 meters
    if (strcmp(strlwr(name), "accelerationtranstion") == 0)
    {
    }
    if (strcmp(strlwr(name), "simulatepostfrontalcombustion") == 0)
    {
        if (strcmp(strlwr(value), "true") == 0)
        {
            CheckPostFrontal(1);

        }
        else
        {
            CheckPostFrontal(0);
        }
    }
    //fuel input options are
    //1 Never combine surface and cwd fuels
    //2 Always combine surface and cwd fuels
    //3 Use surface fues when CWD absent (default)
    if (strcmp(strlwr(name), "fuelinputoption") == 0)
    {
        if (strcmp(strlwr(value), "never") == 0)
        {
            WoodyCombineOptions(1);
        }
        else if (strcmp(strlwr(value), "always") == 0)
        {
            WoodyCombineOptions(2);
        }
        else if (strcmp(strlwr(value), "absent") == 0)
        {
            WoodyCombineOptions(3);
        }
        else
        {
            printf("Invalid Fuel Option specified! Valid options are never, always, or absent.\n");
        }

    }
    //calculation precision options are normal or high
    if (strcmp(strlwr(name), "calculationprecision") == 0)
    {
        if (strcmp(strlwr(value), "high") == 0)
        {
            burn.post.BurnupPrecision(1);
        }
        else
        {
            burn.post.BurnupPrecision(0);
        }
    }
}

void TFarsiteInterface::FlatOpenProject()
{
    SetSimulationDuration(ConvertActualTimeToSimtime(GetEndMonth(),GetEndDay(), GetEndHour(), GetEndMin(), false));
    Execute_StartRestart();
}

void TFarsiteInterface::SetModelParams()
{
    double ActualTimeStep = 30.0;	// minutes
    double VisibleTimeStep=30.0;	// minutes
    //bool secondaryVisibleStep = false;
    bool isEnglishints = false;
    double perimeterResolution = 90.0;     // keep this bigger than distance resolution!
    double distanceResolution = 60.0;

    SetActualTimeStep(ActualTimeStep);
    SetVisibleTimeStep(VisibleTimeStep);
    SetSecondaryVisibleTimeStep(-1.0);

    double MetricConvert = 1.0;
    if (isEnglishints)
        MetricConvert = 0.914634;
    SetPerimRes(double (perimeterResolution) * MetricConvert);  			// actual resolution always in meters ??
    SetDistRes(double (distanceResolution) * MetricConvert);
}






void TFarsiteInterface::SaveVisPerimeter()
{
    char NewFile[200];
    FILE* newfile;
    long easti, northi, type;

    //strcpy(NewFile, FilenameData.FileName);
    newfile = fopen(NewFile, "w");
    if (newfile == NULL)
    {
        //SetFileAttributes(NewFile, FILE_ATTRIBUTE_NORMAL);
        chmod(NewFile,S_IRWXO);
        newfile = fopen(NewFile, "w");
    }
    OpenVisPerimFile(1);
    while ((type = GetVisPerimFile(&easti, &northi)) >= 0)
    {
        if (type == 0)
        {
            if (GetVisPerimFile(&easti, &northi) >= 0)
                fprintf(newfile, "%d %d\n", -1, -1);
        }
        else
            fprintf(newfile, "%ld %ld\n", easti, northi);
    }
    fprintf(newfile, "%d %d\n", -1, -1);
    fclose(newfile);
    CloseVisPerimFile(0, 1);
    VisPerimSize = 0;   	  // reset vis perim size flag so no save message at end
}


void TFarsiteInterface::InitializeLandscapeWindow()
{
    CreateLandscapeSemaphore();
    memset(FarsiteTitle, 0x0, sizeof FarsiteTitle);					// initialize strings as blanks
    strcat(FarsiteTitle, GetLandFileName());
}


void TFarsiteInterface::StartSimThread()
{
    //unsigned int SimThreadID;
    //char Name[128] = "";
    //char TimeID[128] = "";
    //SYSTEMTIME st;
    //GetSystemTime(&st);
    //sprintf(TimeID, "%ld%ld%ld%ld%ld%ld%ld", st.wYear, st.wMonth, st.wDay,
    //	st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    SimRequest = SIMREQ_NULL;

    RunSimThread(this);
}


void TFarsiteInterface::EndSimThread()
{
    /*
    	if (hFarSimEvent)
    		CloseHandle(hFarSimEvent);
    	hFarSimEvent = 0;
    	if (hWaitEvent)
    		CloseHandle(hWaitEvent);
    	hWaitEvent = 0;
    */
}


unsigned TFarsiteInterface::RunSimThread(void* FarIntFace)
{
    static_cast <TFarsiteInterface*>(FarIntFace)->FarsiteSimulationLoop();

    return 1;
}



unsigned TFarsiteInterface::RunWaitThread(void* FarIntFace)
{
    static_cast <TFarsiteInterface*>(FarIntFace)->WaitRequest();

    return 1;
}


void TFarsiteInterface::WaitRequest()
{
    /*
    	DWORD ret;
    	 long	InMode;
    	 if(hWaitEvent!=NULL)
    	 {    WaitInProgress=true;
    		do
    		 {	ret=WaitForSingleObject(hWaitEvent, 2000);
    	 		if(TerminateWait==true || CancelWait(GETVAL)==true)
    		  	{    SimRequest=SIMREQ_NULL;
    			   	break;
    		  	}
    		} while(ret!=WAIT_OBJECT_0);
    	 	//ResetEvent(hWaitEvent);
    	 }
    	 else
    	 	ret=WAIT_OBJECT_0;
    	 WaitInProgress=false;
    	 CancelWait(false);
    	 if(ret==WAIT_OBJECT_0)
    	 {	::SendMessage(Client->HWindow, WM_COMMAND, CM_KILLWAIT, NULL);
    	 	InMode=GetInputMode();
    		  InMode-=100;	// strip "pending" code
    		NULLINPUTS();
    		  SetInputMode(InMode, false);
    		  sprintf(MBStatus, "    %s", "SIMULATION SUSPENDED");
    	 	switch(InMode)
    		  {	case NULLLOCATION:
    					if(SimRequest==SIMREQ_INITIATETERMINATE || SimRequest==SIMREQ_STARTRESTART || SimRequest==SIMREQ_RESET)
    					{	FARSITE_GO=false;
    						SetEvent(hFarSimEvent);//ResumeThread(hSimThread);
    					}
    		  		break;
    		  	case IGNITIONLOCATION:
    					GetFarsiteWindow()->SetCursor(GetFarsiteWindow()->GetModule(), "CURTORCH");
    				sprintf(MBMode, "    %s", "IGNITION MODE");
    					ign->SetButtonState(TButtonGadget::Down);
    					break;
    			   case BARRIERLOCATION:
    				GetFarsiteWindow()->SetCursor(GetFarsiteWindow()->GetModule(), "CURDOZER");
    				sprintf(MBMode, "    %s", "ADDBARRIERS MODE");
    					DrawTemporaryFires();
    					bar->SetButtonState(TButtonGadget::Down);
    					break;
    			  case EDITLOCATION:
    				//GetFarsiteWindow()->SetCursor(GetFarsiteWindow()->GetModule(), "CURSAW");
    				//sprintf(MBStatus, "    %s", "SIMULATION SUSPENDED");
    				//sprintf(MBMode, "    %s", "EDIT LANDSCAPE MODE");
    				eds->SetButtonState(TButtonGadget::Down);
    				SendMessage(MainWindow->HWindow, CM_SWITCHMESSAGE, TParam1(SWITCH_INPUTMODE), TParam2(EDITLOCATION));
    				   	break;
    			   case WEATHERLOCATION:
       				GetFarsiteWindow()->SetCursor(GetFarsiteWindow()->GetModule(), "CURSTATION");
    					SetInputMode(WEATHERLOCATION, false);
    				sprintf(MBStatus, "    %s", "SIMULATION SUSPENDED");
    				sprintf(MBMode, "    %s", "WEATHERSTATION MODE");
    				sta->SetButtonState(TButtonGadget::Down);
    					SendMessage(MainWindow->HWindow, CM_SWITCHMESSAGE, TParam1(SWITCH_INPUTMODE), TParam2(WEATHERLOCATION));
    			   	break;
    		  }
    		WriteMessageBar(0);
    		 Beep(24000, 150);
    	 }
    	 else
    	 	SetInputMode(NULLLOCATION, false);
    		*/
}


void TFarsiteInterface::FlatSimulateInitiateTerminate()
{
    Execute_InitiateTerminate();
}


void TFarsiteInterface::Execute_InitiateTerminate()
// DISPLAYS LANDSCAPE WINDOW AND ENABLES/DISABLES MENU SELECTIONS
{
    bool CanGo = true;
    if (F_ON)
    {
        // 	Beep(24000, 150);
        //int Response = IDYES;
    }
    else
    {
        if (!Inputs.LandID)
        {
            printf("Load Landscape File (.lcp) Data Input Incomplete\n");
            CanGo = false;
        }
        else if (!Inputs.WtrID)
        {
            printf("Load Weather File (.wtr) Data Input Incomplete\n");
            CanGo = false;
        }
        else if (!Inputs.WndID)
        {
            printf("Load Wind File (.wnd) Data Input Incomplete\n");
            CanGo = false;
        }
        else if (!Inputs.FuelMoisID)
        {
            printf("Load Initial Fuel Moistures (.fms) Data Input Incomplete\n");
            CanGo = false;
        }
        else if (!Inputs.AdjID)
        {
            printf("Load Adjustment File (.adj) Data Input Incomplete\n");
            CanGo = false;
        }
        else if (NeedCustFuelModels() && !HaveCustomFuelModels())
        {
            printf("Load Custom Fuel Models (.fmd) Data Input Incomplete\n");
            CanGo = false;
        }
        else if (NeedConvFuelModels() && !HaveFuelConversions())
        {
            printf("Load Fuel Conversions (.cnv) Data Input Incomplete\n");
            CanGo = false;
        }
        if (CanGo)
        {
            Initiate() ;
        }
    }
    StepThrough = false;
    SaveSettingsToFile("./Settings.txt");    //JAS  added 9/30/05
}

void TFarsiteInterface::Terminate()
{
    long i, j ;

    FARSITE_GO=false;
    SIMULATE_GO=false;
    ResetFarsite();
    if(F_ON==true)
    {
        Inputs.LandID=false;
        CloseLandFile();
        Inputs.WtrID=false;
        Inputs.WndID=false;
        Inputs.AdjID=false;
        Inputs.FuelMoisID=false;
        for(i=0; i<5; i++)    // initialize strings for fileanmes
        {
            memset(Inputs.WeatherFile[i], 0x0, sizeof Inputs.WeatherFile[i]);
            memset(Inputs.WindFile[i], 0x0, sizeof Inputs.WindFile[i]);
            strcpy(Inputs.MBWtr[i], "Load Weather");
            strcpy(Inputs.MBWnd[i], "Load Winds");
            FreeWeatherData(i);
            FreeWindData(i);
        }
        SetAtmosphereGrid(0);
    }
    memset(Inputs.LandscapeFile, 0x0, sizeof(Inputs.LandscapeFile));
    FreeStationGrid();								// reset data structures for model
    sprintf(Inputs.MBLand, " %s", "Load Landscape");
    sprintf(MBStatus, "%s", "SIMULATION TERMINATED");
    //printf("%s\n",MBStatus);
    //printf("SIMULATION TERMINATED\n");
    ResetCustomFuelModels();
    ResetFuelConversions();
    SetActualTimeStep(0.0);    // reset actual timestep, must be reset for new sim
    SetDistRes(0.0);           // reset distance check, must be reset for new simulation
    SetPerimRes(0.0);
    ResetDuration();
    SetStartDate(0);
    SetStartMonth(0);
    SetStartDay(0);
    SetStartHour(0);
    SetStartMin(0);
    SetEndDate(0);
    SetEndMonth(0);
    SetEndDay(0);
    SetEndHour(0);
    SetEndMin(0);
    SetConditMonth(0);
    SetConditDay(0);
    Inputs.ResetData();
    FreeAllVectorThemes();	//StoredVectorFiles();
    FreeAllCompoundCrews();
    j=GetNumCrews();           /// must use constant j because NumCrews changes in FreeCrew();
    for(i=j-1; i>-1; i--)
        FreeCrew(i);
    j=GetNumAirCraft();        /// must use constant j because NumCrews changes in FreeCrew();
    for(i=0; i<j; i++)
        FreeAirCraft(i);
    memset(CrewFileName, 0x0, sizeof(CrewFileName));//TransGroundData.ResetData();
    if(gaat)
        delete gaat;
    gaat=0;
    memset(CrewFileName, 0x0, sizeof(CrewFileName));
    FreeBurnPeriod();

    FreeAllCoarseWoody();
    LastFMCalcTime=0;
    burn.post.ResetAllThreads();
    burn.ResetAllPerimThreads();
    burn.env->ResetAllThreads();
    burn.env->TerminateHistory();
    burn.CloseCrossThreads();
    ResetAllStopLocations();
    CheckPostFrontal(false);
    ResetNewFuels();
}

void TFarsiteInterface::Initiate()
{
    long i;

    //CopyDDEFileInfo();
    //mb->NULLHintTextPointer();
    a1 = 0;
    p1 = 0;
    p2 = 0;
    sarea = 0;
    firesizeh = 0;
    firesizes = 0;			// for area and perimeter calculations
    PrimaryVisEqual = 0;
    PrimaryVisCount = 0;
    SecondaryVisEqual = -1;
    SecondaryVisCount = 0;
    CurrentFire = 0;
    //added EB  for override
    F_ON = true;

    ResetVisPerimFile();		 // reset visperim file to 0
    for (i = 0; i < 5; i++)
        WStat[i].tws = -1.0;
    LastFMCalcTime = 0;
    //SYSTEMTIME tb;
    //GetSystemTime(&tb);
    //srand(tb.wMilliseconds);
    idum = -((rand() % 20) + 1);				// initialize random num gen
    StepThrough = false;
}


void TFarsiteInterface::Execute_StartRestart()
{
    // THIS FUNCTION MAKES STARTS SIMULATION PROCESS LOOP
    /*     if(DDE.DDE_SIMCOMMAND)   	// delay response for dde return
    			 {    Sleep(500);
    				  DDE.DDE_SIMCOMMAND=false;
    			 }
    		*/
    //SIMULATE_GO=true;
    if (SIMULATE_GO)
    {
        int Response = IDYES;
        if (!FARSITE_GO)
            Response = IDYES;
        printf("? Are You Sure You Want To Restart The Simulation ? RESTART FARSITE SIMULATION ?\n");
        if (Response == IDYES)
        {
            Execute_Reset();
            //  							  //CmSimulateRestart();
            // reset the simulation to beginning
            if (IgnitionResetAtRestart(GETVAL))
                LoadIgnitions();
            CountFires();
        }
    }
    else
    {
        if (F_ON)
        {
            long i, RealFires = 0;
            for (i = 0; i < GetNewFires(); i++)
            {
                if (GetInout(i) > 0 && GetInout(i) < 3)
                {
                    RealFires = 1;
                    break;
                }
            }
            if (RealFires > 0)	//GetNewFires()>0
            {
                if (GetSimulationDuration() == 0.0)//numaximum==0)
                {
                    StepThrough = false;
                    printf("Set Duration (Simulate | Duration) No Duration For Simulation\n");
                }
                else if (GetActualTimeStep() == 0)
                {
                    StepThrough = false;
                    printf("Set Model | Parameters before Starting the Simulation Time- and Space-resolutions not set\n");
                }
                else
                {
                    long i;
                    bool NEEDMX = false;
                    char msg[128] = "", msg2[128] = "";
                    LandscapeTheme* grid;

                    /*
                    										for(i=14;  i<257; i++)
                    										{	if(!IsNewFuelReserved(i) && GetNewFuel(i, 0))
                    											 {	if(!InitialFuelMoistureIsHere(i))
                    												{	NEEDMX=true;
                    													sprintf(msg, "Custom Fuel Model %ld Has No Initial Fuel Moisture", i);
                    													   sprintf(msg2, "Update File %s before continuing", Inputs.MBMois);
                    													   break;
                    												}
                    										   }
                    										}
                    										*/

                    if (!NEEDMX)
                    {
                        grid = GetLandscapeTheme();
                        for (i = 1; i <= grid->NumAllCats[3]; i++)
                        {
                            if (GetFuelConversion(grid->AllCats[3][i]) > 0 &&
                                    GetFuelConversion(grid->AllCats[3][i]) < 257)
                            {
                                if (!InitialFuelMoistureIsHere(GetFuelConversion(grid->AllCats[3][i])))
                                {
                                    NEEDMX = true;
                                    sprintf(msg, "Fuel Model %ld Has No Initial Fuel Moisture",(long) GetFuelConversion(grid->AllCats[3][i]));
                                    sprintf(msg2,
                                            "Update File %s before continuing",
                                            Inputs.MBMois);
                                    break;
                                }
                            }
                        }
                    }

                    if (!NEEDMX)
                    {
                        if (HaveGroundFuels())
                        {
                            if (GetTheme_Units(W_DATA) != 0 &&
                                    Inputs.CwdID == false &&
                                    CheckPostFrontal(GETVAL))
                            {
                                sprintf(msg,
                                        "Post-frontal combustion enabled, but missing CWD file");
                                sprintf(msg2,
                                        "Add .CWD File to Project before continuing");
                                NEEDMX = true;
                            }
                        }
                    }

                    if (NEEDMX)
                    {
                        printf(" %s   %s\n",msg2,msg);  //Pretty sure this was the intended use of msg2 and msg.  So I've inserted it to replace next line down!  JAS 10/17/05
                        return;
                    }

                    //CopyDDEFileInfo();
                    //SaveIgnitions();
                    ////mb->NULLHintTextPointer();
                    SIMULATE_GO = true;
                    FARSITE_GO = true;
                    sprintf(MBStatus, "    %s", "SIMULATION RUNNING");
                    //printf("%s\n",MBStatus);
                    //NULLINPUTS();
                    //WriteMessageBar(0);
                    //mb->NULLHintTextPointer();
                    LEAVEPROCESS = false;
                    burn.SIMTIME = 0.0;				   // reset start of FARSITE iterations
                    burn.CuumTimeIncrement = 0.0;
                    maximum = 0;
                    numspots = 0;
                    //pdata.ReSet();
                    //adata.ReSet();
                    ProcNum = 1;					   // first process can begin
                    burn.DistMethod = DistanceCheckMethod(GETVAL);  		 // set distance check method
                    AddDownTime(-1.0);	// set down time to 0.0;
                    ConvertAbsoluteToRelativeBurnPeriod();
                    NextFireAfterInterrupt = 0;
                    OldFireAfterInterrupt = 0;
                    smolder = flaming = 0.0;
                    //EB next line only
                    //StartSimThread();

                    //uncommented EB
                    FarsiteSimulationLoop();
                    // start FARSITE process and check message loop between iteratations


                    if (RESUMES) SaveLastState();
                }
            }
            else
            {
                StepThrough = false;
                printf("Input Ignition(s) on Landscape Ignition(s) Required\n");
            }
        }
        else
            printf("Must Initiate Simulation First (Display Landscape) Can't Start Simulation\n");
    }
}


void TFarsiteInterface::Chain()
{
    Execute_ResumeSuspend();
}


void TFarsiteInterface::Execute_ResumeSuspend()
{
    // resume/suspend simulation
    if (F_ON)
    {
        if (SIMULATE_GO)
        {
            if (FARSITE_GO && SIM_SUSPENDED == false)
            {
                StepThrough = false;
                //FARSITE_GO=false;		   // don't do it with mt version
                sprintf(MBStatus, "    %s", "SIMULATION SUSPENDED");
                SIM_SUSPENDED = true;
            }
            else
            {
                if (GetSimulationDuration() == 0.0)//numaximum==0)
                {
                    StepThrough = false;
                    printf("Verify or Reset Duration Weather/Wind Streams Changed\n");
                }
                else
                {
                    SIM_SUSPENDED = false;
                    SimRequest = SIMREQ_NULL;
                    //mb->NULLHintTextPointer();
                    FARSITE_GO = true;
                    //mb->NULLHintTextPointer();
                    //if (!StepThrough)
                    //Beep(24000, 150);
                    //if(burn.SIMTIME>=maximum)	 // if duration has been extended
                    //CopyDDEFileInfo();
                    ConvertAbsoluteToRelativeBurnPeriod();
                    FarsiteSimulationLoop() ; // for single thread. [BLN]
                    //SetEvent(hFarSimEvent);//StartSimThread();//FarsiteSimulationLoop();	   // reenter FarsiteSimulationLoop()

                }
            }
        }
        else
            printf(" START Simulation First Can't Resume Simulation\n");
    }
    else
        printf("Can't Run Simulation\n");
}






void TFarsiteInterface::Execute_Reset()
{
    ResetFarsite();
    SIMULATE_GO = false;
}


void TFarsiteInterface::ResetFarsite()
// resets data structures for model
{
    /*
    long i;
    a1 = p1 = p2 = 0.0;
    sarea = firesizeh = firesizes = 0.0;
    burn.prod.cuumslope[0] = burn.prod.cuumslope[1] = 0.0;
    burn.SetSpotLocation(-3);			// destroy all remaining spot fires
    for (i = 0; i < NUM_FUEL_SIZES; i++)
    burn.env->ResetData(i);
    memset(FMMFileName, 0x0, 256 * sizeof(char));
    memset(FMMDescription, 0x0, 512 * sizeof(char));
    burn.ResetIntersectionArrays();
    //burn.post.ResetAllThreads();
    if (WARROWS_ON)
    {
    //WARROWS_ON=false;
    if (WData)
    delete[] WData;					 // reset stored wind data
    WData = 0;
    NumWData = 0;
    }
    if (GetRastMake())
    {
    WriteGISLogFile(0);
    burn.rast.RasterReset();
    }
    if (GetVectMake())
    {
    if (GetVectVisOnly())
    {
    if (PrimaryVisCount == PrimaryVisEqual)    // visible timestep
    {
    	for (i = 0; i < GetNumFires(); i++)
    	{
    		if (GetInout(i) == 0)   		  // if fire exists
    			continue;
    		if (ExcludeBarriersFromVectorFiles(GETVAL) &&
    			GetInout(i) == 3)
    			continue;
    		vect.VectData(i, burn.SIMTIME);
    	}   							   // write last perim array that was not shown
    }
    }
    else
    {
    for (i = 0; i < GetNumFires(); i++)
    {
    	if (GetInout(i) == 0)   		  // if fire exists
    		continue;
    	if (ExcludeBarriersFromVectorFiles(GETVAL) && GetInout(i) == 3)
    		continue;
    	vect.VectData(i, burn.SIMTIME);
    }
    }
    vect.VectData(-1, burn.SIMTIME);	 // terminate vector file if present
    SetVectMake(false); 				// reset vector output
    WriteGISLogFile(1);
    }
    if (GetShapeMake())
    {
    bool WriteVect = false;
    if (!shape.VisOnly)
    WriteVect = true;
    else if (PrimaryVisCount == PrimaryVisEqual)
    WriteVect = true;
    if (WriteVect)
    {
    for (i = 0; i < GetNumFires(); i++)
    {
    	if (GetInout(i) == 0)
    		continue;
    	if (ExcludeBarriersFromVectorFiles(GETVAL) && GetInout(i) == 3)
    		continue;
    	shape.ShapeData(i, burn.SIMTIME);	// write fires
    }
     if(DDE.AdviseLoopActive)	// DDE every fire as it is written
    					{    if(GetInout(CurrentFire)<3)
    						{    if(!DDE.PostAdvise(DDE_FIREFILE, shape.GetShapeFileName()))
    																printf(" No Data Sent  DDE connection Error\n");						}
    						 else
    						{    if(!DDE.PostAdvise(DDE_BARRIERFILE, ""));
    						 		printf(" No Data Sent  DDE connection Error\n");						}
    					}


    		}
    		WriteGISLogFile(2);
    	}
    	shape.ResetShape();
    	SetShapeMake(false);
    	PrimaryVisEqual = 0;
    	PrimaryVisCount = 0;
    	SecondaryVisEqual = -1;
    	SecondaryVisCount = 0;
    	SetRastMake(false); 					 // reset raster output
    	TransOutputData.ResetData(false);
    	if (VisPerimSize)
    		SaveVisPerimeter(); 		  	// save diskcopy of visible fire perimeters
    	ResetVisPerimFile();
    	//-----------------------------
    	// NOT NEEDED IN VERSION 4.0
    	//if(ProcNum==2)
    	//	burn.embers.SpotReset(GetNumFires()-NextFireAfterInterrupt, burn.embers.CurSpot);
    	//-----------------------------
    	ClearAllFires();
    	if (CheckPostFrontal(GETVAL))
    	{
    		// finish outputs for post frontal, then...
    		FreeAllFireRings();
    	}
    	SetNumFires(0);
    	SetNewFires(0);
    	SetSkipFires(0);
    	CurrentFire = 0;
    	//-----------------------------
    	//burn.embers.CarryOverEmbers=0;		 // no carry over embers
    	//burn.embers.EmberReset();
    	//-----------------------------
    	//pdata.ReSet();
    	//adata.ReSet();
    	//fdata.ReSet();
    	//pfdata.ResetData();
    	burn.SIMTIME = 0.0;
    	SetTemporaryTimeStep(0.0);
    	burn.CuumTimeIncrement = 0.0;
    	numspots = 0;
    	if (DurationResetAtRestart(GETVAL))
    	{
    		maximum = 0; // numaximum=
    		ResetDuration();
    	}
    	TimeKeepCurrent.ReSet();		   	  	// global time keeping structures
    	TimeKeepElapsed.ReSet();
    	sprintf(ElTime, "%02d %02d%s%02d", 0, 0, ":", 0);
    	sprintf(CurTime, "%02d%s%02d  %02d%s%02d", GetStartMonth(), "/",
    		GetStartDay(), GetStartHour() / 100, ":", GetStartMin());
    	for (i = 0; i < 5; i++) 	// reset weather stats for wind gauges
    		WStat[i].tws = -1.0;
    	WStation.ReSet();
    	sprintf(MBStatus, "    %s", "SIMULATION RESET");
    	//WriteMessageBar(0);
    	ResetBitmap(2);
    	ResetBitmap(3);
    	SetFuelsAltered(0);
    	LEAVEPROCESS = true;
    	FARSITE_GO = false;
    	ProcNum = 1;							   // process number 1 in simulation
    	NextFireAfterInterrupt = 0; 			   // reset beginning of burn-loop
    	OldFireAfterInterrupt = 0;
    	FreeAllAttacks();					 // eliminate all attacks on fires
    	FreeAllAirAttacks();
    	FreeAllGroupAirAttacks();
    	//FreeAllCompoundCrews();			// can't do it here, because may need them
    	StepThrough = false;
    	//DDE.DDE_SIMCOMMAND=false;
    	FreeElev();
    	CountFires();
    	CanSetRasterResolution(1);  	  // false
    	SIM_SUSPENDED = false;
    	EndSimThread();
    	CanModifyInputs(true);
    	NumSimThreads = 0;
    	for (i = 0; i < GetNumStopLocations(); i++)
    		EnableStopLocation(i, true);
    */
}


void TFarsiteInterface::ClearAllFires()
{
    long i;

    for (i = 0; i < GetNewFires(); i++)
    {
        if (GetInout(i) > 0)				   // if fire exists
        {
            FreePerimeter1(i);  		   // reset numpts, inout, and memory
            SetNumPoints(i, 0);
            SetInout(i, 0);
        }
    }
    FreePerimeter2();
}


void TFarsiteInterface::UpdateFuelMoistures()
{
    //burn.env->UpdateFuelMoistureMap();
}


void TFarsiteInterface::WriteGISLogFile(long LogType)
{
    char LogFile[256];
    memset(LogFile, 0x0, sizeof(LogFile));
    long len;
    //SYSTEMTIME SysTime;

    //GetLocalTime(&SysTime);
    if (LogType == 0)
    {
        len = strlen(GetRasterFileName(0));
        strncpy(LogFile, GetRasterFileName(0), len - 3);
        strcat(LogFile, "LGR");
    }
    else if (LogType == 1)
    {
        len = strlen(GetVectorFileName());
        strncpy(LogFile, GetVectorFileName(), len - 3);
        strcat(LogFile, "LGV");
    }
    else if (LogType == 2)
    {
        if (!shape.GetShapeFileName())
            return;
        len = strlen(shape.GetShapeFileName());
        strncpy(LogFile, shape.GetShapeFileName(), len - 3);
        strcat(LogFile, "LGS");
    }
    else
        return;

    FILE* logfile = fopen(LogFile, "w");
    if (logfile == NULL)
    {
        //SetFileAttributes(LogFile, FILE_ATTRIBUTE_NORMAL);
        chmod(LogFile,S_IRWXO);
        logfile = fopen(LogFile, "w");
    }
    fprintf(logfile, "%s %s\n", "Log File: ", LogFile);
    //fprintf(logfile, "%s %02u%s%02u%s%d\n", "Date File Created:",
    //	SysTime.wMonth, "\\", SysTime.wDay, "\\", SysTime.wYear);
    //fprintf(logfile, "%s %02u%s%02u\n\n", "Time File Created:",
    //	SysTime.wHour, ":", SysTime.wMinute);
    if (LogType < 2)
    {
        if (AccessOutputUnits(GETVAL) == 0)
            fprintf(logfile, "File units: Metric\n");
        else
            fprintf(logfile, "File units: English\n");
    }
    fprintf(logfile, "%s %s\n", "Landscape File: ", Inputs.LandscapeFile);
    for (len = 0; len < GetNumStations(); len++)
    {
        fprintf(logfile, "%s %ld: %s\n", "Weather File", len + 1,
                Inputs.WeatherFile[len]);
        fprintf(logfile, "%s %ld: %s\n", "Wind File", len + 1,
                Inputs.WindFile[len]);
        if (len > 4)
            break;
    }
    fprintf(logfile, "%s %s\n", "Adjustment File: ", Inputs.AdjustmentFile);
    fprintf(logfile, "%s %s\n", "Fuel Moisture File: ",
            Inputs.FuelMoistureFile);
    if (HaveFuelConversions())
        fprintf(logfile, "%s %s\n", "Conversion File: ",
                Inputs.ConvertFile);
    else
        fprintf(logfile, "%s %s\n", "Conversion File: ", "None");
    if (HaveCustomFuelModels())
        fprintf(logfile, "%s %s\n", "Custom Fuel Model File: ",
                Inputs.FuelModelFile);
    else
        fprintf(logfile, "%s %s\n", "Custom Fuel Model File: ", "None");
    if (EnableCrowning(GETVAL))
    {
        fprintf(logfile, "%s\n", "Crown Fire: Enabled");
        if (LinkDensityWithCrownCover(GETVAL))
            fprintf(logfile, "%s\n", "Crown Density LINKED to Crown Cover");
        else
            fprintf(logfile, "%s\n",
                    "Crown Density NOT LINKED to Crown Cover");
    }
    else
        fprintf(logfile, "%s\n", "Crown Fire: Disabled");
    if (EnableSpotting(GETVAL))
    {
        fprintf(logfile, "%s\n", "Ember Generation: Enabled");
        if (EnableSpotFireGrowth(GETVAL))
            fprintf(logfile, "%s\n", "Spot Growth: Enabled");
        else
            fprintf(logfile, "%s\n", "Spot Growth: Disabled");
    }
    else
        fprintf(logfile, "%s\n", "Ember Generation: Disabled");
    if (ConstantBackingSpreadRate(GETVAL))
        fprintf(logfile, "%s\n",
                "Backing Spread: Calculated from No Wind/No Slope");
    else
        fprintf(logfile, "%s\n",
                "Backing Spread: Calculated from Elliptical Dimensions");
    if (AccelerationON())
    {
        /*	if (strlen(TransAccelData.Dat.AccelLoad) > 0)
        {
        	if (strlen(TransAccelData.Dat.AccelSave) > 0)
        		fprintf(logfile, "%s %s\n", "Acceleration File Used: ",
        			TransAccelData.Dat.AccelSave);
        	else
        		fprintf(logfile, "%s %s\n", "Acceleration File Used: ",
        			TransAccelData.Dat.AccelLoad);
        }
        else
        	fprintf(logfile, "%s %s\n", "Acceleration File Used: ",
        		"Default Values");
        			  */
    }
    else
        fprintf(logfile, "%s %s\n", "Acceleration File Used: ",
                "None (feature toggled off)");
    fprintf(logfile, "\n");
    fprintf(logfile, "%s %ld%s%ld %ld:00\n",
            "Simulation Started (Day Hour:Min):", GetStartMonth(), "/",
            GetStartDay(), GetStartHour() / 100);
    fprintf(logfile, "%s %s\n", "Simulation Ended (Day Hour:Min):",
            CurTime);
    fprintf(logfile, "%s %s\n\n", "Elapsed Time (Days Hours:Mins):",
            ElTime);
    fprintf(logfile, "%s %lf\n", "Actual Time Step (min):",
            GetActualTimeStep());
    fprintf(logfile, "%s %lf\n", "Visible Time Step (min):",
            GetVisibleTimeStep());
    fprintf(logfile, "%s %lf\n", "Perimeter Resolution (m):",
            GetPerimRes());
    fprintf(logfile, "%s %lf\n", "Distance Resolution (m):", GetDistRes());
    fclose(logfile);
    //free(LogFile);
}

void TFarsiteInterface::SetResume(int valor,int valor2,char *filenameI, char *filenameO)
{
    RESUMEL = valor;
    RESUME = valor;
    RESUMES = valor2;
    StateFileNameI = filenameI;
    StateFileNameO = filenameO;
}

void TFarsiteInterface::SaveLastState()
{
    // for saving ignitions after a phase
    long i, j, k;
    double xpt, ypt, ros, fli;
    FILE* IgFile;
    if ((IgFile = fopen(StateFileNameO, "w")) == NULL)
    {
        chmod(StateFileNameO,S_IRWXO);
        IgFile = fopen(StateFileNameO, "w");
    }

    if (IgFile)
    {
        fprintf(IgFile, "%ld\n", GetNewFires());
        for (i = 0; i < GetNewFires(); i++)
        {
            k = GetNumPoints(i) + 1;
            if (k > 0)
            {
                fprintf(IgFile, "%ld %ld %ld\n", i, k, GetInout(i));
                for (j = 0; j < k; j++)
                {
                    xpt = GetPerimeter1Value(i, j, XCOORD);
                    ypt = GetPerimeter1Value(i, j, YCOORD);
                    ros = GetPerimeter1Value(i, j, ROSVAL);
                    fli = GetPerimeter1Value(i, j, FLIVAL);
                    fprintf(IgFile, "%lf %lf %lf %lf\n", xpt, ypt, ros, fli);
                }
            }
        }
        fclose(IgFile);
        printf("Saved state in %s\n",StateFileNameO);
    }
    else
        printf("Check File Attributes and Change From 'READ ONLY' Could Not Write File %s\n",StateFileNameO);
}

void TFarsiteInterface::LoadLastState()
{
    // for reloading ignitions after one phase
    long i, j, k, l, m, n;
    double xpt, ypt, ros, fli;
    FILE* IgFile;

    if ((IgFile = fopen(StateFileNameI, "r")) != NULL)
    {
        fscanf(IgFile, "%ld", &m);
        //SetNewFires(m);
        n = 0;
        for (i = 0; i < m; i++)
        {
            fscanf(IgFile, "%ld %ld %ld", &j, &k, &l);
            if (l > 0)
            {
                SetNumPoints(j, k - 1);
                SetInout(j, l);
                AllocPerimeter1(j, k);
                n++;
            }
            for (j = 0; j < k; j++)
            {
                fscanf(IgFile, "%lf %lf %lf %lf", &xpt, &ypt, &ros, &fli);
                if (l < 1)
                    continue;
                SetPerimeter1(i, j, xpt, ypt);
                SetFireChx(i, j, ros, fli);
            }
        }
        SetNewFires(n);
        fclose(IgFile);
        printf("Loaded state from %s\n",StateFileNameI);
    }
    else
    {
        printf("Problem reading LastState.txt\n");

    }
}

void TFarsiteInterface::SaveIgnitions()
{
    // for saving ignitions before restarting
    long i, j, k;
    double xpt, ypt, ros, fli;
    FILE* IgFile;

    //memset(IgFileName, 0x0, sizeof(IgFileName));
    //strcpy(IgFileName, FarsiteDirectory);
    //strcat(IgFileName, "\\ignition.ign");
    if ((IgFile = fopen(IgFileName, "w")) == NULL)
    {
        //SetFileAttributes(IgFileName, FILE_ATTRIBUTE_NORMAL);
        chmod(IgFileName,S_IRWXO);
        IgFile = fopen(IgFileName, "w");
    }

    if (IgFile)
    {
        fprintf(IgFile, "%ld\n", GetNewFires());
        for (i = 0; i < GetNewFires(); i++)
        {
            k = GetNumPoints(i) + 1;
            if (k > 0)
            {
                fprintf(IgFile, "%ld %ld %ld\n", i, k, GetInout(i));
                for (j = 0; j < k; j++)
                {
                    xpt = GetPerimeter1Value(i, j, XCOORD);
                    ypt = GetPerimeter1Value(i, j, YCOORD);
                    ros = GetPerimeter1Value(i, j, ROSVAL);
                    fli = GetPerimeter1Value(i, j, FLIVAL);
                    fprintf(IgFile, "%lf %lf %lf %lf\n", xpt, ypt, ros, fli);
                }
            }
        }
        fclose(IgFile);
    }
    else
        printf("Check File Attributes and Change From 'READ ONLY' Could Not Write File\n");
}

void TFarsiteInterface::lIgn()
{
    //printf("IgnLoaded!\n");
    LoadIgnitions();
    printf("IgnLoaded!");
}
void TFarsiteInterface::sIgn()
{
    SaveIgnitions();
    printf("IgnSaved!");
}

void TFarsiteInterface::LoadIgnitions()
{
    // for reloading ignitions after restarting
    long i, j, k, l, m, n;
    double xpt, ypt, ros, fli;
    FILE* IgFile;

    if ((IgFile = fopen(IgFileName, "r")) != NULL)
    {
        fscanf(IgFile, "%ld", &m);
        //SetNewFires(m);
        n = 0;
        for (i = 0; i < m; i++)
        {
            fscanf(IgFile, "%ld %ld %ld", &j, &k, &l);
            if (l > 0)
            {
                SetNumPoints(j, k - 1);
                SetInout(j, l);
                AllocPerimeter1(j, k);
                n++;
            }
            for (j = 0; j < k; j++)
            {
                fscanf(IgFile, "%lf %lf %lf %lf", &xpt, &ypt, &ros, &fli);
                if (l < 1)
                    continue;
                SetPerimeter1(i, j, xpt, ypt);
                SetFireChx(i, j, ros, fli);
            }
        }
        SetNewFires(n);
        fclose(IgFile);
        //		for (i = 0; i < GetNewFires(); i++)
        //			DrawFire(i);
    }
}

void TFarsiteInterface::ResetDuration()
{
    SetSimulationDuration(0);
}


//----------------------------------------------------------------------------------------------------
//
//			 FARSITE PROCESSES
//
//----------------------------------------------------------------------------------------------------


void TFarsiteInterface::CurrentWeather()
{
    //double fxc, fyc;
    //long i;
    //EnvironmentData lmw;
    //LandscapeStruct ld;

    /*	for (i = 0; i < WStation.Number; i++)
    {
    fxc = WStation.wxc[i];
    fyc = WStation.wyc[i];
    burn.fe->GetLandscapeData(fxc, fyc, ld);
    burn.fe->GetFireEnvironment(burn.env, burn.SIMTIME, true);
    burn.fe->GetEnvironmentData(&lmw);//LoadLocalFEData(burn.fe);
    WStation.SetData(i, lmw.ones * 100, lmw.tens * 100,
    			lmw.hundreds * 100, lmw.temp, lmw.humid);
    if (WStation.welev[i] == 0)		// elev will be zero on first access of ws
    {
    	WStation.welev[i] = ld.elev * 3.28;
    	WStation.wslope[i] = 100 * (tan(PI * (((double) ld.slope) / 180.0)));
    	if(burn.cover==99)		// cover==0
    						WStation.wcover[i]=0;
    					else
    						WStation.wcover[i]=burn.cover*20;


    			WStation.wcover[i] = ld.cover;
    			int a = ld.aspectf / PI*180.0;
    			if (a > 360)
    				a -= 360;
    			WStation.waspect[i] = a;
    		}
    	}
    	WStation.CheckAlloc();  		   // increment number of records and check storage
    	WStation.CheckChange();			// check for changes in graph axes dimensions
    		*/
}


bool TFarsiteInterface::FarsiteProcess1()
// THIS FUNCTION CONTAINS THE FARSITE PROCESS CONTROL
{
    long InOut, FireNum;
    int initialWr=0;
    IN_BURNPERIOD = true;

    if (burn.SIMTIME==0.0)
        initialWr=1;
    if (DistanceCheckMethod(GETVAL) == DISTCHECK_SIMLEVEL)  	  // if simlevel timestep and/or burnout enabled
    {
        if (NextFireAfterInterrupt == 0)
            burn.DetermineSimLevelTimeStep();
        if (!InquireInBurnPeriod(burn.SIMTIME))//+GetTemporaryTimeStep()))
        {
            if (burn.prod.cuumslope[1] > 1e-9)
                sarea = (a1 - firesizeh) /
                        (cos(atan((PI * burn.prod.cuumslope[0] / 180.0) /
                                  burn.prod.cuumslope[1])));
            else
                sarea = 0.0;
            sarea = firesizes + sarea;
            firesizeh = a1;
            firesizes = sarea;
            AddDownTime(GetTemporaryTimeStep());
            burn.CuumTimeIncrement += GetTemporaryTimeStep();
            IN_BURNPERIOD = false;

            return true;
        }
        else if (NextFireAfterInterrupt == 0 && burn.CuumTimeIncrement == 0.0)
        {
            a1 = 0.0;
            p1 = 0.0;
            p2 = 0.0;
        }
        for (CurrentFire = NextFireAfterInterrupt; CurrentFire < GetNumFires(); CurrentFire++)			// for all fires
        {
            if ((InOut = GetInout(CurrentFire)) == 0)
                continue;
            burn.BurnIt(CurrentFire);
            if (InOut < 3 && burn.CuumTimeIncrement == 0.0)//GetTemporaryTimeStep()==burn.CuumTimeIncrement)
            {
                a1 += burn.prod.area;   			// +area of all fires (&& -inward fire area)
                p1 += burn.prod.perimeter;  		// +perimeter of all fires)
                p2 += burn.prod.sperimeter; 		// +slope perimeter of all fires
            }
            //if(CurrentFire<GetNumFires()-1)
            //{	NextFireAfterInterrupt=CurrentFire+1;
            //if(SimRequest==SIMREQ_RESUMESUSPEND && !StepThrough) // Allow interruptions by user
            //{	Execute_ResumeSuspend();
            //	SuspendThread(hSimThread);
            //}
            //if(!FARSITE_GO)
            //	break;
            //}
        }
        //if(SimRequest==SIMREQ_RESUMESUSPEND && !StepThrough)
        //{	Execute_ResumeSuspend();
        //   	SuspendThread(hSimThread);
        //}
        if (!FARSITE_GO)
            return false;
        NextFireAfterInterrupt = 0;
        AddDownTime(-1.0);
    }
    else
    {
        if (!InquireInBurnPeriod(burn.SIMTIME))//+GetActualTimeStep()))
        {
            if (burn.prod.cuumslope[1] > 1e-9)
                sarea = (a1 - firesizeh) /
                        (cos(atan((PI * burn.prod.cuumslope[0] / 180.0) /
                                  burn.prod.cuumslope[1])));
            else
                sarea = 0.0;
            sarea = firesizes + sarea;
            firesizeh = a1;
            firesizes = sarea;
            AddDownTime(GetActualTimeStep());
            for (CurrentFire = NextFireAfterInterrupt; CurrentFire < GetNumFires(); CurrentFire++)			// for all fires
            {
                if (OutputVectsAsGrown)
                    if (!initialWr)
                        WriteVectorOutputs();   		   // output perimeters to screen and/or vector file
            }
            IN_BURNPERIOD = false;

            return true;
        }
        else if (NextFireAfterInterrupt == 0)
        {
            a1 = 0.0;
            p1 = 0.0;
            p2 = 0.0;
        }
        for (CurrentFire = NextFireAfterInterrupt;
                CurrentFire < GetNumFires();
                CurrentFire++)			// for all fires
        {
            if ((InOut = GetInout(CurrentFire)) == 0)
                continue;
            //if(GetInout(CurrentFire)<3)   		// no outputs of vector barriers
            if (OutputVectsAsGrown)
            {
                if (!initialWr)
                    WriteVectorOutputs();   		   // output perimeters to screen and/or vector file
            }
            burn.BurnIt(CurrentFire);   			// burn the fire
            if (InOut < 3)
            {
                a1 += burn.prod.area;   			// +area of all fires (&& -inward fire area)
                p1 += burn.prod.perimeter;  		// +perimeter of all fires)
                p2 += burn.prod.sperimeter; 		// +slope perimeter of all fires
            }
            //if(CurrentFire<GetNumFires()-1)
            //{	NextFireAfterInterrupt=CurrentFire+1;
            //if(SimRequest==SIMREQ_RESUMESUSPEND && !StepThrough)
            //{	Execute_ResumeSuspend();
            //	SuspendThread(hSimThread);
            //}
            //if(!FARSITE_GO)
            //	break;
            //}
        }
        //if(SimRequest==SIMREQ_RESUMESUSPEND && !StepThrough)
        //{	Execute_ResumeSuspend();
        //    	SuspendThread(hSimThread);
        //}
        if(!FARSITE_GO)
            return false;
        NextFireAfterInterrupt = 0;
        AddDownTime(-1.0);
    }

    while (GetNumFires() < GetNewFires())   		// condense array of fire perimeter pointers
    {
        CurrentFire = GetNumFires();
        if (GetInout(CurrentFire) != 0)
        {
            FireNum = 0;
            while (GetInout(FireNum) > 0)
            {
                FireNum++;
            }
            if (FireNum < CurrentFire)
            {
                FireNum = CurrentFire;
                burn.RePositionFire(&CurrentFire);
                if (CheckPostFrontal(GETVAL))
                    SetNewFireNumber(FireNum, CurrentFire,
                                     burn.post.AccessReferenceRingNum(1, GETVAL));
            }
        }
        IncNumFires(1);				   // increment number of active fires
    }
    IncNumFires(-GetSkipFires());			// subtract merged or extinguished fires
    SetNewFires(GetNumFires());
    SetSkipFires(0);						// reset skipfire to 0
    if (burn.prod.cuumslope[1] > 1e-9)
        sarea = (a1 - firesizeh) /
                (cos(atan((PI * burn.prod.cuumslope[0] / 180.0) /
                          burn.prod.cuumslope[1])));
    else
        sarea = 0.0;
    sarea = firesizes + sarea;
    firesizeh = a1;
    firesizes = sarea;

    return true;
}


bool TFarsiteInterface::FarsiteProcessSpots()
// THIS FUNCTION CONTAINS THE FARSITE PROCESS CONTROL
{
    if(!IN_BURNPERIOD)
        return true;

    double Actual;
    double simtime = burn.SIMTIME;
    double cuumtime = burn.CuumTimeIncrement;
    double TempStep = 0.0;
    bool SpotActivityChanged = false;
    long InOut, TempTime, CurrentSpot;
    Embers::spotdata* curspot;

    if (EnableSpotting(GETVAL))
    {
        EnableSpotting(false);
        SpotActivityChanged = true;
    }
    if (OldFireAfterInterrupt == 0)
        burn.SetSpotLocation(0);//burn.embers.CurSpot=burn.embers.FirstSpot;
    Actual = GetActualTimeStep();
    if (DistanceCheckMethod(GETVAL) == DISTCHECK_SIMLEVEL)
        TempStep = GetTemporaryTimeStep();

    for (CurrentFire = 0;
            CurrentFire < burn.TotalSpots;
            CurrentFire++)			// for all fires
    {
        curspot = burn.GetSpotData(Actual);
        if (curspot == NULL)
            continue;
        if (curspot->TimeRem < 0.10)
            curspot->TimeRem = 0.10;

        CurrentSpot = InsertSpotFire(curspot->x, curspot->y);
        if (CurrentSpot < 0)
            continue;

        //CurrentSpot=(long) curspot->vecdir;  // this variable is used to hold the position
        // in the perimeter1[] array
        if ((InOut = GetInout(CurrentSpot)) == 0)
            continue;
        // truncate to nearest 0.1 minutes for easier precision in Burn::
        curspot->TimeRem *= 10.0;
        TempTime = (long)curspot->TimeRem;
        curspot->TimeRem = (double) TempTime / 10.0;

        burn.SIMTIME += (Actual - curspot->TimeRem);//burn.embers.CurSpot->TimeRem;
        if (burn.SIMTIME < 0.0)
            burn.SIMTIME = 0.0;

        if (DistanceCheckMethod(GETVAL) == DISTCHECK_FIRELEVEL)
        {
            burn.CuumTimeIncrement = 0.0;
            SetActualTimeStep(curspot->TimeRem);//burn.embers.CurSpot->TimeRem);
        }
        else
        {
            burn.CuumTimeIncrement = Actual - curspot->TimeRem;
            SetTemporaryTimeStep(curspot->TimeRem);
        }

        if (!InquireInBurnPeriod(burn.SIMTIME))
        {
            burn.SIMTIME = simtime;

            continue;
        }

        burn.BurnIt(CurrentSpot);   	   	// burn the fire
        if (InOut < 3)
        {
            a1 += burn.prod.area;   			// +area of all fires (&& -inward fire area)
            p1 += burn.prod.perimeter;  		// +perimeter of all fires)
            p2 += burn.prod.sperimeter; 		// +slope perimeter of all fires
        }
        burn.SIMTIME = simtime;
        burn.CuumTimeIncrement = cuumtime;
    }
    NextFireAfterInterrupt = OldFireAfterInterrupt = 0;
    long FireNum;
    while (GetNumFires() < GetNewFires())      // condense array of fire perimeter pointers
    {
        CurrentFire = GetNumFires();
        if (GetInout(CurrentFire) != 0)
        {
            FireNum = 0;
            while (GetInout(FireNum) > 0)
            {
                FireNum++;
            }
            if (FireNum < CurrentFire)
            {
                FireNum = CurrentFire;
                burn.RePositionFire(&CurrentFire);
                if (CheckPostFrontal(GETVAL))
                    SetNewFireNumber(FireNum, CurrentFire,
                                     burn.post.AccessReferenceRingNum(1, GETVAL));
            }
        }
        IncNumFires(1);				   // increment number of active fires
    }
    IncNumFires(-GetSkipFires());			// subtract merged or extinguished fires
    SetNewFires(GetNumFires());
    SetSkipFires(0);						// reset skipfire to 0
    if (burn.prod.cuumslope[1] > 1e-9)
        sarea = (a1 - firesizeh) /
                (cos(atan((PI * burn.prod.cuumslope[0] / 180.0) /
                          burn.prod.cuumslope[1])));
    else
        sarea = 0.0;
    sarea = firesizes + sarea;
    firesizeh = a1;
    firesizes = sarea;
    if (DistanceCheckMethod(GETVAL) == DISTCHECK_FIRELEVEL)
        SetActualTimeStep(Actual);
    else
        SetTemporaryTimeStep(TempStep);
    if (SpotActivityChanged)
        EnableSpotting(true);
    burn.SetSpotLocation(-1);     // elimate all spots

    return true;
}


long TFarsiteInterface::InsertSpotFire(double xpt, double ypt)
{
    long k, newspots;
    long NumVertex;
    double Sxpt, Sypt, NewAngle;
    double Xlo, Xhi, Ylo, Yhi;
    double OutsideX, OutsideY;
    double AngleIncrement, AngleOffset;

    NumVertex = 6;
    GetNumSpots(&newspots, true);	// reserve an address

    Xlo = Xhi = xpt;
    Ylo = Yhi = ypt;									// initialize bounding rectangle
    AngleOffset = ((double) (rand2(&idum) * 1000)) / 999.0 * PI / 2.0;	// randomize orientation of fire polygon
    AngleIncrement = PI / ((double) NumVertex / 2.0);				// to avoid coincident sides
    OutsideX = -1.0;	// no outside points
    OutsideY = -1.0;
    AllocPerimeter1(newspots, NumVertex + 1);
    long tID;
    int threads=1;

#ifdef _OPENMP
    threads=omp_get_max_threads();
    omp_set_num_threads(threads);
#endif
    ;

    //#pragma omp parallel for schedule (dynamic) private(k,Sxpt,Sypt,NewAngle)
    for (k = 0; k < NumVertex; k++)
    {
        NewAngle = ((double) k) * AngleIncrement + AngleOffset;
        Sxpt = xpt + cos(NewAngle);   	//Stime is the time step for growing spot fires
        Sypt = ypt + sin(NewAngle);
        SetPerimeter1(newspots, k, Sxpt, Sypt); 	 // save head fire ros at time step
        SetFireChx(newspots, k, 0.0, 0.0);
        SetReact(newspots, k, 0.0);
        if (Sxpt < Xlo)
            Xlo = Sxpt;
        else if (Sxpt > Xhi)
            Xhi = Sxpt;
        if (Sypt < Ylo)
            Ylo = Sypt;
        else if (Sypt > Yhi)
            Yhi = Sypt;
    }
    SetPerimeter1(newspots, k, Xlo, Xhi);    // save bounding rectangle
    SetFireChx(newspots, k, Ylo, Yhi);
    //#pragma omp parallel for schedule (dynamic) private(k,Sxpt,Sypt,OutsideX, OutsideY)
    for (k = 0; k < NumVertex; k++) 	// check to see if
    {
        Sxpt = GetPerimeter1Value(newspots, k, 0);    // points outside
        Sypt = GetPerimeter1Value(newspots, k, 1);
        if (Sxpt <= GetLoEast())
            OutsideX = Xhi;
        else if (Sxpt >= GetHiEast())
            OutsideX = Xlo;
        if (Sypt >= GetHiNorth())
            OutsideY = Ylo;
        else if (Sypt <= GetLoNorth())
            OutsideX = Yhi;
    }
    if (OutsideX != -1.0 || OutsideY != -1.0)
    {
        FreePerimeter1(newspots);
        newspots = -1;
    }
    else
    {
        SetInout(newspots, 1);
        SetNumPoints(newspots, NumVertex);
    }

    return newspots;
}


bool TFarsiteInterface::FarsiteProcess2()
{
    if (!IN_BURNPERIOD)
    {
        ProcNum = 3;

        return false;
    }

    if (DistanceCheckMethod(GETVAL) == DISTCHECK_SIMLEVEL)
    {
        if (ProcNum == 1)	// only first time through
            burn.CuumTimeIncrement += GetTemporaryTimeStep();
        if (burn.CuumTimeIncrement < GetActualTimeStep())
        {
            ProcNum = 3;

            return false;
        }
        else
            burn.CuumTimeIncrement = GetActualTimeStep();
    }

    long OldNumSpots, Numspots;

    OldNumSpots = numspots; 			 	// save old number of spots/embers
    burn.BurnSpotThreads();				// ember flight and ignition, after all fires have burned this step
    //DrawSpots();
    numspots += OldNumSpots;				  // sum OldNumSpots and new numspots, after DrawSpots()
    //IncNewFires(burn.TotalSpots);//embers.SpotFires); 	// add spotfires to total number to check for mergers
    //SetNumFires(GetNewFires());   		  // update the number of new fires
    GetNumSpots(&Numspots, false);
    SetNumFires(Numspots);
    if (burn.SpotFires) //burn.embers.SpotFires)
    {
        NextFireAfterInterrupt = GetNumFires() - (burn.SpotFires);//+burn.FailedSpots);//burn.embers.SpotFires;
        burn.SpotFires = 0;//burn.embers.SpotFires=0;
        ProcNum = 2;

        return true;
    }
    ProcNum = 3;
    burn.SetSpotLocation(-2);

    return false;
}


void TFarsiteInterface::FarsiteProcess3()
// THIS FUNCTION PROCESSES FIRE MERGERS
{
    long i, j, NewFires, NewPts;
    double mx[10] =
    {
        90.0, 90.0, 90.0, 90.0, 90.0, 90.0, 90.0, 90.0, 90.0, 90.0
    };
    AttackData* atk;
    AirAttackData* aatk;
    FireRing* firering;


    for (i = 0;
            i < GetNumAttacks();
            i++)   	 // perform indirect attack after all fires have burned
    {
        if ((atk = GetAttackByOrder(i, true)) != 0)
        {
            NewFires = GetNewFires();		// save value before attack
            if (atk->FireNumber == -1)
                NewFires++;
            if (atk->Indirect == 2)
                Atk.ConductBurnout(atk);
            else if (!Atk.IndirectAttack(atk, GetActualTimeStep()))
            {
                CancelAttack(atk->AttackNumber);
                i--;	  // need to decrement counter because numattacks has changed
            }   		   // and order in GetAttackByOrder() has also changed
            /*else
            						{	//burn.ReorderPerimeter(atk->FireNumber, burn.FindExternalPoint(atk->FireNumber, 0));
            						burn.FindOuterFirePerimeter(atk->FireNumber);
            						NewPts=GetNumPoints(atk->FireNumber);
            						FreePerimeter1(atk->FireNumber);
            						AllocPerimeter1(atk->FireNumber, NewPts+1);
            						burn.tranz(atk->FireNumber, NewPts);
            							 Atk.BoundingBox();
            						}*/
            if (NewFires < GetNewFires())
            {
                burn.ReorderPerimeter(NewFires,
                                      burn.FindExternalPoint(NewFires, 0));
                burn.FindOuterFirePerimeter(NewFires);
                NewPts = GetNumPoints(NewFires);
                FreePerimeter1(NewFires);
                if (NewPts > 0)
                {
                    AllocPerimeter1(NewFires, NewPts + 1);
                    burn.tranz(NewFires, NewPts);
                    Ignition.BoundingBox(NewFires);
                    if (CheckPostFrontal(GETVAL))
                    {
                        firering = burn.post.SetupFireRing(NewFires,
                                                           burn.SIMTIME +
                                                           burn.CuumTimeIncrement,
                                                           burn.SIMTIME +
                                                           burn.CuumTimeIncrement);
                        for (j = 0; j < NewPts; j++)
                            AddToCurrentFireRing(firering, j, 0, 0, 0, mx, 0.0);
                    }
                }
                else
                {
                    //FreePerimeter2();
                    SetNumPoints(NewFires, 0);
                    SetInout(NewFires, 0);
                    IncSkipFires(1);
                }
            }
            SetNumFires(GetNewFires());
        }
    }
    for (i = 0;
            i < GetNumAirAttacks();
            i++)		  // check air attack effectiveness
    {
        aatk = GetAirAttackByOrder(i);
        if (!AAtk.CheckEffectiveness(aatk, GetActualTimeStep()))
        {
            //DrawFire(-aatk->PatternNumber); 		// redraw elapsed pattern
            --aatk->PatternNumber;
            FreePerimeter1(aatk->PatternNumber);
            SetNumPoints(aatk->PatternNumber, 0);
            SetInout(aatk->PatternNumber, 0);
            IncSkipFires(1);
            CancelAirAttack(aatk->AirAttackNumber);
            i--;	  // decrement i becuase airattack number has changed.
        }
    }
    if (gaat)    // if object for handling group air attacks exists
    {
        if (DistanceCheckMethod(GETVAL) == DISTCHECK_SIMLEVEL)
            gaat->ExecuteAllIndirectAttacks(GetTemporaryTimeStep());
        else
            gaat->ExecuteAllIndirectAttacks(GetActualTimeStep());
    }

    if (IN_BURNPERIOD == true)
    {
        if (GetNumFires() > 1)
        {
            CurrentFire = 0;
            burn.CrossFires(1, &CurrentFire);  	// check for crosses between fires outward fires
            SetNumFires(GetNewFires());
        }
        CheckStopLocations();
    }

    if (CheckPostFrontal(GETVAL))   		  // reset to flag first num
    {
        CondenseRings(burn.post.AccessReferenceRingNum(1, GETVAL));
        burn.post.bup.BurnFireRings(burn.post.AccessReferenceRingNum(1, GETVAL),
                                    GetNumRings());
        burn.post.ComputePostFrontal(burn.SIMTIME + burn.CuumTimeIncrement,
                                     &smolder, &flaming);
        //CondenseRings(0);	// done in burn.post.ComputePostFrontal()
        burn.post.AccessReferenceRingNum(1, GetNumRings()); // could set to -1 for automatic
        //DrawPostFrontal();
    }
    burn.ResetIntersectionArrays();		// free allocations for intersections

    if (StepThrough)
    {
        if (PrimaryVisCount == PrimaryVisEqual)
        {
            if (DistanceCheckMethod(GETVAL) == DISTCHECK_SIMLEVEL)
            {
                if (burn.CuumTimeIncrement == GetActualTimeStep())
                {
                    SimRequest = SIMREQ_RESUMESUSPEND;//FARSITE_GO=false;
                    StepThrough = false;
                    sprintf(MBStatus, "    %s", "SIMULATION SUSPENDED");
                    //printf(MBStatus);
                    //Beep(24000, 150);
                    //if(DDE.AdviseLoopActive)	// DDE every fire as it is written
                    //	DDE.PostAdvise(DDE_STEPFINISHED, "Finished");
                }
            }
            else
            {
                SimRequest = SIMREQ_RESUMESUSPEND;//FARSITE_GO=false;
                StepThrough = false;
                sprintf(MBStatus, "    %s", "SIMULATION SUSPENDED");
                //printf(MBStatus);
            }
        }
    }
    if (DistanceCheckMethod(GETVAL) == DISTCHECK_FIRELEVEL)
    {
        burn.SIMTIME += GetActualTimeStep();		// update simulation time
        burn.CuumTimeIncrement = 0.0;
    }
    else
    {
        if (GetActualTimeStep() - burn.CuumTimeIncrement <= 0.0)  // end of timestep
        {
            burn.SIMTIME += GetActualTimeStep();	// update simulation time
            burn.CuumTimeIncrement = 0.0;

            pfdata.SetData(burn.SIMTIME, flaming / GetActualTimeStep() * 60.0,
                           smolder / GetActualTimeStep() * 60.0);
            smolder = 0.0;
            flaming = 0.0;
        }
    }
    //   FreeElev();
}


void TFarsiteInterface::CheckStopLocations()
{
    bool found = false;
    int response;
    long i, j;
    double xpt, ypt;

    for (j = 0; j < GetNumStopLocations(); j++)
    {
        if (!GetStopLocation(j, &xpt, &ypt))
            continue;
        for (i = 0; i < GetNumFires(); i++)
        {
            if (GetInout(i) != 1)
                continue;
            burn.startx = xpt;
            burn.starty = ypt;
            if (burn.Overlap(i))
            {
                EnableStopLocation(j, false);
                if (!found)
                {
                    response = IDYES;
                    printf("Do you want to suspend the simulation?? Fire Encountered Stop Location\n");
                    found = true;
                    if (response == IDYES)
                        SimRequest = SIMREQ_RESUMESUSPEND;
                }
            }
        }
    }
}


void TFarsiteInterface::CountFires()
{
    VectorBarriers = 0;
    CountInwardFires = 0;
    CountTotalFires = GetNewFires();//GetNumFires();
    for (long i = 0; i < CountTotalFires; i++)
    {
        if (GetInout(i) == 2)
            CountInwardFires++;
        if (GetInout(i) == 3)
            VectorBarriers++;
    }
    CountTotalFires -= VectorBarriers;
    /* DDE.NumFires=CountTotalFires;
     DDE.NumBarriers=VectorBarriers;
     DDE.StartStatus=(long) SIMULATE_GO;
     DDE.ResumeStatus=(long) FARSITE_GO;
     */
}


void TFarsiteInterface::WriteOutputs(int type)
{
    // if area and perimeter calculations are desired
    //	if (WStation.Number > 0)
    //		CurrentWeather();
    WriteClocks();						// update the clocks if they're visible
    if (GetNumStations() > 1 || AtmosphereGridExists() > 0)
    {
        //			CmWindGaugeControl();
    }
    else
    {
        EnvironmentData env;
        //headdata hd=GetHeaderInformation();

        burn.fe->GetLandscapeData(GetLoEast() + GetNumEast() / 2,
                                  GetLoNorth() + GetNumNorth() / 2);
        burn.fe->ld.elev = (short)
                           (GetLoElev() + (GetHiElev() - GetLoElev()) / 2);

        burn.fe->ld.slope = 0;
        burn.fe->ld.aspect = 0;
        burn.fe->ld.fuel = -1;//(short) GetFuelConversion(hd.fuels[1]);   // make sure that there is fuel there
        burn.fe->ld.cover = 0;   // make sure that there is fuel there
        burn.fe->GetFireEnvironment(burn.env, burn.SIMTIME, false);
        burn.fe->GetEnvironmentData(&env);
        //WindGauge->tws = env.tws;
        //WindGauge->winddir = env.winddir;
        //	InvalidateRect(WindGauge->HWindow, NULL, true);
        //	UpdateWindow(WindGauge->HWindow);
    }
    /*
    adata.SetData(a1 * type, sarea * type);    	// type determines the color of the output
    pdata.SetData(p1 * type, p2 * type);			// if visible timestep, then red else blue
    fdata.count = CountInwardFires;
    fdata.count = 0;
    for (int i = 0;
    	i < GetNumFires();
    	i++)  	// counting is now done in ::CountFires
    {
    	if (GetInout(i) == 2)
    		fdata.count++;
    }
    fdata.SetData(GetNumFires() * type, numspots * type, fdata.count * type);
        */
    //if (burn.SIMTIME > 0)
    //	fdata.SetData((CountTotalFires + numspots) * type, numspots * type,
    //			fdata.count * type);
    //	else
    //		fdata.SetData(CountTotalFires*type, 0, fdata.count*type);
    //	if(p2>pdata.HIY)						// keep record of largest Y-value
    //		pdata.HIY=p2;
    WriteArea();
    WritePerim();
    WriteFireData();
    WritePFData();
    //if(InquireInBurnPeriod)
    //{	a1=0.0;
    //	p1=0.0;
    //	p2=0.0;					// reset area and perimeter accumulators
    //}
    numspots = 0;

    //
    //UpdateWindow(FireChxChart->HWindow);
}


unsigned TFarsiteInterface::RunMoistureDialog(void* FarIntFace)
{
//	static_cast <TFarsiteInterface*>(FarIntFace)->MoistureDialog();

    return 1;
}






void TFarsiteInterface::StartMoistThread()
{
    //unsigned int MoistThreadID;
    //HANDLE hMoistThread;

    TerminateMoist = false;
    /*
    hMoistThread = (HANDLE) ::_beginthreadex(NULL, 0,
    							&TFarsiteInterface::RunMoistThread, this,
    							NULL, &MoistThreadID);
    CloseHandle(hMoistThread);
    */

    RunMoistThread(this);
}


unsigned  TFarsiteInterface::RunMoistThread(void* FarIntFace)
{
    static_cast <TFarsiteInterface*>(FarIntFace)->MoistThread();
    return 1;
}


void TFarsiteInterface::MoistThread()
{
    double Interval;

    //     burn.env->CalcMapFuelMoistures(maximum);
    // use the shortest interval, ten hour at present, longer ones will also be taken care of
    MxRecalculated = false;
    MoistSimTime = burn.SIMTIME + GetActualTimeStep();
    Interval = GetMoistCalcInterval(SIZECLASS_10HR, FM_INTERVAL_TIME);
    do
    {
        MoistSimTime += Interval;
        MxRecalculated = burn.env->CalcMapFuelMoistures(MoistSimTime);
        if (TerminateMoist)
            break;
    }
    while (MoistSimTime < maximum);
    TerminateMoist = true;
}


bool TFarsiteInterface::PreCalculateFuelMoistures()
{
    char Line[64] = "";
    long mo, dy, hr, mn;
    double SimTime, Interval;

    if (DistanceCheckMethod(GETVAL) == DISTCHECK_SIMLEVEL)    // if simlevel timestep and/or burnout enabled
    {
        GetTemporaryTimeStep();
    }   									  // else if fire-level timestep
    else if (burn.SIMTIME == 0.0)
        SimTime = burn.SIMTIME + GetActualTimeStep();
    // use the shortest interval, ten hour at present, longer ones will also be taken care of
    Interval = GetMoistCalcInterval(SIZECLASS_10HR, FM_INTERVAL_TIME);
    if (EnvironmentChanged(GETVAL, 0, SIZECLASS_10HR))	// if weather etc. changed
        LastFMCalcTime = 0;
    if (burn.env->CalcMapFuelMoistures(SimTime))
    {
    }
    else
    {
        if (LastFMCalcTime >= maximum)
        {
            return false;
        }
        else
        {
            ConvertSimtimeToActualTime(burn.SIMTIME, &mo, &dy, &hr, &mn, false);
            sprintf(Line, "%02ld/%02ld %02ld:%02ld", mo, dy, hr, mn);
        }
    }

    StartMoistThread();
    do
    {
        //WaitForSingleObject(hMoistEvent, 1000);
        //Sleep(300);
        ConvertSimtimeToActualTime(MoistSimTime, &mo, &dy, &hr, &mn, false);
        sprintf(Line, "%02ld/%02ld %02ld:%02ld", mo, dy, hr, mn);
        CheckMessageLoop();
    }
    while (!TerminateMoist);

    LastFMCalcTime = MoistSimTime;//maximum;

    //Sleep(1000);
    burn.env->CalcMapFuelMoistures(burn.SIMTIME + GetActualTimeStep());
    return true;
}


void TFarsiteInterface::FarsiteSimulationLoop()
// THIS FUNCTION ACTIVATES THE FARSITE LOOP AND CHECKS FOR MESSAGES BETWEEN ITERATIONS
{
    CheckSteps();						// check visual and actual timestep for changes
    systime1 = clock();
    systime2 = systime1 + 1;					// keep systime2-systime1>0.0

    //ResetEvent(hWaitEvent);

    if (RESUMEL)
    {
        LoadLastState();
        printf("Previous State Execution Loaded!\n");
    }
    printf("Simulation State:\n");
    while (burn.SIMTIME <= maximum)
    {
        //printf("SIMTIME %d of %d\n",burn.SIMTIME,maximum);
        //fflush(stdout);
        printf("%d %d\n",int(burn.SIMTIME),maximum);
        //printf("\r %d Percent done  ",(int)(burn.SIMTIME*100/maximum));
        if (LEAVEPROCESS)				// allows exit from FARSITE process control
            break;
        systime = clock();
        if (SimRequest != SIMREQ_NULL) //SIMREQ_RESUMESUSPEND)
        {
            //::SendMessage(Client->HWindow, WM_COMMAND, CM_EXECUTESUSPEND, NULL);//
            //Execute_ResumeSuspend();
            //SetEvent(hWaitEvent);
            CanModifyInputs(true);
            break ;
            //WaitForSingleObject(hFarSimEvent, INFINITE);
            //SuspendThread(hSimThread);
            //ResetEvent(hWaitEvent);
        }
        CanModifyInputs(false);
        CheckSteps();
        if (FARSITE_GO)
        {
            if (PreCalcMoistures(GETVAL))
            {
                PreCalculateFuelMoisturesNoUI();
                if (!FARSITE_GO)	// can happen if moistures are canceled
                {
                    printf("!FARSITE_GO\n");
                    FARSITE_GO = true;
                    SimRequest = SIMREQ_RESUMESUSPEND;
                    continue;
                }
            }
            else
            {
                burn.env->CalcMapFuelMoistures(burn.SIMTIME + GetActualTimeStep());
                LastFMCalcTime = burn.SIMTIME + GetActualTimeStep();
            }
            if (ProcNum < 3) 				// if all inputs ready for FARSITE model
            {
                if (NextFireAfterInterrupt == 0)
                    CountFires();
                if (ProcNum == 1)
                {
                    //if(!OutputVectsAsGrown && NextFireAfterInterrupt==0)
                    //{   	for(CurrentFire=0; CurrentFire<GetNumFires(); CurrentFire++)
                    //     {	if(GetInout(CurrentFire)==0)
                    //  			continue;
                    //  		WriteVectorOutputs();   	  // output perimeters to screen and/or vector file
                    //    	}
                    //}
                    if (FarsiteProcess1())    // do another iteration of FARSITE process
                    {
                        while (FarsiteProcess2())
                        {
                            if (!FarsiteProcessSpots())
                                break;
                        }
                    }
                    else
                        break;
                }
                else if (ProcNum == 2)
                {
                    if (FarsiteProcessSpots())
                    {
                        while (FarsiteProcess2())
                        {
                            if (!FarsiteProcessSpots())
                                break;
                        }
                    }
                }
            }
            //}
            //else
            //	break;
            //CheckMessageLoop();				  // allow messages to be received
            if (LEAVEPROCESS)				// allows exit from FARSITE process control
                break;
            //CheckSteps();						// check visual and actual timesteps for changes
            //if(FARSITE_GO)
            //{
            if (ProcNum == 3)
            {
                //if(burn.SIMTIME==0)
                //	WriteOutputs(1);
                FarsiteProcess3();  		 	// mergers between fires and increment iteration
                ProcNum = 1;
                if (!OutputVectsAsGrown && NextFireAfterInterrupt == 0)
                {
                    for (CurrentFire = 0; CurrentFire < GetNumFires(); CurrentFire++)
                    {
                        if (GetInout(CurrentFire) == 0)
                            continue;
                        WriteVectorOutputs();   	  // output perimeters to screen and/or vector file
                    }
                }
                if (NextFireAfterInterrupt == 0)
                {
                    if (burn.SIMTIME > 0.0 &&
                            PrimaryVisCount == PrimaryVisEqual)
                    {
                        if (DistanceCheckMethod(GETVAL) == DISTCHECK_SIMLEVEL)
                        {
                            if (burn.CuumTimeIncrement == 0.0)//GetActualTimeStep())
                            {
                                if (CheckPostFrontal(GETVAL))
                                {
                                    //DrawPostFrontal();
                                }
                                WriteOutputs(-1);   		  // write areas and perimeters to data structures and screen
                                PrimaryVisCount = 1;
                            }
                        }
                        else
                        {
                            WriteOutputs(-1);   			   // write areas and perimeters to data structures and screen
                            PrimaryVisCount = 1;
                        }
                    }
                    else
                    {
                        if (DistanceCheckMethod(GETVAL) == DISTCHECK_SIMLEVEL)
                        {
                            if (burn.CuumTimeIncrement == 0.0)//GetActualTimeStep())
                            {
                                //if(CheckPostFrontal(GETVAL))
                                //	DrawPostFrontal();
                                WriteOutputs(1);
                                PrimaryVisCount++;
                            }
                        }
                        else
                        {
                            WriteOutputs(1);
                            PrimaryVisCount++;
                        }
                    }
                    if (burn.SIMTIME > 0.0)
                    {
                        if (SecondaryVisCount >= SecondaryVisEqual)
                            SecondaryVisCount = 1;
                        else
                        {
                            if (DistanceCheckMethod(GETVAL) ==DISTCHECK_SIMLEVEL)
                            {
                                if (burn.CuumTimeIncrement == 0.0)//GetActualTimeStep())
                                    SecondaryVisCount++;
                            }
                            else
                                SecondaryVisCount++;
                        }
                    }
                    else
                        SecondaryVisCount = 1;
                }
            }
        }
        else
            break;  						   // check visual and actual timesteps for changes
        systime2 = clock();
        systime1 = systime;

        if (burn.SIMTIME > maximum)
        {
            FARSITE_GO = false;
            if (GetRastMake())
            {
                WriteGISLogFile(0);
            }
            if (GetVectMake())
                WriteGISLogFile(1);
            if (GetShapeMake())
                WriteGISLogFile(2);
            //EB commentsout 1 line
            //printf("  End of Run    Simulation Completed\n");
            sprintf(MBStatus, "    %s", "SIMULATION COMPLETED");
            //EB 1 line
            //WriteMessageBar(0);
            SimRequest = SIMREQ_NULL ;
            SetXORArrows(false);
            SIM_SUSPENDED = true;
            StepThrough = false;
            SIM_COMPLETED = true;
            //SetEvent(hWaitEvent);
            CanModifyInputs(true);
            //SuspendThread(hSimThread);
            //WaitForSingleObject(hFarSimEvent, INFINITE);
            //ResetEvent(hWaitEvent);
            CheckSteps();
        }
    }
    //printf("End loop\n");
    ProcessSimRequest();

    // write raster files at end of timestep...
    if (GetRastMake())
    {
        burn.rast.SelectMemOutputs(GetRastFormat());	// write to raster file
    }

    //std::cerr << "In FarsiteBurnLoop()" << burn.SIMTIME << std::endl ;
    if (burn.SIMTIME > maximum)
    {
        FARSITE_GO = false;
        if (GetRastMake())
            WriteGISLogFile(0);
        if (GetVectMake())
            WriteGISLogFile(1);
        if (GetShapeMake())
            WriteGISLogFile(2);
        //printf("End of Run Simulation Completed\n");
    }
    //printf("End Farsite Simulation Loop void\n");
}


void TFarsiteInterface::ProcessSimRequest()
{
    switch (SimRequest)
    {
    case SIMREQ_NULL:
        break;
    case SIMREQ_INITIATETERMINATE:
        //FARSITE_GO=true;
        SimRequest = SIMREQ_NULL;
        ResetFarsite();
        Execute_InitiateTerminate();  	// doesn't work with multithreading ?? who knows why
        //::SendMessage(Client->HWindow, WM_COMMAND, CM_EXECUTETERMINATE, NULL);
        break;
    case SIMREQ_STARTRESTART:
        FARSITE_GO = true;
        SimRequest = SIMREQ_NULL;
        Execute_StartRestart();
        //::SendMessage(Client->HWindow, WM_COMMAND, CM_EXECUTERESTART, NULL);
        break;
    case SIMREQ_RESUMESUSPEND:
        //FARSITE_GO=true;
        SimRequest=SIMREQ_NULL;
        Execute_ResumeSuspend();
        break;
    case SIMREQ_RESET:
        FARSITE_GO = true;
        SimRequest = SIMREQ_NULL;
        Execute_Reset();
        //::SendMessage(Client->HWindow, WM_COMMAND, CM_EXECUTERESET, NULL);
        break;
    }
    //SIM_SUSPENDED = false;
    //if (hWaitEvent != NULL)
    //SetEvent(hWaitEvent);
    CanModifyInputs(true);
}


bool TFarsiteInterface::WriteClocks()
{
    ElTimeConvert();
    CurTimeConvert();
    return true;
}


void TFarsiteInterface::ResetThreads()
{
    FreeFarsiteEvents(EVENT_BURN);
    FreeFarsiteEvents(EVENT_MOIST);
    FreeFarsiteEvents(EVENT_BURNUP);
    FreeFarsiteEvents(EVENT_INTEG);
    FreeFarsiteEvents(EVENT_CROSS);
    FreeFarsiteEvents(EVENT_BURN_THREAD);
    FreeFarsiteEvents(EVENT_MOIST_THREAD);
    FreeFarsiteEvents(EVENT_BURNUP_THREAD);
    FreeFarsiteEvents(EVENT_INTEG_THREAD);
    FreeFarsiteEvents(EVENT_CROSS_THREAD);
    AllocFarsiteEvents(EVENT_BURN, GetMaxThreads(), "SyncEventBurn", false,
                       false);
    AllocFarsiteEvents(EVENT_MOIST, GetMaxThreads(), "SyncEventMoist", false,
                       false);
    AllocFarsiteEvents(EVENT_BURNUP, GetMaxThreads(), "SyncEventBurnup",
                       false, false);
    AllocFarsiteEvents(EVENT_INTEG, GetMaxThreads(), "SyncEventInteg", false,
                       false);
    AllocFarsiteEvents(EVENT_CROSS, GetMaxThreads(), "SyncEventCross", false,
                       false);
    AllocFarsiteEvents(EVENT_BURN_THREAD, GetMaxThreads(),
                       "SyncEventBurnThread", true, false);
    AllocFarsiteEvents(EVENT_MOIST_THREAD, GetMaxThreads(),
                       "SyncEventMoistThread", true, false);
    AllocFarsiteEvents(EVENT_BURNUP_THREAD, GetMaxThreads(),
                       "SyncEventBurnupThread", true, false);
    AllocFarsiteEvents(EVENT_INTEG_THREAD, GetMaxThreads(),
                       "SyncEventIntegThread", true, false);
    AllocFarsiteEvents(EVENT_CROSS_THREAD, GetMaxThreads(),
                       "SyncEventCrossThread", true, false);
    NumSimThreads = GetMaxThreads();
}


bool TFarsiteInterface::CheckThreadNumbers()
{
    if (NumSimThreads != GetMaxThreads())
    {
        burn.post.ResetAllThreads();
        burn.ResetAllPerimThreads();
        burn.env->ResetAllThreads();
        burn.CloseCrossThreads();
        //Sleep(100);
        //add EB
        ResetThreads();
        //  ::SendMessage(Client->HWindow, WM_COMMAND, CM_RESETTHREADS, NULL);
    }

    return true;
}


void TFarsiteInterface::CheckSteps()
{
    if (NextFireAfterInterrupt == 0)
    {
        burn.rastmake = GetRastMake();					// update decision to make raster file
        SetNumFires(GetNewFires());				// newfires set from mouse or from file
        long oldequal = PrimaryVisEqual; 			// recalc PrimaryVisEqual from new visual and actual ts
        if (oldequal !=
                (PrimaryVisEqual = (long)
                                   (GetVisibleTimeStep() / GetActualTimeStep())))
        {
            PrimaryVisCount = 0;					// reset visible interval to 0;
            SecondaryVisCount = 0;  				  // reset secondary visible interval to 0;
        }
        if (GetSecondaryVisibleTimeStep() != -1)
            SecondaryVisEqual = (long)
                                (GetSecondaryVisibleTimeStep() / GetActualTimeStep());
        else
            SecondaryVisCount = SecondaryVisEqual - 1;
        //if (!VISDIM)
        maximum = (long)GetSimulationDuration();//numaximum;
        //else						  			 // maximum time is determined by visual time steps
        //	maximum = GetSimulationDuration() * GetVisibleTimeStep();
        if (DistanceCheckMethod(GETVAL) == DISTCHECK_FIRELEVEL)
            OutputVectsAsGrown = ShowFiresAsGrown(GETVAL);
        else
            OutputVectsAsGrown = 0;
        CheckThreadNumbers();
    }
    // DDE.StartStatus=(long) SIMULATE_GO;
    // DDE.ResumeStatus=(long) FARSITE_GO;
    SIM_COMPLETED = false;
}


void TFarsiteInterface::WriteVectorOutputs()
{
    bool WriteVect = false;


    if(burn.SIMTIME==0.0 || PrimaryVisCount==PrimaryVisEqual)   // if visible time step
    {
        if(DistanceCheckMethod(GETVAL)==DISTCHECK_SIMLEVEL)
        {
            if(burn.CuumTimeIncrement!=0.0)
                return;
        }
    }
    if (GetVectMake())
    {
        if (!GetVectVisOnly())						  // if write all perimeters
            WriteVect = true;
        else if (burn.SIMTIME == 0.0 || PrimaryVisCount == PrimaryVisEqual)
            WriteVect = true;
        if (ExcludeBarriersFromVectorFiles(GETVAL) &&
                GetInout(CurrentFire) == 3)
            WriteVect = false;
        if (WriteVect)
            vect.VectData(CurrentFire, burn.SIMTIME);
    }
    WriteVect = false;
    if (GetShapeMake())    // if shapefileoutput
    {
        if (!shape.VisOnly)
            WriteVect = true;
        else if (burn.SIMTIME == 0 || PrimaryVisCount == PrimaryVisEqual)
            WriteVect = true;
        if (ExcludeBarriersFromVectorFiles(GETVAL) &&
                GetInout(CurrentFire) == 3)
            WriteVect = false;
        if (WriteVect)
            shape.ShapeData(CurrentFire, burn.SIMTIME);	// write fires
    }
}

/*
bool TFarsiteInterface::AllocWindStats(long numstats)
{// allocate number of windstat cells
	if((WStat=(WindStats*) calloc(numstats, sizeof(WindStats))==NULL)
	{	NumWindStats=0;

	 	return false;
	 }
	 NumWindStats=numstats;

	return true;
}


bool TFarsiteInterface::FreeWindStats()
{// free windstat cells
	if(WStat)
	 	free(WStat);

	 WStat=0;
}
*/




void TFarsiteInterface::ElTimeConvert()
{
    double TEMPTIME = burn.SIMTIME - GetActualTimeStep();
    double Hr, Dy;
    long dy = 0, hr = 0, min;
    char colon[] = ":";

    if (TEMPTIME >= 1440.0)
    {
        Dy = TEMPTIME / 1440.0;
        dy = (long) Dy;
        TEMPTIME -= 1440 * dy;
    }
    if (TEMPTIME >= 60.0)
    {
        Hr = TEMPTIME / 60.0;
        hr = (long) Hr;
        TEMPTIME -= 60 * hr;
    }
    min = (long)TEMPTIME;//*60;
    //TimeKeepElapsed.SetData(dy, hr, min, 0);
    sprintf(ElTime, "%02d %02d%s%02d", (int)dy, (int)hr, colon, (int)min);
}


void TFarsiteInterface::CurTimeConvert()
{
    double TEMPTIME = burn.SIMTIME - GetActualTimeStep();
    double Hr, Dy;
    long mo = GetStartMonth(), dy = 0, hr = 0, min;
    char colon[] = ":";
    char slash[] = "/";

    if (TEMPTIME >= 1440.0)
    {
        Dy = TEMPTIME / 1440.0;
        dy = (long) Dy;
        TEMPTIME -= (1440 * dy);					 // truncate to nearest day
    }
    if (TEMPTIME >= 60.0)
    {
        Hr = TEMPTIME / 60.0;
        hr = (long) Hr;
        TEMPTIME -= (60 * hr);					 // truncate to nearesat hour
    }
    min = (long) TEMPTIME;  						// minutes left over
    min += GetStartMin();
    if (min >= 60)
    {
        min -= 60;
        hr++;
    }
    hr += (GetStartHour() / 100);
    if (hr >= 24)
    {
        hr -= 24;
        dy++;
    }
    dy += GetStartDay();
    long days, oldmo;
    do
    {
        oldmo = mo;
        switch (mo)
        {
        case 1:
            days = 31;
            break;			 // days in each month, ignoring leap year
        case 2:
            days = 28;
            break;
        case 3:
            days = 31;
            break;
        case 4:
            days = 30;
            break;
        case 5:
            days = 31;
            break;
        case 6:
            days = 30;
            break;
        case 7:
            days = 31;
            break;
        case 8:
            days = 31;
            break;
        case 9:
            days = 30;
            break;
        case 10:
            days = 31;
            break;
        case 11:
            days = 30;
            break;
        case 12:
            days = 31;
            break;
        }
        if (dy > days)
        {
            dy -= days;
            mo++;
            if (mo > 12)
                mo -= 12;
        }
    }
    while (mo != oldmo);						// allows startup of current clock at any time, will find cur month
    //TimeKeepCurrent.SetData(dy, hr, min, mo);
    sprintf(CurTime, "%02ld%s%02ld  %02ld%s%02ld", mo, slash, dy, hr, colon,
            min);
}


bool TFarsiteInterface::WriteArea()
{
    return true;
}
bool TFarsiteInterface::WritePerim()
{
    return true;
}
bool TFarsiteInterface::WriteFireData()
{
    return true;
}
bool TFarsiteInterface::WritePFData()
{
    return true;
}

bool TFarsiteInterface::PreCalculateFuelMoisturesNoUI()
{
    //char Line[64] = "";
    long mo, dy, hr, mn;
    double SimTime, Interval;
    fflush(stdin);

    SimTime = burn.SIMTIME + GetActualTimeStep();
    // use the shortest interval, ten hour at present, longer ones will also be taken care of
    Interval = GetMoistCalcInterval(SIZECLASS_10HR, FM_INTERVAL_TIME);
    if (EnvironmentChanged(GETVAL, 0, SIZECLASS_10HR))	// if weather etc. changed
        LastFMCalcTime = 0;
    //if (burn.env->CalcMapFuelMoistures(SimTime))
    if (true)
    {
    }
    else
    {
        if (LastFMCalcTime >= maximum)
        {
            return false;
        }
        else
        {
            ConvertSimtimeToActualTime(burn.SIMTIME, &mo, &dy, &hr, &mn, false);
        }
    }
    StartMoistThread();
    do
    {
        //Sleep(300);
        ConvertSimtimeToActualTime(MoistSimTime, &mo, &dy, &hr, &mn, false);
    }
    while (!TerminateMoist);

    LastFMCalcTime = MoistSimTime;
    //Sleep(1000);
    burn.env->CalcMapFuelMoistures(burn.SIMTIME + GetActualTimeStep());
    return true;
}



void TFarsiteInterface::SaveSettingsToFile(const char *fname)
{
    FILE* outfile;
    char Text[8]="";
    char String[256]="";
    char AclFile[256]="";
    long i;
    char CurrentDirectory[256] = "";

    outfile=fopen(fname, "w");
    if(outfile==NULL)
    {
        printf("Unable to open Settings.txt file!\n");
        return;
    }


    getcwd(CurrentDirectory, 255);
//----------------------------------
//	 INPUTS
//----------------------------------
    fprintf(outfile, "Inputs:\n");
    if(strlen(Inputs.MBLand)>0)
        fprintf(outfile, "   %s %s\n", "Landscape:", strupr(Inputs.MBLand));
    for(i=0; i<5; i++)
    {
        if(strcmp(Inputs.MBWtr[i], "Load Weather"))
            fprintf(outfile, "   %s %s\n", "Weather:", strupr(Inputs.MBWtr[i]));
    }
    for(i=0; i<5; i++)
    {
        if(strcmp(Inputs.MBWnd[i], "Load Winds"))
            fprintf(outfile, "   %s %s\n", "Winds:", strupr(Inputs.MBWnd[i]));
    }
    if(strlen(Inputs.MBAdj)>0)
        fprintf(outfile, "   %s %s\n", "Adjustments:", strupr(Inputs.MBAdj));
    if(strlen(Inputs.MBMois)>0)
        fprintf(outfile, "   %s %s\n", "Fuel Moistures:", strupr(Inputs.MBMois));
    if(strlen(Inputs.MBConv)>0)
        fprintf(outfile, "   %s %s\n", "Conversions:", strupr(Inputs.MBConv));
    else
        fprintf(outfile, "   %s %s\n", "Conversions:", "NONE");

    if(strlen(Inputs.MBFuelMods)>0)
        fprintf(outfile, "   %s %s\n", "Custom Fuel Models:", strupr(Inputs.MBFuelMods));
    else
        fprintf(outfile, "   %s %s\n", "Custom Fuel Models:", "NONE");

    if(strlen(Inputs.MBCwd)>0)
        fprintf(outfile, "   %s %s\n", "Coarse Woody Fuels:", strupr(Inputs.MBCwd));
    else
        fprintf(outfile, "   %s %s\n", "Coarse Woody Fuels:", "NONE");

    if(strlen(Inputs.MBBpd)>0)
        fprintf(outfile, "   %s %s\n", "Burning Period:", strupr(Inputs.MBBpd));
    else
        fprintf(outfile, "   %s %s\n", "Burning Period:", "NONE");

    fprintf(outfile, "   %s %s\n", "Project File:", Inputs.ProjectFile);
    fprintf(outfile, "   %s %s\n", "Bookmark File:", Inputs.BookmarkFile);

//----------------------------------
//	 OUTPUTS
//----------------------------------
    fprintf(outfile, "Outputs:\n");

    char RevCopy[256]="", TempCopy[256]="";
    char pathdiv[]="\\";              // search for path
    long length;

    if(GetShapeMake())
    {
        long m, n, o;
        memset(String, 0x0, sizeof(String));
        strcpy(RevCopy, GetShapeFileChx(&m, &n, &o));	    // copy file path string
        strrev(RevCopy);			    // reverse file path string
        length=strcspn(RevCopy, pathdiv); // compute length without path
        strncpy(TempCopy, RevCopy, length);  // copy length w/o path
        strrev(TempCopy);
        fprintf(outfile, "   %s: %s\n", "Shapefile", TempCopy);
    }
    if(GetVectMake())
    {
        memset(String, 0x0, sizeof(String));
        strcpy(RevCopy, GetVectorFileName());	    // copy file path string
        strrev(RevCopy);			    // reverse file path string
        length=strcspn(RevCopy, pathdiv); // compute length without path
        strncpy(TempCopy, RevCopy, length);  // copy length w/o path
        strrev(TempCopy);
        fprintf(outfile, "   %s: %s\n", "Vector file", TempCopy);
    }

    if(GetRastMake())
    {
        for(i=20; i<27; i++)
        {
            memset(String, 0x0, sizeof(String));
            if(GetFileOutputOptions(i))
            {
                strcpy(String, "Raster File: ");
                strcpy(RevCopy, GetRasterFileName(i));	    // copy file path string
                strrev(RevCopy);			    // reverse file path string
                length=strcspn(RevCopy, pathdiv); // compute length without path
                strncpy(TempCopy, RevCopy, length);  // copy length w/o path
                strrev(TempCopy);
                strcat(String, TempCopy);
                fprintf(outfile, "   %s: %s\n", "Raster file", String);
            }
        }
    }
    if(!AccessDisplayUnits(GETVAL))
        fprintf(outfile, "   %s\n", "Display Units: METRIC");
    else
        fprintf(outfile, "   %s\n", "Display Units: ENGLISH");
    if(!AccessOutputUnits(GETVAL))
        fprintf(outfile, "   %s\n", "Output File Units: METRIC");
    else
        fprintf(outfile, "   %s\n", "Output File Units: ENGLISH");
    switch(OutputFireParam(GETVAL))
    {
    case 0:
        sprintf(String, "Perimeter Color: Fireline Intensity");
        break;
    case 1:
        sprintf(String, "Perimeter Color: Spread Rate");
        break;
    case 2:
        sprintf(String, "Perimeter Color: Flame Length");
        break;
    case 3:
        sprintf(String, "Perimeter Color: Heat/Area");
        break;
    case 4:
        sprintf(String, "Perimeter Color: White");
        break;
    case 5:
        sprintf(String, "Perimeter Color: Reaction Intensity");
        break;
    }

//----------------------------------
//	 MODEL
//----------------------------------
    fprintf(outfile, "Model:\n");

    fprintf(outfile, "   %s %3.1lf\n", "Parameters: TimeStep", GetActualTimeStep());
    fprintf(outfile, "   %s %3.1lf, %3.1lf\n", "Parameters: Visibles", GetVisibleTimeStep(), GetSecondaryVisibleTimeStep());
    fprintf(outfile, "   %s %3.1lf\n", "Parameters: Perim Res", GetPerimRes());
    fprintf(outfile, "   %s %3.1lf\n", "Parameters: Dist Res", GetDistRes());

    if(EnableCrowning(GETVAL))
        fprintf(outfile, "   %s: %s\n", "Options: Crown Fire", "ENABLED");
    else
        fprintf(outfile, "   %s: %s\n", "Options: Crown Fire", "DISABLED");
    if(EnableSpotting(GETVAL))
        fprintf(outfile, "   %s: %s\n", "Options: Spotting", "ENABLED");
    else
        fprintf(outfile, "   %s: %s\n", "Options: Spotting", "DISABLED");
    if(EnableSpotFireGrowth(GETVAL))
        fprintf(outfile, "   %s: %s\n", "Options: Spot Growth", "ENABLED");
    else
        fprintf(outfile, "   %s: %s\n", "Options: Spot Growth", "DISABLED");
    //fprintf(outfile, "   %s: %4.2lf %\n", "Options: Ignition Frequency", (double) PercentIgnition(GETVAL));
    fprintf(outfile, "   %s: %ld mins\n", "Options: Ignition Delay", (long) IgnitionDelay(GETVAL));

    if(DistanceCheckMethod(GETVAL)==0)
        fprintf(outfile, "   %s: %s\n", "Options:", "Sim Level Dist. Check");
    else
        fprintf(outfile, "   %s: %s\n", "Options", "Fire Level Dist. Check");

    if(AccelerationON())
        fprintf(outfile, "   %s: %s\n", "Acceleration", "ON");
    else
        fprintf(outfile, "   %s: %s\n", "Acceleration", "OFF");

    if(strlen(AclFile)>0)
        fprintf(outfile, "   %s: %s\n", "Acceleration", AclFile);
    else
    {
        fprintf(outfile, "   %s: %s\n", "Acceleration", "DEFAULTS");
    }
    if(CheckPostFrontal(GETVAL))
    {
        fprintf(outfile, "   %s: %s\n", "Post Frontal", "ON");
    }
    else
    {
        fprintf(outfile, "   %s: %s\n", "Post Frontal", "OFF");
    }
    if(PreCalcMoistures(GETVAL))
        fprintf(outfile, "   %s: %s\n", "Dead Fuel Moisture", "PRE-CALCULATED");
    else
        fprintf(outfile, "   %s: %s\n", "Dead Fuel Moisture", "AS-NEEDED");

//----------------------------------
//	 SIMULATE
//----------------------------------
    fprintf(outfile, "Simulate:\n");

    fprintf(outfile, "   %s %ld%s%ld\n", "Duration: Conditioning (Mo/Day):",
            GetConditMonth(), "/", GetConditDay());

    fprintf(outfile, "   %s %ld%s%ld %ld:%ld\n", "Duration: Starting (Mo/Day Hour:Min):",
            GetStartMonth(), "/", GetStartDay(), GetStartHour()/100, GetStartMin());

    fprintf(outfile, "   %s %ld%s%ld %ld:%ld\n", "Duration: Ending (Mo/Day Hour:Min):",
            GetEndMonth(), "/", GetEndDay(), GetEndHour()/100, GetEndMin());

    if(DurationResetAtRestart(GETVAL))
        strcpy(Text, "TRUE");
    else
        strcpy(Text, "FALSE");
    fprintf(outfile, "   %s%s%s\n", "Options: ", "Duration Reset: ", Text);

    if(IgnitionResetAtRestart(GETVAL))
        strcpy(Text, "TRUE");
    else
        strcpy(Text, "FALSE");
    fprintf(outfile, "   %s%s%s\n", "Options: ", "Restore Ignitions: ", Text);

    if(RotateSensitiveIgnitions(GETVAL))
        strcpy(Text, "TRUE");
    else
        strcpy(Text, "FALSE");
    fprintf(outfile, "   %s%s%s\n", "Options: ", "Rotation Sensitive Ignitions: ", Text);

    if(ShowFiresAsGrown(GETVAL))
        strcpy(Text, "TRUE");
    else
        strcpy(Text, "FALSE");
    fprintf(outfile, "   %s%s%s\n", "Options: ", "Show Fires as Grown: ", Text);

    if(AdjustIgnitionSpreadRates(GETVAL))
        strcpy(Text, "TRUE");
    else
        strcpy(Text, "FALSE");
    fprintf(outfile, "   %s%s%s\n", "Options: ", "Ignition Spread Rates: ", Text);

    if(PreserveInactiveEnclaves(GETVAL))
        strcpy(Text, "TRUE");
    else
        strcpy(Text, "FALSE");
    fprintf(outfile, "   %s%s%s\n", "Options: ", "Preserve Inactive Enclaves: ", Text);
    fprintf(outfile, "   %s%s%02ld\n", "Options: ", "Simulation Threads: ", GetMaxThreads());

//----------------------------------
//	 ATTACK
//----------------------------------
    fprintf(outfile, "Attack:\n");
    //if(GetNumCrews()>0)
    //		 fprintf(outfile, "   %s: %s\n", "Ground Resources", CrewFile);
    //	else
    //		 fprintf(outfile, "   %s\n", "Ground Resources: NONE");
    //if(GetNumAirCraft()>0)
    //		 fprintf(outfile, "   %s: %s\n", "Air Resources", AirFile);
    //else
    //		fprintf(outfile, "   %s\n", "Air Resources: NONE");

//----------------------------------
//	 VIEW
//----------------------------------
    fprintf(outfile, "View:\n");

    if(GetNumViewEast()!=GetNumEast() || GetNumViewNorth()!=GetNumNorth())
        fprintf(outfile, "   %s %s\n", "Viewport:", "ZOOMED");
    else
        fprintf(outfile, "   %s %s\n", "Viewport:", "MAXIMIZED");
    fclose(outfile);
} /*End Function SaveSettingsToFile()*/

