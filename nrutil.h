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
//#ifndef _NR_UTILS_H_
//#define _NR_UTILS_H_

#ifndef NRUTILS
#define NRUTILS

//static double sqrarg;
//#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg);

//static double dsqrarg;
//#define DSQR(a) ((dsqrarg=(a)) ==0.0 ? 0.0 : dsqrarg*dsqrarg);

//static double dmaxarg1, dmaxarg2;
//#define DMAX(a, b) (dmaxarg1=(a), dmaxarg2=(b), (dmaxarg1)>(dmaxarg2) ? (dmaxarg1) : (dmaxarg2)

//static double dminarg1, dminarg2;
//#define DMIN(a, b) (dminarg1=(a), dminarg2=(b), (dminarg1)<(dminarg2) ? (dminarg1) : (dminarg2)

//static double maxarg1, maxarg2;
//#define FMAX(a, b) (maxarg1=(a), maxarg2=(b), (maxarg1)>(maxarg2) ? (maxarg1) : (maxarg2)

//static double minarg1, minarg2;
//#define FMIN(a, b) (minarg1=(a), minarg2=(b), (minarg1)<(minarg2) ? (minarg1) : (minarg2)

//static long lmaxarg1, lmaxarg2;
//#define LMAX(a, b) (lmaxarg1=(a), lmaxarg2=(b), (lmaxarg1)>(lmaxarg2) ? (lmaxarg1) : (lmaxarg2)

//static long lminarg1, lminarg2;
//#define LMIN(a, b) (lminarg1=(a), lminarg2=(b), (lminarg1)<(lminarg2) ? (lminarg1) : (lminarg2)

//static long imaxarg1, imaxarg2;
//#define IMAX(a, b) (imaxarg1=(a), imaxarg2=(b), (imaxarg1)>(imaxarg2) ? (imaxarg1) : (imaxarg2)

//static long iminarg1, iminarg2;
//#define IMIN(a, b) (iminarg1=(a), iminarg2=(b), (iminarg1)<(iminarg2) ? (iminarg1) : (iminarg2)

#define SIGN(a, b) ((b) > 0.0 ? fabs(a) : -fabs(a))

//#if defined(__STDC__) || defined(ANSI) || defined(NRANSI)

void nrerror(char error_text[]);
float* fvector(long nl, long nh);
double* vector(long nl, long nh);
long* ivector(long nl, long nh);
unsigned char* cvector(long nl, long nh);
unsigned long* lvector(long nl, long nh);
double* dvector(long nl, long nh);
double** matrix(long nrl, long nrh, long ncl, long nch);
double** dmatrix(long nrl, long nrh, long ncl, long nch);
long** imatrix(long nrl, long nrh, long ncl, long nch);
double** submatrix(double** a, long oldrl, long oldrh, long oldcl, long oldc,
	long newrl, long newcl);
double** convert_matrix(double* a, long nrl, long nrh, long ncl, long nch);
double*** f3tensor(long nrl, long nrh, long ncl, long nch, long ndl, long ndh);
void free_fvector(float* v, long nl, long nh);
void free_vector(double* v, long nl, long nh);
void free_ivector(long* v, long nl, long nh);
void free_cvector(unsigned char* v, long nl, long nh);
void free_lvector(unsigned long* v, long nl, long nh);
void free_dvector(double* v, long nl, long nh);
void free_matrix(double** m, long nrl, long nrh, long ncl, long nch);
void free_dmatrix(double** m, long nrl, long nrh, long ncl, long nch);
void free_imatrix(long** m, long nrl, long nrh, long ncl, long nch);

void free_submatrix(double** b, long nrl, long nrh, long ncl, long nch);
void free_convert_matrix(double** b, long nrl, long nrh, long ncl, long nch);
void free_f3tensor(double*** t, long nrl, long nrh, long ncl, long nch,
	long ndl, long ndh);

//#else

//void nrerror();
//double *vector();

#endif
//#endif
