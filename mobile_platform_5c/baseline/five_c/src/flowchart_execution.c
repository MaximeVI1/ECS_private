/*
 *
 * @file algorithm_execution.c
 * @brief Generic flow chart DAG solver
 *
 * This file contains the implementation of the generic flow chart DAG solver.
 * There is an optimized version which executes an array of function pointers in the case
 * of a DAG with only 1 branch.
 *
 * (c) Filip Reniers (KU Leuven) 10.09.20
 *
 */

#include <five_c/scheduler/flow_chart_composition_and_execution/flowchart_execution.h>
#include <five_c/scheduler/flow_chart_composition_and_execution/flow_chart_block_types.h>

#include<stdbool.h>

void do_iteration_complete_DAG(DAG_t *dag){
    vertex_t *v = get_vertex_by_name(dag, "entry");
    v = next_first_vertex(v);
    bool stop = false;
    while (!stop) {
        bool decision_expression = false;
        char *vertex_type = get_vertex_type(v);
        if (!strcmp(vertex_type, "connection_block")) {
            if (!strcmp(get_vertex_name(v), "exit")) {
                stop = true;
            } else {
                v = next_first_vertex(v);
            }
        } else if (!strcmp(vertex_type, "process_block")) {
            process_block_t *pb = (process_block_t *) get_vertex_attribute(v);
            pb->function(pb->arguments);
            v = next_first_vertex(v);
        } else if (!strcmp(vertex_type, "decision_block")) {

            decision_block_t *db = (decision_block_t *) get_vertex_attribute(v);
            decision_expression = (bool) db->function(db->arguments);
            if (decision_expression) {
                v = next_first_vertex(v);
            } else {
                v = next_second_vertex(v);
            }
        } else {
            stop = true;
        }
    }
}
