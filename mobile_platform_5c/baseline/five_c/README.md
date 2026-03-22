# Five-C library
## Description
This project contains the generic data-structures for the communication, configuration, coordination, compute and composition (5 C's) of software components. 
Function, algorithms and activities are meant for synchronous execution. Activities are the interface to the asynchronous world.

The application specific part of an application resides in different projects which links to the generic structures in this project.

## Dependencies that have to be _manually_ installed:
* CGraph
    * For the DAG / Flow chart implementation of an algorithm
    * Linux install ```$ sudo apt install libgraphviz-dev```

## Dependencies that that are automatically installed under the build-tree of the package:
* ut_hash
    * C hash table implementation (used for making dictionaries)
    * Click [here](https://troydhanson.github.io/uthash/) for UT_hash documentation

* Coordination_libraries: 
   * Common coordination datas tructures and mechanisms (petrinet, fsm, lcsm, DAG).
   * KU Leuven internal [gitlab repository](https://gitlab.kuleuven.be/u0144428/coordination_libraries).
    
## Build
After installation of dependencies (```CGraph```), do
```bash
$ mkdir build
$ cd build
$ cmake ..
$ make 
```

