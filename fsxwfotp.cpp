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
//#include <io.h>
#include<unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <algorithm>

OutputFile::OutputFile()
{
	filepos = 0;
	NumRastAlloc = 0;   
	NumRastData = 0;
};


OutputFile::~OutputFile()
{
	FreeRastData();
};


void OutputFile::SelectOutputs(long OutputFormat)
{
	setHeaderType(OutputFormat) ; 
	
	// save current state of object to map
	SetRastData(x,y,t,f,r,rx,d) ; 
	
	if (OutputFormat == 1 || OutputFormat == 5)
	{
		WriteRastMemFiles();
	}
	else if (OutputFormat > 1)
	{
		FileOutput = OutputFormat;	// specify vector file for optional
		OptionalOutput(false);
	}
}

void OutputFile::SelectMemOutputs(long OutputFormat)
{
	setHeaderType(OutputFormat) ; 

	if (OutputFormat == 1 || OutputFormat == 5)
	{
		WriteRastMemFiles();
	}
	else if (OutputFormat > 1)
	{
		FileOutput = OutputFormat;	// specify vector file for optional
		OptionalOutput(true);
	}
}



void OutputFile::WriteRastMemFiles()
{
	WriteFile(RAST_ARRIVALTIME);
	if (GetFileOutputOptions(RAST_FIREINTENSITY))
		WriteFile(RAST_FIREINTENSITY);
	if (GetFileOutputOptions(RAST_FLAMELENGTH))
		WriteFile(RAST_FLAMELENGTH);
	if (GetFileOutputOptions(RAST_SPREADRATE))
		WriteFile(RAST_SPREADRATE);
	if (GetFileOutputOptions(RAST_HEATPERAREA))
		WriteFile(RAST_HEATPERAREA);
	if (GetFileOutputOptions(RAST_REACTIONINTENSITY))
		WriteFile(RAST_REACTIONINTENSITY);
	if (GetFileOutputOptions(RAST_CROWNFIRE))
		WriteFile(RAST_CROWNFIRE);
	if (GetFileOutputOptions(RAST_FIREDIRECTION))
		WriteFile(RAST_FIREDIRECTION);
}



bool OutputFile::RastMemFile(long Type)
{
	double OutDoubleData;
	long OutLongData;

	// loop over all rows and all columns
	for (long j=0; j < numrows; j++) { 
		for (long i=0; i < numcols; i++) {  
			
			// check to see if this cell has raster data written to it.
			coordinate testxy = std::make_pair(i,j) ; 
			RasterMap::iterator data = rd.find(testxy) ; 
			
			// if this cell has no data yet, just print a -1 
			if (data == rd.end()) {
				// output -1 as either a float or a long.
				if (Type == RAST_FIREINTENSITY ||
				 	Type == RAST_HEATPERAREA ||
					Type == RAST_REACTIONINTENSITY || 
					Type == RAST_ARRIVALTIME ||
				    Type == RAST_FLAMELENGTH || 
				    Type == RAST_SPREADRATE) { 
					fprintf(otpfile, "%09.3lf\n", -1.0);
				} else { 
					fprintf(otpfile, "%03ld\n", -1l) ; 
				}
			} else { 
				if (Type != RAST_ARRIVALTIME)
				{
					if ((data->second).Write == false)
						continue;
				}
				x = (data->second).x;
				y = (data->second).y;
				t = (data->second).Time;
				f = (data->second).Fli;
				r = (data->second).Ros;
				rx = (data->second).Rcx;
				d = (data->second).Dir;
				switch (Type)
				{
				case RAST_ARRIVALTIME:
					OutDoubleData = t;
					break;
				case RAST_FIREINTENSITY:
					OutDoubleData = f / convf1;
					break;
				case RAST_FLAMELENGTH:
					Calcs(FLAME_LENGTH);					// calculate flame length
					OutDoubleData = l;
					break;
				case RAST_SPREADRATE:
					OutDoubleData = r * convf2;
					break;
				case RAST_HEATPERAREA:
					Calcs(HEAT_PER_AREA);   				  // calculate heat/unit area
					OutDoubleData = h;
					break;
				case RAST_REACTIONINTENSITY:
					Calcs(HEAT_PER_AREA);   				  // calculate heat/unit area
					OutDoubleData = rx;
					break;
				case RAST_CROWNFIRE:
					Calcs(CROWNFIRE);
					OutLongData = c;
					break;
				case RAST_FIREDIRECTION:
					OutLongData = d;
					break;
				}
				if (Type == RAST_ARRIVALTIME)
				{
					fprintf(otpfile, "%09.3lf\n", OutDoubleData);
					(data->second).Write = true;
				} else if (Type == RAST_FIREINTENSITY ||
					Type == RAST_HEATPERAREA ||
					Type == RAST_REACTIONINTENSITY)
					fprintf(otpfile, "%09.3lf\n", OutDoubleData);
				else if (Type == RAST_FLAMELENGTH || Type == RAST_SPREADRATE)
					fprintf(otpfile, "%05.2lf\n", OutDoubleData);
				else
					fprintf(otpfile, "%03ld\n", OutLongData);
			}
		}
	}

	return false;
}

