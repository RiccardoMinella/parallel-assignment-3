# Parallel Assignment 3

## Instructions

### Set up Environment

Load the necessary modules before compiling the programs:\
module load gcc91 mpich-3.2.1--gcc-9.1.0\

Compile Programs\
Compile the programs using the following commands:\
mpicc matT.c -o matT.out -lm\
mpicc matT_blocks.c -o matT_blocks.out -lm\

Run Experiments\
Execute the PBS file based on the desired number of processors. Ensure you change the working directory accordingly.\

Program Execution\
The experiments files are named matT.... These files execute both programs.\
