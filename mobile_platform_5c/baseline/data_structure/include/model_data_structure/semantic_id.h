#ifndef SEMANTIC_ID_DATA_STRUCTURE_H
#define SEMANTIC_ID_DATA_STRUCTURE_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct semantic_id_s{  
    //! The combination meta_model + model + num is unique
    int num;    ///< deprecated
    char model[50];    ///< model id
    char meta_model[50];    ///< meta-models id
    char meta_meta_model[50];  ///< meta-meta-models id
}semantic_id_t;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SEMANTIC_ID_DATA_STRUCTURE_H