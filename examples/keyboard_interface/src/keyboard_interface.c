/**
 * Example of data sharing between thread for
 * keyboard control of linear velocity.
 *
 * @file: keyboard_interface.c
 */

// Always include the file's header
#include <keyboard_interface.h>

/**
 * Define sigint handler.
 * This function is needed to shut down the application.
 */
static void sigint_handler(int sig);

/**
 * Listen to the keyboard and increment (W) or decrement (S)
 * the counter accordingly.
 */
void *keyboard_listen(void *arg)
{
    while (!shutdown)
    {
        // Keyboard interface data structure
        human_computer_interface_t hci;

        // Data structure to store keyboard state
        keyboard_state_t keyboard_state;

        keyboard_hit(&keyboard_state.new_key_available);
        // Transform to standard format
        keyboard_to_hci(&keyboard_state, &hci);

        // If W-key was pressed, increase counter value
        if (hci.discrete.up)
        {
            counter += 1;
        }

        // If S-key was pressed, decrease counter
        if (hci.discrete.down)
        {
            counter -= 1;
        }

        clean_pressed_key(&keyboard_state);
        register_key(&keyboard_state);

        // Sleep thread for 0.01 second.
        sleep(0.01);
    }
}

/**
 * Print the shared counter to terminal.
 * In a real application, this function would do something useful
 * with the shared data, such as sending it to a motor as velocity
 * setpoint.
 */
void *print_counter(void *arg)
{
    while (!shutdown)
    {
        printf("\nCurrent value of counter: %d\n", counter);
        sleep(1);
    }
}

// Main function is executed when running compiled code
int main()
{
    // sigint_handler will be called when CTRL+C is pressed
    signal(SIGINT, sigint_handler);

    // Set initial value of global counter to 0:
    counter = 0;

    // Set shutdown flag to 0
    shutdown = false;

    // Create threads for incrementing and printing the counter
    pthread_t keyboard_thread;
    pthread_t printing_thread;

    pthread_create(&keyboard_thread, NULL, keyboard_listen, NULL);
    pthread_create(&printing_thread, NULL, print_counter, NULL);

    pthread_join(keyboard_thread, NULL);
    pthread_join(printing_thread, NULL);

    return 0;
}

static void sigint_handler(int sig)
{
    printf("\nShutting down\n");
    shutdown = true;
}