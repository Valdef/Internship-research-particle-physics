import pandas as pd 
import os
from sklearn.preprocessing import MinMaxScaler
import joblib
from tqdm import tqdm

# ------ start to concatenate the file --------
folder_path = r'/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/csv_2gamma_200'     #path of folder with file to concatenate

all_files = os.listdir(folder_path)
csv_files = [f for f in all_files if f.endswith('.csv')]    # find all the csv file
df_list = []

start_event_id = 0
# ------ read and concatenate all the file in a csv file ------
for csv in csv_files:
    file_path = os.path.join(folder_path, csv)
    df = pd.read_csv(file_path, chunksize = 5000)

    for chunk in df:
        chunk['eventID'] = chunk['eventID'] + start_event_id
        df_list.append(chunk)
    start_event_id = chunk['eventID'].max() + 1

big_df = pd.concat(df_list, ignore_index=True)

mixed_events = []
for i in tqdm(range(0, len(big_df) -1, 2)):
    event1 = big_df.iloc[i]
    event2 = big_df.iloc[i+1]

    mixed_event = {
        'beamE_1': event1['beamE'],
        'beamX_1': event1['beamX'],
        'beamY_1': event1['beamY'],
        'Edep_1': event1['Edep'],
        'beamE_2': event2['beamE'],
        'beamX_2': event2['beamX'],
        'beamY_2': event2['beamY'],
        'Edep_2': event2['Edep'],
        'eventID': max(event1['eventID'], event2['eventID']),
        'isMixed': 1
    }

    mixed_events.append(mixed_event)

mixed_df = pd.DataFrame(mixed_events)

big_df['isMixed'] = 0
combined_df = pd.concat([big_df, mixed_df], ignore_index=True)
# ------ Normalize data ------
columns_to_normalize = ['beamE_1', 'beamX_1', 'beamY_1', 'beamE_2', 'beamX_2', 'beamY_2', 'Edep_1', 'Edep_2']  # get all the columns to normalise
scaler = MinMaxScaler(feature_range=(-1,1)) # normalise between a range
combined_df[columns_to_normalize[0:6]] = scaler.fit_transform(combined_df[columns_to_normalize[0:6]]) #normalise beamE, beamX, beamY
joblib.dump(scaler, 'scaler')   # save the normalisation 
combined_df[['Edep_1', 'Edep_2']] = scaler.fit_transform(combined_df[['Edep_1', 'Edep_2']])

combined_df.to_csv(os.path.join('/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/combined_file.csv'), index=False)

print('\nroot files have been compacted into 1 csv file.')