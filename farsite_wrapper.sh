#!/bin/bash

version=$1
nCores=$2
simulationFile=$3

farsiteMPI="/home/sgeadmin/two_stage_prediction_framework/farsite/farsite4P"
farsiteSerial="/home/sgeadmin/two_stage_prediction_framework/farsite/farsite4P"

if [ "${version}" = "mpi" ]
then
    mpirun -np ${nCores} ${farsiteMPI} -f ${nCores} -i ${simulationFile} >/dev/null 2>&1
else
    ${farsiteSerial} -f ${nCores} -i ${simulationFile} >/dev/null 2>&1
fi

exit $?

