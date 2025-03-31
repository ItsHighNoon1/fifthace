/**
 * @file vkboilerplate.c
 * @author ItsHighNoon
 * @date 02-16-2025
 * 
 * @copyright Copyright (c) 2025
 */

#include "vkboilerplate.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "os/display.h"
#include "util/options.h"

static const char* DEVICE_EXTENSIONS[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static const char* VALIDATION_LAYERS[] = {
    "VK_LAYER_KHRONOS_validation"
};

static VkInstance instance;
static VkPhysicalDevice physical_device;
static VkDevice device;
static VkQueue graphics_queue;
static VkQueue present_queue;
static VkSurfaceKHR surface;
static VkSwapchainKHR swap_chain;
static VkFormat swap_chain_format;
static VkExtent2D swap_chain_extent;
static VkImage* swap_chain_images;
static int swap_chain_images_len;
static VkImageView* swap_chain_image_views;
static int swap_chain_image_views_len;

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR* formats;
    uint32_t formats_len;
    VkPresentModeKHR* present_modes;
    uint32_t modes_len;
};

struct QueueFamilyIndices {
    uint32_t graphics_family;
    int found_graphics_family;
    uint32_t present_family;
    int found_present_family;
};

struct SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device) {
    struct SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formats_len, NULL);
    details.formats = malloc(details.formats_len * sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formats_len, details.formats);

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.modes_len, NULL);
    details.present_modes = malloc(details.modes_len * sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.modes_len, details.present_modes);

    return details;
}

struct QueueFamilyIndices find_queue_families(VkPhysicalDevice device) {
    struct QueueFamilyIndices qfi;
    memset(&qfi, 0, sizeof(qfi));

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);
    VkQueueFamilyProperties* queue_families = malloc(queue_family_count * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

    for (int queue_family_idx = 0; queue_family_idx < queue_family_count; queue_family_idx++) {
        if (queue_families[queue_family_idx].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            qfi.graphics_family = queue_family_idx;
            qfi.found_graphics_family = 1;
        }
        VkBool32 present_support;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, queue_family_idx, surface, &present_support);
        if (present_support) {
            qfi.present_family = queue_family_idx;
            qfi.found_present_family = 1;
        }
    }

    free(queue_families);
    return qfi;
}

static VkSurfaceFormatKHR choose_swap_surface_format(struct SwapChainSupportDetails* details) {
    for (int format_idx = 0; format_idx < details->formats_len; format_idx++) {
        if (details->formats[format_idx].format == VK_FORMAT_B8G8R8A8_SRGB
            && details->formats[format_idx].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return details->formats[format_idx];
        }
    }

    return details->formats[0];
}

