#######################################################################

TAUROOTDIR      = /home/tomas/soft/tau

##include $(TAUROOTDIR)/x86_64/lib/Makefile.tau-pdt-openmp-opari-trace

CXX             = $(TAU_CXX)

CC              = $(TAU_CC)

CFLAGS          = $(TAU_INCLUDE) $(TAU_DEFS) -fPIC  -DUNIX -mtune=native -march=native

CFLAGS2         = -w -O0 -pg -fPIC  -DUNIX -fopenmp -fpermissive

CFLAGS3         =  -w -O3 -fPIC  -DUNIX -fopenmp -fpermissive -mtune=native -march=native

CFLAGS4         = -w -O3 -fPIC  -DUNIX -mtune=native -march=native -fpermissive

CCOMPP          = kinst-ompp

LIBS            = $(TAU_LIBS)

LDFLAGS         =  $(USER_OPT) -lncurses  -mtune=native -march=native

LDFLAGS2        =  -lncurses  -mtune=native -march=native -L/home/tomas/soft/gperftool/lib

LDFLAGS3        = -lncurses  -mtune=native -march=native

MAKEFILE        = Makefile

PRINT           = pr

RM              = /bin/rm -f

EXTRAOBJS       =

##############################################



BASE=/usr
TARGET_LIBDIR=$(BASE)/lib


export TAU_OPTIONS=-optCompInst -optTauSelectFile="select"
export TAU_MAKEFILE=/home/tomas/soft/tau/x86_64/lib/Makefile.tau-pdt-openmp-opari-trace
#export TAU_OPTIONS=-optPDTInst
#export TAU_MAKEFILE=/home/tomas/soft/tau/x86_64/lib/Makefile.tau-callpath-papi-pdt-profile-trace

CC4=g++
CC5=scalasca -instrument g++
CXX=tau_cxx.sh -tau_makefile=$(TAU_MAKEFILE) -optCompInst -optTauSelectFile="select"


LD_EXE= -lfarsite
SOURCES=batch.cpp portablestrings.cpp farsite4.cpp fcopy.cpp fsairatk.cpp \
        fsxwaccl.cpp fsxw.cpp fsxwignt.cpp fsxwspot.cpp shpopen.cpp \
        burnupw.cpp fsxwatm.cpp fsxwcrwn.cpp fsxwinpt.cpp fsxwutil.cpp \
        newfms.cpp vec.cpp crossthread.cpp fsglbvar.cpp fsxwattk.cpp \
        fsxwenvt2.cpp fsxwmech.cpp fsxwvect.cpp dbfopen.cpp fsxpfront.cpp \
        fsxwbar.cpp fsxwfms2.cpp fsxwrast.cpp gridthem.cpp nrutil.cpp \
        fsxsync.cpp fsxwburn4.cpp fsxwfotp.cpp fsxwshap.cpp newclip.cpp TimeCounter.cpp \
        rand2.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=farsite4P
EXECUTABLES=farsite4S
TARGET= $(SOURCES)



test:
	$(CC4) $(SOURCES) $(CFLAGS3) $(LDFLAGS3) -o farsite4P_test

gprof:
	$(CC4) $(SOURCES) $(CFLAGS2) $(LDFLAGS) -o $(EXECUTABLE)

normal:
	$(CC4) $(SOURCES) $(CFLAGS3) $(LDFLAGS3) -o $(EXECUTABLE)
ompp:
	$(CCOMPP) $(CC4) $(SOURCES)  $(CFLAGS3) $(LDFLAGS3) -o $(EXECUTABLE)

serial:
	$(CC4) $(SOURCES) $(CFLAGS4) $(LDFLAGS3) -o $(EXECUTABLES)
scalasca:
	$(CC5) $(SOURCES) $(CFLAGS) $(LDFLAGS) -o $(EXECUTABLE)
tau:
	$(CXX) $(SOURCES)  -c $(CFLAGS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE) $(LIBS)


clean:
	rm *opari*
	rm $(EXECUTABLE)
	rm $(EXECUTABLES)	
	rm *.o
	rm *.pdb
	sudo rm /usr/lib/libfarsite.so

