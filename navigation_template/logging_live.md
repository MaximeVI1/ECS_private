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
```


### Starting LTTNG on the robot

**Before executing these commands, ensure you are ssh'ed into the robot as explained in the README.**

Use the following command to start LTTNG:

```bash
lttng-relayd  --live-port=tcp://192.168.0.105:5344
```

Now open a second terminal and ssh into the robot again,
Next run the following commands:

```bash
lttng create session-live --live
lttng enable-event --userspace --session=session-live logging:drive_sensor_trace
lttng enable-event --userspace --session=session-live logging:drive_actuation_trace
lttng start
``` 

The tracing session is now live and over the network and you can try to connect to it using your laptop.

If you want to stop the LTTNG tracing session, you can use the command

```bash
lttng stop
```

Afterwards, the tracing can be restarted using 

```bash
lttng start
```

### Connecting to LTTNG from your laptop

In a local terminal, check that you can see the traces uses the following command:

```bash
babeltrace2 net://192.168.0.105 --input-format=lttng-live
```

The output of the command should look like this:

```bash
net://192.168.0.105/host/raspberrypi/session-live
```

You can now connect to the session using the following command

```bash
babeltrace2 net://192.168.0.105/host/raspberrypi/session-live
```

This will print out the results of the logging to the terminal.
Scripts to connect to babeltrace and parse incomming streams will be given later.

### Processing the data

In case you want to plot or process the data from the live logging, modify the example file "listener.py".
