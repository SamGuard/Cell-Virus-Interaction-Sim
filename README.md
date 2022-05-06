# Cell-Virus-Interaction-Sim
Current Time Worked: \
[![wakatime](https://wakatime.com/badge/user/f35eb4e7-f40f-4f52-83ec-797025bb7666/project/8908e463-958e-407d-b108-a57c126a2ae9.svg)](https://wakatime.com/badge/user/f35eb4e7-f40f-4f52-83ec-797025bb7666/project/8908e463-958e-407d-b108-a57c126a2ae9)

## Runing the code
To be able to run this code you must be able to compile it using the RepastHPC library. The easiest way to do this is using docker. You can also install from https://repast.github.io/repast_hpc.html.

### Docker install
This will install the required libraries and then retrieve the source code for this project from GitHub. The main branch is not pulled, there is a server branch specifically for running the code in this docker container.
1. Navigate to the docker_install folder
1. Run "docker build -t "cell-virus" ." with elevated priviliges. This will can take around 20 minutes
1. Finally run "docker run --name cell-virus-sim -it "cell-virus" bash" this will start the container and run bash.

To extract data from the container you must create and mount a drive that to the /root/Cell-Virus-Interaction-Sim/output folder.


## Structure
Code is compiled into the bin folder. The output folder will containt the csv files from the simulation(s). The data on agent states and positions will also appear here.

## Compiling, Installing and Running
To compile the code run "make" then "make run" to run it. \
To change the parameters of the simulation edit the model.props file \
To change the amount of processors edit the env file.

## Parameter Sweeps
Before carrying out a parameter sweep, adjust the constants at the top of the param_sweep.py file.
PROC_DIM_X/Y are used to define the dimensions of processors for each simulation. Make sure these multiplied together must be less than the number of processors on your machine.
MAX_BATCH is the number of simulations to run in parallel. Make sure this value multiplied by both processor dims must be less than the available processors on your machine.

To run a parameter sweep run "python3 param_sweep.py PARAM_NAME START END STEP"

