# ======= import modules =======
import pandas as pd
import numpy as np
from tensorflow.keras.layers import Dense, Input
from tensorflow.keras.models import Model
from tensorflow.keras.utils import plot_model
from keras.callbacks import Callback
from PIL import Image
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')
import plotly.express as px
import joblib
from scipy.stats import norm

datafile = "/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/compact_file.csv"

# ======= import data =======
df = pd.read_csv(datafile)
df = df.sample(frac=1)  # mix the data
output_data, input_data = df.iloc[:,1:4], df.iloc[:,4:] # get the different dataframe

# ======= process data =======
percentage_train = 0.66  # % percentage training data

train_end = int(percentage_train * len(input_data))
X_train, Y_train = input_data[:train_end], output_data[:train_end] # get data from 0% to train %
X_test, Y_test = input_data[train_end:], output_data[train_end:]   # Get data from train % to 100

# ======= optimiser, loss, metrics =======
optimiser = 'adam'  # best optimiser
loss = 'huber_loss' # probably best choice here
metrics = ['mean_absolute_error', 'mean_squared_error']

# ======= create model =======
input_layer = Input(shape=(input_data.shape[1],))           # input
layer_1 = Dense(600, activation = 'leaky_relu')(input_layer)
layer_2 = Dense(500, activation = 'leaky_relu')(layer_1)
layer_3 = Dense(1000, activation = 'leaky_relu')(layer_2)
layer_4 = Dense(600, activation = 'leaky_relu')(layer_3)
layer_5 = Dense(300, activation = 'leaky_relu')(layer_4)
layer_6 = Dense(200, activation = 'leaky_relu')(layer_5)
layer_7 = Dense(400, activation = 'leaky_relu')(layer_6)
output_layer = Dense(3, activation = 'leaky_relu')(layer_7)  # output
model = Model(inputs = input_layer, outputs = output_layer)  #create model
model.compile(optimizer = optimiser, loss = loss, metrics = metrics)   # compile the model with optimiser, loss and metrics

# ====== print the summary of the layers =====
print(model.summary())
plot_model(model, to_file = '/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/model.png')
img = Image.open('/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/model.png')
#img.show()  # show a picutre of the model

# ======= parameters =======
batch_size = 64    # number of example before improving
no_epochs = 2     # number of time data is bieng passed
validation_split = 0.4  # % use for validation
verbosity = 1

# ======= train model =======
history = model.fit(X_train, Y_train, batch_size = batch_size, epochs = no_epochs, verbose = verbosity, validation_split = validation_split)  # train the model

# ======= evaluate network =======
score = model.evaluate(X_test, Y_test, verbose=0)
print(f'Test loss: {score[0]} / Test accuracy: {score[1]}') # show progress of training


# ===== prediction and write csv =====
prediction = model.predict(X_test)  # make prediction
prediction_df = pd.DataFrame(prediction, columns =output_data.columns)
Y_test_df = pd.DataFrame(Y_test.values, columns=output_data.columns)

# ====== normalise =======
#scaler = joblib.load('scaler')  # call back the normalisation parameters saved in compact_multiple_csv_fil
#Y_test_df = pd.DataFrame(scaler.inverse_transform(Y_test_df), columns=output_data.columns)  # unormalise data
#prediction_df = pd.DataFrame(scaler.inverse_transform(prediction_df), columns=output_data.columns)  # unormalise data

results_df = pd.concat([Y_test_df.add_prefix('Real_'), prediction_df.add_prefix('Prediction_')], axis=1) 
print(results_df.head())

results_df.to_csv("/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/prediction_mixed.csv")   # write a csv file with prediction and real values

# ====== create function to find std and plot gaussian =====
def plot_gaussian(data,title, xlabel, ylabel, filename):
    # calculate mean and std
    mu, std = norm.fit(data)
    print('mean:', mu)
    print('std:', std)

    # plot histogram
    plt.figure()
    plt.hist(data, bins=100, density=True)

    # plot Gaussian
    xmin, xmax = plt.xlim()
    x = np.linspace(xmin, xmax, 100)
    p = norm.pdf(x, mu, std)
    plt.plot(x, p)
    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.legend([f'mean: {mu:.6f}', f'std: {std:.6f}'])
    plt.savefig("/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/"+filename+".png")

