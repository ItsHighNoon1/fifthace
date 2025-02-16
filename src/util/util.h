/**
 * @file util.h
 * @author ItsHighNoon
 * @date 02-15-2025
 * 
 * @copyright Copyright (c) 2025
 * 
 * Miscellaneous utility functions.
 */

#pragma once

/**
 * Hash a string to an unsigned long.
 *
 * https://stackoverflow.com/questions/7666509/hash-function-for-string
 * @param string A pointer to the string to hash. Assumed to be null terminated.
 * @return The hashed string value.
 */
unsigned long fa_util_hash(const char* string);