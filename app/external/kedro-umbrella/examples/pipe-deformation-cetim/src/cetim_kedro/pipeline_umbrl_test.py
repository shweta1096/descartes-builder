"""
This is a boilerplate pipeline
generated using Kedro 0.18.8
"""

from kedro.pipeline import Pipeline, pipeline
from kedro_umbrella import processor
from kedro_umbrella.library import *

# pro => doesn't need to code the node +- => one still need to think in advance
# about wanting to output some valid function that can be called further ahead
# dis => longer pipeline


def create_pipeline(**kwargs) -> Pipeline:
    return pipeline(
        [
            # TESTING PIPELINE
            processor(inputs=["X_xform", "X_test"], outputs="X_test_red"),
            processor(inputs=["regressor", "X_test_red"], outputs="Y_pred_red"),
            processor(inputs=["Y_inv_xform", "Y_pred_red"], outputs="Y_pred"),
            processor(
                func=score,
                name="score",
                inputs=["Y_test", "Y_pred"],
                outputs=["nmrse", "r2"],
            )
        ]
    )
