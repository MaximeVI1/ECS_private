# mobile_platform_5c

This directory contains the software that runs on the [mobile robot](https://u0011821.pages.gitlab.kuleuven.be/images/mobile-platform-KELO-wheel.svg) of the ECS course.

The <em>generic code</em> is located in the [`baseline`](https://gitlab.kuleuven.be/rob-ecs/ecs-2025/-/tree/main/Software/mobile_platform_5c/baseline) folder.
The most application-independent part of that generic code base is in the [five_c](https://gitlab.kuleuven.be/rob-ecs/ecs-2025/-/tree/main/Software/mobile_platform_5c/external/five_c) directory, with template code for [threads](https://gitlab.kuleuven.be/rob-ecs/ecs-2025/-/blob/main/Software/mobile_platform_5c/external/five_c/include/five_c/thread/thread.h), [activities](https://gitlab.kuleuven.be/rob-ecs/ecs-2025/-/blob/main/Software/mobile_platform_5c/external/five_c/include/five_c/activity/activity.h), [schedules](https://gitlab.kuleuven.be/rob-ecs/ecs-2025/-/blob/main/Software/mobile_platform_5c/external/five_c/include/five_c/schedule/schedule.h), etc.
The [KELO wheel activity](https://gitlab.kuleuven.be/rob-ecs/ecs-2025/-/blob/main/Software/mobile_platform_5c/external/mobile_platform_drive/src/activity/kelo_drive_activity.c) interfaces with the two actuated wheels on the ECS mobile robot, via an EtherCat connection. The latter in turn makes use of the 
[SOEM](https://openethercatsociety.github.io/doc/soem/index.html) (Simple Open EtherCAT Master) library.

## Installation instructions

Install dependencies:

`$ sudo apt install git cmake build-essential libgraphviz-dev`

Compile the code: 

`$ mkdir build`

`$ cd build`

`$ cmake ..`

`$ make`

