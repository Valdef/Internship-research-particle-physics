def compact_multiple_csv_file():
    import pandas as pd 
    import os

    # ------ start to concatenate the file --------
    folder_path = r'/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/csv_file'     #path of folder with file to concatenate

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
    big_df.to_csv(os.path.join('/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/combined_file.csv'), index=False)

    print('\nroot files have been compacted into 1 csv file.')

# to run the program on its own, uncomment the line below
compact_multiple_csv_file()