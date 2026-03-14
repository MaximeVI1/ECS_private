// A file with the extension .h is by convention
// a header file. 
// <https://gcc.gnu.org/onlinedocs/cpp/Header-Files.html>

// When including a file using #include <name-of-the-file>
// the compiler will copy and paste there entire .h file. The
// guard below (#ifndef/#define) ensures that a file is not 
// included multiple times, preventing multiple definitions 
// of the same functions, constants, enum, data structures, 
// etc.. which are typically defined in header files.
#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include<stdio.h>

typedef struct encoder_s{
  int number_of_ticks;  
  int position;         //! angular position of encoder [rad]
  int velocity;         //! angular velocity of encoder [rad/s]
  double resolution;	//! angular resolution per tick [rad]
}encoder_t;  // Do not forget this semi-colon

// Next, functions are declared but not implemented.

//! @brief Fill in the encoder object with initial values. 
void create_encoder(encoder_t* encoder);

//! @brief Log/print the model
void log_encoder_model(FILE *stream);

//! @brief Set the configuration of the encoder.
void configure_encoder(encoder_t *encoder, double resolution);

//! @brief Log/print the encoder data to a desired stream, e.g.,
// a file or console. 
void log_encoder_data(FILE *stream, encoder_t *encoder);

#endif // you have to use if you used ifndef