static VkPresentModeKHR choose_swap_present_mode(struct SwapChainSupportDetails* details) {
    for (int mode_idx = 0; mode_idx < details->formats_len; mode_idx++) {
        if (details->present_modes[mode_idx] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return details->present_modes[mode_idx];
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D choose_swap_extent(struct SwapChainSupportDetails* details) {
    if (details->capabilities.currentExtent.width != UINT_MAX) {
        return details->capabilities.currentExtent;
    } else {
        int width;
        int height;
        glfwGetFramebufferSize(_fa_display_get_handle(), &width, &height);

        if (width < details->capabilities.minImageExtent.width) {
            width = details->capabilities.minImageExtent.width;
        }
        if (width > details->capabilities.maxImageExtent.width) {
            width = details->capabilities.maxImageExtent.width;
        }
        if (height < details->capabilities.minImageExtent.height) {
            height = details->capabilities.minImageExtent.height;
        }
        if (height > details->capabilities.maxImageExtent.height) {
            height = details->capabilities.maxImageExtent.height;
        }

        VkExtent2D extent = {
            width,
            height
        };
        return extent;
    }
}

static int check_device_extensions(VkPhysicalDevice device) {
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, NULL);
    VkExtensionProperties* extensions = malloc(extension_count * sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, extensions);

    int found[sizeof(DEVICE_EXTENSIONS) / sizeof(char*)];
    for (int requested_idx = 0; requested_idx < sizeof(DEVICE_EXTENSIONS) / sizeof(char*); requested_idx++) {
        for (int extension_idx = 0; extension_idx < extension_count; extension_idx++) {
            if (strcmp(DEVICE_EXTENSIONS[requested_idx], extensions[extension_idx].extensionName) == 0) {
                found[requested_idx] = 1;
                break;
            }
        }
    }

    free(extensions);

    int found_all = 1;
    for (int found_idx = 0; found_idx < sizeof(DEVICE_EXTENSIONS) / sizeof(char*); found_idx++) {
        found_all = found_all && found[found_idx];
    }

    return found_all;
}

static int check_validation_layers() {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);
    VkLayerProperties* layers = malloc(layer_count * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&layer_count, layers);

    int found[sizeof(VALIDATION_LAYERS) / sizeof(char*)];
    for (int requested_idx = 0; requested_idx < sizeof(VALIDATION_LAYERS) / sizeof(char*); requested_idx++) {
        for (int layer_idx = 0; layer_idx < layer_count; layer_idx++) {
            if (strcmp(VALIDATION_LAYERS[requested_idx], layers[layer_idx].layerName) == 0) {
                found[requested_idx] = 1;
                break;
            }
        }
    }

    free(layers);

    int found_all = 1;
    for (int found_idx = 0; found_idx < sizeof(VALIDATION_LAYERS) / sizeof(char*); found_idx++) {
        found_all = found_all && found[found_idx];
    }

    return found_all;
}

static void create_graphics_pipeline() {

}

static void create_image_views() {
    swap_chain_image_views_len = swap_chain_images_len;
    swap_chain_image_views = malloc(swap_chain_image_views_len * sizeof(VkImageView));

    for (int image_idx = 0; image_idx < swap_chain_images_len; image_idx++) {
        VkImageViewCreateInfo create_info;
        memset(&create_info, 0, sizeof(create_info));
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = swap_chain_images[image_idx];
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = swap_chain_format;
        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &create_info, NULL, &swap_chain_image_views[image_idx]) != VK_SUCCESS) {
            printf("Failed to create image view %d :(\n", image_idx);
            exit(1);
        }
    }
}

static void create_swap_chain() {
    struct SwapChainSupportDetails details = query_swap_chain_support(physical_device);

    VkSurfaceFormatKHR format = choose_swap_surface_format(&details);
    VkPresentModeKHR mode = choose_swap_present_mode(&details);
    VkExtent2D extent = choose_swap_extent(&details);

    uint32_t image_count = details.capabilities.minImageCount + 1;
    if (details.capabilities.maxImageCount > 0 && image_count > details.capabilities.maxImageCount) {
        image_count = details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info;
    memset(&create_info, 0, sizeof(create_info));
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = format.format;
    create_info.imageColorSpace = format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    struct QueueFamilyIndices qfi = find_queue_families(physical_device);
    uint32_t queue_indices[] = { qfi.graphics_family, qfi.present_family };
    if (qfi.graphics_family != qfi.present_family) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_indices;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = NULL;
    }

    create_info.preTransform = details.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device, &create_info, NULL, &swap_chain) != VK_SUCCESS) {
        printf("Failed to create swap chain :(\n");
        exit(1);
    }

    free(details.formats);
    free(details.present_modes);

    vkGetSwapchainImagesKHR(device, swap_chain, &image_count, NULL);
    swap_chain_images = malloc(image_count * sizeof(VkImage));
    vkGetSwapchainImagesKHR(device, swap_chain, &image_count, swap_chain_images);

    swap_chain_format = format.format;
    swap_chain_extent = extent;
}

