/**
 * @file options.h
 * @author ItsHighNoon
 * @date 02-15-2025
 * 
 * @copyright Copyright (c) 2025
 * 
 * Options management, similar to Quake/Source cvars.
 */

#pragma once

#define FA_OPTION_UNSET 0
#define FA_OPTION_INT 1
#define FA_OPTION_FLOAT 2
#define FA_OPTION_STRING 3

// The maximum length of an option name. Has an impact on memory footprint.
#define FA_OPTION_MAX_LENGTH 224

typedef struct {
    /**
     * One of the following:
     * - FA_OPTION_UNSET - There is no value stored here.
     * - FA_OPTION_INT
     * - FA_OPTION_FLOAT
     * - FA_OPTION_STRING
     */
    int type;
    union {
        /**
         * The value stored here, if type is FA_OPTION_INT.
         */
        int int_value;

        /**
         * The value stored here, if type is FA_OPTION_FLOAT.
         */
        float float_value;

        /**
         * A pointer to the value stored here, if type is FA_OPTION_STRING.
         */
        char* string_value;
    };
} FA_Value;

void _fa_options_init();

void _fa_options_teardown();

/**
 * Set the value of a global option to the specified integer.
 * @param name The name of the option. Assumed to be null terminated.
 * @param value The desired value of the option.
 * @return 0 on success, 1 if name is too long.
 */
int fa_options_set_int(const char* name, int value);

/**
 * Set the value of a global option to the specified float.
 * @param name The name of the option. Assumed to be null terminated.
 * @param value The desired value of the option.
 * @return 0 on success, 1 if name is too long.
 */
int fa_options_set_float(const char* name, float value);

/**
 * Set the value of a global option to the specified string.
 * @param name The name of the option. Assumed to be null terminated.
 * @param value The desired value of the option. Will be copied to a new location. Assumed to be null terminated.
 * @return 0 on success, 1 if name is too long.
 */
int fa_options_set_string(const char* name, const char* value);

/**
 * Reset a global option to having no value.
 * @param name The name of the option. Assumed to be null terminated.
 * @return 0 on success, 1 if name is too long.
 */
int fa_options_unset(const char* name);

/**
 * Get the value of a global option.
 * @param name The name of the option. Assumed to be null terminated.
 * @return The value of the requested parameter. See FA_Value. Will be FA_OPTION_UNSET if the option is not set or if name is too long.
 */
FA_Value fa_options_get(const char* name);