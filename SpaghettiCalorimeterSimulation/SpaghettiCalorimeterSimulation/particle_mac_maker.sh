#!/bin/bash


#############################################################################
# Change these for interaction


Beam="gamma"
#ProtonNumber=6
#AtomicNumber=12

############################alpha_On_Paper_Output_9MeV.root##### Beam #####################################
#Defines unit (in terms of keV or MeV as needed by the beam)
Unit="keV"
# Beam types:
# e-, e+, gamma, neutron, pi0, pi+, proton, alpha
# Leptons: e±, μ±, ....
# Bosons: G4Gamma, G4OpticalPhoton, ....
# Geantino is a particle without any interaction
# “Stable” hadrons: π±, K±, ....
################################# Target #####################################

Target="Lead"

ScriptDir="/shared/storage/physhad/JLab/mn688/YorkMedPhys_Copy/"$Target"/build"
cd $ScriptDir
Output="/shared/storage/physhad/JLab/mn688/YorkMedPhys_Copy/Output/"$Target

################################# Interaction #####################################

# Define interaction, beam and target
Interaction="gamma_On_"$Target
#Interaction=$Beam"_On_"$Target
# Create directory for output if it doesn't exist
mkdir $Output
mkdir $Output'/'$Interaction
mkdir $Output'/'$Interaction'/CSV'

# Energy steps
declare -i Energy_Steps=200
declare -i Energy_Start=200

#############################################################################

for NUMBER in `seq $1 $2`;


do
   declare -i Energy=$Energy_Start
   declare -i Increase=$NUMBER*$Energy_Steps
   Energy=$Energy+$Increase
# $[$a+$b]
echo $Energy" "$Unit

    rm particle_"$NUMBER".mac

    touch particle_"$NUMBER".mac

    echo '/run/initialize' >> particle_"$NUMBER".mac

    echo '/gun/particle '$Beam >> particle_"$NUMBER".mac

    echo "/gun/energy "$Energy" "$Unit >> particle_"$NUMBER".mac

    echo '/run/printProgress 10000' >> particle_"$NUMBER".mac

    echo '/run/beamOn 100000' >> particle_"$NUMBER".mac


########################Ion Setup###########################################
   #rm ion_"$NUMBER".mac

   #touch ion_"$NUMBER".mac

   #echo '/run/initialize' >> ion_"$NUMBER".mac

   #echo '/gun/ion '"$ProtonNumber" "$AtomicNumber" >> ion_"$NUMBER".mac

   #echo "/gun/energy "$Energy" "$Unit >> ion_"$NUMBER".mac

   #echo '/run/printProgress 1000' >> ion_"$NUMBER".mac

   #echo '/run/beamOn 100000' >> ion_"$NUMBER".mac
done


for NUMBER in `seq $1 $2`;

do

   declare -i Energy=$Energy_Start
   declare -i Increase=$NUMBER*$Energy_Steps
   Energy=$Energy+$Increase

   echo $Interaction"_Output_"$Energy$Unit".root"

      test ()
      {

         ./YorkMedPhys -m particle_$NUMBER.mac -t 1
      }
   test

   wait

   mv YorkMedPhys.root $Output'/'$Interaction"/"$Interaction"_Output_"$Energy$Unit".root"

done