/*
 * \brief encloses the logic to write either a single value to the "optional"
 * output file, or writes the entire accumulated array to the file.  THis 
 * does not output in a gridded format, but rather just dumps the accumulated
 * points to the file.
 */
void OutputFile::OptionalOutput(bool FromMemory)
{
	char RasterCopy[256] = "";

	if (FileOutput == 3)
		strcpy(RasterCopy, GetRasterFileName(0));
	else
		strcpy(RasterCopy, GetVectorFileName());
	if (filepos == 0)
	{
		if (access(RasterCopy, F_OK) == 0)
			chmod(RasterCopy, S_IWRITE);
		otpfile = fopen(RasterCopy, "w");
		filepos = 1;
	}
	else
		otpfile = fopen(RasterCopy, "a");

	if (otpfile == NULL)
		return;
	if (FromMemory)
	{
		RasterMap::const_iterator data ; 
		for (data = rd.begin(); data != rd.end(); ++data)
		{
			x = (data->second).x;
			y = (data->second).y;
			t = (data->second).Time;
			f = (data->second).Fli;
			r = (data->second).Ros;
			rx = (data->second).Rcx;
			d = (data->second).Dir;
			WriteOptionalFile();
			fprintf(otpfile, "\n");
		}
	}
	else
	{
		WriteOptionalFile();
		fprintf(otpfile, "\n");
	}
	fclose(otpfile);
}


/**
 * \brief Writes a single value to the already opened, already positioned 
 * output file.  
 */
void OutputFile::WriteOptionalFile()
{
	x = ConvertEastingOffsetToUtm(x);
	y = ConvertNorthingOffsetToUtm(y);
	fprintf(otpfile, "%010.4lf %010.4lf %010.4lf", x, y, t);
	if (GetFileOutputOptions(RAST_FIREINTENSITY))
		fprintf(otpfile, " %010.4lf", f / convf1);
	if (GetFileOutputOptions(RAST_FLAMELENGTH))
	{
		Calcs(FLAME_LENGTH);
		fprintf(otpfile, " %010.4lf", l);
	}
	if (GetFileOutputOptions(RAST_SPREADRATE))
		fprintf(otpfile, " %010.4lf", r * convf2);
	if (GetFileOutputOptions(RAST_HEATPERAREA))
	{
		Calcs(HEAT_PER_AREA);
		fprintf(otpfile, " %010.4lf", h);
	}
	if (GetFileOutputOptions(RAST_REACTIONINTENSITY))
	{
		Calcs(HEAT_PER_AREA);
		fprintf(otpfile, " %010.4lf", rx);
	}
	if (GetFileOutputOptions(RAST_CROWNFIRE))
	{
		Calcs(CROWNFIRE);
		fprintf(otpfile, " %ld", c);
	}
	if (GetFileOutputOptions(RAST_FIREDIRECTION))
		fprintf(otpfile, " %ld", d);
}


