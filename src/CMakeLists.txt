cmake_minimum_required(VERSION 3.10)
project(fifthace)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME})
target_include_directories(./)
target_link_libraries(${PROJECT_NAME} glfw Vulkan::Vulkan)