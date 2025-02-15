/**
 * @file main.c
 * @author ItsHighNoon
 * @date 02-13-2025
 * 
 * @copyright Copyright (c) 2025
 * 
 * Entry point of the application. Start and stop subsystems.
 */

#include <stdio.h>

#include "util/options.h"

int main(int argc, char** argv) {
   _fa_options_init();

   fa_options_set_string("amongus", "test");
   fa_options_set_int("amongus", 5);
   FA_Value val = fa_options_get("amongus");
   printf("%d\n", val.int_value);
   fa_options_set_string("amongus", "test2");
   fa_options_unset("amongus");

   _fa_options_teardown();
   return 0;
 }