void OutputFile::Calcs(CalcType TYPE)
{
	long garbage;
	double Io, Ro;
	celldata cd;
	crowndata rd;
	grounddata gd;

	CellData(x, y, cd, rd, gd, &garbage);
	if (cd.c > 0)
	{
		ld.BaseConvert(rd.b);
		ld.DensityConvert(rd.p);
		c = 1;
		Io = pow(0.010 * ld.ld.base * (460.0 + 25.9 * GetFoliarMC()), 1.5);
		if (f >= Io)
		{
			c = 2;
			if(ld.ld.density>0.0)
               {    if(GetCrownFireCalculation()==0)
	               	Ro=2.8/ld.ld.density;     // close to the ro+0.9(rac-ro)
                    else
	               	Ro=3.0/ld.ld.density;     
               	if(r>=Ro)
                        	c=3;
          	}
		}
	}
	else if (r > 1e-6)
		c = 1;

	switch (TYPE)
	{
	case FLAME_LENGTH:
		if (c > 1)
			l = ((0.2 * pow(f / 3.4613, 2.0 / 3.0)) / 3.2808) * convf2;		// fl for crown fire from Thomas in Rothermel 1991 converted to m
		else
			l = 0.0775 * pow(f, 0.46) * convf2;						// fl from Byram for surface fires
		break;
	case HEAT_PER_AREA:
		if (r > 1e-6)
			h = (60.0 * f / convf2) / (r * convf1);
		else
			h = 0.0;
		break; 	// just hpua
	case FL_AND_HPA:
		if (c > 1)
			l = ((0.2 * pow(f / 3.4613, 2.0 / 3.0)) / 3.2808) * convf2;		// fl for crown fire from Thomas in Rothermel 1991 converted to m
		else
			l = 0.0775 * pow(f, 0.46) * convf2;						// fl from Byram for surface fires
		if (r > 1e-6)
			h = (60.0 * f / convf2) / (r * convf1);
		else
			h = 0.0;
		break;
	case CROWNFIRE:
		/*CellData(x, y, cd, rd, gd, &garbage);
			if(cd.c>0)
			{    ld.BaseConvert(rd.b);
			 	ld.DensityConvert(rd.p);
			 	c=1;
			 	Io=pow(0.010*ld.ld.base* (460.0+25.9*GetFoliarMC()),1.5);
			 	if(f>=Io)
			 	{    c=2;
				 		if(ld.ld.density>0.0)
				 		{    Ro=2.8/ld.ld.density;     // close to the ro+0.9(rac-ro)
					  		if(r>=Ro)
								c=3;
				 		}
				  }
			 }
			 else
			 if(r>1e-6)
				  c=1;
			 */
		break;
	case REACTION_INTENSITY:
		rx /= convf3;
		break;
	default: 
	   ; // do nothing
	}
}


void OutputFile::ConvF()
{
	convf1 = convf2 = convf3 = 1.0;
	if (AccessOutputUnits(GETVAL))
	{
		convf1 = 3.4613;
		convf2 = 3.2808;
		convf3 = 0.18189275;
	}
}


void OutputFile::InitRasterFiles(long FileType)
{
	HeaderType = FileType ; 
//	FirstFile(FileType, RAST_ARRIVALTIME);
//	if (GetFileOutputOptions(RAST_FIREINTENSITY))
//		FirstFile(FileType, RAST_FIREINTENSITY);
//	if (GetFileOutputOptions(RAST_FLAMELENGTH))
//		FirstFile(FileType, RAST_FLAMELENGTH);
//	if (GetFileOutputOptions(RAST_SPREADRATE))
//		FirstFile(FileType, RAST_SPREADRATE);
//	if (GetFileOutputOptions(RAST_HEATPERAREA))
//		FirstFile(FileType, RAST_HEATPERAREA);
//	if (GetFileOutputOptions(RAST_REACTIONINTENSITY))
//		FirstFile(FileType, RAST_REACTIONINTENSITY);
//	if (GetFileOutputOptions(RAST_CROWNFIRE))
//		FirstFile(FileType, RAST_CROWNFIRE);
//	if (GetFileOutputOptions(RAST_FIREDIRECTION))
//		FirstFile(FileType, RAST_FIREDIRECTION);
	CanSetRasterResolution(0);  	  // false
}



