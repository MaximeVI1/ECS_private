#include<stdio.h>
#include<math.h>

typedef struct encoder_s{
  int number_of_ticks;  
  int position;         //! angular position of encoder [rad]
  int velocity;         //! angular velocity of encoder [rad/s]
  double resolution;	//! angular resolution per tick [rad]
}encoder_t;  // Do not forget this semi-colon

/**
 * Fill in the encoder object with initial values,
 * sing a _pointer_ to the encoder data structure
 * as the argument to the function below:
*/
void create_encoder(encoder_t* encoder)
{
  encoder->number_of_ticks = 0;    // the "->" directs from the _pointer_ to the actual _data_
  encoder->position = 0;
  encoder->velocity = 0;
  encoder->resolution = 0;
}

/**
 * Set the configuration of the encoder
 */
void configure_encoder(encoder_t *encoder, double resolution)
{
  encoder->resolution = resolution;
}

/**
 * Log/print the model
 */ 
void log_encoder_model(FILE *stream)
{
  fprintf(stream, "[encoder] number_of_ticks, position, velocity, resolution\n");
}

/**
 * Log/print the encoder data to a desired stream, e.g.,
 * a file or console
 */ 
void log_encoder_data(FILE *stream, encoder_t *encoder)
{
  fprintf(stream, "%d, %d, %d, %f\n",
    encoder->number_of_ticks, 
    encoder->position,
    encoder->velocity,
    encoder->resolution);
}

int main(int argc, char *argv)
{
  encoder_t encoder;

  // The operator & refers to the address of a variable
  create_encoder(&encoder);
  log_encoder_model(stdout);
  log_encoder_data(stdout, &encoder);
  configure_encoder(&encoder, M_PI/1800);
  log_encoder_data(stdout, &encoder);

  return 0;
}
