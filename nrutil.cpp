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
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
//#include <windows.h>
#include <string.h>
#define NR_END 1
#define FREE_ARG char*

#include "nrutil.h"

void nrerror(char error_text[])
{
	fprintf(stderr, "Numerical Recipies run-time error ...\n");
	fprintf(stderr, "%s\n", error_text);
	fprintf(stderr, "..now exiting to system...\n");
	exit(1);
}

float* fvector(long nl, long nh)
{
	float* v;

	v = (float *) malloc((size_t) ((nh - nl + 1 + NR_END) * sizeof(float)));
	memset(v,0x0,(size_t) ((nh - nl + 1 + NR_END) * sizeof(float)));
	if (!v)
		nrerror("allocation failure in vector()");

	return v - nl + NR_END;
}

double* vector(long nl, long nh)
{
	double* v;

	v = (double *) malloc((size_t) ((nh - nl + 1 + NR_END) * sizeof(double)));
	memset(v,0x0,(size_t) ((nh - nl + 1 + NR_END) * sizeof(double)));
	if (!v)
		nrerror("allocation failure in vector()");

	return v - nl + NR_END;
}

long* ivector(long nl, long nh)
{
	long* v;

	v = (long *) malloc((size_t) ((nh - nl + 1 + NR_END) * sizeof(long)));
	memset(v,0x0,(size_t) ((nh - nl + 1 + NR_END) * sizeof(long)));
	if (!v)
		nrerror("allocation failure in vector()");

	return v - nl + NR_END;
}

unsigned char* cvector(long nl, long nh)
{
	unsigned char * v;

	v = (unsigned char *) malloc( (size_t) ((nh - nl + 1 + NR_END) * sizeof(unsigned char)));
	memset(v,0x0, (size_t) ((nh - nl + 1 + NR_END) * sizeof(unsigned char)));
	if (!v)
		nrerror("allocation failure in vector()");

	return v - nl + NR_END;
}

unsigned long* lvector(long nl, long nh)
{
	unsigned long * v;

	v = (unsigned long *) malloc( (size_t) ((nh - nl + 1 + NR_END) * sizeof(unsigned long)));
	memset(v,0x0, (size_t) ((nh - nl + 1 + NR_END) * sizeof(unsigned long)));
	if (!v)
		nrerror("allocation failure in vector()");

	return v - nl + NR_END;
}

double* dvector(long nl, long nh)
{
	return vector(nl, nh);
}

double** matrix(long nrl, long nrh, long ncl, long nch)
{
	long i;
	long nrow = nrh - nrl + 1;
	long ncol = nch - ncl + 1;
	double** m;

	m = (double * *) malloc( (size_t) ((nrow + NR_END) * sizeof(double *)));
	memset(m,0x0, (size_t) ((nrow + NR_END) * sizeof(double *)));
	if (!m)
		nrerror("allocation failure 1 in matrix()");
	m += NR_END;
	m -= nrl;

	m[nrl] = (double *)malloc( (size_t) ((nrow * ncol + NR_END) * sizeof(double)));
	memset(m[nrl],0x0, (size_t) ((nrow * ncol + NR_END) * sizeof(double)));
	if (!m[nrl])
		nrerror("allocation failure 2 in matrix()");
	m[nrl] += NR_END;
	m[nrl] -= ncl;

	for (i = nrl + 1; i <= nrh; i++)
		m[i] = m[i - 1] + ncol;

	return m;
}

double** dmatrix(long nrl, long nrh, long ncl, long nch)
{
	return matrix(nrl, nrh, ncl, nch);
}

long** imatrix(long nrl, long nrh, long ncl, long nch)
{
	long i;
	long nrow = nrh - nrl + 1;
	long ncol = nch - ncl + 1;
	long** m;

//	m = (long * *) GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
//					(size_t) ((nrow + NR_END) * sizeof(long *)));
	m = (long **) malloc((size_t)((nrow + NR_END) * sizeof(long *)));
	memset(m,0X0,(size_t)((nrow + NR_END) * sizeof(long *)));

	if (!m)
		nrerror("allocation failure 1 in matrix()");
	m += NR_END;
	m -= nrl;

	//m[nrl] = (long *) GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
//						(size_t) ((nrow * ncol + NR_END) * sizeof(long)));
	
	m[nrl] = (long *) malloc((size_t) ((nrow * ncol + NR_END) * sizeof(long)));
	memset(m[nrl],0X0,(size_t)((nrow * ncol + NR_END) * sizeof(long)));
	if (!m[nrl])
		nrerror("allocation failure 2 in matrix()");
	m[nrl] += NR_END;
	m[nrl] -= ncl;

	for (i = nrl + 1; i <= nrh; i++)
		m[i] = m[i - 1] + ncol;

	return m;
}


double** submatrix(double** a, long oldrl, long oldrh, long oldcl, long oldch,
	long newrl, long newcl)
{
	long i, j;
	long nrow = oldrh - oldrl + 1;
	//long ncl = oldcl - newcl;
	double** m;

	//m = (double * *) GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
//						(size_t) ((nrow + NR_END) * sizeof(double *)));
	m = (double * *) malloc((size_t) ((nrow + NR_END) * sizeof(double *)));
	memset(m,0X0,(size_t) ((nrow + NR_END) * sizeof(double *)));
	if (!m)
		nrerror("Allocation failure in submatrix()");
	m += NR_END;
	m -= newrl;

	for (i = oldrl, j = newrl; i <= oldrh; i++, j++)
		m[j] = a[i] + newrl;

	return m;
}

