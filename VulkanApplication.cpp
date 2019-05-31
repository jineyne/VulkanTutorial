#include "VulkanApplication.hpp"

void VulkanApplication::run() {
    // init
    initWindow();
    DEBUG_ONLY(setupDebugCallback());
    initVulkan();
    DEBUG_ONLY(initDebug());
    pickPhysicalDevice();

    mainLoop();

    // deinit
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
    return indices.isComplete;
}

QueueFamilyIndices VulkanApplication::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices{};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            indices.isComplete = true;
        }

        if (indices.isComplete) {
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

    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            mPhysicalDevice = device;
            break;
        }
    }

    if (mPhysicalDevice == nullptr) {
        throw std::runtime_error("failed to find a suitable GPU");
    }
}
