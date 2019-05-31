#include <cstdlib>
#include <iostream>

#include <vulkan/vulkan.h>

class VulkanApplication {
public:
    void run() {
        // init
        initVulkan();

        mainLoop();

        // deinit
        deinitVulkan();
    }

private:
    void initVulkan() {

    }

    void deinitVulkan() {

    }

    void mainLoop() {

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
