# Database project

## Team members
1. Mohit
2. Pranav
3. Rohit

## Summary

In this project we are implementing the algorithms discussed in the paper "[Computing A Near-Maximum Independent Set in LinearTime by Reducing-Peeling](https://dl.acm.org/doi/10.1145/3035918.3035939)". The algorithms find the maximum independent set of graphs using Reducing-Peeling framework.

The algorithms that we have implemented, that were discussed in this paper are,
1. BDOne
2. BDTwo
3. LinearTime

Along with these algorithms, we have implemented an additional greedy algorithm to compare against the other algorithms.

## Datasets used 
- CA-AstroPh
- CA-CondMat
- CA-GrQc
- Email-EuAll
- soc-Epinions1
- WikiTalk

## Environment 
- Windows 11
- Intel i7 processor
- 16GB RAM
- G++ v13.2.0

**Note -** As we did not have access to a Linux or Mac machine, we have only tested our program on a Windows machine.

## Steps to run the program
1. Compile the C++ program and generate the `program.exe`` file using the command,
```bash
make
```
2. Run the .exe file with path to the dataset folder as the argument.
```bash
./program.exe ./src/data/<data_set_name>
```
3. The `<data_set_name>` can be one of the dataset names mentioned above. The folder in the `./src/data` can also be referred to check the available datasets. Example - `./program.exe ./src/data/WikiTalk`.
4. A prompt will show up, where an algorithm can be selected to compute the maximum independent set.
