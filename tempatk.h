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

#ifndef NEWATKDLGS
#define NEWATKDLGS

//#include <owl/dialog.h>
//#include <owl/picklist.h>
//#include <owl/treewind.h>
//#include <owl/radiobut.h>
//#include <owl/button.h>
//#include <owl/combobox.h>
//#include <owl/updown.h>
//#include <owl/dialog.h>
//#include <owl/edit.h>
#include "fsglbvar.h"
#include "fsxwattk.h"
//#include "fsatkdlg.h"
//#include "fsatkdlg.rh"


class _USERCLASS TAtkTreeWindow : public TTreeWindow
{
	void EvLButtonDblClick(uint modKeys, TPoint& point);
	void EvRButtonDown(uint modKeys, TPoint& point);
	void SetupWindow();

	DECLARE_RESPONSE_TABLE(TAtkTreeWindow);
public:
	TAtkTreeWindow(TWindow* parent, int resID);
};



class NewGroundResources : public TDialog
{
	bool Refreshing, IsCrew, IsGroup, * GrpExpanded;
	char SelText[256];
	char* FileName;
	long NumLocalCrews, NumLocalGroups, NumStoredCrews, NumStoredGroups;
	long NumAllocCrews, NumAllocGroups;
	long Selection, CrewIndex, NumObs, NewUnits;
	unsigned int StartPos, EndPos;
	double LineMult, LimMult, RateMult;
	TEdit* crewedit, * linerate;
	TStatic* filename;
	TRadioButton* chhr, * mmin, * ftmin;
	TUpDown* udlinerate;
	TButton* loadcrew, * savecrew, * changename;
	TButton* dupcrw, * delgrps, * newgrp, * newcrw, * add, * set;
	TComboBox* crewname;
	TAtkTreeWindow* treewin;
	Crew** crw;
	CompoundCrew* grp;

	void EvRButtonDown(uint modKeys, TPoint& point);
	void SelectCrew();
	void LoadCrew();
	bool LoadLocalCrews(bool AppendList);
	void SaveCrew();
	void AddRate();
	void SetRate();
	bool LineRate(TNmUpDown&);
	void AllocCrews(long Num);
	void FreeAllLocalCrews();
	void FreeAllLocalGroups();
	void AllocGroups(long Num);
	void MakeModification(long Type);
	void NewCrew();
	void NewCrew2();
	void DupCrew();
	void CreateGroup();
	void DeleteGroup();
	void AddToGroup();
	void RemoveFromGroup();
	void ViewLineRate();
	void EditLineRate();
	void DupGroup();
	void DelAllGroups();
	void RefreshTree();
	bool GetSelectedCrew(char* SelText, char* ParentText, long* ChildOrder);
	void WriteToEditWindow();
	bool ReadFromEditWindow();
	void CopyToLocalCrew();
	void CopyToLocalGroup();
	void OpenPickList();
	void TvnItemExpanded(TTwNotify far&);
	void SetupWindow();
	void SetupCrews();
	void UnitsMsg();
	void ChangeUnits();
	void ChangeCrewName();
	void ChangeGroupName();
	bool CanClose();
	void EvClose();
	void CmCancel();
	void CmHelp();

	DECLARE_RESPONSE_TABLE(NewGroundResources);
public:
	NewGroundResources(TWindow* parent, const char* resID, char* name);
	~NewGroundResources();
};

/*
class NewReviseAttack: public TDialog
{
	 bool 	*WinOn;
	 char 	String[20];
	 long 	CurOn, Index;
	 long 	Indirect;
	 long SortDirR, SortDirT, SortDirW, HiLite;
	 AttackData 	*atk;

	 TAtkTreeWindow *tree;
	TButton		*srtype, *satktac, *swrkhrs;
	 TButton 		*loc, *ex, *gprop;
	 TRadioButton 	*cont, *cancel;
	 TRadioButton   *reassignd, *reassigni, *reassignp, *reassigng;
	 TRadioButton	*suspend, *resume;
	 TCheckBox		*bleft, *bright;

	 void ContinueAtk();
	 void CancelAtk();
	 void ReassignDirect();
	 void ReassignIndirect();
	 void ReassignParallel();
	 void ReassignGroup();
	 void SuspendAtk();
	 void ResumeAtk();
	 void Burnout();
	 void BurnLeft();
	 void BurnRight();
	 void GroupProperties();
	 void ShowLocations();
	 void DrawAttackNumbers(long AttackNum);
	 void EvPaint();
	 void LocateResource();
	 void ExecuteRevision();
	void SetupWindow();
	 bool CanClose();
	 void EvClose();
	 void CmCancel();
	 void CmHelp();
	 void MoveToNewGroup(); 	   // notified from farsitewindow
	 void MoveToExistingGroup();	// notified from farsitewindow

	 DECLARE_RESPONSE_TABLE(NewReviseAttack);
public:
	NewReviseAttack(TWindow *parent, const char *resID, bool WinON, long HiLiteNum);
};
*/

#endif  // NEWATKDLGS
