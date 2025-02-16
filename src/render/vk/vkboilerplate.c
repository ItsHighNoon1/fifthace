/**
 * @file vkboilerplate.c
 * @author ItsHighNoon
 * @date 02-16-2025
 * 
 * @copyright Copyright (c) 2025
 */

#include "vkboilerplate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "util/options.h"

static VkInstance instance;

static void create_instance() {
    VkApplicationInfo app_info;
    memset(&app_info, 0, sizeof(app_info));
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    FA_OptionValue app_name_value = fa_options_get("app.name");
    if (app_name_value.type == FA_OPTION_STRING) {
        app_info.pApplicationName = app_name_value.string_value;
    } else {
        fa_options_set_string("app.name", "Unknown Application");
        app_info.pApplicationName = "Unknown Application";
    }
    FA_OptionValue app_vers_value = fa_options_get("app.version");
    if (app_vers_value.type == FA_OPTION_INT) {
        app_info.applicationVersion = app_vers_value.int_value;
    } else {
        fa_options_set_int("app.version", 0);
        app_info.applicationVersion = 0;
    }
    app_info.pEngineName = "Fifth Ace";
    app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.apiVersion = VK_API_VERSION_1_3;

    unsigned int glfw_ext_count;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_ext_count);

    VkInstanceCreateInfo create_info;
    memset(&create_info, 0, sizeof(create_info));
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = glfw_ext_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;
    create_info.enabledLayerCount = 0;

    VkResult result = vkCreateInstance(&create_info, NULL, &instance);
    if (result != VK_SUCCESS) {
        printf("Failed to create Vulkan instance :(\n");
        exit(1);
    }
}

void _fa_vk_init() {
    create_instance();
}

void _fa_vk_teardown() {
    vkDestroyInstance(instance, NULL);
}