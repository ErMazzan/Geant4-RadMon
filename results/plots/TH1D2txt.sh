#!/bin/bash

particle_name="proton"
particle_source="beam"

declare -a EDepString=("EDep2-Scint1-2" "EDep3-Scint1-2-3" "EDep3-Scint1-2-3-4" "ESumCoinc1-2" "ESumCoinc1-2-3" "ESumCoinc1-2-3-4")
# declare -a EDepString=("EDep1-Scint1-2" "EDep2-Scint1-2" "EDep2-Scint1-2-3"
#                       "EDep3-Scint1-2-3" "EDep2-Scint1-2-3-4"
#                       "EDep3-Scint1-2-3-4")
                       
declare -a EnergiesString=("100-MeV" "200-MeV" "300-MeV" "400-MeV" "500-MeV" "700-MeV"
                           "1-GeV" "2-GeV")
 
#declare -a EnergiesString=("proton_GCR_min")

################################################################################
IsInt="false" # Set to true if you want the outputs of your bins to be integer
###############################################################################

for Energy in "${EnergiesString[@]}"
do
    echo "$Energy"
    ROOTfilein="../$particle_name/$particle_source/$Energy.root"

    for EDep in "${EDepString[@]}"
    do
        echo "$EDep"
        histoName="$EDep"
        nameout="$EDep.txt"
        root -q -b TH1D2txt.C\(\"$ROOTfilein\",\"$histoName\",\"$IsInt\"\)
        mv outHist.txt EDep/$particle_name/$particle_source/$Energy/$nameout
    done

done
