#ifndef DATA_STRUCTURE_HCI_HCI_H
#define DATA_STRUCTURE_HCI_HCI_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct human_computer_interface_s{
    struct{
        bool up, down, left, right;
        bool stop;
    }discrete;
    struct{
        float up, down, left, right;
    }continuous;
}human_computer_interface_t;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // DATA_STRUCTURE_HCI_HCI_H
