#!/bin/bash


Beam = "e-"

Unit = "GeV"

#############################################################################

ScriptDir="/home/userfs/g/gbx505/Documents/Geant4Examples/basic/B4/B4c/build"
cd $ScriptDir
Output="/home/userfs/g/gbx505/Documents/Geant4Examples/basic/B4/B4c/Ouput/"

#############################################################################


for beamEnergy in 0.5, 1, 2, 3.5, 5, 10, 15, 18; do

    echo $beamEnergy" "$Unit

        rm run_"$beamEnergy$Unit".mac

        touch run_"$beamEnergy""$Unit".mac

        echo /run/initialize >> run_"$beamEnergy""$Unit".mac

        echo '/gun/particle '$Beam >> run_"$beamEnergy""$Unit".mac

        echo "/gun/energy "$Energy" "$Unit >> run_"$beamEnergy""$Unit".mac

        echo "/tracking/verbose 0" >> run_"$beamEnergy""$Unit".mac

        echo '/run/printProgress 10000' >> run_"$beamEnergy""$Unit".mac

        echo '/run/beamOn 50000' >> run_"$beamEnergy""$Unit".mac   

done


for beamEnergy in 0.5, 1, 2, 3.5, 5, 10, 15, 18; do

   echo "Output_"$Energy$Unit".root"

      test ()
      {

         ./exampleB4c -m run_"$beamEnergy""$Unit".mac -t 1
      }
   test

   wait

   mv B4.root $Output "/" "Output_"$Energy$Unit".root"

done
