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
#ifndef FARSITE_MAIN
#define FARSITE_MAIN
#define UNIX 1
#ifdef UNIX
//#include <dir.h>
//#include <io.h>
#else 
#include <unistd.h>
#include <string.h>
#endif
#include "fsxwignt.h"		// ignition input from files
#//include "fswther.h"
#//include "fswwtype.h"		// weather and wind typing
#include "fsxwinpt.h"
#include "fsxwattk.h"		// attack functions and structures
#include "fsairatk.h"
#include "fsxwshap.h"		// shapefile output
#include "rand2.h"
#include "fsxsync.h"

/**
 * \brief The main FARSITE class.  
 *
 * This class organizes all the information required
 * to execute a simulation.  It also contains all the initialization code
 * and the main simulation loop.  In the days of the GUI, this was the 
 * main application, so it also contained all the logic to manage the GUI
 * using the Borland UI toolkit.  Finally, this file contains the thread
 * management code.
 *
 * This class was modified for batch mode by including 
 * functions which read a settings file and set each simulation variable 
 * appropriately.  Threading and GUI logic has been either removed or commented
 * out.  
 *
 * The main problem with adapting this code to the API is that it does 
 * try and contain all of the required information.  The "Inputs" object
 * is a private member of this class.  An external class in the API, like 
 * FarsiteConfig, may not be able to maintain it's own instance of a 
 * FarInputs class and have it apply here.  If not, all of the logic 
 * contained herein must be exported to the API class.  Alternatively, 
 * it may be possible to either overload the assignment operator for 
 * FarInputs or modify this class to allow the client to set the FarInputs
 * object they desire.
 */
class TFarsiteInterface //: public TApplication
{
	bool LEAVEPROCESS, SIMULATE_GO,   IN_BURNPERIOD;	// using input data instead of files;
	char CrewFileName[256];
	char HelpFile[256];
	char BookMarkArg[256];
	char FarsiteTitle[256];
	char MessageText[200];
	char IgFileName[256];
	char FarsiteDirectory[256];
	char MBStatus[64], MBMode[64];
	double a1, p1, p2, sarea, firesizeh, firesizes;			// for area and perimeter calculations
	double smolder, flaming;
	double oldactual, oldvisual;
	long PrimaryVisEqual, PrimaryVisCount;
	long SecondaryVisEqual, SecondaryVisCount;				// visible timestep counters
	long CurrentFire;
	long NextFireAfterInterrupt, OldFireAfterInterrupt;
	short ProcNum;
	long maximum, numspots, idum;					// maximum time in simulation, updated with global "numaximum"
	long VisPerimSize, OutputVectsAsGrown;
	long CountInwardFires, CountTotalFires, VectorBarriers;		// just for display purposes
	bool ReviseGroundAtkON;
	bool ReviseAirAtkON;
	double LastFMCalcTime;
	int TextFilterIndex, RasterFilterIndex, VectorFilterIndex;
	long NumSimThreads;
	unsigned long systime1, systime2, systime;
	int CM_SWITCHMESSAGE;
	int RESUME;
	int RESUMEL;
	int RESUMES;
	char *StateFileNameO;
	char *StateFileNameI; 

	Attack Atk;
	AirAttack AAtk;
	ShapeFileUtils shape;
	GroupAirAttack* gaat;
	FILE* CurrentFile;
	
	//BookMarkData bookmark;
	struct WindGridData
	{
		double X;
		double Y;
		double WS;						// stores wind speed data used for wind maps
		double WD;						// stores wind direction data used for wind maps
	};
	WindGridData* WData;
	long NumWData, NumEDim, NumNDim;

	struct
	{
		double tws;
		double winddir;
	} WStat[5];

	Vectorize vect;						// object that writes vector files

	bool OpenLandscapeFile();
	void MakeTheProject(bool Changes); 		// core function for project make
	void InitializeStrings();   				 // memset and initialization of strings
	void InitializeLandscapeWindow();			// sets up and creates visible Landscape window
	void CheckSteps();							 // check for changes in actual or visual timesteps

