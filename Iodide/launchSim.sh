#!/bin/bash

helpFunction()
{
   echo ""
   echo "Usage: $0 [singleGamma, multipleGamma, viewGeometry]"
   echo "    Use $0 singleGamma for a single gamma energy"
   echo "    Use $0 multipleGamma for multiple gamma lines"
   echo "    Use $0 viewGeometry for geometry visualization"
   exit 1 # Exit script after printing help
}

[ -z "$1" ] && helpFunction

#Materials G4_CESIUM_IODIDE, G4_POTASSIUM_IODIDE
export SCINTILLATOR_MATERIAL="G4_CESIUM_IODIDE"

# CalibrationGamma, CalibrationEu152, Detector, EnvironmentalGamma
export GENERATOR="CalibrationEu152"

# Gamma energy keV
export ENERGY="1460"

export NEVENTS=100000
export SEVENTS=1000

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

if [[ "$1" == "singleGamma" ]] then
  eval "restG4 ${SCRIPTPATH}/iodide.rml -e ${SEVENTS}"
elif [[ "$1" == "multipleGamma" ]] then

#Note take into account the branching ratio (energy in keV)
Th232Gammas=(238.63 727.18 583.19 860.56 2614.53 383.32 911.02 968.97)
U238Gammas=(241.91 295.17 351.9 609.31 1120.27 1764.51 1001.00)
Eu152Gammas=(121.7817 244.6974 295.9387 344.2785 367.7891 411.1165 443.965 563.99 688.67 778.9045 867.38 964.079 1085.837 1089.737 1112.076 1212.948 1299.142 1408.013)

  for N in ${Eu152Gammas[@]}
  do
    export ENERGY="$N"
    echo "Energy $ENERGY"
    eval "restG4 ${SCRIPTPATH}/iodide.rml -e ${SEVENTS}"
  done
elif [[ "$1" == "viewGeometry" ]] then
  cd $SCRIPTPATH/geometry/
  eval "root -l ../viewGeometry.C"
  cd -
else
  helpFunction
fi



