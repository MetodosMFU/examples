#!/bin/bash

helpFunction()
{
   echo ""
   echo "Usage: $0 [Energy]"
   echo "    Use $0 gammaEnergy"
   exit 1 # Exit script after printing help
}

[ -z "$1" ] && helpFunction

#Materials G4_CESIUM_IODIDE, G4_POTASSIUM_IODIDE
export SCINTILLATOR_MATERIAL="G4_POTASSIUM_IODIDE"

# CalibrationGamma, CalibrationEu152, Detector, EnvironmentalGamma
export GENERATOR="EnvironmentalGamma"

# Gamma energy keV
export ENERGY="$1"

export NEVENTS=100000000
export SEVENTS=20000

eval "restG4 iodide.rml -e ${SEVENTS}"



