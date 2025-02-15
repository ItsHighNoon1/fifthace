/**
 * @file util.c
 * @author ItsHighNoon
 * @date 02-15-2025
 * 
 * @copyright Copyright (c) 2025
 */

#include "util.h"

unsigned long fa_hash(const char* string) {
    unsigned long hash = 5381;
    char c;
    while ((c = *string++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}