void OutputFile::WriteFile(long Type)
{
	double xres, yres;
	char RasterCopy[256] = "";
	long numcells;
	double MetersToKm = 1.0;

	GetRastRes(&xres, &yres);
	numcols = (long) ((East - West) / xres);//((GetEastUtm()-GetWestUtm())/xres);
	numrows = (long) ((North - South) / yres);//((GetNorthUtm()-GetSouthUtm())/yres);
	numcells = numrows * numcols;
	memset(RasterCopy, 0x0, sizeof RasterCopy);
	strcpy(RasterCopy, GetRasterFileName(Type));

	if (CheckCellResUnits() == 2)
		MetersToKm = 0.001;

	otpfile = fopen(RasterCopy, "w");
	if (otpfile == NULL)
	{
		//SetFileAttributes(RasterCopy, FILE_ATTRIBUTE_NORMAL);
		chmod(RasterCopy,S_IRWXO);
		otpfile = fopen(RasterCopy, "w");
	}
	switch (HeaderType)
	{
	case 1:
		if (GetNorthUtm() == 0.0 || GetEastUtm() == 0.0)
		{
			fprintf(otpfile, "%s    %lf\n", "north:", North);//*MetersToKm);
			fprintf(otpfile, "%s    %lf\n", "south:", South);//*MetersToKm);
			fprintf(otpfile, "%s     %lf\n", "east:", East);//*MetersToKm);
			fprintf(otpfile, "%s     %lf\n", "west:", West);//*MetersToKm);
		}
		else
		{
			fprintf(otpfile, "%s    %lf\n", "north:",
				ConvertNorthingOffsetToUtm(North));//*MetersToKm);
			fprintf(otpfile, "%s    %lf\n", "south:",
				ConvertNorthingOffsetToUtm(South));//*MetersToKm);
			fprintf(otpfile, "%s     %lf\n", "east:",
				ConvertEastingOffsetToUtm(East));//*MetersToKm);
			fprintf(otpfile, "%s     %lf\n", "west:",
				ConvertEastingOffsetToUtm(West));//*MetersToKm);
		}
		fprintf(otpfile, "%s     %ld\n", "rows:", numrows);
		fprintf(otpfile, "%s     %ld\n", "cols:", numcols);
		break;
	case 5:
		fprintf(otpfile, "%s %ld\n", "NCOLS", numcols);
		fprintf(otpfile, "%s %ld\n", "NROWS", numrows);
		if (GetNorthUtm() == 0.0 || GetEastUtm() == 0.0)
		{
			fprintf(otpfile, "%s %lf\n", "XLLCORNER", West);//*MetersToKm);
			fprintf(otpfile, "%s %lf\n", "YLLCORNER", South);//*MetersToKm);
		}
		else
		{
			fprintf(otpfile, "%s %lf\n", "XLLCORNER",
				ConvertEastingOffsetToUtm(West));//*MetersToKm);
			fprintf(otpfile, "%s %lf\n", "YLLCORNER",
				ConvertNorthingOffsetToUtm(South));//*MetersToKm);
		}
		fprintf(otpfile, "%s %lf\n", "CELLSIZE", xres * MetersToKm);  //GetRastRes()
		fprintf(otpfile, "%s %s\n", "NODATA_VALUE", "-1");
		break;
	}
	
	// write the actual data to the file.
	RastMemFile(Type) ; 
	fclose(otpfile);
}


void OutputFile::GetRasterExtent()
{
	if (CanSetRasterResolution(GETVAL) == 0)
		return;

	if (SetRasterExtentToViewport(GETVAL))
	{
		North = GetViewNorth();
		South = GetViewSouth();
		East = GetViewEast();
		West = GetViewWest();
	}
	else
	{
		North = GetHiNorth();
		South = GetLoNorth();
		East = GetHiEast();
		West = GetLoEast();
	}
}






void OutputFile::FreeRastData()
{
	rd.clear() ; 
	NumRastAlloc = 0;
	NumRastData = 0;
}

/**
 * \brief Aggregates the provided set of fire behavior characteristics onto
 * the raster.  
 * 
 * <p>
 * The client code supplies a location (xpt, ypt) expressed in real world 
 * coordinates (UTM), and the associated fire behavior characteristics (time, 
 * fli, ros, rcx, dir).  This method translates the location into a grid 
 * cell index (i,j) and stores the fire behavior data for that cell.  
 * </p>
 * 
 * <p>
 * In most cases, if the cell already contains a data value, the old value 
 * is simply overwritten by the new value.  Time of arrival is an exception 
 * to this rule.  The resultant value for the grid cell is the minimum of all
 * the time values assigned.
 * </p>
 */
bool OutputFile::SetRastData(double xpt, double ypt, double time, double fli,
	double ros, double rcx, long dir)
{

	double xres, yres ; 
	coordinate tempxy ; 
	RastData temp ; 

	GetRastRes(&xres, &yres);

	// compute the grid cell value
	tempxy.first = (long) ((xpt - West) / xres) ;
	tempxy.second = (long) ((North - ypt) / yres) ;
	
	// check to see if this cell already has an entry
	RasterMap::iterator oldVal = rd.find(tempxy) ; 


	if (oldVal != rd.end()) { 
		// handle special cases where cell already has value
		temp.Time = std::min((oldVal->second).Time, time) ; 
	} else { 
		// handle special cases where cell doesn't have value yet
		temp.Time = time ; 
	}
	
	// handle "standard" case where we blithely replace the value
	temp.x = xpt ; 
	temp.y = ypt ; 
	temp.Fli = fli;
	temp.Ros = ros;
	temp.Rcx = rcx;
	temp.Dir = dir;
	
	// add this to the map (if it was already in the map, tell where)
	if (oldVal != rd.end()) { 
		rd.insert(oldVal, std::make_pair(tempxy,temp)) ; 
	} else { 
		rd.insert(std::make_pair(tempxy,temp)) ; 
	}

	NumRastData = rd.size() ; 

	return true;
}



