#include "data_structure.h" 

// This files containts the implementations of the
// functions defined in ex3_data_structure.c

void create_encoder(encoder_t* encoder)
{
  encoder->number_of_ticks = 0;
  encoder->position = 0;
  encoder->velocity = 0;
  encoder->resolution = 0;
  
} // this one creates variables who are used later on


//fprintf(encoder);

void log_encoder_model(FILE *stream)
{
  fprintf(stream, "[encoder] number_of_ticks, position, velocity, resolution\n");
}

void configure_encoder(encoder_t *encoder, double resolution)
{
  encoder->resolution = resolution;
}

void log_encoder_data(FILE *stream, encoder_t *encoder)
{
  fprintf(stream, "%d, %d, %d, %f\n",
    encoder->number_of_ticks, 
    encoder->position,
    encoder->velocity,
    encoder->resolution);
}