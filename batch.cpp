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
/**
Jan 18,2004
added the read from console if the settings file is not passed in on the command line
added all the model settings under the gui model menu item
migrated the win api memset, memove and memcpy to ansi C equivalents.

*/
//---------------------------------------------------------------------------
#include <unistd.h>
#include "farsite4.h"
#include "portablestrings.h"
#include "sampleMT.h"
#include <omp.h>
#include "TimeCounter.hpp"
#include <sys/time.h>
#include <sys/resource.h>

//#include <dir.h>
//---------------------------------------------------------------------------
static void PrintHelp();

int main(int argc, char* argv[]) {
    char BookMarkArg[256] = "";
    char FarsiteDirectory[256] = "";
    char pathdiv[] = "\\"; // search for path
    long len;
    char *settingsfile="";
    char *resumeFile="";
    char *SaveFileName="";
    char *TimeFileName="";
    char *resolution="";
    int resumeL=0;
    int resumeS=0;
    int startTime=0;
    char o;
    int timeCounter=0;
    FILE *TimeFile;
    struct rlimit rl;
    struct rlimit lc;
    getrlimit (RLIMIT_CPU, &rl);
    getrlimit (RLIMIT_CORE,&lc);
    int cores=0;
    int generation=0;
    int n=0;
    int worker=0;
    int setLimit=0;

    if (argc >=2) {
        while ((o = getopt (argc, argv, "i:r:s:t:l:f:g:n:w:p:")) != -1) {
            switch (o)
            {
            case 'i':
                settingsfile = optarg;
                break;
            case 'p':
                resolution = optarg;
                break;
            case 'r':
                resumeL=1;
                resumeFile = optarg;
                break;
            case 's':
                resumeS=1;
                SaveFileName = optarg;
                break;
            case 't' :
                timeCounter = 1;
                TimeFileName = optarg;
                break;
            case 'l' :
                setLimit=1;
                rl.rlim_cur=atoi(optarg);
                break;
            case 'f':
                cores=atoi(optarg);
                break;
            case 'g':
                generation=atoi(optarg);
                break;
            case 'n':
                n=atoi(optarg);
                break;
            case 'w':
                worker=atoi(optarg);
                break;
            default:
                PrintHelp();
                return 0;
                break;
            }
        }
    } else {
        PrintHelp();
        return 0;
    }
	#ifdef _OPENMP
    if (cores==0) {
        printf("No number of cores specified, using max available.\n");
        cores=omp_get_max_threads();
    }
    omp_set_num_threads(cores);
	#endif

    if(!(access(settingsfile,F_OK)==0)) {
        printf("Non existent setting file or no access.\n");
        return 0;
    }

    if(resumeL && !(access(resumeFile,F_OK)==0)) {
        printf("Non existent resume file or no access.\n");
        return 0;
    }

    if (strlen(argv[0]) != 0) {
        strcpy(FarsiteDirectory, argv[0]);
        strrev(FarsiteDirectory);
        len = strcspn(FarsiteDirectory, pathdiv);
        memset(FarsiteDirectory, 0x0, 256 * sizeof(char));
        strncpy(FarsiteDirectory, argv[0], strlen(argv[0]) - len);
    } else {
        getcwd(FarsiteDirectory,512);
    }

    TFarsiteInterface farsite = TFarsiteInterface("FARSITE 4.0",FarsiteDirectory, BookMarkArg);

    farsite.SetInputsFromFile(settingsfile);

    if(resumeL) { printf("Resume file input:%s\n",resumeFile); }
    if(resumeS) { printf("Resume file output:%s\n",SaveFileName); }

    lc.rlim_cur = lc.rlim_max = 0;
    setrlimit(RLIMIT_CORE,&lc);
    if (setLimit) {
        setrlimit (RLIMIT_CPU, &rl);
    }
    TimeCounter::Instance()->startTS();
    farsite.SetResume(resumeL,resumeS,resumeFile,SaveFileName);
    farsite.InitMainWindow();
    farsite.Terminate();
    TimeCounter::Instance()->stopTS();

    fflush(stdout);

    if( timeCounter ) {
        //-> printf("FARSITE:%d_%d_%d_%f_%f_%d_%s\n",generation,n,cores,TimeCounter::Instance()->getTimeS(),TimeCounter::Instance()->getTime(),worker,resolution);
    }
    return 0;
}

static void PrintHelp() {
    printf("Help:\n");
    printf("farsite4 -i SettingsFileName -r ResumeFileInput -s ResumeFileOutput -p resolution -w worker -l limit -f cores -g generation -n individuo \n");
}

//---------------------------------------------------------------------------
