# ======= import modules =======
import pandas as pd
import numpy as np
from tensorflow.keras.layers import Dense, Input, Dropout, BatchNormalization
from tensorflow.keras.models import Model
from tensorflow.keras.utils import plot_model
from tensorflow.keras.regularizers import l2
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
metrics = ['accuracy']
l2_value = 0.00 # L2 regularization strength

for i in range(0, 10):
    predictions = []

    # ======= create model =======
    input_layer = Input(shape=(input_data.shape[1],))           # input
    layer_1 = Dense(3000, activation = 'leaky_relu')(input_layer)
    layer_2 = Dense(2500, activation = 'leaky_relu')(layer_1)
    layer_3 = Dense(2000, activation = 'leaky_relu')(layer_2)
    layer_4 = Dense(1500, activation = 'leaky_relu')(layer_3)
    layer_5 = Dense(1000, activation = 'leaky_relu')(layer_4)
    layer_6 = Dense(750, activation = 'leaky_relu')(layer_5)
    layer_7 = Dense(500, activation = 'leaky_relu')(layer_6)
    output_layer = Dense(3, activation = 'leaky_relu')(layer_7)  # output
    model = Model(inputs = input_layer, outputs = output_layer)  #create model
    model.compile(optimizer = optimiser, loss = loss, metrics = metrics)   # compile the model with optimiser, loss and metrics

# ====== print the summary of the layers =====
#print(model.summary())
#plot_model(model, to_file = '/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/model.png')
#img = Image.open('/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/model.png')
#img.show()  # show a picutre of the model

    # ======= parameters =======
    batch_size = 64    # number of example before improving
    no_epochs = 40     # number of time data is bieng passed
    validation_split = 0.45  # % use for validation
    verbosity = 1

    # ======= train model =======
    history = model.fit(X_train, Y_train, batch_size = batch_size, epochs = no_epochs, verbose = verbosity, validation_split = validation_split)  # train the model

    predictions.append(model.predict(X_test))

prediction = np.mean(predictions, axis=0)

# ======= evaluate network =======
score = model.evaluate(X_test, Y_test, verbose=0)
print(f'Test loss: {score[0]} / Test accuracy: {score[1]}') # show progress of training


# ===== prediction and write csv =====
#prediction = model.predict(X_test)  # make prediction
prediction_df = pd.DataFrame(prediction, columns =["beamE", "beamX", "beamY"])
Y_test_df = pd.DataFrame(Y_test.values, columns=["beamE", "beamX", "beamY"])

# ====== normalise =======
scaler = joblib.load('scaler')  # call back the normalisation parameters saved in compact_multiple_csv_fil
Y_test_df = pd.DataFrame(scaler.inverse_transform(Y_test_df), columns=["beamE", "beamX", "beamY"])  # unormalise data
prediction_df = pd.DataFrame(scaler.inverse_transform(prediction_df), columns=["beamE", "beamX", "beamY"])  # unormalise data

results_df = pd.DataFrame({                     # create a dataframe with preciction and real values
    "Real_beamE": Y_test_df["beamE"],
    "prediction_beamE": prediction_df["beamE"],
    "Real_beamX": Y_test_df["beamX"],
    "prediction_beamX": prediction_df["beamX"],
    "Real_beamY": Y_test_df["beamY"],
    "prediction_beamY": prediction_df["beamY"]})     

results_df.to_csv("/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/prediction.csv")   # write a csv file with prediction and real values

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
    x = np.linspace(xmin, xmax, 300)
    p = norm.pdf(x, mu, std)
    plt.plot(x, p)
    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    if filename =='deltaE':
        plt.xlim(-5,5)
    if filename =='deltaX' or filename =='deltaY':
        plt.xlim(-40,40)
    plt.axvline(x = mu-std, color='r', linestyle='--')
    plt.axvline(x = mu+std, color='r', linestyle='--')
    plt.legend([f'mean: {mu:.1f}', f'std: {std:.1f}'])
    plt.savefig("/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/"+filename+".png")

# ======= plot model loss and accuracy =========
plt.plot(1)
plt.plot(history.history['accuracy'], label='Train Accuracy')
plt.plot(history.history['val_accuracy'], label='Validation Accuracy')
plt.title('Model accuracy')
plt.ylabel('Accuracy')
plt.xlabel('Epoch')
plt.legend(loc='upper left')
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

# ======= graphs of Delta E/X/Y =======
deltaE = Y_test_df['beamE'] - prediction_df['beamE']
deltaX = Y_test_df['beamX'] - prediction_df['beamX']
deltaY = Y_test_df['beamY'] - prediction_df['beamY']

# beamE 
plot_gaussian(deltaE,'Graph of ΔE','ΔE (GeV)','Count number','deltaE')

# beamX
plot_gaussian(deltaX,'Graph of ΔX','ΔX (mm)','Count number','deltaX')

# beamY
plot_gaussian(deltaY,'Graph of ΔY','ΔY (mm)','Count number','deltaY')

# ======= graphs of DeltaE / E, Delta X / X, Delta Y/Y =======
E = Y_test_df['beamE']
X = Y_test_df['beamX']
Y = Y_test_df['beamY']

# create dataframe to make things easier
df = pd.DataFrame({'beamX': X,'beamE': E,'beamY': Y, 'deltaX': deltaX, 'deltaY': deltaY, 'deltaE': deltaE})

def plot_Delta(df, x, y, title, xaxis, yaxis, filename):
    fig = px.density_heatmap(df, x, y, nbinsx = 150, nbinsy = 150, color_continuous_scale='viridis')
    fig.update_layout(title=title, xaxis_title = xaxis, yaxis_title=yaxis)
    fig.write_image("/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/"+filename+".png")

# DeltaE / E
plot_Delta(df,"beamE", "deltaE","Graph of ΔE vs. E","E (GeV)","ΔE (GeV)","deltaE_E")

# DeltaE / X
plot_Delta(df,"beamX", "deltaE","Graph of ΔE vs. X","X (mm)","ΔE (GeV)","deltaE_X")

# DeltaE / Y
plot_Delta(df,"beamY", "deltaE","Graph of ΔE vs. Y","Y (mm)","ΔE (GeV)","deltaE_Y")

# DeltaX / X
plot_Delta(df,"beamX", "deltaX","Graph of ΔX vs. X","X (mm)","ΔX (mm)","deltaX_X")

# DeltaX / E
plot_Delta(df,"beamE", "deltaX","Graph of ΔX vs. E","E (GeV)","ΔX (mm)","deltaX_E")

# DeltaX / Y
plot_Delta(df,"beamY", "deltaX","Graph of ΔX vs. Y","Y (mm)","ΔX (mm)","deltaX_Y")

# DeltaY / Y
plot_Delta(df,"beamY", "deltaY","Graph of ΔY vs. Y","Y (mm)","ΔY (mm)","deltaY_Y")

# DeltaY / E
plot_Delta(df,"beamE", "deltaY","Graph of ΔY vs. E","E (GeV)","ΔY (mm)","deltaY_E")

# DeltaY / X
plot_Delta(df,"beamX", "deltaY","Graph of ΔY vs. X","X (mm)","ΔY (mm)","deltaY_X")