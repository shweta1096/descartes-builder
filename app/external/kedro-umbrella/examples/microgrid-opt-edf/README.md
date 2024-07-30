# Pipe deformation use case 

To run the pipeline, use the Makefile accordingly. Different rules are provided. 

_Goal._ This example focuses on a micro-grid setting. The goal is to predict the change of frequency triggered by an increase of current in the system. The input `X` of the model is the current produced by generators and `Y` is the change of frequency. 

_Motivation._ Monitoring the change of frequency is important to prevent blackout as this is the most often root cause of blackout in energy system. The frequency must stays within a tight bound around 50Hz or 60Hz depending on the grid standard. 

The pipeline proceeds as follows:
1. Obtain data from a complex physics model used to model the micro-grid for `X` and `Y`
2. Obtain the PCA reduction of `Y` with a Coder box and apply it to obtain `Y_train_red` 
3. Train a model X $\rightarrow$ Y_train_red.