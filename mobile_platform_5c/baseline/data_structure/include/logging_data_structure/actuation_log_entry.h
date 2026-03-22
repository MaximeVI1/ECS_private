#ifndef LOGGING_DATA_STRUCTURE_ACTUATION_LOG_ENTRY_H
#define LOGGING_DATA_STRUCTURE_ACTUATION_LOG_ENTRY_H

#include <time.h>
#include <stdint.h>

#include <actuation_data_structure/differential_drive_actuation.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct actuation_log_entry_s{
    uint64_t id;
    differential_drive_actuation_t actuation;
} actuation_log_entry_t;

#ifdef __cplusplus
}
#endif

#endif