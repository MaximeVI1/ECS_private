#ifndef DATA_STRUCTURE_DATA_TABLE_H
#define DATA_STRUCTURE_DATA_TABLE_H

#include <string.h>
#include <threads.h>

#include <uthash.h>

#include <data_structure/registration/hash_table_utils.h>

#include <model_data_structure/semantic_id.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct variable_entry_s 
{
    void *pointer;
    mtx_t mutex;
}variable_entry_t;

typedef struct variable_entry_hh_s
{
    semantic_id_t sid;
    variable_entry_t entry;
    UT_hash_handle hh_sid; /* makes this structure hashable */
}variable_entry_hh_t;

typedef struct variable_table_s
{
    variable_entry_hh_t *root;
    mtx_t mutex;
}variable_table_t;

void create_variable_table(variable_table_t *variable_table);

typedef struct variable_registration_args_s
{
    semantic_id_t data;
    semantic_id_t activity;
}variable_registration_args_t;

hash_result_t find_variable_entry(variable_entry_hh_t **table, 
    semantic_id_t *sid); 

hash_result_t get_variable_entry(variable_entry_hh_t **table, 
    semantic_id_t *sid, variable_entry_t **entry);

#define FIND_VARIABLE_IN_TABLE(table, name_of_variable, datatype, result) do{   \
    variable_entry_hh_t tmp;                                                    \
    variable_entry_hh_t *entry_in_the_table = NULL;                             \
    semantic_id_t sid = {.meta_model=#datatype};                                \
    strcpy(sid.model, name_of_variable);                                        \
    memset(&tmp, 0, sizeof(variable_entry_hh_t));                               \
    memcpy(&tmp.sid, &sid, sizeof(semantic_id_t));                              \
    HASH_FIND(hh_sid, *table, &tmp.sid, sizeof(tmp.sid), entry_in_the_table);   \
    if (entry_in_the_table)                                                     \
    {                                                                           \
        result = HASH_ENTRY_FOUND;                                              \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        result = HASH_ENTRY_NOT_FOUND;                                          \
    }                                                                           \
}while(0)                                                                       \

#define GET_VARIABLE_IN_TABLE(table, variable_registration, datatype,           \
    object_pointer, object_mutex, result) do{                                   \
    variable_entry_hh_t tmp;                                                    \
    variable_entry_hh_t *entry_in_the_table = NULL;                             \
    semantic_id_t sid = {.meta_model=#datatype};                                \
    strcpy(sid.model, variable_registration.data.model);                        \
    memset(&tmp, 0, sizeof(variable_entry_hh_t));                               \
    memcpy(&tmp.sid, &sid, sizeof(semantic_id_t));                              \
    HASH_FIND(hh_sid, table->root, &tmp.sid, sizeof(tmp.sid), entry_in_the_table);   \
    if (entry_in_the_table)                                                     \
    {                                                                           \
        object_pointer = (datatype *) (entry_in_the_table)->entry.pointer;      \
        object_mutex =  &(entry_in_the_table)->entry.mutex;                     \
        result = HASH_ENTRY_FOUND;                                              \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        result = HASH_ENTRY_NOT_FOUND;                                          \
    }                                                                           \
}while(0)                                                                       \

#define REGISTER_VARIABLE_IN_TABLE(table, variable_registration, datatype, result)do{\
    variable_entry_hh_t *entry_hh = NULL;                                       \
    semantic_id_t sid = {.meta_model=#datatype};                                \
    strcpy(sid.model, variable_registration.data.model);                        \
    mtx_lock(&table->mutex);                                                    \
    if (find_variable_entry(&table->root, &sid) == HASH_ENTRY_NOT_FOUND)        \
    {                                                                           \
        entry_hh = (variable_entry_hh_t *)malloc(sizeof(variable_entry_hh_t));  \
        if(entry_hh!=NULL)                                                      \
        {                                                                       \
            memset(entry_hh, 0, sizeof(variable_entry_hh_t));                   \
            memcpy(&(entry_hh)->sid, &sid, sizeof(semantic_id_t));              \
            (entry_hh)->entry.pointer = malloc(sizeof(datatype));               \
            mtx_init(&(entry_hh)->entry.mutex, mtx_plain );		                \
            if( (entry_hh)->entry.pointer != NULL )                             \
            {                                                                   \
                HASH_ADD(hh_sid, table->root, sid, sizeof(semantic_id_t),       \
                entry_hh);                                                      \
                result = HASH_ENTRY_ADDED;                                      \
            }                                                                   \
            else                                                                \
            {                                                                   \
                result = HASH_ENTRY_MEMORY_ALLOCATION_ERROR;                    \
            }                                                                   \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            result = HASH_ENTRY_MEMORY_ALLOCATION_ERROR;                        \
        }                                                                       \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        result = HASH_ENTRY_ALREADY_EXISTS;                                     \
    }                                                                           \
    mtx_unlock(&table->mutex);                                                  \
}while(0)                                                                       \

#define REGISTER_AND_GET_VARIABLE_IN_TABLE(table, variable_registration,        \
    datatype, object_pointer, object_mutex, result) do{                         \
    variable_entry_hh_t *entry_hh = NULL;                                       \
    semantic_id_t sid = {.meta_model=#datatype};                                \
    strcpy(sid.model, variable_registration.data.model);                        \
    mtx_lock(&table->mutex);                                                    \
    if (find_variable_entry(&table->root, &sid) == HASH_ENTRY_NOT_FOUND)        \
    {                                                                           \
        entry_hh = (variable_entry_hh_t *)malloc(sizeof(variable_entry_hh_t));  \
        if(entry_hh!=NULL)                                                      \
        {                                                                       \
            memset(entry_hh, 0, sizeof(variable_entry_hh_t));                   \
            memcpy(&(entry_hh)->sid, &sid, sizeof(semantic_id_t));              \
            (entry_hh)->entry.pointer = malloc(sizeof(datatype));               \
            if( (entry_hh)->entry.pointer != NULL )                             \
            {                                                                   \
                HASH_ADD(hh_sid, table->root, sid, sizeof(semantic_id_t),       \
                entry_hh);                                                      \
                object_pointer = (datatype *)  (entry_hh)->entry.pointer;       \
                object_mutex =  &(entry_hh)->entry.mutex;                       \
                result = HASH_ENTRY_ADDED;                                      \
            }                                                                   \
            else                                                                \
            {                                                                   \
                result = HASH_ENTRY_MEMORY_ALLOCATION_ERROR;                    \
            }                                                                   \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            result = HASH_ENTRY_MEMORY_ALLOCATION_ERROR;                        \
        }                                                                       \
                                                                                \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        result = HASH_ENTRY_ALREADY_EXISTS;                                     \
    }                                                                           \
    mtx_unlock(&table->mutex);                                                  \
}while(0)                                                                       \

#ifdef __cplusplus
}
#endif

#endif