double** convert_matrix(double* a, long nrl, long nrh, long ncl, long nch)
{
	long i, j;
	long nrow = nrh - nrl + 1;
	long ncol = nch - ncl + 1;
	double** m;

	//m = (double * *) GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
//						(size_t) ((nrow + NR_END) * sizeof(double *)));
	m = (double * *) malloc((size_t) ((nrow + NR_END) * sizeof(double *)));
	memset(m,0X0,(size_t) ((nrow + NR_END) * sizeof(double *)));
	if (!m)
		nrerror("Allocation failure in convert_matrix()");
	m += NR_END;
	m -= nrl;

	m[nrl] = a - ncl;
	for (i = 1, j = nrl + 1; i < nrow; i++, j++)
		m[j] = m[j - 1] + ncol;

	return m;
}

double*** f3tensor(long nrl, long nrh, long ncl, long nch, long ndl, long ndh)
{
	long i, j;
	long nrow = nrh - nrl + 1;
	long ncol = nch - ncl + 1;
	long ndep = ndh - ndl + 1;
	double*** t;

	//t = (double * **) GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
	//					(size_t) ((nrow + NR_END) * sizeof(double * *)));
	t = (double * **) malloc((size_t) ((nrow + NR_END) * sizeof(double * *)));
	memset(t,0X0,(size_t) ((nrow + NR_END) * sizeof(double * *)));
	if (!t)
		nrerror("allocation failure 1 in f3tensor()");
	t += NR_END;
	t -= nrl;

	//t[nrl] = (double * *) GlobalAlloc(GMEM_FIXED |
	//						GMEM_ZEROINIT,
	//						(size_t)
	//						((nrow * ncol + NR_END) * sizeof(double *)));
	t[nrl] = (double * *)malloc((size_t)((nrow * ncol + NR_END) * sizeof(double *)));
	memset(t[nrl],0X0,(size_t)((nrow * ncol + NR_END) * sizeof(double *)));
	if (!t)
		nrerror("allocation failure 2 in f3tensor()");
	t[nrl] += NR_END;
	t[nrl] -= ncl;

	//t[nrl][ncl] = (double *)
	//	GlobalAlloc(GMEM_FIXED |
	//		GMEM_ZEROINIT,
	//		(size_t)
	//		((nrow * ncol * ndep + NR_END) * sizeof(double)));
	t[nrl][ncl] = (double *)malloc((size_t)((nrow * ncol * ndep + NR_END) * sizeof(double)));
	memset(t[nrl][ncl],0X0,(size_t)((nrow * ncol * ndep + NR_END) * sizeof(double)));
	if (!t[nrl][ncl])
		nrerror("allocation failure 3 in f3tensor()");
	t[nrl][ncl] += NR_END;
	t[nrl][ncl] -= ndl;

	for (j = ncl + 1; j <= nch; j++)
		t[nrl][j] = t[nrl][j - 1] + ndep;
	for (i = nrl + 1; i < nrl; i++)
	{
		t[i] = t[i - 1] + ncol;
		t[i][ncl] = t[i - 1][ncl] + ncol * ndep;
		for (j = ncl + 1; j <= nch; j++)
			t[i][j] = t[i][j - 1] + ndep;
	}

	return t;
}

void free_fvector(float* v, long nl, long nh)
{
	free((FREE_ARG) (v + nl - NR_END));
}

void free_vector(double* v, long nl, long nh)
{
	free((FREE_ARG) (v + nl - NR_END));
}

void free_ivector(long* v, long nl, long nh)
{
	free((FREE_ARG) (v + nl - NR_END));
}

void free_cvector(unsigned char* v, long nl, long nh)
{
	free((FREE_ARG) (v + nl - NR_END));
}

void free_lvector(unsigned long* v, long nl, long nh)
{
	free((FREE_ARG) (v + nl - NR_END));
}

void free_dvector(double* v, long nl, long nh)
{
	free((FREE_ARG) (v + nl - NR_END));
}

void free_matrix(double** m, long nrl, long nrh, long ncl, long nch)
{
	free((FREE_ARG) (m[nrl] + ncl - NR_END));
	free((FREE_ARG) (m + nrl - NR_END));
}

void free_dmatrix(double** m, long nrl, long nrh, long ncl, long nch)
{
	free((FREE_ARG) (m[nrl] + ncl - NR_END));
	free((FREE_ARG) (m + nrl - NR_END));
}

void free_imatrix(long** m, long nrl, long nrh, long ncl, long nch)
{
	free((FREE_ARG) (m[nrl] + ncl - NR_END));
	free((FREE_ARG) (m + nrl - NR_END));
}

void free_submatrix(double** b, long nrl, long nrh, long ncl, long nch)
{
	free((FREE_ARG) (b + nrl - NR_END));
}

void free_convert_matrix(double** b, long nrl, long nrh, long ncl, long nch)
{
	free((FREE_ARG) (b + nrl - NR_END));
}

void free_f3tensor(double*** t, long nrl, long nrh, long ncl, long nch,
	long ndl, long ndh)
{
	free((FREE_ARG) (t[nrl][ncl] + ndl - NR_END));
	free((FREE_ARG) (t[nrl] + ncl - NR_END));
	free((FREE_ARG) (t + nrl - NR_END));
}


