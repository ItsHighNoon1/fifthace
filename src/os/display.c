/**
 * @file display.c
 * @author ItsHighNoon
 * @date 02-16-2025
 * 
 * @copyright Copyright (c) 2025
 */

#include "display.h"

#include "util/options.h"

#define FALLBACK_WIDTH 800
#define MIN_WIDTH 100
#define MIN_HEIGHT MIN_WIDTH

static GLFWwindow* window;

GLFWwindow* _fa_display_get_handle() {
    return window;
}

void _fa_display_open() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    int width = FALLBACK_WIDTH;
    FA_OptionValue width_value = fa_options_get("window.width");
    if (width_value.type == FA_OPTION_INT && width_value.int_value > MIN_WIDTH) {
        width = width_value.int_value;
    } else {
        fa_options_set_int("window.width", width);
    }

    int height = width * 9 / 16;
    FA_OptionValue height_value = fa_options_get("window.height");
    if (height_value.type == FA_OPTION_INT && height_value.int_value > MIN_HEIGHT) {
        height = height_value.int_value;
    } else {
        fa_options_set_int("window.height", height);
    }

    int fullscreen = 0;
    FA_OptionValue fullscreen_value = fa_options_get("window.fullscreen");
    if (fullscreen_value.type == FA_OPTION_INT) {
        fullscreen = fullscreen_value.int_value;
    } else {
        fa_options_set_int("window.fullscreen", fullscreen);
    }

    int monitor_count;
    GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
    if (fullscreen > 0 && fullscreen > monitor_count) {
        fullscreen = 1;
    }
    if (fullscreen > 0) {
        const GLFWvidmode* mode = glfwGetVideoMode(monitors[fullscreen - 1]);

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        window = glfwCreateWindow(mode->width, mode->height, "Fifth Ace", monitors[fullscreen - 1], NULL);
    } else {
        window = glfwCreateWindow(width, height, "Fifth Ace", NULL, NULL);
    }
}

void _fa_display_close() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void _fa_display_poll_and_refresh() {
    glfwPollEvents();
}

int _fa_display_close_requested() {
    return glfwWindowShouldClose(window);
}