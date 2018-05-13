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
//
//
//------------------------------------------------------------------------------

#ifndef WTRWNDEDITDIALOGS
#define WTRWNDEDITDIALOGS

//#include <owl\edit.h>
//#include <owl\button.h>
//#include <owl\static.h>
//#include <owl\updown.h>
//#include <owl\checkbox.h>
#include "fsglbvar.h"
#include "wndwtr.rh"

class ChangeValue : public TDialog
{
	double* Val;
	char* Kind;
	TEdit* change;
	TStatic* type;

	void SetupWindow();
	void CmCancel();
	void EvClose();
	bool CanClose();

	DECLARE_RESPONSE_TABLE(ChangeValue);
public:
	ChangeValue(TWindow* Parent, const char* resID, char* kind, double* val);
};



struct TransWndEdit
{
	char FileName[256];
};


class WindEdit : public TDialog
{
	char File[256];
	long i, LastMod, StationNumber;
	long NumObs, Metric;
	double LastVal;
	unsigned int StartPos, EndPos;
	WindData* wnd;

	TEdit* wedit;
	TEdit* aspd, * adir, * shr, * ehr;
	TUpDown* uaspd, * uadir, * ushr, * uehr;
	TButton* baspd, * badir, * bmspd, * setspd, * setdir;
	TButton* load, * save, * newfile, * undo, * extend;
	TButton* mom, * dym, * spm, * drm, * clm;
	TStatic* wndfile;
	TCheckBox* metric;

	bool CanClose();
	void CmCancel();
	void CmHelp();
	void EvClose();
	void SetupWindow();
	void MakeModification(long Type, double val);
	bool StartHour(TNmUpDown& mov);
	bool EndHour(TNmUpDown& mov);
	void LoadFile();
	void SaveFile();
	void NewFile();
	void SetSpeed();
	void MonthMod();
	void DayMod();
	void SpeedMod();
	void DirMod();
	void CloudMod();
	void SetDir();
	void Extend();
	void AddSpeed();
	void MultSpeed();
	void AddDir();
	void CopyToLocalWnd();
	void WriteToEditWindow();
	bool ReadFromEditWindow();
	void UndoLastChange();
	void MetricUnits();

	DECLARE_RESPONSE_TABLE(WindEdit);

public:
	WindEdit(TWindow* AParent, const char* resID, TransWndEdit& ts,
		long stationnumber);
	~WindEdit();
};


//----------------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------------


struct TransWtrEdit
{
	char FileName[256];
};



class WeatherEdit : public TDialog
{
	char File[256];
	long i, StationNumber;
	long NumObs, LastMod, Metric;
	unsigned int StartPos, EndPos;
	double LastVal;
	WeatherData* wtr;

	TEdit* wedit;
	TEdit* htmp, * ltmp, * hhmd, * lhmd;
	TButton* mo;
	TUpDown* uhtmp, * ultmp, * uhhmd, * ulhmd;
	TButton* bhtmp, * bltmp, * bhhmd, * blhmd;
	TButton* shtmp, * sltmp, * shhmd, * slhmd;
	TButton* load, * save, * newfile, * undo, * extend;
	TStatic* wtrfile;
	TCheckBox* metric;

	bool CanClose();
	void CmHelp();
	void CmCancel();
	void EvClose();
	void SetupWindow();
	void MakeModification(long Type, double val);
	void LoadFile();
	void SaveFile();
	void NewFile();
	void AddHiTemp();
	void AddLoTemp();
	void AddHiHumid();
	void AddLoHumid();
	void SetHiTemp();
	void SetLoTemp();
	void SetHiHumid();
	void SetLoHumid();
	void SetMonth();
	void CopyToLocalWtr();
	void WriteToEditWindow();
	bool ReadFromEditWindow();
	void UndoLastChange();
	void Extend();
	void MetricUnits();

	DECLARE_RESPONSE_TABLE(WeatherEdit);

public:
	WeatherEdit(TWindow* AParent, const char* resID, TransWtrEdit& ts,
		long StationNumber);
	~WeatherEdit();
};


#endif
