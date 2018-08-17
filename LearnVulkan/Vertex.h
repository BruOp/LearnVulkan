
#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <array>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static vk::VertexInputBindingDescription getBindingDescription();
    static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
};
