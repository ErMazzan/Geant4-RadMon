#!/bin/bash
################################################################################
ROOTfilein="../proton/sphere_GCR/proton_GCR_min.root"
histoName="EDep1-vs-EDep2-Scint1-2"
IsInt=false # Set to true if you want the outputs of your bins to be integer
nameout="EDep1-vs-EDep2-Scint1-2.txt"
###############################################################################

root -l TH2D2txt.C\(\"$ROOTfilein\",\"$histoName\",$IsInt\)

mv outHist.txt $nameout
