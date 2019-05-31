#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cstring>

#include <vector>

#include <sstream>
#include <iostream>

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

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

class VulkanApplication {
private:
    const uint32_t Width = 800, Height = 600;

    GLFWwindow *mWindow = nullptr;

    VkInstance mInstance = nullptr;

    VkDebugUtilsMessengerEXT mDebugMessenger = nullptr;
    VkDebugUtilsMessengerCreateInfoEXT mDebugReportCallbackCreateInfo{};

public:
    void run() {
        // init
        initWindow();
        DEBUG_ONLY(setupDebugCallback());
        initVulkan();

        mainLoop();

        // deinit
        deinitVulkan();
        deinitWindow();
    }

private:
    void initWindow() {
        // init glfw library
        if (!glfwInit()) {
            throw std::runtime_error("failed to init glfw");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        mWindow = glfwCreateWindow(Width, Height, "Vulkan Application", nullptr, nullptr);
        assert(mWindow != nullptr);
    }

    void deinitWindow() {
        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }

    void initVulkan() {
        DEBUG_ONLY({
                       if (!checkValidationLayerSupport()) {
                           throw std::runtime_error("Validation layers requested. but not available.");
                       }
                   });

        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.apiVersion = VK_VERSION_1_1;
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pApplicationName = "Vulkan Tutorial";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "No Engine";

        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.enabledExtensionCount = glfwExtensionCount;
        instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;
        instanceCreateInfo.pNext = &mDebugReportCallbackCreateInfo;
#if DEBUG_MODE
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
#else
        instanceCreateInfo.enabledLayerCount = 0;
#endif

        CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance), "failed to create instance!");

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensionProperties(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());

        std::cout << "Available extensions: " << std::endl;
        for (const auto &property : extensionProperties) {
            std::cout << "\t" << property.extensionName << std::endl;
        }
    }

    void deinitVulkan() {
        vkDestroyInstance(mInstance, nullptr);
    }

    void setupDebugCallback() {
        mDebugReportCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        mDebugReportCallbackCreateInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        mDebugReportCallbackCreateInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        mDebugReportCallbackCreateInfo.pfnUserCallback = DebugCallback;
        mDebugReportCallbackCreateInfo.pUserData = nullptr;

        CHECK(createDebugUtilsMessengerEXT(mInstance, &mDebugReportCallbackCreateInfo, nullptr, &mDebugMessenger),
              "failed to set up debug messenger.");
    }

    bool checkValidationLayerSupport() {
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

    void mainLoop() {
        while (!glfwWindowShouldClose(mWindow)) {
            glfwPollEvents();
        }
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    std::vector<const char *> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        DEBUG_ONLY(extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME));

        return extensions;
    }

    VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator,
                                          VkDebugUtilsMessengerEXT *pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                               "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }
};

int main() {
    std::cout << "Running!" << std::endl;
    VulkanApplication app;
    try {
        app.run();
    } catch (std::exception &e) {
        std::cerr << "Failed: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
