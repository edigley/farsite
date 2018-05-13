#define _GNU_SOURCE
#include <sched.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <papi.h>
#include <math.h>

#define MAX_PROCS 32

// create structs per thread, so that data goes to different cache block lines
struct Proc_INFO {
  long long val[4];
  long long tot[4];
  long long count;
  int       EventSet;
  double    Sum, SumSQR;
} Sample_INFO [MAX_PROCS];

static int N_EVENTS, Ev[4], N_PROCS;
char *EventStr[4];

#define ERROR_RETURN(retval)  { fprintf(stderr, "Error %d %s:line %d: \n", retval,__FILE__,__LINE__);  exit(retval); }

static inline char *GETENV(char *envstr)
{
   char *env = getenv(envstr);
   if (!env) return "0";
   else return env;
}

int proc_get_num_cpus (void) {
    return sysconf(_SC_NPROCESSORS_ONLN);
}

void Sample_SetCPU (int cpu_id)
{
    cpu_set_t   cpu_set;
    CPU_ZERO (&cpu_set);
    CPU_SET  (cpu_id, &cpu_set);
    sched_setaffinity (0, sizeof (cpu_set), &cpu_set);
}


void Sample_ON (int i) {
  struct Proc_INFO *pInfo = Sample_INFO + i;

  if (pInfo->EventSet == PAPI_NULL) {  // register events in executing processor core
    int j, retval;
    if ((retval= PAPI_create_eventset(&(pInfo->EventSet))) != PAPI_OK)
      ERROR_RETURN(retval);
    for (j=0; j<N_EVENTS; j++) 
      if((retval = PAPI_add_event(pInfo->EventSet, Ev[j])) != PAPI_OK)
        ERROR_RETURN(retval);
  }
  PAPI_start(pInfo->EventSet);
}

void Sample_OFF (int i) {
  struct Proc_INFO *pInfo = Sample_INFO + i;

  PAPI_stop(pInfo->EventSet, pInfo->val);
  pInfo->tot[0] += pInfo->val[0];
  pInfo->tot[1] += pInfo->val[1];
  pInfo->tot[2] += pInfo->val[2];
  pInfo->tot[3] += pInfo->val[3];
  pInfo->count++;
}


void Sample_Value (int i, long long V) {
  struct Proc_INFO *pInfo= Sample_INFO + i;

  pInfo->Sum += V; pInfo->SumSQR += (double) V * (double) V;

  if (pInfo->count) {  
    if (V > pInfo->tot[1]) pInfo->tot[1] = V;  // maximum V
    if (V < pInfo->tot[2]) pInfo->tot[2] = V;  // minimum V
    if (V == pInfo->tot[3]) pInfo->val[0]++;   // reinforce V
    else if (pInfo->val[0]) pInfo->val[0]--;   // weaken V
    else {
      pInfo->tot[3] = V;  // new most frequent V
      pInfo->val[0] = 1;
    }
  }
  else {
    pInfo->tot[1] = V;  // maximum V
    pInfo->tot[2] = V;  // minimum V
    pInfo->tot[3] = V;  // most frequent V
    pInfo->val[0] = 1;
  }
  pInfo->count++;
}


void Sample_ValueDiff (int i, long long V) {
  struct Proc_INFO *pInfo= Sample_INFO + i;
  long long dV;

  dV = V - pInfo->tot[0];
  pInfo->tot[0] = V;  // store current V as old V
  pInfo->count++;

  if (pInfo->count == 1) {
    if (pInfo->val[0] == 0) { // filter first case
      pInfo->val[0] = 1;
      pInfo->count = 0;
      return;
    }
    // first difference
    pInfo->tot[1] = dV;  // maximum dV
    pInfo->tot[2] = dV;  // minimum dV
    pInfo->tot[3] = dV;  // most frequent dV
    pInfo->val[0] = 0;   // will be increased in brief
  }

  if (dV > pInfo->tot[1])  pInfo->tot[1] = dV;  // maximum dV
  if (dV < pInfo->tot[2])  pInfo->tot[2] = dV;  // minimum dV
  if (dV == pInfo->tot[3]) pInfo->val[0]++;     // reinforce dV
  else if (pInfo->val[0]) pInfo->val[0]--;      // weaken dV
  else {
    pInfo->tot[3] = dV;  // new most frequent dV
    pInfo->val[0] = 1;
  }

  dV = llabs(dV);
  pInfo->Sum += dV; pInfo->SumSQR += (double) dV * (double) dV;
}


