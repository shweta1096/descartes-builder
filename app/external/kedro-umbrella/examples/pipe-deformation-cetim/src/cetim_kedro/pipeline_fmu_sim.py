"""
This is a boilerplate pipeline
generated using Kedro 0.18.8
"""

from kedro.pipeline import Pipeline, node, pipeline

from kedro_umbrella import processor
from kedro_umbrella.library import *

from .nodes import load_and_simulate_cetim_fmu

#from .nodes import basic_trainer, score, split_data_1, xform_data, load_and_simulate_cetim_fmu

# pro => doesn't need to code the node +- => one still need to think in advance
# about wanting to output some valid function that can be called further ahead
# dis => longer pipeline


def create_pipeline(**kwargs) -> Pipeline:
    return pipeline(
        [
            # TRAINING PIPELINE
            processor(
                func=split_data,
                name="split_data",
                inputs=["displ", "eps", "parameters"],
                outputs=["X_train", "X_test", "Y_train", "Y_test"],
            ),
            node(
                func=load_and_simulate_cetim_fmu,
                inputs=["params:fmu_path", "X_test"],
                outputs="Y_pred",
                name="load_and_simulate_fmu",
            ),
            # TESTING PIPELINE
            node(
                func=score,
                name="score",
                inputs=["Y_test", "Y_pred"],
                outputs=["mse", "r2"],
            ),
        ]
    )
