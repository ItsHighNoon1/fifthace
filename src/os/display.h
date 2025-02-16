/**
 * @file display.h
 * @author ItsHighNoon
 * @date 02-16-2025
 * 
 * @copyright Copyright (c) 2025
 * 
 * Interface with the window in the operating system.
 */

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

GLFWwindow* _fa_display_get_handle();

void _fa_display_open();

void _fa_display_close();

void _fa_display_poll_and_refresh();

int _fa_display_close_requested();