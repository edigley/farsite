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
#include "fsglbvar.h"
//#include "fswwtype.h"
//#include "fsxwdvct.h"
//#include "fsxwfmd.rh"
//#include "shapefil.h"


#ifndef FARSITE_INPUT_DATA
#define FARSITE_INPUT_DATA

/*
struct TTransCanopyData
{
	CanopyData Dat;
	TTransCanopyData();
};


struct FMDStruct
{
	int modmet, modeng, otpmet, otpeng;
	int fmlo, fmmed, fmhi;
};

struct TTransFMD
{
	// 	int modmet, modeng, otpmet, otpeng;
	FMDStruct fm;
	TTransFMD();
};


struct FileChanged
{
	bool LCP;
	bool WTR[5];
	bool WND[5];
	bool ADJ;
	bool CNV;
	bool FMS;
	bool FMD;
	bool BPD;
	bool CWD;
	FileChanged()
	{
		ResetChanges();
	};
	void ResetChanges()
	{
		LCP = WTR[0] = WTR[1] = WTR[2] = WTR[3] = WTR[4] = false;
		WND[0] = WND[1] = WND[2] = WND[3] = WND[4] = ADJ = FMS = FMD = BPD = CWD = false;
	};
};
*/

struct FarInputs
{
  private:
  static const int FILE_LEN = 256 ; 

  public:
	char BookmarkFile[FILE_LEN];
	char LandscapeFile[FILE_LEN];
	char ProjectFile[FILE_LEN];
	char AdjustmentFile[FILE_LEN];
	char FuelMoistureFile[FILE_LEN];
	char ConvertFile[FILE_LEN];
	char FuelModelFile[FILE_LEN];
	char WeatherFile[5][FILE_LEN];
	char WindFile[5][FILE_LEN];
	char BurnPeriod[FILE_LEN];
	char CoarseWoody[FILE_LEN];
	char MBLand[64], MBWtr[5][64], MBWnd[5][64];
	char MBAdj[64], MBMois[64], MBConv[64], MBFuelMods[64], MBBpd[64],
		MBCwd[64];
	bool LandID, WtrID, WndID, AdjID, FuelMoisID, BpdID, CwdID;
	bool SVGA_ON;
 	char pathdiv[1];
	bool inited;
	bool m_shapeFileInited;
	bool m_gridEastNorthSet;
	bool m_gridRowSet;
	char m_GridRow[256];
	char m_eastNorth[256];
	int  m_BurnPeriodIsHere;
	int  version;
	int  m_dcount;
	long GridInterval;
	//FileChanged Changes;
	//TWindow *Client;
	FILE* CurrentFile;

	FarInputs();
	void ResetData();
	void InitializeLandscapeFile();
	bool LoadWeatherData(long FileNum);
	bool LoadWindData(long FileNum);
	bool LoadAdjustmentFile();
	bool LoadFuelMoistureFile();
	bool LoadCustomModelFile();
	bool LoadConversionFile();
	bool LoadBurnPeriodFile();
	bool LoadCoarseWoodyFile(char* ErrMsg);
	bool TestProjectFileVersion();
	bool OpenVersion1ProjectFile();
	bool OpenVersion2ProjectFile();
	bool OpenVersion4ProjectFile();
	void MakeProjectFile();
	void WriteWeatherWindProject();
	bool TestForAtmWeather();
	bool TestForAtmWinds();

