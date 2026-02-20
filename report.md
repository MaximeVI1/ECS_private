report of Maxime Van Insberghe, Richard Nollet, Martijn Spaepen

## experiment setup

The robot drives in a straight line such that both wheels experience a step increase in rolling resistance when transitioning from a smooth surface to a high-friction surface, (for example a carpet).
This creates a disturbance torque without mechanical impact or configuration changes.
![Alt text](setup.svg "Setup")
The experiment is executed twice: once with wheel velocity control and once with wheel torque control. 
Logged data: wheel encoders (angular position) <a>$\theta$</a>, estimated motor torque <a>$\hat{\tau}$</a>, motor current <a>*I*</a>.

## experiment expectations
### velocity control

On the smooth surface, the wheels reach steady angular velocity <a>$\omega^*$</a>, with motor current settling to a certain value balancing internal losses and rolling resistance.

When the robot reaches the carpet, the friction will suddendly increase.
From first principles: 
<a>$J \dot{\omega} = \tau_{motor}-\tau_{resist}$</a>.

The increased <a>$\tau_{resist}$</a> causes a brief drop in <a>$\omega$</a>. The controller reacts to this velocity error by increasing motor current, producing higher torque. After a transient, <a>$\omega$</a> returns close to <a>$\omega^*$</a>, while <a>$\hat{\tau}$</a> and <a>*I*</a> settle at a higher steady value corresponding to the increased friction.

### torque control
A constant motor torque <a>$\tau^*$</a> is set.

On the smooth surface, this produces a steady angular velocity. When entering the carpet, <a>$\tau_{resist}$</a> increases while <a>$\tau_{motor}$</a> remains fixed. Consequently, net torque decreases and the wheel decelerates:

<a>$J \dot{\omega} = \tau^*-\tau_{resist} < 0$</a>

This leads to a reduction in <a>$\omega$</a>. Unlike velocity control, no corrective torque is applied. Motor current remains approximately constant, while wheel speed drops to a lower equilibrium. In the event that <a>$\tau_{resist}$</a> surpasses <a>$\tau_{motor}$</a>, the net driving force will become insufficient to sustain motion. Consequently, the robot will decelerate and eventually reach a complete stall, as the system lacks the necessary torque to overcome the increased external load.

### data collection
Encoder data provides <a>$\theta(t)$</a>, firmware logs give <a>*I*(t)</a>. <a>$\hat{\tau}(t)$</a> is provided by the firmware interface directly (if available) or estimated offline from <a>*I*(t)</a> using a torque constant. Comparing signals before and after the transition reveals how each type of control handles increased external load.
