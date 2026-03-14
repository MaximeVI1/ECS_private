#include <stdio.h>
#include <stdint.h>

#include <stdbool.h>

#ifndef KEYBOARD_H
#define KEYBOARD_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct human_computer_interface_s{
    struct{
        bool up, down, left, right;
        bool stop;
    } discrete;
    struct{
        float up, down, left, right;
    } continuous;
}human_computer_interface_t;

typedef struct keyboard_state_s{
    bool key_pressed[255];
    uint8_t last_key_pressed;
    bool new_key_available;
}keyboard_state_t;

void keyboard_hit(bool *new_key_available);

void register_key(keyboard_state_t *keyboard_state);

void clean_pressed_key(keyboard_state_t *keyboard_state);

void reset_keyboard(keyboard_state_t *keyboard_state);

void keyboard_to_hci(const keyboard_state_t *keyboard_state,
    human_computer_interface_t *hci);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // KEYBOARD_H