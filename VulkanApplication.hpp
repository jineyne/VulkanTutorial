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
#include <algorithm>
#include <fstream>

#include "Vertex.hpp"

#define CHECK(result, msg) if ((result) != VK_SUCCESS) throw std::runtime_error(msg)

#if defined(DEBUG) || defined(_DEBUG) || defined(NDEBUG)
#define DEBUG_MODE 1
#define DEBUG_ONLY(x) x
#else
#define DEBUG_MODE 0
#define DEBUG_ONLY(x)
#endif

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char *> validationLayers = {
        "VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

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

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
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
    std::vector<VkImage> mSwapchainImages;
    VkFormat mSwapchainImageFormat;
    VkExtent2D mSwapchainExtent;

    std::vector<VkImageView> mSwapchainImageViews;

    VkRenderPass mRenderPass;

    VkPipeline mPipeline;
    VkDescriptorSetLayout mDescriptorSetLayout;
    VkPipelineLayout mPipelineLayout;

    std::vector<VkFramebuffer> mSwapchainFrameBuffers;

    VkCommandPool mCommandPool;
    std::vector<VkCommandBuffer> mCommandBuffers;

    std::vector<VkSemaphore> mImageAvailableSemaphores;
    std::vector<VkSemaphore> mRenderFinishedSemaphores;
    std::vector<VkFence> mInFlightFences;
    size_t mCurrentFrame = 0;

    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexDeviceMemory;
    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexDeviceMemory;
    std::vector<VkBuffer> mUniformBuffers;
    std::vector<VkDeviceMemory> mUniformBuffersMemory;


    VkDebugUtilsMessengerEXT mDebugMessenger = nullptr;
    VkDebugUtilsMessengerCreateInfoEXT mDebugReportCallbackCreateInfo{};

public:
    void run();

    void beginDraw();

    void endDraw();

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

    void initImageViews();

    void deinitImageViews();

    void initRenderPass();

    void deinitRenderPass();

    void initDescriptorSetLayout();

    void deinitDescriptorSetLayout();

    void initGraphicsPipeline();

    void deinitGraphicsPipeline();

    void initFrameBuffers();

    void deinitFrameBuffers();

    void initCommandPool();

    void deinitCommandPool();

    void initCommandBuffers();

    void deinitCommandBuffers();

    void initVertexBuffer();

    void deinitVertexBuffer();

    void initIndexBuffer();

    void deinitIndexBuffer();

    void initUniformBuffers();

    void deinitUniformBuffers();

    void initSync();

    void deinitSync();

    void onResize();

    bool checkValidationLayerSupport();

    void mainLoop();
};

#endif //VULKANTUTORIAL_VULKANAPPLICATION_HPP
