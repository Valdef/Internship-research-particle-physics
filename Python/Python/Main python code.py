import compact_multiple_csv_file as cmcf
import hit_compact as hc
import test_ML

# If you want to run each python code indivudually,
# open them and uncomment the last line of code.
# You then just need to run the file
# PS: In order you should run:
# - compact_multiple_csv_file.py
# - hit_compact.py
# - test_ML.py

# cmcf take the root file name B4_latest---.root
# --- should be a number minimum 1
# It then compact the information (eventID, beamE/X/Y, cnX/Y/Z, Edep) from the different root file into 1 csv file
# the eventID number are continuous between all the different csv file when compacted
cmcf.compact_multiple_csv_file() 

# hc take either a csv file or a root file 
# It will compact cnX/Y/Z into one UniqueID 
# The Edep that have the same eventID and UniqueID are then summed
# The summed Edep are also normalised between 0 and 1
# eventID, beamE/X/Y, UniqueID and summed Edep are written on a csv file
# The csv file looks like this:
# eventID   beamX   beamY   0   1   2   3   4   ... 3600
#   0        ...     ...    0.0 0.0 0.2 0.0 0.0 ... 0.0
#   1        ...     ...    0.0 0.3 0.0 0.0 0.0 ... 0.9  
#   2        ...     ...    0.0 0.0 0.1 0.2 0.0 ... 0.0
hc.compact_Edep()

# test_ML is the actual machine learning algorithm
# It takes a csv file as input
# The csv file has a format like the one shown above
# The algorithm will be trained with different parameters and number of layers that can be changed
# The metrics, loss function and optimizer can also be changed
# Once the model has learned it try to do some predictions
# The output of this code are a few graphs:
# - One graph showing model loss
# - One graph showing model accuracy
# - 3 graph showing difference in true and predicted beamE/X/Y
test_ML.ML()