 	//old load project file replacement functions	
	bool intializeDir(char* Name);
	bool setWeatherFile(int WeatherFileNum, const char* weatherFileName);
	bool setWindFile(int WindFileNum, const char* WindFileName);
	bool setLandscapeFile(const char* value);
	void initWindAndWeather();
	bool loadUpAdjustmentsFile(const char* adjFileName);
	bool loadUpMoistureFile(const char* moistureFile);
	bool loadUpFuelModelFile(const char* moistureFile);
	bool loadUpConversionFile(const char* ConvertFile);
	bool loadUpCoarseWoodyFile(const char* CoarseWoody);
	bool loadUpBurnPeriodFile(const char* FileName1);
	void saveGridEastNorth(char* eastNorth);
	void saveGridRow(char* GridRow);
	bool loadUpEastNorth(char* eastNorth, char* GridRow);
	bool loadUpLandscapeTheme(const char* FileName1);
	void initShapeFileLoad();
	bool loadUpShapeFile(char* shapefile);
	bool loadUpStoredVector(char* storedVectorFile);
	bool loadUpSetDirection(char* value,char direction);
	bool loadUpCanopyCheck(char* value);


	bool SaveWtr(long Station);
	bool SaveWnd(long Station);
	bool SaveAdj();
	bool SaveFms();
	bool SaveCnv();
	bool SaveFmd();
	bool SaveBpd();
	bool SaveCwd();
};


