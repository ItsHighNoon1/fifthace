cmake_minimum_required(VERSION 3.10)
project(fifthace VERSION 0.0.1)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

add_executable(${PROJECT_NAME} main.c os/display.c render/vk/vkboilerplate.c util/options.c util/util.c)
target_include_directories(${PROJECT_NAME} PUBLIC ./)
target_link_libraries(${PROJECT_NAME} glfw Vulkan::Vulkan)
add_dependencies(${PROJECT_NAME} shaders)

add_custom_target(shaders)

file(GLOB shader_sources "shader/*.vert" "shader/*.frag")

foreach(shader_source ${shader_sources})
    add_custom_command(
        OUTPUT ${shader_source}.bin
        DEPENDS ${shader_source}
        COMMAND
            ${glslc_executable}
            --target-env=vulkan
            -o ${shader_source}.bin
            ${shader_source}
    )
    target_sources(shaders PRIVATE ${shader_source}.bin)
endforeach()