from sklearn.preprocessing import FunctionTransformer, StandardScaler
from sklearn.decomposition import PCA
from sklearn.pipeline import make_pipeline

PCA_RATIO = 0.999

def _code_data(data, parameters):
    process = parameters["data_xform"]
    random_state = parameters.get("random_state", None)  # Use provided random state or None
    
    if process == "none":
        # the identity
        model = FunctionTransformer()
    elif process == "std":
        model = StandardScaler(with_mean=False)
    elif process == "pca":
        model = PCA(n_components=PCA_RATIO, random_state = random_state)
    elif process == "pca_std":
        model = make_pipeline(
            PCA(n_components=PCA_RATIO, random_state = random_state), 
            StandardScaler(with_mean=False)
        )
    elif process == "std_pca":
        model = make_pipeline(
            StandardScaler(with_mean=False), 
            PCA(n_components=PCA_RATIO, random_state=random_state)
        )
    return model.fit(data)

def xform_data(data, parameters):
    xform = _code_data(data, parameters)
    return xform.transform, xform.inverse_transform
