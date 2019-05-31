#include "VulkanApplication.hpp"

static bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto &extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

void VulkanApplication::run() {
    // init
    initWindow();
    DEBUG_ONLY(setupDebugCallback());
    initVulkan();
    DEBUG_ONLY(initDebug());
    initSurface();
    pickPhysicalDevice();
    initLogicalDevice();

    mainLoop();

    // deinit
    deinitLogicalDevice();
    deinitSurface();
    DEBUG_ONLY(deinitDebug());
    deinitVulkan();
    deinitWindow();
}

void VulkanApplication::initWindow() {
    // init glfw library
    if (!glfwInit()) {
        throw std::runtime_error("failed to init glfw");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    mWindow = glfwCreateWindow(Width, Height, "Vulkan Application", nullptr, nullptr);
    assert(mWindow != nullptr);
}

void VulkanApplication::deinitWindow() {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void VulkanApplication::initVulkan() {
    DEBUG_ONLY({
                   if (!checkValidationLayerSupport()) {
                       throw std::runtime_error("Validation layers requested. but not available");
                   }
               });

    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.apiVersion = VK_VERSION_1_1;
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pApplicationName = "Vulkan Tutorial";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "No Engine";

    auto glfwExtensions = getRequiredExtensions();

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.enabledExtensionCount = glfwExtensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions.data();
#if DEBUG_MODE
    instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();

    instanceCreateInfo.pNext = &mDebugReportCallbackCreateInfo;
#else
    instanceCreateInfo.enabledLayerCount = 0;
#endif

    CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance), "failed to create instance");
}

void VulkanApplication::deinitVulkan() {
    vkDestroyInstance(mInstance, nullptr);
}

void VulkanApplication::setupDebugCallback() {
    mDebugReportCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    mDebugReportCallbackCreateInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    mDebugReportCallbackCreateInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    mDebugReportCallbackCreateInfo.pfnUserCallback = DebugCallback;
    mDebugReportCallbackCreateInfo.pUserData = nullptr;
}

bool VulkanApplication::checkValidationLayerSupport() {
    uint32_t count = 0;
    vkEnumerateInstanceLayerProperties(&count, nullptr);
    std::vector<VkLayerProperties> properties(count);
    vkEnumerateInstanceLayerProperties(&count, properties.data());

    for (auto layerName : validationLayers) {
        bool layerFound = false;

        for (const auto &property : properties) {
            if (strcmp(layerName, property.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

void VulkanApplication::mainLoop() {
    while (!glfwWindowShouldClose(mWindow)) {
        glfwPollEvents();
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanApplication::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                                void *pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

std::vector<const char *> VulkanApplication::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    DEBUG_ONLY(extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));

    return extensions;
}

bool VulkanApplication::isDeviceSuitable(VkPhysicalDevice device) {
    auto indices = findQueueFamilies(device);
    auto supported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (supported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(mPhysicalDevice, mSurface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isGraphicsFamilyHasValue && indices.isPresentFamilyHasValue && supported && swapChainAdequate;
}

QueueFamilyIndices VulkanApplication::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices{};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            indices.isGraphicsFamilyHasValue = true;
        }

        VkBool32 presentSurport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &presentSurport);

        if (queueFamily.queueCount > 0 && presentSurport) {
            indices.presentFamily = i;
            indices.isPresentFamilyHasValue = true;
        }

        if (indices.isGraphicsFamilyHasValue || indices.isPresentFamilyHasValue) {
            break;
        }

        i++;
    }

    return indices;
}

void VulkanApplication::initDebug() {
    static PFN_vkCreateDebugUtilsMessengerEXT func = nullptr;
    if (func == nullptr) {
        func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(mInstance, "vkCreateDebugUtilsMessengerEXT");
    }

    if (func != nullptr) {
        CHECK(func(mInstance, &mDebugReportCallbackCreateInfo, nullptr, &mDebugMessenger),
              "failed to create debug messenger");
    } else {
        throw std::runtime_error("VK_ERROR_EXTENSION_NOT_PRESENT");
    }
}

void VulkanApplication::deinitDebug() {
    static PFN_vkDestroyDebugUtilsMessengerEXT func = nullptr;
    if (func == nullptr) {
        func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(mInstance,
                                                                           "vkDestroyDebugUtilsMessengerEXT");
    }

    if (func != nullptr) {
        func(mInstance, mDebugMessenger, nullptr);
    } else {
        throw std::runtime_error("VK_ERROR_EXTENSION_NOT_PRESENT");
    }
}

void VulkanApplication::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

    for (const auto &device : devices) {
        if (isDeviceSuitable(device)) {
            mPhysicalDevice = device;
            break;
        }
    }

    if (mPhysicalDevice == nullptr) {
        throw std::runtime_error("failed to find a suitable GPU");
    }
}

void VulkanApplication::initLogicalDevice() {
    auto indices = findQueueFamilies(mPhysicalDevice);
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

#if DEBUG_MODE
    deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
#else
    deviceCreateInfo.enabledLayerCount = 0;
#endif

    CHECK(vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mLogicalDevice),
          "failed to create logical device");

    vkGetDeviceQueue(mLogicalDevice, indices.graphicsFamily, 0, &mGraphicsQueue);
    vkGetDeviceQueue(mLogicalDevice, indices.presentFamily, 0, &mPresentQueue);
}

void VulkanApplication::deinitLogicalDevice() {
    vkDestroyDevice(mLogicalDevice, nullptr);
}

void VulkanApplication::initSurface() {
    CHECK(glfwCreateWindowSurface(mInstance, mWindow, nullptr, &mSurface), "failed to create surface");
}

void VulkanApplication::deinitSurface() {
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
}