# ======= plot model loss and accuracy =========
'''
plt.plot(1)
plt.plot(history.history['mean_absolute_error'])
plt.plot(history.history['val_mean_absolute_error'])
plt.title('Model accuracy')
plt.ylabel('mean absolute error')
plt.xlabel('Epoch')
plt.legend(['Train', 'Validation'], loc='upper left')
plt.savefig("/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/model_accuracy.png")

# Plot training & validation loss values
plt.figure(2)
plt.plot(history.history['loss'])
plt.plot(history.history['val_loss'])
plt.title('Model loss')
plt.ylabel('Loss')
plt.xlabel('Epoch')
plt.legend(['Train', 'Validation'], loc='upper left')
plt.savefig("/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/loss_model.png")
'''
# ======= graphs of Delta E/X/Y =======
deltaE = results_df['Prediction_beamE'] - results_df['Real_beamE']
deltaX = results_df['Prediction_beamX'] - results_df['Real_beamX']
deltaY = results_df['Prediction_beamY'] - results_df['Real_beamY']

# beamE 
plot_gaussian(deltaE,'Graph of ΔE','ΔE (GeV)','Count number','deltaE_mixed')

# beamX
plot_gaussian(deltaX,'Graph of ΔX','ΔX (mm)','Count number','deltaX_mixed')

# beamY
plot_gaussian(deltaY,'Graph of ΔY','ΔY (mm)','Count number','deltaY_mixed')

# ======= graphs of DeltaE / E, Delta X / X, Delta Y/Y =======
E = Y_test_df['beamE']
X = Y_test_df['beamX']
Y = Y_test_df['beamY']

#deltaE = deltaE/E
#deltaX = deltaX/X
#deltaY = deltaY/Y

'''
# DeltaE / E
plt.figure(6)
plt.hist2d(E, deltaE ,bins=(30,30), cmap='viridis')
plt.colorbar(label='density')
plt.xlabel('Real Energy (GeV)')
plt.ylabel('ΔE')
plt.xlim([0,20])
plt.savefig("/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/test_E.png")

# DeltaX / X
plt.figure(7)
plt.hist2d(X, deltaX ,bins=(30,30), cmap='viridis')
plt.colorbar(label='density')   
plt.xlabel('Real X')
plt.ylabel('ΔX')
plt.savefig("/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/test_X.png", dpi=300)

# DeltaY/Y
plt.figure(8)
plt.hist2d(X, deltaE ,bins=(30,30), cmap='viridis')
plt.colorbar(label='density')
plt.xlabel('Real Y')
plt.ylabel('ΔY')
plt.savefig("/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/test_Y.png", dpi=300)

'''
# create dataframe to make things easier
df = pd.DataFrame({'beamX': X,'beamE': E,'beamY': Y, 'deltaX': deltaX, 'deltaY': deltaY, 'deltaE': deltaE})

def plot_Delta(df, x, y, title, xaxis, yaxis, filename):
    fig = px.density_heatmap(df, x, y, nbinsx = 150, nbinsy = 150, color_continuous_scale='viridis')
    fig.update_layout(title=title, xaxis_title = xaxis, yaxis_title=yaxis)
    #fig.update_yaxes(range=[-5, 5])
    fig.write_image("/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/"+filename+".png")

# DeltaE / E
plot_Delta(df,"beamE", "deltaE","Graph of ΔE vs. E","E (GeV)","ΔE (GeV)","deltaE_E_mixed")

# DeltaE / X
plot_Delta(df,"beamX", "deltaE","Graph of ΔE vs. X","X (mm)","ΔE (GeV)","deltaE_X_mixed")

# DeltaE / Y
plot_Delta(df,"beamY", "deltaE","Graph of ΔE vs. Y","Y (mm)","ΔE (GeV)","deltaE_Y_mixed")

# DeltaX / X
plot_Delta(df,"beamX", "deltaX","Graph of ΔX vs. X","X (mm)","ΔX (mm)","deltaX_X_mixed")

# DeltaX / E
plot_Delta(df,"beamE", "deltaX","Graph of ΔX vs. E","E (GeV)","ΔX (mm)","deltaX_E_mixed")

# DeltaX / Y
plot_Delta(df,"beamY", "deltaX","Graph of ΔX vs. Y","Y (mm)","ΔX (mm)","deltaX_Y_mixed")

# DeltaY / Y
plot_Delta(df,"beamY", "deltaY","Graph of ΔY vs. Y","Y (mm)","ΔY (mm)","deltaY_Y_mixed")

# DeltaY / E
plot_Delta(df,"beamE", "deltaY","Graph of ΔY vs. E","E (GeV)","ΔY (mm)","deltaY_E_mixed")

# DeltaY / X
plot_Delta(df,"beamX", "deltaY","Graph of ΔY vs. X","X (mm)","ΔY (mm)","deltaY_X_mixed")