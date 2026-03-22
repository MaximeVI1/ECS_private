/*
 *
 * @file petrinet_scheduler.c
 * @brief Generic petrinet scheduler implementation
 *
 * This file contains the implementation of a generic petrinet scheduler.
 * This petrinet scheduler is used in the eventloop scheduler Coordinate step. It first converts flags to tokens,
 * subsequently triggers the petrinet until a steady state is reached and will eventually convert tokens back to flags.
 *
 * (c) Filip Reniers (KU Leuven) 28.09.20
 *
 */

#include <five_c/scheduler/eventloop_composition_and_execution/petrinet_scheduler.h>

void coordinate_with_petrinet(petrinet_scheduler_state_t *petrinet_scheduler_state) {
    communicate_token_flags_flag_map(petrinet_scheduler_state->petrinet[petrinet_scheduler_state->active_petrinet], 
        petrinet_scheduler_state->flag_token_conversion_map[petrinet_scheduler_state->active_petrinet]);
}

void coordinate_all_petrinets(petrinet_scheduler_state_t *petrinet_scheduler_state) {
    for (int i = 0; i < petrinet_scheduler_state->number_of_petrinets; i++) {
        communicate_token_flags_flag_map(petrinet_scheduler_state->petrinet[i], petrinet_scheduler_state->flag_token_conversion_map[i]);
    }
}

void communicate_token_flags_flag_map(petrinet_t *p, flag_token_conversion_map_t *flag_map) {

    for (int i = 0; i < flag_map->converting_sources.number_of_flags; i++){
        convert_flag_to_token(p, flag_map->converting_sources.names[i], flag_map->converting_sources.flags[i], BLACK_TOKEN);
    }

    for (int i = 0; i < flag_map->tracking_sources.number_of_flags; i++){
        token_tracks_flag(p, flag_map->tracking_sources.names[i], flag_map->tracking_sources.flags[i], BLACK_TOKEN);
    }
    //print_marking(p);
    trigger_petrinet(p);
    //print_marking(p);
    for (int i = 0; i < flag_map->converting_sinks.number_of_flags; i++){
        convert_token_to_flag(p, flag_map->converting_sinks.names[i], BLACK_TOKEN, flag_map->converting_sinks.flags[i]);
    }

    for (int i = 0; i < flag_map->tracking_sinks.number_of_flags; i++){
        flag_tracks_token(p, flag_map->tracking_sinks.names[i], BLACK_TOKEN, flag_map->tracking_sinks.flags[i]);
    }
}

/*
eventloop_scheduler_alg_t *create_eventloop_petrinet_scheduler() {
    eventloop_scheduler_alg_t *eventloop = (eventloop_scheduler_alg_t *)malloc(sizeof(eventloop_scheduler_alg_t));

    eventloop->state = (eventloop_scheduler_state_t *)malloc(sizeof(eventloop_scheduler_state_t));
    eventloop->state->coordination_state = malloc(sizeof(petrinet_scheduler_state_t));
    eventloop->state->composed_eventloop_schedule.number_of_schedules = 0;

    //eventloop->conf = (eventloop_scheduler_conf_t *)malloc(sizeof(eventloop_scheduler_conf_t));

    return eventloop;
}

void destroy_eventloop_petrinet_scheduler(eventloop_scheduler_alg_t *eventloop) {

    //free(eventloop->conf);
    free(eventloop->state->coordination_state);
    free(eventloop->state);
    free(eventloop);
}
*/