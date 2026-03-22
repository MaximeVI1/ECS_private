## Navigation template
This folder contains two templates for a navigation activity. The main difference between the two templates is in the actuation level.

[navigation_with_platform_control](https://gitlab.kuleuven.be/rob-ecs/ecs-2026/-/blame/main/Software/navigation_template/src/activity/navigation_with_platform_control.c): The navigation activity provides set-points for the forward and angular velocity of the platform. The velocity set-points are described in a body-fixed coordinate frame attached to the rear axle of the vehicle (x-axis is forward, z-axis is upward). The [platform velocity control activity](https://gitlab.kuleuven.be/rob-ecs/ecs-2026/-/tree/main/Software/mobile_platform_5c/baseline/mobile_platform_control) reads the velocity set-points via shared memory and, using the kinematic model of the vehicle, it computes set-points for the left and right wheels of the Kelo drive.

[navigation_with_kelo_control](https://gitlab.kuleuven.be/rob-ecs/ecs-2026/-/blame/main/Software/navigation_template/src/activity/navigation_with_kelo_control.c?ref_type=heads#L384): The navigation activity provides set-points for the left and right wheels of the Kelo drive. There are two possibilities: torque or velocity set-points. More details on how to select between the two is given in the code.

### Dependencies
On Linux and WSL:

```bash
sudo apt-get install git cmake build-essential libgraphviz-dev
sudo apt-get install lttng-tools
sudo apt-get install liblttng-ust-dev
```

#### How to use this project?
Copy this project to your software development folder. In your local copy, update the variable PATH_TO_MOBILE_PLATFORM_5C in [CMakeLists.txt](https://gitlab.kuleuven.be/rob-ecs/ecs-2026/-/blob/main/Software/navigation_template/CMakeLists.txt#L7), which stores the relative path from the root of this project to the [mobile_platform_5c](https://gitlab.kuleuven.be/rob-ecs/ecs-2026/-/tree/main/Software/mobile_platform_5c) package. There is no need to update the variable if you copy to the [Software](https://gitlab.kuleuven.be/rob-ecs/ecs-2026/-/tree/main/Software) folder.

To compile, navigate in a terminal to your copy of this project and type:

```bash
mkdir build
cd build
```

To switch between the two templates, we use the CMake variable PLATFORM_CONTROL. If the variable is "ON" then, the _navigation_with_platform_control_ will be compiled and linked with the application. If the variable is "OFF", then _navigation_with_kelo_control_ will be used instead. For example,

```bash
cmake .. -DPLATFORM_CONTROL=OFF
make
```

This command will compile first all the libraries that are necessary (also in `mobile_platform_5c` folder),  and then link to the executable.

#### Move the application on the mobile platform

Plug the router into a wall socket to turn it on.
The raspberry pi should automatically connect to the router network when it is turned on.
Connect your PC to the network: TP-Link_D860
The password is on the back of the router: 87865790


To check that your pc and the raspberry pi are in the same network, you can ping it.

```bash
ping 192.168.0.105
```

If you do not receive packages on your end, you are not in the same network.

Once your PC and the raspberry pi are both connected to the TP-Link_D860 network, you can SSH into it.
Open a terminal and type:

```bash
ssh ecs-pi-1@192.168.0.105
```

The raspberry pi will ask for a password, this is 'ecspi'.
You should be connected now.

To copy your source code onto the robot, use the following commands in a terminal on your pc.

```bash
cd .. #now you are in navigation template
rm -rf build
cd ../.. # 
scp -r mobile_platform_5c ecs-pi-1@192.168.0.105:~/ecs-2026/Students/YourDirectory # copy the local mobile platform 5c into /home/ecs-pi-1/ecs-2026 in the raspberry pi memory
scp -r navigation_template ecs-pi-1@192.168.0.105:~/ecs-2026/Students/YourDirectory # copy the local navigation template into /home/ecs-pi-1/ecs-2026 in the raspberry pi memory
```

The raspberry pi will ask for a password, this is 'ecspi'.

Once this is done, you can open a remote shell via ssh.
Open a new terminal and type:

```bash
ssh ecs-pi-1@192.168.0.105
```

and there compile the software as explained before.

```bash
cd ecs-2026/Software/navigation_template
mkdir build
cd build
cmake .. #  or cmake .. -DPLATFORM_CONTROL=OFF
make
```

#### Running
In the build folder:

`$ ./application ../configuration/application.json`

At this point, the application has several activities running on different threads. The activities are writing and reading data via shared memory, but no actual behaviour is implemented. This is up to you :) 

[Kelo drive activity](https://gitlab.kuleuven.be/rob-ecs/ecs-2026/-/tree/main/Software/mobile_platform_5c/baseline/mobile_platform_drive?ref_type=heads)  
[Proprioception activity](https://gitlab.kuleuven.be/rob-ecs/ecs-2026/-/tree/main/Software/mobile_platform_5c/baseline/mobile_platform_proprioception?ref_type=heads)  
[Platform velocity control activity](https://gitlab.kuleuven.be/rob-ecs/ecs-2026/-/tree/main/Software/mobile_platform_5c/baseline/mobile_platform_control?ref_type=heads) [optional] 

#### Shutting down

To shut down the platform use the following command

```bash
sudo shutdown -h now
```

#### Where to start?
_Have you chosen which navigation template to start with?_
* I  will compute set-points to the platform velocity control: [navigation_with_platform_control, running, compute](https://gitlab.kuleuven.be/rob-ecs/ecs-2026/-/blob/main/Software/navigation_template/src/activity/navigation_with_platform_control.c#L367) 
* I will compute set-points to the Kelo drive: [navigation_with_kelo_control, running, compute](https://gitlab.kuleuven.be/rob-ecs/ecs-2026/-/blob/main/Software/navigation_template/src/activity/navigation_with_kelo_control.c#356)

 Most of your work will take place in the section of the code pointed by either of the two links above. During the running state, the navigation activity receives data from the mobile drive, lidar, and proprioception activities. **Use this data to do something useful in your perception, control, and monitor algorithms**. For controlling the robot, you can write the desired platform forward velocity and angular rate OR the torque/velocity wheel set-points for the Kelo drive.

[application](https://gitlab.kuleuven.be/rob-ecs/ecs-2026/-/blob/main/Software/navigation_template/application/app_with_platform_control.c): In the application file we create, configure, and register activities in threads. The "plumbing" work also gets done here: some of the pointers of different activities are set to point to the same memory area for data exchange. You will need to update this file if you create your own activity/threads.   

Do not hesitate in contacting the teaching staff via the _mailing list_ if you run into trouble or need any help in understanding the code.
