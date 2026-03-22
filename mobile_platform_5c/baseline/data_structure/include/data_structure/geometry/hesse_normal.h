#ifndef DATA_STRUCTURE_GEOMETRY_HESSE_NORMAL_H
#define DATA_STRUCTURE_GEOMETRY_HESSE_NORMAL_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct hesse_normal_s{
    float normal_x, normal_y;
    float distance_to_line;
}hesse_normal_t;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // DATA_STRUCTURE_GEOMETRY_HESSE_NORMAL_H

