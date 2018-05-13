#!/bin/bash

CCOMPP=${1}
CC4=""
SOURCES="batch.cpp portablestrings.cpp farsite4.cpp fcopy.cpp fsairatk.cpp fsxwaccl.cpp fsxw.cpp fsxwignt.cpp fsxwspot.cpp shpopen.cpp burnupw.cpp fsxwatm.cpp fsxwcrwn.cpp fsxwinpt.cpp fsxwutil.cpp newfms.cpp vec.cpp crossthread.cpp fsglbvar.cpp fsxwattk.cpp fsxwenvt2.cpp fsxwmech.cpp fsxwvect.cpp dbfopen.cpp fsxpfront.cpp fsxwbar.cpp fsxwfms2.cpp fsxwrast.cpp gridthem.cpp nrutil.cpp fsxsync.cpp fsxwburn4.cpp fsxwfotp.cpp fsxwshap.cpp newclip.cpp TimeCounter.cpp rand2.cpp"
CFLAGS3="-w -O3 -fPIC  -DUNIX -fopenmp -fpermissive -mtune=native -march=native"
LDFLAGS3="-lncurses  -mtune=native -march=native"
EXECUTABLE=${2}

${CCOMPP} ${CC4} ${SOURCES} ${CFLAGS3} ${LDFLAGS3} -o ${EXECUTABLE}

exit 0

