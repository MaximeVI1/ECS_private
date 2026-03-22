/*
 *
 * @file algorithm_flow_chart.h
 * @brief Generic flow chart block data structures
 *
 * This file is a header that specifies flow chart block structs
 *
 * (c) Filip Reniers (KU Leuven) 08.09.20
 *
 */

#ifndef DEPLOYMENT_ARCHITECTURE_ALGORITHM_FLOW_CHART_H
#define DEPLOYMENT_ARCHITECTURE_ALGORITHM_FLOW_CHART_H

#include <five_c/schedule/schedule.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef
struct named_expression_s { // 1 function
    char name[MAX_LENGTH_FUNCTION_NAME];
    expression_ptr_t function;
    void *arguments;
} named_expression_t;

typedef named_function_t process_block_t;

typedef named_expression_t decision_block_t;

#ifdef __cplusplus
}
#endif

#endif //DEPLOYMENT_ARCHITECTURE_ALGORITHM_FLOW_CHART_H