	void CurrentWeather();					// get readings from weather stations
	bool WriteArea();						// write the area data and graph
	bool WritePerim();						// write the perimeter data and graph
	bool WriteFireData();			  		// write fire data to window
	bool WritePFData();						// write post frontal fire data to window
	bool WriteClocks();						// update the clocks if visible
	//	void DrawWindArrows();					// draws wind direction/strength arrows on map
	void WriteVectorOutputs();				// output fire perimeters to vector file and monitor
	void OpenFuelMods();					// opens fuel model file
	bool FarsiteProcess1();					// FARSITE process split to allow message processing
	bool FarsiteProcessSpots();				// FARSITE process split to allow message processing
	bool FarsiteProcess2();					// FARSITE process split to allow message processing
	void FarsiteProcess3();					// FARSITE process split to allow message processing
	void WriteOutputs(int Type);			   	// writes area and perimeter data to arrays and draws graphs
	void CountFires();  						 // just counts fires for display
	void ResetFarsite();					// reset all critical farsite data structures
	void ResetDuration();					// resets duration dialog box for simulation
	void ElTimeConvert(); 					// convert time to Elapsed time
	void CurTimeConvert();					// convert time to Current Time
	void WriteGISLogFile(long LogType);		// write log file for GIS outputs
	void SaveIgnitions();					// for internal use with start/restart
	void LoadIgnitions();
	void ClearAllFires();
	void UpdateFuelMoistures();
	void CancelMoistureCalculation();
	void WaitRequest();
	unsigned RunWaitThread(void* FarIntFace);
	bool PreCalculateFuelMoistures();			// do all fuel moisture calculations ahead of the simulation
	unsigned RunMoistureDialog(void* FarIntFace);
	bool CheckThreadNumbers();
	void ResetThreads();
	void StartMoistureDialog();
	void MoistureDialog();
	void KillMoistureDialog();
	long InsertSpotFire(double xpt, double ypt);
	void EvClose();
	bool CanClose();

protected:
	void FlatOpenProject();
	void SaveVisPerimeter();					 // save visible fire perimeters
	void FlatSimulateInitiateTerminate();
	void CheckStopLocations();
	void FarsiteSimulationLoop();				// calls FARSITE processes and CheckMessageLoop()
	void SaveSettingsToFile(const char *);         //Added this function JAS!
	void SaveLastState();
	void LoadLastState();

     //-------------------------------------------------------------------------
	// multithreaded additions 10/1/1999
	//-------------------------------------------------------------------------

	//HANDLE hSimThread;
	//HANDLE hWaitThread;
	//HANDLE hFarSimEvent;	// simulation resume and suspend
	//HANDLE hWaitEvent;
	//HANDLE hMoistThread;
	//     HANDLE hMoistEvent;//, hMoistUpdateEvent;	
	IgnitionFile Ignition;					// Ignition file inputs
	FarInputs Inputs;
	Burn burn;   								  // declare Burn object, contains burn,area,intersections,embers
	long SimRequest;
	bool SIM_SUSPENDED, SIM_COMPLETED, FARSITE_GO, TerminateWait, WaitInProgress,
		TerminateMoist, MxRecalculated, StepThrough;
	double MoistSimTime;

	void StartWinThread();
	void StartSimThread();
	void EndSimThread();
	unsigned RunSimThread(void*);

	void Execute_InitiateTerminate();
    /**
     * \brief Restarts or initializes the FARSITE simulation.  
     * 
     * The simulation then begins executing. 
     */ 
	void Execute_StartRestart();
	void Execute_ResumeSuspend();
	void Execute_Reset();
	void ProcessSimRequest();

	void StartMoistThread();
	unsigned RunMoistThread(void*);
	void MoistThread();

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------

	//	DECLARE_RESPONSE_TABLE(TFarsiteInterface);

public:
	char FMMFileName[256];
	char FMMDescription[512];

	TFarsiteInterface(const char* name, const char* directory, 
					const char* BookMark);
	~TFarsiteInterface();
	void InitMainWindow();
	void SetModelParams();
	bool PreCalculateFuelMoisturesNoUI();
	void loadValues();
	void lIgn();
	void sIgn();
	void Chain();
	void SetResume(int,int, char*,char*);
    /**
     * \brief Reads in the simulation settings from a file.  
     *
     * This is used in conjunction with the batch-mode FARSITE simulation...
     */  
	void SetInputsFromFile(char* settingsfilename);
    /**
     * \brief Used by SetInputsFromFile(), this function performs the actual 
     * setting of simulation parameters specified in the file.  
     * 
     * This is a good place 
     * to look when trying to find out what function to call or variable to 
     * set when attempting to affect specific parameters.  
     *
     * \todo Units are not set.
     */
	void processNameValue(char* name, char* value);
	
	/**
	 * \brief Configures the FARSITE simulator for initial simulation loop.
	 * 
	 * This method assumes that all has been initialized correctly.
	 */
	void Initiate() ; 
	
	/**
	 * \brief Dumps all configuration info from the FARSITE simulator.
	 * 
	 * After using this method, the user is required to load another 
	 * configuration into memory before simulating.
	 */
	void Terminate() ; 
	

};


#endif // FARSITE_MAIN
