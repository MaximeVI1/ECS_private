#include <termios.h>
#include <sys/ioctl.h>

#include <keyboard.h>

#define KEYBOARD_LETTER_w 119
#define KEYBOARD_LETTER_s 115
#define KEYBOARD_LETTER_a 97
#define KEYBOARD_LETTER_d 100
#define KEYBOARD_space_bar 32

void keyboard_hit(bool *new_key_available) {
    struct termios term;
    tcgetattr(0, &term);

    struct termios term2 = term;
    term2.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(0, TCSANOW, &term2);

    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);

    tcsetattr(0, TCSANOW, &term);

    *new_key_available = byteswaiting > 0;

    // printf("here: %d\n",*new_key_available);
}

void register_key(keyboard_state_t *keyboard_state){
    
    if (keyboard_state->new_key_available)
    {
        int n = getchar();
        keyboard_state->key_pressed[n] = true;  // Atomic operation
        keyboard_state->last_key_pressed = n;
    }
}

void clean_pressed_key(keyboard_state_t *keyboard_state){
    // Clean previously pressed key
    if (keyboard_state->last_key_pressed >= 0)
    {
        keyboard_state->key_pressed[keyboard_state->last_key_pressed] = false;
        keyboard_state->last_key_pressed = -1;
    }
}

void reset_keyboard(keyboard_state_t *keyboard_state){
   for(int i=0; i<255; i++)
        keyboard_state->key_pressed[i] = false;
    // Initializing continuous state
    keyboard_state->last_key_pressed = -1;
    keyboard_state->new_key_available = false;
}

void keyboard_to_hci(const keyboard_state_t *keyboard_state,
    human_computer_interface_t *hci){
    //
    hci->discrete.up = keyboard_state->key_pressed[KEYBOARD_LETTER_w];
    hci->discrete.down = keyboard_state->key_pressed[KEYBOARD_LETTER_s];
    hci->discrete.left = keyboard_state->key_pressed[KEYBOARD_LETTER_a];
    hci->discrete.right = keyboard_state->key_pressed[KEYBOARD_LETTER_d];
    hci->discrete.stop = keyboard_state->key_pressed[KEYBOARD_space_bar];
}