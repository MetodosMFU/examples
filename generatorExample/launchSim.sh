#!/bin/bash

helpFunction()
{
   echo ""
   echo "Usage: $0 [CosmicMuons, CosmicProtons, CosmicGammas]"
   exit 1 # Exit script after printing help
}

[ -z "$1" ] && helpFunction

if [ "$1" = "CosmicMuons" ] || [ "$1" = "CosmicProtons" ] || [ "$1" = "CosmicGammas" ]; then
    echo "Using $1 generator"
else
  helpFunction
fi

#Materials G4_CESIUM_IODIDE, G4_POTASSIUM_IODIDE,...
export SCINTILLATOR_MATERIAL="G4_CESIUM_IODIDE"

# CalibrationGamma, DetectorGamma, EnvironmentalGamma, DetectorIsotope, Source
export GENERATOR=$1

export NEVENTS=0
export SEVENTS=1000

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

eval "restG4 ${SCRIPTPATH}/iodide.rml -e ${SEVENTS}"



