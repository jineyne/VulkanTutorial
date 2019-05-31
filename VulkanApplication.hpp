#ifndef VULKANTUTORIAL_VULKANAPPLICATION_HPP
#define VULKANTUTORIAL_VULKANAPPLICATION_HPP

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <cassert>
#include <cstdint>
#include <cstdlib>

#define CHECK(result, msg) if ((result) != VK_SUCCESS) throw std::runtime_error(msg)

const std::vector<const char *> validationLayers = {
        "VK_LAYER_LUNARG_standard_validation"
};

#if defined(DEBUG) || defined(_DEBUG) || defined(NDEBUG)
#define DEBUG_MODE 1
#define DEBUG_ONLY(x) x
#else
#define DEBUG_MODE 0
#define DEBUG_ONLY(x)
#endif

struct QueueFamilyIndices {
    uint32_t graphicsFamily;
    bool isComplete;
};

class VulkanApplication {
private:
    const uint32_t Width = 800, Height = 600;

    GLFWwindow *mWindow = nullptr;

    VkInstance mInstance = nullptr;

    VkPhysicalDevice mPhysicalDevice = nullptr;

    VkDebugUtilsMessengerEXT mDebugMessenger = nullptr;
    VkDebugUtilsMessengerCreateInfoEXT mDebugReportCallbackCreateInfo{};

public:
    void run();

private:
    void initWindow();

    void deinitWindow();

    void initVulkan();

    void deinitVulkan();

    void setupDebugCallback();

    void initDebug();

    void deinitDebug();

    void pickPhysicalDevice();

    bool checkValidationLayerSupport();

    void mainLoop();

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

    std::vector<const char *> getRequiredExtensions();

    bool isDeviceSuitable(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};

#endif //VULKANTUTORIAL_VULKANAPPLICATION_HPP
