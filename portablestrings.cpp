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
/*
 *  Portable, public domain strupr() & strlwr()
 */
#ifdef UNIX

#include <ctype.h>
#include <string.h>
#ifndef __PORTABLESTRINGS__
#define __PORTALBESTRINGS__  1
int min(int a, int b){
	if(a>b){
	  return b;
	}
	return a;
}
double min(double a, double b){
	if(a>b){
	  return b;
	}
	return a;
}
long min(long a, long b){
	if(a>b){
	  return b;
	}
	return a;
}
double max(double a, double b){
	if(a<b){
	  return b;
	}
	return a;
}
long max(long a, long b){
	if(a<b){
	  return b;
	}
	return a;
}

char *strupr(char *str)
{
      char *string = str;

      if (str)
      {
            for ( ; *str; ++str)
                  *str = toupper(*str);
      }
      return string;
}

char *strlwr(char *str)
{
      char *string = str;

      if (str)
      {
            for ( ; *str; ++str)
                  *str = tolower(*str);
      }
      return string;
}

/*
**  STRREV.C - reverse a string in place
**
**  public domain by Bob Stout
*/


char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}

void getNameFromFile(char* source, char* filename,int length){
   strcpy(filename,strrchr(source,'/'));
}


/*#include <stdio.h>

int maintest(int argc, char *argv[])
{
      while (--argc)
      {
            printf("\\n"%s\" backwards is ", *++argv);            printf("\\n"%s\"\n", strrev(*argv));      }
}
 */
#endif /*portable strings*/
#endif /* UnixPort */
