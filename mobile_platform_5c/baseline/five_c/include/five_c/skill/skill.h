/*
 *
 * @file skill.h
 */

#ifndef FIVE_C_SKILL_H
#define FIVE_C_SKILL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct skill_conf_s {
    void *params;
} skill_conf_t;

typedef struct skill_computational_state_s {
    void *discrete; 
    void *continuous;
}skill_computational_state_t;

typedef struct skill_state_s {
    skill_computational_state_t computational_state;
} skill_state_t;

typedef struct skill_s {
    skill_conf_t conf;
    skill_state_t state;
}skill_t;


#ifdef __cplusplus
}
#endif

#endif // FIVE_C_SKILL_H



