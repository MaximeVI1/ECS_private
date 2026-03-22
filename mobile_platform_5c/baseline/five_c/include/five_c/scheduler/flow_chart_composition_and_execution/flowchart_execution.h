/*
 *
 * @file algorithm_execution.h
 * @brief Header of Generic flow chart DAG solver
 *
 * This file is the header of the implementation of the generic flow chart DAG solver.
 * There is an optimized version which executes an array of function pointers in the case
 * of a DAG with only 1 branch.
 *
 * (c) Filip Reniers (KU Leuven) 10.09.20
 *
 */

#ifndef DEPLOYMENT_ARCHITECTURE_ALGORITHM_EXECUTION_H
#define DEPLOYMENT_ARCHITECTURE_ALGORITHM_EXECUTION_H

#include <coordination_libraries/DAG/DAG.h>

#ifdef __cplusplus
extern "C" {
#endif

void do_iteration_complete_DAG(DAG_t *dag);

#ifdef __cplusplus
}
#endif

#endif //DEPLOYMENT_ARCHITECTURE_ALGORITHM_EXECUTION_H
