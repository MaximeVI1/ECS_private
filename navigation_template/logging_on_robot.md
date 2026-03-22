## Logging

To log data from your experiments, the platform uses [LTTNG](https://lttng.org/), an open source 
tracing toolkit for Linux systems. Full documentation can be found [here](https://lttng.org/docs/v2.15/),
but this document will go over the necessary commands for logging in ECS.

To write data to LTTNG, you will need to execute a series of commands while ssh'ed into the robot.
To read that data, you will need to connect to the robot's LTTNG session from you laptop.

### Dependencies

You should install the following dependencies:

```bash
sudo apt-get install lttng-tools
sudo apt-get install liblttng-ust-dev
sudo apt-get install babeltrace2
```


### Starting LTTNG on the robot

**Before executing these commands, ensure you are ssh'ed into the robot as explained in the README.**

Use the following command to start LTTNG in the **robot** terminal:

```bash
lttng-sessiond --daemonize
```

### Creating a logging session

You can now either start a local logging session.

When you start a local logging session, the logs will be stored on the robot in the directory you specify.
As it is not a live session, the session needs to be stopped or destroyed in order to access the files with babeltrace2.

#### Local logging session

To start a local logging session, run the following commands in the **robot** terminal:

```bash
lttng create session-in-robot --output=/home/ecs-pi-1/ecs-2026/Students/<YOUR_DIRECTORY>/logs/<EXPERIMENT_NAME>
lttng enable-event --userspace --session=session-in-robot logging:drive_sensor_trace
lttng enable-event --userspace --session=session-in-robot logging:drive_actuation_trace
lttng start
``` 

The tracing session is now live and over the network and you can try to connect to it using your laptop.

If you want to stop the LTTNG tracing session, you can use the command (needed to read the log files with babeltrace2, otherwise permission issues arise):

```bash
lttng stop
```

Afterwards, the tracing can be restarted using 

```bash
lttng start
```

When you have finished your experiment, destroy the session (needed to read the log files with babeltrace2, otherwise permission issues arise).

```bash
lttng destroy
```

We recommend you create a new session for each experiment you do, 
such that you can give meaningful names to your log directories.

To start new sessions or live sessions, make sure you destroyed previous sessions.

### Read the log file

In a **robot** terminal, check that you can see the traces uses the following command:

```bash
babeltrace2 /home/ecs-pi-1/ecs-2026/Students/<YOUR_DIRECTORY>/logs/<EXPERIMENT_NAME>
```

This will print out the results of the logging to the terminal.
