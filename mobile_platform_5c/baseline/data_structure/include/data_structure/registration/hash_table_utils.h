#ifndef DATA_STRUCTURE_HASH_H
#define DATA_STRUCTURE_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    HASH_ENTRY_FOUND,
    HASH_ENTRY_ADDED,
    HASH_ENTRY_NOT_FOUND,
    HASH_ENTRY_ALREADY_EXISTS,
    HASH_ENTRY_UNKOWN_DATATYPE,
    HASH_ENTRY_MEMORY_ALLOCATION_ERROR
}hash_result_t;

void hash_result_to_string(hash_result_t result, char *string);

#ifdef __cplusplus
}
#endif

#endif
