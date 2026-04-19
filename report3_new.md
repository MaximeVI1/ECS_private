# Report III — Corridor Navigation and Doorway Approach  
Authors: Martijn Spaepen, Richard Nollet, Maxime Van Insberghe  

---

## 1. Introduction

The goal of this project is to design a control system that enables the robot to navigate through a corridor, remain centered between walls, detect a doorway, and position itself in front of that doorway.

The focus is on implementing a **feasible level of autonomy** on the platform, where perception, estimation, and control operate continuously. 
---

## 2. Task Description and Real-World Context

The robot operates in a real corridor environment in the robotics lab.

Unlike an idealized geometric model, in the real corridor environment:

- walls are not perfectly straight or parallel  
- LIDAR measurements are noisy and may contain missing or spurious readings  
- doorway edges are not sharply defined  
- small objects or irregularities may be present along the walls 

The robot performs the following task:

1. Move forward while staying centered between walls  
2. Continuously monitor free space using LIDAR  
3. Detect a doorway (opening in the wall)  
4. Align with the doorway  
5. Stop in front of the doorway  

The task is defined in terms of locally observable features and conditions.

---

## 3. World Model

The world model is minimal and task-driven, representing only features required for the task.

### Stored variables:

- d_L : distance to left wall  
- d_R : distance to right wall  
- e : lateral error (d_L - d_R)  
- Δθ_opening : angular width of detected opening  
- opening_detected : boolean  

No global position or map is maintained.

---

## 4. Software Architecture

The system consists of three asynchronous threads:

### Thread 1 — Sensor Processing (10 Hz)

Input: raw LIDAR scan  
Output:
- d_L, d_R  
- Δθ_opening  
- opening_detected  

### Thread 2 — Control Loop (50 Hz)

Input: filtered d_L, d_R  
Output: velocity commands (v, ω)

### Thread 3 — FSM (10 Hz)

Input: opening_detected  
Output: current task state  

Data exchange is performed through shared variables updated at their respective rates.

---

## 5. Task FSM

### State 1: Corridor Following
- Maintain centered motion  
- Monitor doorway conditions  

### State 2: Doorway Approach
- Reduce speed  
- Align with opening  

### State 3: Stop
- Robot stops in front of doorway  

---

## 6. Detailed Design — State 1 (Corridor Following)

### 6.1 Sensor Processing

Let the LIDAR provide ranges[i] with angular resolution Δθ.

Distances are computed as:

d_L = mean(ranges[i]) for θ ∈ [60°, 90°]  
d_R = mean(ranges[i]) for θ ∈ [-90°, -60°]

To reduce noise, a temporal filter is applied:

d_L(t) = α · d_L(t) + (1 - α) · d_L(t-1)  
d_R(t) = α · d_R(t) + (1 - α) · d_R(t-1)

---

### 6.2 Opening Detection

Define:

S = { i | ranges[i] > d_threshold }

Δθ_opening = max_consecutive_beams(S) · Δθ

A doorway is detected if:

- (d_L > d_threshold or d_R > d_threshold)  
- Δθ_opening > θ_min  
- condition holds for N consecutive scans  

This ensures robustness against noise and transient measurements.

---

### 6.3 Control Model

The robot platform exhibits non-linear kinematic behavior.  
However, for this task, a local simplified model is sufficient.

The control objective is:

e = d_L - d_R → 0

---

### 6.4 Control Loop

A closed-loop velocity controller is used:

ω = clamp(-K_p · e, -ω_max, ω_max)  
v = v_0  

where ω is the rotational velocity of the robot platform.

---

### 6.5 Control Execution (50 Hz)

loop:
    read d_L, d_R  
    e = d_L - d_R  

    ω = -K_p · e  
    ω = clamp(ω, -ω_max, ω_max)  

    v = v_0  

    send_command(v, ω)

---

### 6.6 Estimation vs Control

The system separates estimation and control:

- **Estimation (Sensor Processing):**
  Extracts d_L and d_R from noisy LIDAR data and applies temporal filtering.

- **Control:**
  Uses the estimated error e to compute motion commands.

This ensures that control decisions are based on consistent observations over time, rather than instantaneous measurements.

---

### 6.7 Assumptions and Trade-offs

- Only local distances are estimated → no global map required  
- Temporal filtering improves robustness but introduces delay  
- Velocity control is chosen because it provides sufficient performance for the task while remaining compatible with the platform’s actuator interface.
- No trajectory tracking → motion defined by local error correction  

---

## 7. State Transitions

State 1 → State 2:

if opening_detected == true:
    transition to State 2  

State 2 → State 3:

if robot aligned with opening AND distance_to_opening < d_stop:
    transition to State 3  

---

## 8. Discussion

This design avoids:

- global path planning  
- precise trajectory tracking  
- idealized geometric environments  

Instead, it focuses on:

- real-world sensor-driven behavior  
- asynchronous execution  
- robustness to imperfect observations  

---

## 9. Conclusion

A concrete control architecture was developed for corridor navigation and doorway detection.

The report includes:
- explicit sensor processing equations  
- a defined control loop  
- asynchronous system design  
- separation of estimation and control  

The level of detail is sufficient to begin implementation in Report 4.