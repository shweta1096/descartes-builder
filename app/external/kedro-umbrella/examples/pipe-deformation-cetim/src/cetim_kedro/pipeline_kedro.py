"""
This is a boilerplate pipeline
generated using Kedro 0.18.8
"""

from kedro.pipeline import Pipeline, node, pipeline

from .nodes import dual_code, split_data, test_model, train1


def create_pipeline(**kwargs) -> Pipeline:
    return pipeline(
        [
            node(
                func=split_data,
                inputs=["cetim_data", "parameters"],
                outputs=["X_train", "X_test", "Y_train", "Y_test"],
                name="split_data",
            ),
            node(
                func=dual_code,
                inputs=["X_train", "Y_train", "parameters"],
                outputs=["reducer_X", "reducer_Y", "X_train_red", "Y_train_red"],
                name="dual_code",
            ),
            node(
                func=train1,
                inputs=["X_train_red", "Y_train_red", "parameters"],
                outputs="fit_function",
                name="train",
            ),
            node(
                func=test_model,
                inputs=["fit_function", "X_test", "Y_test", "reducer_X", "reducer_Y"],
                outputs="Y_pred",
                name="test_model",
            ),
        ]
    )
