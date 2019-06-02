cmake_minimum_required(VERSION 3.10)

project(VulkanTutorial)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_DOCS OFF)
add_subdirectory(vender/glfw)

set(GLM_TEST_ENABLE OFF)
add_subdirectory(vender/glm)

find_package(Vulkan REQUIRED)

add_executable(VulkanTutorial main.cpp VulkanApplication.cpp VulkanApplication.hpp Vertex.cpp Vertex.hpp)
target_link_libraries(VulkanTutorial PUBLIC Vulkan::Vulkan glfw glm)

file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/resources/shaders/*.frag"
        "${PROJECT_SOURCE_DIR}/resources/shaders/*.vert")

if (Win32)
    file(TO_NATIVE_PATH "${Vulkan_LIBRARIES}/../../bin/glslangValidator" GLSL_VALIDATOR)
else ()
    file(TO_NATIVE_PATH "glslangValidator" GLSL_VALIDATOR)
endif ()

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_BINARY_DIR}/resources/shaders/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/resources/shaders/"
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
        Shaders
        DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(VulkanTutorial Shaders)

add_custom_command(TARGET VulkanTutorial PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:VulkanTutorial>/shaders/"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/resources/shaders/"
        "$<TARGET_FILE_DIR:VulkanTutorial>/shaders/")