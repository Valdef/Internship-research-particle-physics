import uproot

root_file = "/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/GEANT4/output/B4_latest0.root"       #directory of root file 
csv_file = "/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/csv_file/beam0.csv"      # directory of the csv file

file = uproot.open(root_file)
df = file["B4_ScintillatorData;1"].arrays(library = "pd")

df = df.sort_values(by = 'eventID') #sort by eventID
df = df[['eventID', 'beamE', 'beamX', 'beamY', 'Edep', 'copyNumberX', 'copyNumberY', 'copyNumberZ']] #only get those columns

df.to_csv(csv_file, index = False) # write file
