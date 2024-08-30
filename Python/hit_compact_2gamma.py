import pandas as pd

# --------- All the file -------
compact_hit_file = "/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/compact_file.csv"  # Directory for final file with hit compact on each fiber

csv_file = "/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/combined_file.csv"
df = pd.read_csv(csv_file)  # Open CSV file

# ----- Assign variables on the DataFrame ------
# Ensure that all required columns exist in the DataFrame
required_columns = ['eventID', 'copyNumberX', 'copyNumberY', 'copyNumberZ', 'Edep']
for col in required_columns:
    if col not in df.columns:
        raise ValueError(f"Missing required column: {col}")

eventID = df['eventID']
cnX = df['copyNumberX']
cnY = df['copyNumberY']
cnZ = df['copyNumberZ']
Edep = df.get('Edep', pd.Series([0] * len(df)))  # Default to 0 if 'Edep' is missing

# Create a Unique ID with cnX/Y/Z
UniqueID = cnX + cnY + 50 + cnZ * 60
single_eventID = df.drop_duplicates(subset=['eventID']).sort_values(by='eventID')  # Drop duplicates of eventID and sort by eventID

# ------- Add the Edep for each UniqueID and eventID ------
dataframe = pd.DataFrame({'eventID': eventID, 'Edep': Edep, 'UniqueID': UniqueID})
dataframe = dataframe.groupby(['eventID', 'UniqueID'], as_index=False)['Edep'].sum()  # Sum Edep for same UniqueID and eventID
dataframe = dataframe.sort_values(by=['eventID', 'UniqueID'])
dataframe = dataframe.pivot(index='eventID', columns='UniqueID', values='Edep').fillna(0)  # Pivot to horizontal format

# ------ Remove unnecessary columns (X, Y, Z...) and add the DataFrame with UniqueID and Edep -------
single_eventID = single_eventID.drop(single_eventID.columns[4:], axis=1, errors='ignore')  # Remove columns beyond the first four, ignore errors if columns are missing
single_eventID.reset_index(drop=True, inplace=True)  # Reset index to match DataFrame
dataframe.reset_index(drop=True, inplace=True)  # Reset index to match single_eventID
single_eventID = pd.concat([single_eventID, dataframe], axis=1)

single_eventID.to_csv(compact_hit_file, index=False)

print('\nCalculation has been done on the CSV file and Edep has been summed.')
