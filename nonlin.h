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
#ifndef NRUTIL
#define NRUTIL

#include "nrutil.h"
#include <stdio.h>
#include <math.h>

double pow2(double input);

void mrqmin(double x[], double y[], double sig[], long ndata, double a[],
	long ia[], long ma, double** covar, double** alpha, double* chisq,
	void (*funcs) (double, double [], double *, double[], long),
	double* alamda);


void covsrt(double** covar, long ma, long ia[], long mfit);
void gaussj(double** a, long n, double** b, long m);
void mrqcof(double x[], double y[], double sig[], long ndata, double a[],
	long ia[], long ma, double** alpha, double beta[], double* chisq,
	void (*funcs) (double, double [], double*, double [], long));
void lfit(double x[], double y[], double sig[], long ndat, double a[],
	long ia[], long ma, double** covar, double* chisq,
	void (*funcs) (double, double [], long));

void svdfit(double x[], double y[], double sig[], long ndata, double a[],
	long ma, double** u, double** v, double w[], double* chisq,
	void (*funcs) (double, double[], long));

void svdar(double** v, long ma, double w[], double** cvm);
void svbksb(double** u, double w[], double** v, long m, long n, double b[],
	double x[]);
void svdcmp(double** a, long m, long n, double w[], double** v);
double pythag(double a, double b);



#endif 	// NRUTIL
