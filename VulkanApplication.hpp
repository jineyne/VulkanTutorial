#ifndef VULKANTUTORIAL_VULKANAPPLICATION_HPP
#define VULKANTUTORIAL_VULKANAPPLICATION_HPP

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <cstring>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <limits>

#define CHECK(result, msg) if ((result) != VK_SUCCESS) throw std::runtime_error(msg)

const std::vector<const char *> validationLayers = {
        "VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
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
    uint32_t presentFamily;

    bool isGraphicsFamilyHasValue;
    bool isPresentFamilyHasValue;
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanApplication {
public:
    const static uint32_t Width = 800, Height = 600;

private:
    GLFWwindow *mWindow = nullptr;

    VkInstance mInstance = nullptr;

    VkPhysicalDevice mPhysicalDevice = nullptr;

    VkDevice mLogicalDevice = nullptr;

    VkQueue mGraphicsQueue = nullptr;

    VkQueue mPresentQueue = nullptr;

    VkSurfaceKHR mSurface = nullptr;

    VkSwapchainKHR mSwapchain = nullptr;

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

    void initLogicalDevice();

    void deinitLogicalDevice();

    void initSurface();

    void deinitSurface();

    void initSwapchain();

    void deinitSwapchain();

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
