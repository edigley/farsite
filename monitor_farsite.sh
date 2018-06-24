#!/bin/bash

version=${1}
nCores=${2}
nRepetitions=${3}
simulationFile=${4}
timestep=${5}
perimeterResolution=${6}
distanceResolution=${7}
outputFile=${8}

nodeName=$(hostname)
currentDate=$(date +"%Y-%m-%d")
modelName=$(grep "model name" /proc/cpuinfo | tail -1 | cut -d' ' -f3-)
nProcessors=$(grep "model name" /proc/cpuinfo | wc -l)

instanceId=$(curl -s http://169.254.169.254/latest/meta-data/instance-id)
amiId=$(curl -s http://169.254.169.254/latest/meta-data/ami-id)
instanceType=$(curl -s http://169.254.169.254/latest/meta-data/instance-type)
kernelId=$(curl -s http://169.254.169.254/latest/meta-data/kernel-id)
placement=$(curl -s http://169.254.169.254/latest/meta-data/placement/)
reservationId=$(curl -s http://169.254.169.254/latest/meta-data/reservation-id)
availabilityZone=$(curl -s http://169.254.169.254/latest/meta-data/placement/availability-zone)

settings="settings.txt"
cat ${simulationFile} > ${settings}

sed -i "s/timestep = 10m/timestep=${timestep}/g" ${settings}
sed -i "s/perimeterResolution = 30m/perimeterResolution=${perimeterResolution}/g" ${settings}
sed -i "s/distanceResolution = 30m/distanceResolution=${distanceResolution}/g" ${settings}

farsiteWrapper="/home/sgeadmin/two_stage_prediction_framework/farsite/farsite_wrapper.sh"

header="timestampF timestamp timestep perimeterResolution distanceResolution repetition nCores node availabilityZone instanceType instanceId modelName nProcessors date runtime runtimeF maxRSS nFSOutputs percentageCPU nContextSwitchs exitStatus"

echo ${header} > ${outputFile}
for repetition in `seq 1 ${nRepetitions}`; do 
    timestamp=$(date "+%s")
    timestampF=$(date "+%Y-%m-%d %H:%M:%S")
    printf "\"%s\" %d \"%s\" \"%s\" \"%s\" %d %d \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" %s \"%s\" "  "${timestampF}" ${timestamp} ${timestep} ${perimeterResolution} ${distanceResolution} ${repetition} ${nCores} "${nodeName}" "${availabilityZone}" "${instanceType}" "${instanceId}" "${modelName}" ${nProcessors} "${currentDate}"  >> ${outputFile}
    (/usr/bin/time --format " %e %E %M %O %P %c %x " ${farsiteWrapper} ${version} ${nCores} ${settings}) >> ${outputFile} 2>&1
done

exit 0;

