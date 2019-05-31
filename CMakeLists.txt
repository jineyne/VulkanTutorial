cmake_minimum_required(VERSION 3.10)
project(VulkanTutorial)

find_package(Vulkan REQUIRED)

add_executable(VulkanTutorial main.cpp)
target_link_libraries(VulkanTutorial PUBLIC Vulkan::Vulkan)
