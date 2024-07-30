from setuptools import setup

setup(
    name="kedro-umbrella",
    version="0.0.1",
    packages = ["kedro_umbrella"],
    install_requires=[
        "kedro>=0.18"
    ],
    entry_points={"kedro.project_commands": ["kedro-umbrella = kedro_umbrella.plugin:commands"]}
)
