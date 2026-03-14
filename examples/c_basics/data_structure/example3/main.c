#include <math.h>
#include <stdlib.h>
#include "data_structure.h"

int main(int argc, char **argv)
{
  encoder_t encoder;

  // The operator & refers to the address of a variable
  create_encoder(&encoder);
  //fprintf(encoder);
  log_encoder_model(stdout);
  log_encoder_data(stdout, &encoder);
  configure_encoder(&encoder, M_PI/1800);
  log_encoder_data(stdout, &encoder);
}
