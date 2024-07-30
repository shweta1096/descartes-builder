
from sklearn.tree import DecisionTreeRegressor
from sklearn.linear_model import LinearRegression
from sklearn.neural_network import MLPRegressor
from sklearn import svm

def _train_model(X, Y, parameters):
    model = parameters["model"]
    random_state = parameters.get("random_state", None)  # Use provided random state or None

    if model == "mlp":
        fit = MLPRegressor(hidden_layer_sizes=(50, 50), max_iter=50000, random_state=random_state)
    elif model == "mlp1":
        fit = MLPRegressor(max_iter=10000,
                           hidden_layer_sizes=(100, 100, 100),
                           activation = 'relu',
                           solver = 'lbfgs',
                           random_state=random_state)
    elif model == "lr":
        fit = LinearRegression()
    elif model == "dt":
        fit = DecisionTreeRegressor(random_state=random_state)
    elif model == "svr":
        fit = svm.SVR()
    # TODO potentially train many models and pick the best one here
    return fit.fit(X, Y)

def basic_trainer(X, Y, parameters):
    return _train_model(X, Y, parameters).predict