void read_events() 
{
  int retval, i;

  N_EVENTS=atoi(GETENV("PAPI_EVENTS"));
  if (!N_EVENTS) {
    Ev[0] = PAPI_TOT_CYC;
    Ev[1] = PAPI_TOT_INS;
    EventStr[0] = "PAPI_TOT_CYC";
    EventStr[1] = "PAPI_TOT_INS";
    N_EVENTS = 2;
    return;
  }
  i=N_EVENTS;
  EventStr[0]=GETENV("PAPI_EV0");
  if ((retval = PAPI_event_name_to_code( EventStr[0], Ev))!= PAPI_OK )
      ERROR_RETURN(retval);
  if (!--i) return;
  EventStr[1]=GETENV("PAPI_EV1");
  if ((retval = PAPI_event_name_to_code( EventStr[1], Ev+1))!= PAPI_OK )
      ERROR_RETURN(retval);
  if (!--i) return;
  EventStr[2]=GETENV("PAPI_EV2");
  if ((retval = PAPI_event_name_to_code( EventStr[2], Ev+2))!= PAPI_OK )
      ERROR_RETURN(retval);
  if (!--i) return;
  EventStr[3]=GETENV("PAPI_EV3");
  if ((retval = PAPI_event_name_to_code( EventStr[3], Ev+3))!= PAPI_OK )
      ERROR_RETURN(retval);
  if (!--i) return;
 
  printf("Too much EVENTS (PAPI_EVENTS <= 4!)\n");
}


void Sample_Init() {
  int i;

  if (PAPI_library_init(PAPI_VER_CURRENT)!= PAPI_VER_CURRENT) {
      printf("Library initialization error!\n");
      exit(1);
  }

  N_PROCS= proc_get_num_cpus();
  read_events();

  for (i=0; i<N_PROCS; i++) {
    struct Proc_INFO *pInfo= Sample_INFO + i;

    pInfo->tot[0]   = 0;
    pInfo->tot[1]   = 0;
    pInfo->tot[2]   = 0;
    pInfo->tot[3]   = 0;
    pInfo->val[0]   = 0;
    pInfo->val[1]   = 0;
    pInfo->val[2]   = 0;
    pInfo->val[3]   = 0;
    pInfo->count    = 0;
    pInfo->EventSet = PAPI_NULL;
    pInfo->Sum      = 0.0;
    pInfo->SumSQR   = 0.0;
  }
}

void Sample_End(void)
{
  int i, j;

  //printf("Total Processors: %d\n", N_PROCS);

  for (i=0; i<N_PROCS; i++) {
    struct Proc_INFO *pInfo= Sample_INFO + i;
    printf("T=%d=Processor=%d=Ncount=%2.0f=",N_PROCS, i, (double) pInfo->count);
    if (pInfo->EventSet != PAPI_NULL) // dump registered events
      for (j=0; j<N_EVENTS; j++) 
         printf("%s=%12.0f=", EventStr[j], (double) pInfo->tot[j]);
    else if (pInfo->count) {
      printf("Min= %9.0f. Max= %9.0f. MstFreq= %9.0f (%3.2f%%). ", 
           (double) pInfo->tot[2], (double) pInfo->tot[1], 
           (double) pInfo->tot[3], 100.0 * (double) pInfo->val[0] / (double) pInfo->count);
      printf("Avrg= %9.0f. Var= %9.0f. ",
           pInfo->Sum / pInfo->count,
           sqrt(fabs((pInfo->SumSQR - (pInfo->Sum*pInfo->Sum)/pInfo->count))));
    }
    printf("\n");
  }

  PAPI_shutdown();	
}
