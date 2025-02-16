cmake_minimum_required(VERSION 3.10)
project(fifthace VERSION 0.0.1)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME} main.c os/display.c render/vk/vkboilerplate.c util/options.c util/util.c)
target_include_directories(${PROJECT_NAME} PUBLIC ./)
target_link_libraries(${PROJECT_NAME} glfw Vulkan::Vulkan)