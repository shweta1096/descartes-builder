from sklearn.metrics import mean_squared_error, r2_score
import numpy as np
import logging
import mat73
from sklearn.model_selection import train_test_split
from typing import Any, Dict

logger = logging.getLogger(__name__)

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

def split_data(X, Y, parameters: Dict[str, Any]):
    random_state = parameters.get("random_state", None)  # Use provided random state or None
    X_train, X_test, Y_train, Y_test = train_test_split(
        X, Y, random_state=random_state
    )
    return X_train, X_test, Y_train, Y_test
