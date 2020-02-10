Spiking-Neural-Network-based-SLAM
=================================

This implementation resulted from a project work in the course of studies "Intelligent Systems" at Bielefeld University in the Cognitronics and Sensor Systems Group (http://www.ks.cit-ec.uni-bielefeld.de/), supervised by Christoph Ostrau (https://github.com/costrau). It consists of a simple robot simulator for a point-like robot in an environment with obstacles, and a spiking neural network. The simulator generates data which is then load into the SNN.
The SNN is based on the paper "Pose Estimation and Map Formation with Spiking Neural Networks: towards Neuromorphic SLAM" by Kreiser et al. (https://www.researchgate.net/publication/330595537_Pose_Estimation_and_Map_Formation_with_Spiking_Neural_Networks_towards_Neuromorphic_SLAM). It covers path-integration and map-learning functionalities.
It utilizes the Cypress simulation framework for spiking neural networks (https://github.com/hbp-unibi/cypress).

Prerequisites
-------------

In order to run the Spiking Neural Network, python3 needs to be installed, with the packages numpy and matplotlib. 
Cypress supports different backends like genn or pynn.nest. For more information about possible other spiking simulation backends, take a look at the documentation of cypress.

Building the sources
--------------------

When the prerequisites are fulfilled, run
```bash
git clone https://github.com/twest49/Spiking-Neural-Network-based-SLAM
mkdir Spiking-Neural-Network-based-SLAM/build && cd Spiking-Neural-Network-based-SLAM/build
cmake ..
make 
```

Run the Spiking Neural Network
------------------------------

The implementation can be run either with GUI, to simulate a robot and record data, or without to run the SNN on already recorded data.


To run the SNN with robot-simulator and GUI, run
```bash
./gui genn='{"timestep":0.1, "double": true}'
```

To run the SNN without robot-simulator and gui, run
```bash
./main genn='{"timestep":0.1, "double": true}'
```
