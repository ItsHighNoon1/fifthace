/**
 * @file options.c
 * @author ItsHighNoon
 * @date 02-15-2025
 * 
 * @copyright Copyright (c) 2025
 */

#include "options.h"

#include <stdlib.h>
#include <string.h>

#include "util/util.h"

#define HASH_BUCKETS 32

typedef struct HashTableEntryStruct {
    char name[FA_OPTION_MAX_LENGTH];
    FA_OptionValue value;
    struct HashTableEntryStruct* next;
    struct HashTableEntryStruct* prev;
} HashTableEntry;

static HashTableEntry* hash_table[HASH_BUCKETS];

static HashTableEntry* find(const char* name, int bucket) {
    if (bucket == -1) {
        bucket = (fa_util_hash(name) % HASH_BUCKETS);
    }
    
    for (HashTableEntry* entry = hash_table[bucket]; entry != NULL; entry = entry->next) {
        if (strcmp(name, entry->name) == 0) {
            return entry;
        }
    }

    return NULL;
}

void _fa_options_init() {
    memset(hash_table, 0, sizeof(hash_table));
}

void _fa_options_teardown() {
    for (int bucket = 0; bucket < HASH_BUCKETS; bucket++) {
        HashTableEntry* hash_entry = hash_table[bucket];
        while (hash_entry != NULL) {
            HashTableEntry* next = hash_entry->next;
            if (hash_entry->value.type == FA_OPTION_STRING) {
                free(hash_entry->value.string_value);
            }
            free(hash_entry);
            hash_entry = next;
        }
    }
}

int fa_options_set_int(const char* name, int value) {
    if (strlen(name) >= FA_OPTION_MAX_LENGTH) {
        return 1;
    }
    int bucket = (fa_util_hash(name) % HASH_BUCKETS);
    
    HashTableEntry* old_entry = find(name, bucket);

    if (old_entry != NULL) {
        // Already in the hash table, just set it
        if (old_entry->value.type == FA_OPTION_STRING) {
            free(old_entry->value.string_value);
        }
        old_entry->value.type = FA_OPTION_INT;
        old_entry->value.int_value = value;
    } else {
        // Not in the hash table, make a new entry
        HashTableEntry* new_entry = malloc(sizeof(HashTableEntry));
        new_entry->value.type = FA_OPTION_INT;
        new_entry->value.int_value = value;
        strcpy(new_entry->name, name);

        new_entry->next = hash_table[bucket];
        new_entry->prev = NULL;
        hash_table[bucket] = new_entry;
        if (new_entry->next != NULL) {
            new_entry->next->prev = new_entry;
        }
    }

    return 0;
}

int fa_options_set_float(const char* name, float value) {
    if (strlen(name) >= FA_OPTION_MAX_LENGTH) {
        return 1;
    }
    int bucket = (fa_util_hash(name) % HASH_BUCKETS);
    
    HashTableEntry* old_entry = find(name, bucket);

    if (old_entry != NULL) {
        // Already in the hash table, just set it
        if (old_entry->value.type == FA_OPTION_STRING) {
            free(old_entry->value.string_value);
        }
        old_entry->value.type = FA_OPTION_FLOAT;
        old_entry->value.float_value = value;
    } else {
        // Not in the hash table, make a new entry
        HashTableEntry* new_entry = malloc(sizeof(HashTableEntry));
        new_entry->value.type = FA_OPTION_INT;
        new_entry->value.float_value = value;
        strcpy(new_entry->name, name);

        new_entry->next = hash_table[bucket];
        new_entry->prev = NULL;
        hash_table[bucket] = new_entry;
        if (new_entry->next != NULL) {
            new_entry->next->prev = new_entry;
        }
    }

    return 0;
}

int fa_options_set_string(const char* name, const char* value) {
    if (strlen(name) >= FA_OPTION_MAX_LENGTH) {
        return 1;
    }
    int bucket = (fa_util_hash(name) % HASH_BUCKETS);
    
    HashTableEntry* old_entry = find(name, bucket);

    if (old_entry != NULL) {
        // Already in the hash table, just set it
        if (old_entry->value.type == FA_OPTION_STRING) {
            free(old_entry->value.string_value);
        }
        old_entry->value.type = FA_OPTION_STRING;
        old_entry->value.string_value = malloc(strlen(value) + 1);
        strcpy(old_entry->value.string_value, value);
    } else {
        // Not in the hash table, make a new entry
        HashTableEntry* new_entry = malloc(sizeof(HashTableEntry));
        new_entry->value.type = FA_OPTION_STRING;
        new_entry->value.string_value = malloc(strlen(value) + 1);
        strcpy(new_entry->value.string_value, value);
        strcpy(new_entry->name, name);

        new_entry->next = hash_table[bucket];
        new_entry->prev = NULL;
        hash_table[bucket] = new_entry;
        if (new_entry->next != NULL) {
            new_entry->next->prev = new_entry;
        }
    }

    return 0;
}

int fa_options_unset(const char* name) {
    if (strlen(name) >= FA_OPTION_MAX_LENGTH) {
        return 1;
    }
    int bucket = (fa_util_hash(name) % HASH_BUCKETS);
    
    HashTableEntry* entry = find(name, bucket);

    if (entry != NULL) {
        if (entry->value.type == FA_OPTION_STRING) {
            free(entry->value.string_value);
        }

        // Remove from the hash table
        if (entry->prev != NULL) {
            entry->prev->next = entry->next;
        } else {
            hash_table[bucket] = entry->next;
        }
        if (entry->next != NULL) {
            entry->next->prev = entry->prev;
        }

        free(entry);
    }

    return 0;
}

FA_OptionValue fa_options_get(const char* name) {
    HashTableEntry* entry = find(name, -1);

    if (entry != NULL) {
        return entry->value;
    } else {
        FA_OptionValue dummy;
        dummy.type = FA_OPTION_UNSET;
        return dummy;
    }
}