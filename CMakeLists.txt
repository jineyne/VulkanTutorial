cmake_minimum_required(VERSION 3.10)

project(VulkanTutorial)

set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_DOCS OFF)
add_subdirectory(vender/glfw)

find_package(Vulkan REQUIRED)

add_executable(VulkanTutorial main.cpp VulkanApplication.cpp VulkanApplication.hpp)
target_link_libraries(VulkanTutorial PUBLIC Vulkan::Vulkan glfw)
