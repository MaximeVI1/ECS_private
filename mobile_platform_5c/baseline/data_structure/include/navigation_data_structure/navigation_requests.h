#ifndef NAVIGATION_DATA_STRUCTURE_REQUEST_H
#define NAVIGATION_DATA_STRUCTURE_REQUEST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct nav_request_s{
    int id;
    int area;
    float velocity;
}nav_request_t;

typedef struct nav_update_s{
    int id;
    int completion;
    int area;
}nav_update_t;

#ifdef __cplusplus
}
#endif

#endif