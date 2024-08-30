import pandas as pd

# --------- all the file -------
Compact_hit_file = "/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/compact_file.csv"         # directory for final file with hit compact on each fiber

csv_file = "/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/combined_file.csv"
df = pd.read_csv(csv_file)  # open csv file
        
# ----- assign variable on the dataframe ------
eventID = df['eventID']
cnX = df['copyNumberX']
cnY = df['copyNumberY']
cnZ = df['copyNumberZ']
Edep = df['Edep']

UniqueID = cnX+cnY+50+cnZ*60 # create a Unique ID with cnX/Y/Z
single_eventID = df.drop_duplicates(subset=['eventID']).sort_values(by='eventID')    # drop duplicate of eventID and sort the value by eventID

# ------- add the Edep for each UniqueID and eventID ------
dataframe = pd.DataFrame({'eventID':eventID, 'Edep':Edep, 'UniqueID':UniqueID})
dataframe = dataframe.groupby(['eventID','UniqueID'], as_index = False)['Edep'].sum()  # add all the Edep together for same UniqueID and eventID
dataframe = dataframe.sort_values(by = ['eventID', 'UniqueID'])
dataframe = dataframe.pivot(index='eventID', columns = 'UniqueID', values ='Edep').fillna(0)    # change the dataframe to be horizontal

# ------ remove unecessary columns (X,Y,Z...) and add the dataframe with uniqueID and Edep -------
single_eventID = single_eventID.drop(single_eventID.columns[4:], axis=1)    # remove the columns unwanted
single_eventID.reset_index(drop=True, inplace=True) # reset index to match dataframe
dataframe.reset_index(drop=True, inplace=True)  # reset index to match single_eventID
single_eventID = pd.concat([single_eventID, dataframe], axis=1)

unwanted_data = (single_eventID['beamX'] < 0.9) & (single_eventID['beamX'] > -0.9) & (single_eventID['beamY'] < 0.9) & (single_eventID['beamY'] > -0.9)
single_eventID = single_eventID[unwanted_data]

single_eventID.to_csv(Compact_hit_file, index=False)

print('\ncalculation have been done on the csv file and Edep have been summed.')