//------------------------------------------------------------------------------
//
//	VECTOR Display Class and Transfer Structure
//
//------------------------------------------------------------------------------
/*
struct TTransCompData
{
	TTransCompData();
	int SelFile;
	int W, Bk, Gy, Y, R, LR, B, LB, G, LG;
	int S, Dh, Bd, Dt;
	int TEMP, PERM;
	int D2, D3, fpj;
};


class TCompareDlg: public Draw2DVectors, public TDialog
{
	long VectNum;
	TCheckBox *SelectFile, *TwoD, *ThreeD, *FPJ;
	TStatic *FName;
	TRadioButton *White, *Black, *Grey, *Yellow, *Red, *LtRed, *Blue, *LtBlue;  		g
	TRadioButton *Green, *LtGreen;
	TRadioButton *Solid, *Dashed, *Bold, *Dotted;
	TRadioButton *Temp, *Perm;

	bool TestFileType();
	void GetPenColor();
	void GetPenStyle();
	void ThreeDCheck();
	void TwoDCheck();
	void FPJCheck();
	void EvClose();
	void CmCancel();
	void CmHelp();
	bool CanClose();

public:
	TCompareDlg(TWindow* AParent, const char* resID, TTransCompData& ts);
	TCompareDlg(TWindow* AParent, const char* resID, TTransCompData& ts, long VectNum);
	~TCompareDlg();
	void SelectFileMsg();

	DECLARE_RESPONSE_TABLE(TCompareDlg);
};


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

class VisVectors: public TDialog
{
	int FilterIndex;

	TListBox *list;
	 TButton  *add, *remove;
//     TTransCompData TransCompData;
	VectorTheme *theme;

	void SetupWindow();
	 void ShowFiles();
	 void AddFiles();
	 void RemoveFiles();
	 void EvClose();
	void CmHelp();
	void CmCancel();
	bool CanClose();

	DECLARE_RESPONSE_TABLE(VisVectors);

public:
	VisVectors(TWindow *Parent, const char *resID);
	 ~VisVectors();
};


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

class ConstantValueDlg: public TDialog
{
	long Max, Min;
	long *Val;
	TScrollBar *Selection;
	TStatic *Title, *Value;
	long ConstValue;
	long BarValue;
	char BarString[30];

	void SelectValue( UINT );
	void SetupWindow();
	void EvClose();
	void CmCancel();
	bool CanClose();

	DECLARE_RESPONSE_TABLE(ConstantValueDlg);
public:
	ConstantValueDlg(TWindow* AParent, const char* ResID, long min,
				long max, long *value, long Variable);
};


class TGenerateDlg: public TDialog
{
	int 		BarValue, FilterIndex;
	 bool 	ImportedLCP;
	 bool 	FileSaved;
	long 	NumYCells;
	long 	CNCols, CNRows; 			  // for file headers
	long 	ConstFuel, ConstCover, ConstHeight, ConstBase, ConstDensity, ConstDuff, ConstWoody;
	 long 	NumAllCats[10];
	 long 	AllCats[10][100];
	 short 	ARC_GRID_FORMAT;
	double 	CoordSouth, CoordWest;
	double 	CXLCorner, CYLCorner;		// comparisons
	 double 	HeightMult, BaseMult, DensityMult, DuffMult;
	double 	CCellSize;  				  //
	 double 	maxval[10], minval[10];
	char 	BarString[8];
	char 	DataType[20];
	char 	*ElFile;
	char 	*SlFile;
	char 	*AsFile;
	char 	*FlFile;
	char 	*CvFile;
	char 	*LndFile;
	char 	*LndInFile;
	char 	*HtFile;
	char 	*HlcbFile;
	char 	*CbdFile;
	 char 	*DuffFile;
	 char 	*WoodyFile;

	TCheckBox 	*FConst, *CConst, *HConst, *BConst, *PConst, *DConst, *WConst;
	TCheckBox 	*EL, *SL, *AS, *FL, *CV, *HT, *HLCB, *CBD, *DF, *WDY;
	TRadioButton 	*NEMetric, *NEEnglish, *NEKm, *ELMetric, *ELEnglish, *SPCT, *SDEG;
	TRadioButton 	*AGrass, *ADeg, *Cov14, *Covpct, *DuffTA, *DuffMgHa;
	TComboBox 	*HeightUnits, *BaseUnits, *DensityUnits;
	TCheckBox 	*CustMod, *CvtMod;
	TStatic 		*SEL, *SSL, *SAS, *SFL, *SCV;
	TStatic 		*SHT, *SHLCB, *SCBD, *SDUFF, *SWOODY, *IN_LFILE;
	 TEdit 		*SLat, *Desc;
	TUpDown 		*Lat;
	 TButton 		*Load, *Save, *New;
	 TGauge		*Prog;

	FILE *EFile, *SFile, *AFile, *FFile, *CFile, *LFile, *LIFile;
	FILE *HTFile, *HLCBFile, *CBDFile, *DFile, *WFile;

	char FileType[256];
	 char Description[512];
	void EvClose();
	void CmCancel();
	void CmHelp();
	 void NewLandscape();
	bool CanClose();
	void GrassFormat();
	 void ArcFormat();
	void LcpFormatMsg();
	 void GrassAspectMsg();
	void ConstFuelMsg();
	void ConstCoverMsg();
	void ConstHeightMsg();
	void ConstBaseMsg();
	void ConstDensityMsg();
	 void ConstDuffMsg();
	 void ConstWoodyMsg();
	void ElevFileCheckMsg();
	void SlopeFileCheckMsg();
	void AspectFileCheckMsg();
	void FuelFileCheckMsg();
	void CoverFileCheckMsg();
	void HtFileCheckMsg();
	void HlcbFileCheckMsg();
	void CbdFileCheckMsg();
	 void DuffFileCheckMsg();
	 void WoodyFileCheckMsg();
	void SelectHeightUnits();
	void SelectBaseUnits();
	void SelectDensityUnits();
	bool LatMsg(TNmUpDown &now);
	bool SelectFile(long Type);
	 bool TestRasterFileType(char *name);
	bool TestGrassRead(char *name);
	bool TestArcRead(char *name);
	void GenerateTheLandFile();
	void GenerateFromGrass();
	void GenerateFromArc();
	 bool CheckLCPUnits(headdata *NewHeader);
	 void ReviseLCPData(long Val, double MultBy);
	 void CheckHeaderChanges(bool Prompt);
	void PatchIntoLCP();
	void Reset();
	 void FinalizeHeader(headdata *newheader);
	 void FillCats(celldata *cell, crowndata *crown, grounddata *ground);
	 void SortCats();
	 void SetupWindow();
	 void SaveLCPMsg();
	 bool SpliceFilesIntoMemory(bool PatchInChanges);
	 void UpdateProgress(double fract);

	DECLARE_RESPONSE_TABLE(TGenerateDlg);
public:
	TGenerateDlg(TWindow* AParent, const char* resID, TTransGenData& ts);
};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


class TCanopyDlg: public TDialog
{
	char BarString[15];
	int BarValue;
	float FBarValue;
	TRadioButton *Metric, *English;
	TScrollBar *Treeht, *HLCB, *CrownBD, *Treedbh, *FoliarMC;
	TStatic *STreeht, *SHLCB, *SCrownBD, *STreedbh, *SFoliarMC;
	TRadioButton *TL, *TM, *TH;
	TRadioButton *GF, *DF, *WH, *SF, *ES, *PP, *LP, *WP, *WR;

	void EvClose();
	void CmHelp();
	void CmCancel();
	bool CanClose();
	void TreehtMsg(UINT);
	void HLCBMsg(UINT);
	void CrownBDMsg(UINT);
	void TreedbhMsg(UINT);
	void FoliarMCMsg(UINT);
	void MetUnitsMsg();
	void EngUnitsMsg();
	void ResetUnits();
	 void SetupWindow();

	DECLARE_RESPONSE_TABLE(TCanopyDlg);
public:
	TCanopyDlg(TWindow* AParent, const char* resID, TTransCanopyData& ts);
};


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

class NewFuelDlg: public TDialog
{
	char Value[24];
	 long *mod, *base;
	 long Order[257];

	TListBox *basemod;
	 TEdit *newmod;

	 void SetupWindow();
	 bool CanClose();
	 void EvClose();
	 void CmCancel();
	 void CmHelp();

	 DECLARE_RESPONSE_TABLE(NewFuelDlg);

public:
	NewFuelDlg(TWindow *parent, const char *resID, long *Mod, long *Base);
};


class FuelModelDlg: public TDialog
{
	 long NumModels;
	 char value[16];
	 char Units[16];
	 double WindSpd, TuneFactor, Slope;

	 TEdit *models, *testnum, *wndspd, *slp, *tune;
	 TUpDown *udtestnum, *udwndspd, *udslope, *udtune;
	 TButton *newmdl, *open, *save, *calc, *apply;
	 TRadioButton *modenglish, *modmetric, *otpenglish, *otpmetric;
	 TRadioButton *lo, *med, *hi;
	TStatic *wndunits, *rosunits, *otpros, *flameunits, *otpflame, *fmdfile;
	 TStatic *cbhunits, *cbh, *fmx;
	 TStatic *load, *sav, *depth, *heat;
	 FarInputs *farinput;
	 TTransFMD *tfmd;

	 typedef struct 			  // fuel moisture for low med high
	 {
	   	double hr1, hr10, hr100, lh, lw;
	 } FMS;

	 typedef struct 			  // local storage of fuel models
	 {    double load;
	 	double fp[12];
	 } FMD;

	 NewFuel newfuel[257];

	 FMS fms[3];
	 double moist[5];

	 void SetupWindow();
	 void NewModelMsg();
	 void OpenFMDFile();
	 void SaveFMDFile();
	 void ModelEnglish();
	 void ModelMetric();
	 void OtpEnglish();
	 void OtpMetric();
	 void ApplyTuning();
	 bool TestModel(TNmUpDown &mov);
	 bool TestSlope(TNmUpDown &mov);
	bool TestWind(TNmUpDown &mov);
	bool Tuning(TNmUpDown &mov);
	 void Calculate(long ReadNewData);
	 void RunCalculations();
	 void LowMx();
	 void MedMx();
	 void HiMx();
	 bool CanClose();
	 void EvClose();
	 void CmCancel();
	 void CmHelp();
	 //double pow2(double base);
	 double CalcSpreadRate(long fuel, double *Moisture, double Slope,
					   double WindSpeed, double *FlameLength,
					   double *FirelineIntensity, double *HeatPerUnitArea);


	 void CopyToLocalFMD();
	 void WriteToEditWindow();
	 bool ReadFromEditWindow();

	 DECLARE_RESPONSE_TABLE(FuelModelDlg);

public:
	FuelModelDlg(TWindow *parent, const char *resID, FarInputs *fi, TTransFMD *ts);

};



//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

struct TTransInputData
{
	int Lcp, Adj, Fms, Cnv, Fmd, Cwd, Bpd;
	int Wtr1, Wtr2, Wtr3, Wtr4, Wtr5;
	int Wnd1, Wnd2, Wnd3, Wnd4, Wnd5;
	TTransInputData();
	void ResetData();
};

class FarInputDlg: public TDialog
{
	TCheckBox *LcpFile, *AdjFile, *FmsFile, *CnvFile, *FmdFile, *BpdFile, *CwdFile;
	TCheckBox *WtrFile1, *WtrFile2, *WtrFile3, *WtrFile4, *WtrFile5;
	TCheckBox *WndFile1, *WndFile2, *WndFile3, *WndFile4, *WndFile5;
	TStatic *SLcpFile, *SAdjFile, *SFmsFile, *SCnvFile, *SFmdFile, *SBpdFile, *SCwdFile;
	TStatic *SWtrFile1, *SWtrFile2, *SWtrFile3, *SWtrFile4, *SWtrFile5;
	TStatic *SWndFile1, *SWndFile2, *SWndFile3, *SWndFile4, *SWndFile5;
	TStatic *ProjFile;
	TButton *CanChar, *ViewVect, *ProjRet, *ProjMake, *ProjClose;
	TButton *LCPGen, *AdjGen, *FmsGen, *CnvGen, *FmdGen, *BpdGen, *CwdGen;
	 TButton *wndedit[5], *wtredit[5];
	 FarInputs *farinput;

	VisVectors *visvectors;
	TTransGenData TransGenData;
	 TTransFMD TransFMD;
	TTransCanopyData *TransCanopyData;
	TTransWWStruct *TransWWData;

	TGenerateDlg *GenerateDlg;
	TCanopyDlg *CanopyDlg;

	void SetupWindow();
	void CmHelp();
	void EvClose();
	void CmCancel();
	bool CanClose();
	void CheckDialogs();
	bool CheckProperInputs();
	bool CheckEqualWeatherWindFiles();
	void SelectLandscapeFile();
	void SelectAdjustmentFile();
	void OpenAdjustmentFile();
	void SelectFuelMoistureFile();
	void OpenFuelMoistureFile();
	void SelectConversionFile();
	void OpenConversionFile();
	void SelectFuelModelFile();
	void SelectCustomFuelInputs();
	void OpenFuelModelFile();
	 void SelectBurnPeriodFile();
	 void OpenBurnPeriodFile();
	 void SelectCoarseWoodyFile();
	 void OpenCoarseWoodyFile();
	bool OpenWeatherFiles(long FileNum);
	bool OpenWindFiles(long FileNum);
	void CmWeatherOpen1();
	void CmWeatherOpen2();
	void CmWeatherOpen3();
	void CmWeatherOpen4();
	void CmWeatherOpen5();
	void CmWindOpen1();
	void CmWindOpen2();
	void CmWindOpen3();
	void CmWindOpen4();
	void CmWindOpen5();
	void CmCanopySettings();
	void ViewVisVectors();
	void ProjectRetrieve();
	void ProjectMake();
	 void ProjectClose();
	void GenerateLCPFile();
	void GenerateADJFile();
	void GenerateFMSFile();
	void GenerateCNVFile();
	void GenerateFMDFile();
	 void GenerateBPDFile();
	 void GenerateCWDFile();
	 void EditWinds1();
	 void EditWinds2();
	 void EditWinds3();
	 void EditWinds4();
	 void EditWinds5();
	 void EditWeather1();
	 void EditWeather2();
	 void EditWeather3();
	 void EditWeather4();
	 void EditWeather5();


	DECLARE_RESPONSE_TABLE(FarInputDlg);
public:
	FarInputDlg(TWindow *AParent, const char* ResID, TTransInputData &ts,
				FarInputs *fi,	TTransCanopyData *tc);
};
*/
#endif // FARSITE_INPUT_DATA
