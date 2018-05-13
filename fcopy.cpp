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
 * FCOPY.C - copy one file to another.  Returns the (positive)
 *           number of bytes copied, or -1 if an error occurred.
 * by: Bob Jarvis
 */

#include <stdio.h>
#include <stdlib.h>
#include "fcopy.h"

#define BUFFER_SIZE 1024

int fcopy(char *dest, char *source)
{
        FILE *d, *s;
        char *buffer;
        size_t incount;
        long totcount = 0L;

        s = fopen(source, "rb");
        if(s == NULL)
                return -1L;

        d = fopen(dest, "wb");
        if(d == NULL)
        {
                fclose(s);
                return -1L;
        }

        buffer = (char *) malloc(BUFFER_SIZE);
        if(buffer == NULL)
        {
                fclose(s);
                fclose(d);
                return -1L;
        }

        incount = fread(buffer, sizeof(char), BUFFER_SIZE, s);

        while(!feof(s))
        {
                totcount += (long)incount;
                fwrite(buffer, sizeof(char), incount, d);
                incount = fread(buffer, sizeof(char), BUFFER_SIZE, s);
        }

        totcount += (long)incount;
        fwrite(buffer, sizeof(char), incount, d);

        free(buffer);
        fclose(s);
        fclose(d);

        return totcount;
}


