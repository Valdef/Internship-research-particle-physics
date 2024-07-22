def ML():
    # ======= import modules =======
    import pandas as pd
    import numpy as np
    from tensorflow.keras.layers import Dense, Input
    from tensorflow.keras.models import Model
    from tensorflow.keras.utils import plot_model
    from PIL import Image
    import matplotlib.pyplot as plt
    from sklearn import preprocessing  

    datafile = "/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/compact_file.csv"

    # ======= import data =======
    df = pd.read_csv(datafile)
    df = df.sample(frac=1)  # mix the data

    # ===== normalise data =======
    df.columns = df.columns.astype(str) # convert to str
    process = preprocessing.MinMaxScaler(feature_range=(0,1))   # function to normalise data
    output_data, input_data = df.iloc[:,1:4], df.iloc[:,4:] # get the different dataframe

    input_data = pd.DataFrame(process.fit_transform(input_data), columns=input_data.columns)
    output_data = pd.DataFrame(process.fit_transform(output_data), columns=output_data.columns)

    # ======= process data =======
    percentage_train = 0.7  # % percentage training data

    train_end = int(percentage_train * len(input_data))
    X_train, Y_train = input_data.iloc[:train_end], output_data.iloc[:train_end] # get data from 0% to train %
    X_test, Y_test = input_data.iloc[train_end:], output_data.iloc[train_end:]   # Get data from train % to 100

    # ======= optimiser, loss, metrics =======
    optimiser = 'adam'  # best optimiser
    loss = 'huber_loss' # probably best choice here
    metrics = ['accuracy']

    # ======= create model =======
    input_layer = Input(shape=(input_data.shape[1],))                  #input
    layer_1 = Dense(50, activation = 'leaky_relu')(input_layer)       #layer 1
    layer_2 = Dense(100, activation = 'leaky_relu')(layer_1)     #layer 2
    layer_3 = Dense(300, activation = 'leaky_relu')(layer_2)     #layer 3
    layer_4 = Dense(100, activation = 'leaky_relu')(layer_3)     #layer 4
    layer_5 = Dense(50, activation = 'leaky_relu')(layer_4)     #layer 5
    layer_6 = Dense(50, activation = 'leaky_relu')(layer_5)     #layer 6
    output_layer = Dense(3, activation = 'leaky_relu')(layer_6)   #output
    model = Model(inputs = input_layer, outputs = output_layer)  #create model
    model.compile(optimizer = optimiser, loss = loss, metrics = metrics)   # compile the model with optimiser, loss and metrics

    # ====== print the summary of the layers =====
    print(model.summary())
    plot_model(model, to_file = '/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/model.png')
    img = Image.open('/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/model.png')
    #img.show()  # show a picutre of the model

    # ======= parameters =======
    batch_size = 64    # number of example before improving
    no_epochs = 20     # number of time data is bieng passed
    validation_split = 0.45  # % use for validation
    verbosity = 1

    # ======= train model =======
    history = model.fit(X_train, Y_train, batch_size = batch_size, epochs = no_epochs, verbose = verbosity, validation_split = validation_split)  # train the model

    # ======= evaluate network =======
    score = model.evaluate(X_test, Y_test, verbose=0)
    print(f'Test loss: {score[0]} / Test accuracy: {score[1]}') # show progress of training

    # ===== prediction and write csv =====
    prediction = model.predict(X_test)  # make prediction
    prediction_df = pd.DataFrame(prediction, columns =["beamE", "beamX", "beamY"])
    #prediction_df = pd.DataFrame(process.inverse_transform(prediction_df), columns=prediction_df.columns)   # de-normalise data
    Y_test_df = pd.DataFrame(Y_test.values, columns=["beamE", "beamX", "beamY"])
    #Y_test_df = pd.DataFrame(process.inverse_transform(Y_test_df), columns=Y_test_df.columns)   # de-normalise data

    results_df = pd.DataFrame({
        "Real_beamE": Y_test_df["beamE"],
        "prediction_beamE": prediction_df["beamE"],
        "Real_beamX": Y_test_df["beamX"],
        "prediction_beamX": prediction_df["beamX"],
        "Real_beamY": Y_test_df["beamY"],
        "prediction_beamY": prediction_df["beamY"]})     # create a dataframe with preciction and real values

    results_df.to_csv("/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/ML/output/prediction.csv")   # write a csv file with prediction and real values

    # ====== create function to find std =====
    def std(diff):
        n, bins = np.histogram(diff)
        mids = 0.5*(bins[1:] + bins[:-1])
        mean = np.average(mids, weights=n)
        var = np.average((mids-mean)**2, weights=n)
        std = np.sqrt(var)
        print(std)
        return std
    
    # ======= plot different variable ========
    plt.figure(1)
    plt.plot(history.history['accuracy'])
    plt.plot(history.history['val_accuracy'])
    plt.title('Model accuracy')
    plt.ylabel('accuracy')
    plt.xlabel('Epoch')
    plt.legend(['Train', 'Validation'], loc='upper left')

    # Plot training & validation loss values
    plt.figure(2)
    plt.plot(history.history['loss'])
    plt.plot(history.history['val_loss'])
    plt.title('Model loss')
    plt.ylabel('Loss')
    plt.xlabel('Epoch')
    plt.legend(['Train', 'Validation'], loc='upper left')
    
    # Plot difference true and predicted value
    diff_beamE = Y_test_df['beamE'] - prediction_df['beamE']
    diff_beamX = Y_test_df['beamX'] - prediction_df['beamX']
    diff_beamY = Y_test_df['beamY'] - prediction_df['beamY']
    
    plt.figure(3)
    plt.hist(diff_beamE, bins=100)
    plt.xlabel('difference beamE')
    plt.title('difference true beamE, predicted beamE')

    plt.figure(4)
    plt.hist(diff_beamX, bins=100)
    plt.xlabel('difference beamX')
    plt.title('difference true beamX, predicted beamX')

    plt.figure(5)
    plt.hist(diff_beamY, bins=100)
    plt.xlabel('difference beamY')
    plt.title('difference true beamY, predicted beamY')

    # ===== close all the graphs =====
    def close_plots_on_input():
        input("Press any key to close all the graphs...")
        plt.close('all')

    plt.show(block=False)
    close_plots_on_input()
    
    return std(diff_beamE), std(diff_beamX), std(diff_beamY), score[0]

# to run the program on its own, uncomment the line below
ML()

# this is to run multiple training and get an average of parameters, output
'''
import numpy as np
std_beamE = []
std_beamX = []
std_beamY = []
loss = []
for i in range(0, 10):
    beamE, beamX, beamY, score = ML()
    std_beamE.append(beamE)
    std_beamX.append(beamX)
    std_beamY.append(beamY)
    loss.append(score)

print(len(std_beamE))
print(np.average(std_beamE))
print(np.average(std_beamX))
print(np.average(std_beamY))
print(np.average(score))
'''