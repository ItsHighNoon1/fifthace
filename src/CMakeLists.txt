cmake_minimum_required(VERSION 3.10)
project(fifthace)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME} main.c util/options.c util/util.c)
target_include_directories(${PROJECT_NAME} PUBLIC ./)
target_link_libraries(${PROJECT_NAME} glfw Vulkan::Vulkan)