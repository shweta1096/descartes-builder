from kedro.pipeline import Pipeline, node, pipeline
from kedro_umbrella.library import *
from .nodes import *

def create_pipeline(**kwargs) -> Pipeline:
    return pipeline(
        [
            # Data Retrieval
            node(
                func=get_data,
                inputs={"noise": "params:get_data.noise", "N": "params:get_data.N", "T": "params:get_data.test_T", "F": "params:get_data.F", "data_xform": "params:get_data.data_xform"},
                outputs=["X_test", "Y_test"],
                name="get_data",
            ),
            node(
                func=load_and_simulate_spring_fmu,
                inputs=["params:fmu_path", "X_test"],
                outputs="Y_pred",
                name="load_and_simulate_fmu",
            ),
            # Evaluation
            node(
                func=score,
                name="score",
                inputs=["Y_test", "Y_pred"],
                outputs=["nmrse", "r2"],
            ),
        ]
    )