#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <vector>
#include <iostream>

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#define CHECK(result, msg) if ((result) != VK_SUCCESS) throw std::runtime_error(msg)

class VulkanApplication {
private:
    const uint32_t Width = 800, Height = 600;

    GLFWwindow *mWindow = nullptr;

    VkInstance mInstance = nullptr;

public:
    void run() {
        // init
        initWindow();
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
        instanceCreateInfo.enabledLayerCount = 0;

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

    void mainLoop() {
        while (!glfwWindowShouldClose(mWindow)) {
            glfwPollEvents();
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
