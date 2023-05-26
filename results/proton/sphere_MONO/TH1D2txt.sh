#!/bin/bash
################################################################################
ROOTfilein="100-MeV.root"
histoName="EScint4"
IsInt="false" # Set to true if you want the outputs of your bins to be integer
nameout="100-MeV.txt"
###############################################################################

root -q -b TH1D2txt.C\(\"$ROOTfilein\",\"$histoName\",\"$IsInt\"\)

mv outHist.txt EScint4/$nameout
