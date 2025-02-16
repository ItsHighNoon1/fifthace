/**
 * @file main.c
 * @author ItsHighNoon
 * @date 02-13-2025
 * 
 * @copyright Copyright (c) 2025
 * 
 * Entry point of the application. Start and stop subsystems.
 */

#include "os/display.h"
#include "render/vk/vkboilerplate.h"
#include "util/options.h"

int main(int argc, char** argv) {
   _fa_options_init();

   fa_options_set_string("app.name", "Test Application");
   fa_options_set_int("app.version", VK_MAKE_VERSION(0, 0, 1));
   fa_options_set_int("window.fullscreen", 0);

   _fa_display_open();
   _fa_vk_init();
   while (!_fa_display_close_requested()) {
      _fa_display_poll_and_refresh();
   }
   _fa_vk_teardown();
   _fa_display_close();

   _fa_options_teardown();
   return 0;
}