#include <stdio.h>
#include <math.h>

#define TICKS_PER_PI 2000 // number of encoder pulses per pi radians

//! Define a new data type name as 'struct encoder'
struct encoder_s
{
  int number_of_ticks;
  int position; //! angular position of encoder [rad]
  int velocity; //! angular velocity of encoder [rad/s]

  double resolution; //! angular resolution per tick [rad]
}; // Do not forget this semi-colon

//! The keyword typedef allows to give a new name to
// an existing data type.
typedef struct encoder_s encoder_t;

// The lagging _s and _t are conventions that we have
// adopted in the course.

// We can create a new data type and rename it.
// This is most common approach in the ECS code.
typedef struct wheel_s
{
  encoder_t encoder;
} wheel_t;

// For more information about data structures, follow the link:
// <https://beej.us/guide/bgc/html/split/structs.html#structs>

int main(int argc, char **argv)
{
  // We can instantiate in two different manners.
  struct wheel_s wheel0;
  wheel_t wheel1;

  // Writing in the fields of a data structure.
  // M_PI is a constant is defined in math.h
  wheel0.encoder.number_of_ticks = 0;
  wheel0.encoder.position = 3;
  wheel0.encoder.velocity = 0;
  wheel0.encoder.resolution = M_PI / TICKS_PER_PI; // 0.1 deg resolution

  // create and initialise fields
  wheel_t wheel2 = {
      .encoder = {
          .number_of_ticks = 0,
          .position = 0,
          .velocity = 0,
          .resolution = M_PI / 2 / TICKS_PER_PI}};

  // Accessing the fields of a data structure
  printf("wheel0.encoder.resolution: %.5f\n", wheel0.encoder.resolution);
  printf("wheel2.encoder.resolution: %.5f\n", wheel2.encoder.resolution);

  return 0;
};
