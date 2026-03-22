#ifndef DATA_STRUCTURE_HEADER_H
#define DATA_STRUCTURE_HEADER_H

#include <stdint.h>
#include <data_structure/time/timestamp.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct header_s{  
    uint64_t sequence;
    timestamp_t timestamp;
}header_t;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // DATA_STRUCTURE_HEADER_H

