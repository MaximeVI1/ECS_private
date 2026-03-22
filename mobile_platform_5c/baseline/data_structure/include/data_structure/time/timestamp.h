#ifndef DATA_STRUCTURE_TIMESTAMP_H
#define DATA_STRUCTURE_TIMESTAMP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct timestamp_s{  
    uint64_t seconds;
    uint64_t nanoseconds; 
}timestamp_t;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // DATA_STRUCTURE_TIMESTAMP_H

