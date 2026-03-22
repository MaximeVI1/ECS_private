/*
 *
 * @file petrinet_scheduler.h
 * @brief Header of generic petrinet scheduler implementation
 *
 * This file is the header of the implementation of a generic petrinet scheduler.
 * This petrinet scheduler is used in the eventloop scheduler Coordinate step. It first converts flags to tokens,
 * subsequently triggers the petrinet until a steady state is reached and will eventually convert tokens back to flags.
 *
 * (c) Filip Reniers (KU Leuven) 28.09.20
 *
 */

#ifndef DEPLOYMENT_ARCHITECTURE_PETRINET_SCHEDULER_H
#define DEPLOYMENT_ARCHITECTURE_PETRINET_SCHEDULER_H

#include <coordination_libraries/petrinet/petrinet.h>

#define MAX_NUMBER_OF_PETRINETS 10

#ifdef __cplusplus
extern "C" {
#endif

    typedef
    struct flag_token_array_s {
        char **names;
        bool **flags;
        int number_of_flags;
    } flag_token_array_t;

    typedef
    struct flag_token_conversion_map_s {
        flag_token_array_t converting_sources;
        flag_token_array_t tracking_sources;
        flag_token_array_t converting_sinks;
        flag_token_array_t tracking_sinks;
    } flag_token_conversion_map_t;

    typedef
    struct petrinet_scheduler_state_s{
        petrinet_t *petrinet[MAX_NUMBER_OF_PETRINETS];
        flag_token_conversion_map_t *flag_token_conversion_map[MAX_NUMBER_OF_PETRINETS];
        int active_petrinet;
        int number_of_petrinets;
    } petrinet_scheduler_state_t;

    //eventloop_scheduler_alg_t *create_eventloop_petrinet_scheduler();
    //void destroy_eventloop_petrinet_scheduler(eventloop_scheduler_alg_t *eventloop);

    void coordinate_with_petrinet(petrinet_scheduler_state_t *petrinet_scheduler_state);
    void coordinate_all_petrinets(petrinet_scheduler_state_t *petrinet_scheduler_state);

    void communicate_token_flags_flag_map(petrinet_t *p, flag_token_conversion_map_t *flag_map);

#ifdef __cplusplus
}
#endif

#endif //DEPLOYMENT_ARCHITECTURE_PETRINET_SCHEDULER_H
