#include "VulkanApplication.hpp"

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
