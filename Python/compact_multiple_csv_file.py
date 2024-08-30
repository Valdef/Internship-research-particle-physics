import pandas as pd 
import os
from sklearn.preprocessing import MinMaxScaler
import joblib

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

# ------ Normalize data ------
columns_to_normalize = big_df.columns  # get all the columns to normalise
scaler = MinMaxScaler(feature_range=(-1,1)) # normalise between a range
big_df[columns_to_normalize[1:4]] = scaler.fit_transform(big_df[columns_to_normalize[1:4]]) #normalise beamE, beamX, beamY
joblib.dump(scaler, 'scaler')   # save the normalisation 
big_df[columns_to_normalize[4]] = scaler.fit_transform(big_df[columns_to_normalize[4]].values.reshape(-1,1))  # normalise the rest of the data


big_df.to_csv(os.path.join('/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/combined_file.csv'), index=False)

print('\nroot files have been compacted into 1 csv file.')