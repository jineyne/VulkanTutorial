#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <iostream>

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

class VulkanApplication {
private:
    const uint32_t Width = 800, Height = 600;

    GLFWwindow *mWindow = nullptr;

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

    }

    void deinitVulkan() {

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
