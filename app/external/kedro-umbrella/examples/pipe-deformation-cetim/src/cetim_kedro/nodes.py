import logging
from typing import Any, Dict

import numpy as np
from sklearn import svm
from sklearn.decomposition import PCA
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_squared_error, r2_score
from sklearn.model_selection import train_test_split
from sklearn.neural_network import MLPRegressor
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import FunctionTransformer, StandardScaler
from sklearn.tree import DecisionTreeRegressor
import mat73

PCA_RATIO = 0.999

logger = logging.getLogger(__name__)

def _code_data(data, process="pca"):
    if process == "none":
        # the identity
        model = FunctionTransformer()
    elif process == "std":
        model = StandardScaler(with_mean=False)
    elif process == "pca":
        model = PCA(n_components=PCA_RATIO)
    elif process == "pca_std":
        model = make_pipeline(
            PCA(n_components=PCA_RATIO), StandardScaler(with_mean=False)
        )
    elif process == "std_pca":
        model = make_pipeline(
            StandardScaler(with_mean=False), PCA(n_components=PCA_RATIO)
        )
    return model.fit(data)


def _train_model(X, Y, parameters):
    model = parameters["model"]

    if model == "mlp":
        fit = MLPRegressor(hidden_layer_sizes=(50, 50), max_iter=50000)
    elif model == "mlp1":
        fit = MLPRegressor(max_iter=10000,
                           hidden_layer_sizes=(100, 100, 100),
                           activation = 'relu',
                           solver = 'lbfgs')
    elif model == "lr":
        fit = LinearRegression()
    elif model == "dt":
        fit = DecisionTreeRegressor()
    elif model == "svr":
        fit = svm.SVR()
    # TODO potentially train many models and pick the best one here
    return fit.fit(X, Y)


def score(Y_test, Y_pred):
    rmse = mean_squared_error(Y_test, Y_pred, squared=False)
    nrmse = rmse / (np.max(Y_test) - np.min(Y_test))
    r2 = r2_score(Y_test, Y_pred)
    logger.info(f"NRMSE = {nrmse}, r2 = {r2}")
    return nrmse, r2

def load_mat(parameters):
    data = mat73.loadmat(parameters['data_path'])
    X = data['input'].T  # Input features
    y = data['output'].T  # Output data
    return X, y

def split_data(data, parameters: Dict[str, Any]):
    X_train, X_test, Y_train, Y_test = train_test_split(
        data["displ"], data["eps"], random_state=parameters["random_state"]
    )
    return X_train, X_test, Y_train, Y_test


def dual_code(X, Y, parameters):
    code = parameters["data_xform"]
    reducer_X = _code_data(X, code)
    reducer_Y = _code_data(Y, code)

    X_red = reducer_X.transform(X)
    Y_red = reducer_Y.transform(Y)
    return reducer_X, reducer_Y, X_red, Y_red


def train1(X, Y, parameters):
    return _train_model(X, Y, parameters)


def test_model(model, X, Y, reducer_X, reducer_Y):
    # reduce data
    X_red = reducer_X.transform(X)

    # prediction in reduced domain
    Y_pred_red = model.predict(X_red)

    # project back to full domain
    Y_pred = reducer_Y.inverse_transform(Y_pred_red)

    # test performance
    score(Y, Y_pred)

    return Y_pred


def load_and_simulate_cetim_fmu(fmu_filename, X_test):
    from fmpy import read_model_description, extract
    from fmpy.fmi3 import FMU3Slave
    
    # Load the FMU and get variable references
    model_description = read_model_description(fmu_filename)
    unzipdir = extract(fmu_filename)
    fmu = FMU3Slave(guid=model_description.guid, unzipDirectory=unzipdir, modelIdentifier=model_description.coSimulation.modelIdentifier)
    vrs = {v.name: v.valueReference for v in model_description.modelVariables}
    vr_sample_x = vrs['sample_x']
    vr_y_pred = vrs['y_pred']

    # Define time step for the simulation
    start_time = 0
    step_size = 0.5
    stop_time = 1.0  # Adjust as needed

    # Instantiate and initialize the FMU
    fmu.instantiate()
    fmu.enterInitializationMode()
    fmu.exitInitializationMode()

       # Initialize a list to store predictions
    y_pred_list = []

    # Simulation loop for each sample in X_test
    for sample in X_test:
        time = start_time
        if sample.shape[0] != 2835:
            raise ValueError(f'X_test sample has {sample.shape[0]} features, but 2835 features are required.')

        while time < stop_time:
            fmu.setFloat64([vr_sample_x], sample.flatten().tolist())
            fmu.doStep(currentCommunicationPoint=time, communicationStepSize=step_size)

            # Retrieve the prediction from the FMU
            y_pred = np.array(fmu.getFloat64([vr_y_pred], len(sample))).reshape(-1, len(sample))
            time += step_size

        # Append the prediction for the current sample to the list
        y_pred_list.append(y_pred[0])  # Assuming y_pred is a 2D array with one row

    # Combine all predictions into a single array
    Y_pred = np.array(y_pred_list)

    # Terminate and free the FMU instance
    fmu.terminate()
    fmu.freeInstance()

    return Y_pred