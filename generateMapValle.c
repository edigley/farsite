#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

/* NOTE 1: Change these to modify map size & resolution. */

static int    Rows   = 266;     /* Number of rows in each map. */
static int    Cols   = 382;     /* Number of columns in each map. */
static int    Q1   = 0;     
static int    Q2   = 76;
static int    Q3   = 152;     
static int    Q4   = 229;
static int    Q5   = 305;     
static int    Q6   = 381;     

FILE *elevfile;          	/* file that contains elevation map */
FILE *slopefile;         	/* file that contains slope map */
FILE *aspectfile;        	/* file that contains aspect map */
FILE *fuelfile;        	    /* file that contains fuel map */
FILE *canopyfile;

char *elevfilename;         /* filename of elevation map OPTION -e */
char *slopefilename;        /* filename of slope map OPTION -s */
char *aspectfilename;       /* filename of aspect map OPTION -a */
char *fuelfilename;         /* filename of aspect map OPTION -f */
char *canopyfilename;

static void PrintHelp();

int main ( int argc, char **argv )
{
	int c;
	extern char *optarg;
	extern int optind, optopt;
	char *pattern = (char*)"esah";
	int elev 	= 0;
	int slope 	= 0;
	int aspect 	= 0;
	int fuel = 0;
	int fuelT = 3;
	int cano = 0;
	//int Rows = 0;
	//int Cols = 0;

	while ((c = getopt (argc, argv, "e:s:a:h:c:r:f:t:")) != -1)
	{
		 switch (c)
		   {
		   case 'e':
		     elevfilename = optarg;
		     elev = 1;
		     break;
		   case 's':
		     slopefilename = optarg;
		     slope = 1;
		     break;
		   case 'a':
		     aspectfilename = optarg;
		     aspect = 1;
		     break;
		    case 'f':
		     fuelfilename = optarg;
		     fuel = 1;
		     break;
		   case 'c':
		     canopyfilename = optarg;
		     cano = 1;
		     break;
		   case 't':
		     fuelT = atoi(optarg);
		     break;
		   /*case 'c':
				Cols = (int)optarg;
			 break;
		   case 'r':
				Rows = (int)optarg;
			 break;
			 */
		   case '?':
		     if (optopt == 'h')
		     {
			PrintHelp();
			exit( 0 );
		     }
		     else if (strchr(pattern, optopt))
		       fprintf (stderr, "La opción -%c requiere un argumento.\n", optopt);
		     else if (isprint (optopt))
		       fprintf (stderr, "Opción desconocida `-%c'.\n", optopt);
		     else
		       fprintf (stderr, "Carácter de opción desconocido `\\x%x'.\n", optopt);
		     return 1;
		   default:
		     abort ();
		   }
	 }			

      if( elev == 1 && ( elevfile = fopen( elevfilename, "w+")) == NULL) {
		puts("No se puede abrir el fichero de elevación");
		exit( 0 );
	}
	if( slope == 1 && ( slopefile = fopen( slopefilename, "w+")) == NULL) {
		puts("No se puede abrir el fichero de slope");
		exit( 0 );
	}
	if( aspect == 1 && ( aspectfile = fopen( aspectfilename, "w+")) == NULL) {
		puts("No se puede abrir el fichero de aspect");
		exit( 0 );
	}
	if( fuel == 1 && ( fuelfile = fopen( fuelfilename, "w+")) == NULL) {
		puts("No se puede abrir el fichero de fuel");
		exit( 0 );
	}
	if( cano == 1 && ( canopyfile = fopen( canopyfilename, "w+")) == NULL) {
		puts("No se puede abrir el fichero de fuel");
		exit( 0 );
	}
	int i=0;
	int j=0;
	int elev_tmp = 0;
	
	fprintf(elevfile, "ncols %d\nnrows %d \nxllcorner 157819.328125\nyllcorner 384678.625\ncellsize 30\nNODATA_value  -9999\n",Cols,Rows);
	fprintf(aspectfile, "ncols %d\nnrows %d \nxllcorner 157819.328125\nyllcorner 384678.625\ncellsize 30\nNODATA_value  -9999\n",Cols,Rows);
	fprintf(slopefile, "ncols %d\nnrows %d \nxllcorner 157819.328125\nyllcorner 384678.625\ncellsize 30\nNODATA_value  -9999\n",Cols,Rows);
	fprintf(fuelfile, "ncols %d\nnrows %d \nxllcorner 157819.328125\nyllcorner 384678.625\ncellsize 30\nNODATA_value  -9999\n",Cols,Rows);
	fprintf(canopyfile, "ncols %d\nnrows %d \nxllcorner 157819.328125\nyllcorner 384678.625\ncellsize 30\nNODATA_value  -9999\n",Cols,Rows);
	
	
	for(i=0;i<Rows;i++)
	{
		for(j=0;j<Cols;j++)
		{
			fprintf(canopyfile, " -9999");  
			if(j<=Q1)
			{
				fprintf(elevfile, " 0");
				fprintf(slopefile, " 5");
				fprintf(aspectfile, " 90");
				fprintf(fuelfile," %d", fuelT);
				elev_tmp += 5;
			} 
			else if(j<=Q2)
			{
				fprintf(elevfile, " %d", elev_tmp);
				fprintf(slopefile, " 10");
				fprintf(aspectfile, " 90");
				fprintf(fuelfile," %d", fuelT);
				elev_tmp += 10;
			}
			else if(j<=Q3)
			{
				fprintf(elevfile, " %d", elev_tmp);
				fprintf(slopefile, " 15");
				fprintf(aspectfile, " 90");
				fprintf(fuelfile," %d", fuelT);
				elev_tmp += 15;
			}
			else if(j<=Q4)
			{
				fprintf(elevfile, " %d", elev_tmp);
				fprintf(slopefile, " 20");
				fprintf(aspectfile, " 90");
				fprintf(fuelfile," %d", fuelT);
				elev_tmp += 20;
			}
			else if(j<=Q5)
			{
				fprintf(elevfile, " %d", elev_tmp);
				fprintf(slopefile, " 25");
				fprintf(aspectfile, " 90");
				fprintf(fuelfile," %d", fuelT);
				elev_tmp += 25;
			}
			else
			{
				fprintf(elevfile, " 0");
				fprintf(slopefile, " 0");
				fprintf(aspectfile, " 90");
				fprintf(fuelfile," %d", fuelT);
			}
		}
		elev_tmp = 0;
		fprintf(elevfile, "\n");
		fprintf(slopefile, "\n");
		fprintf(aspectfile, "\n");
		fprintf(fuelfile, "\n");
		fprintf(canopyfile, "\n");
	} 

	fclose(elevfile);
	fclose(slopefile);
	fclose(aspectfile);
	fclose(fuelfile);
	fclose(canopyfile);

	return 0;
}

/*(Carlos B.)*/
static void PrintHelp()
{
	printf("*****************************************************************************\n");
	printf("*                              USO DEL PROGRAMA                             *\n");
	printf("*                                                                           *\n");
	printf("* $./generateMapValle -options arguments                                    *\n");
	printf("*                                                                           *\n");
	printf("*OPTION LIST                                                                *\n");
	printf("*                                                                           *\n");
	printf("* -e arg	Elevation Map input file (REQUIRED)                         		*\n");
	printf("* -s arg	Slope Map input file (REQUIRED)                             		*\n");
	printf("* -a arg	Aspect Map input file (REQUIRED)                            		*\n");
	printf("* -a arg	Fuel Map input file (REQUIRED)                            		*\n");
	printf("* -r arg	Rows, integer (REQUIRED)                            		*\n");
	printf("* -c arg	Columns, integer (REQUIRED)                            		*\n");
	printf("* -h		Display this help                                           		*\n");
	printf("*                                                                           *\n");
	printf("*****************************************************************************\n");	
}