static void create_logical_device() {
    struct QueueFamilyIndices qfi = find_queue_families(physical_device);

    int n_queues = 2;
    if (qfi.graphics_family == qfi.present_family) {
        // TODO use a set
        n_queues = 1;
    }
    VkDeviceQueueCreateInfo* queue_create_infos = malloc(n_queues * sizeof(VkDeviceQueueCreateInfo));

    float queue_priority = 1.0f;
    for (int queue_idx = 0; queue_idx < n_queues; queue_idx++) {
        memset(&queue_create_infos[queue_idx], 0, sizeof(VkDeviceQueueCreateInfo));
        queue_create_infos[queue_idx].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        // TODO TODOTODOTODOTODOTODO
        if (queue_idx == 0) {
            queue_create_infos[queue_idx].queueFamilyIndex = qfi.graphics_family;
        } else {
            queue_create_infos[queue_idx].queueFamilyIndex = qfi.present_family;
        }
        queue_create_infos[queue_idx].queueCount = 1;
        queue_create_infos[queue_idx].pQueuePriorities = &queue_priority;
    }

    VkPhysicalDeviceFeatures device_features;
    memset(&device_features, 0, sizeof(device_features));

    VkDeviceCreateInfo create_info;
    memset(&create_info, 0, sizeof(create_info));
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = queue_create_infos;
    create_info.queueCreateInfoCount = n_queues;
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = sizeof(DEVICE_EXTENSIONS) / sizeof(char*);
    create_info.ppEnabledExtensionNames = DEVICE_EXTENSIONS;
    if (check_validation_layers()) {
        create_info.enabledLayerCount = sizeof(VALIDATION_LAYERS) / sizeof(char*);
        create_info.ppEnabledLayerNames = VALIDATION_LAYERS;
    } else {
        create_info.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physical_device, &create_info, NULL, &device) != VK_SUCCESS) {
        printf("Failed to create logical device :(\n");
        exit(1);
    }

    free(queue_create_infos);

    vkGetDeviceQueue(device, qfi.graphics_family, 0, &graphics_queue);
    vkGetDeviceQueue(device, qfi.present_family, 0, &present_queue);
}

static void pick_physical_device() {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);

    if (device_count == 0) {
        printf("No physical device with Vulkan support :(\n");
        exit(1);
    }

    VkPhysicalDevice* devices = malloc(device_count * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(instance, &device_count, devices);

    VkPhysicalDevice best_device;
    int best_device_score = -1;
    for (int device_idx = 0; device_idx < device_count; device_idx++) {
        // TODO replace with manual selector
        int device_score = 0;
        
        VkPhysicalDeviceProperties device_properties;
        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceProperties(devices[device_idx], &device_properties);
        vkGetPhysicalDeviceFeatures(devices[device_idx], &device_features);

        struct QueueFamilyIndices qfi = find_queue_families(devices[device_idx]);
        if (qfi.found_graphics_family == 0
            || qfi.found_present_family == 0) {
            continue;
        }

        if (check_device_extensions(devices[device_idx]) == 0) {
            continue;
        }

        struct SwapChainSupportDetails swap_chain_details = query_swap_chain_support(devices[device_idx]);
        free(swap_chain_details.formats);
        free(swap_chain_details.present_modes);
        if (swap_chain_details.formats_len == 0 || swap_chain_details.modes_len == 0) {
            continue;
        }

        if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            device_score += 5000;
        }
        device_score += device_properties.limits.maxComputeSharedMemorySize;

        if (device_score > best_device_score) {
            best_device_score = device_score;
            best_device = devices[device_idx];
        }
    }

    free(devices);

    if (best_device_score < 0) {
        printf("No physical device was suitable :(\n");
        exit(1);
    }

    physical_device = best_device;
}

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
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = glfw_ext_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;
    if (check_validation_layers()) {
        create_info.enabledLayerCount = 1;
        create_info.ppEnabledLayerNames = VALIDATION_LAYERS;
    } else {
        create_info.enabledLayerCount = 0;
    }
    

    if (vkCreateInstance(&create_info, NULL, &instance) != VK_SUCCESS) {
        printf("Failed to create Vulkan instance :(\n");
        exit(1);
    }

    if (glfwCreateWindowSurface(instance, _fa_display_get_handle(), NULL, &surface) != VK_SUCCESS) {
        printf("Failed to create surface :(\n");
        exit(1);
    }
}

void _fa_vk_init() {
    create_instance();
    pick_physical_device();
    create_logical_device();
    create_swap_chain();
    create_image_views();
    create_graphics_pipeline();
}

void _fa_vk_teardown() {
    for (int image_view_idx = 0; image_view_idx < swap_chain_image_views_len; image_view_idx++) {
        vkDestroyImageView(device, swap_chain_image_views[image_view_idx], NULL);
    }
    vkDestroySwapchainKHR(device, swap_chain, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyInstance(instance, NULL);
}