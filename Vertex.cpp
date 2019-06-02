#include "Vertex.hpp"

VkVertexInputBindingDescription Vertex::getBindingDescription() {
    VkVertexInputBindingDescription vertexInputBindingDescription{};
    vertexInputBindingDescription.binding = 0;
    vertexInputBindingDescription.stride = sizeof(Vertex);
    vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return vertexInputBindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> Vertex::getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> descriptors{};
    descriptors[0].binding = 0;
    descriptors[0].location = 0;
    descriptors[0].format = VK_FORMAT_R32G32_SFLOAT;
    descriptors[0].offset = offsetof(Vertex, pos);

    descriptors[1].binding = 0;
    descriptors[1].location = 1;
    descriptors[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    descriptors[1].offset = offsetof(Vertex, color);

    return descriptors;
}