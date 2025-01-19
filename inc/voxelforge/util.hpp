#pragma once

#include <vector>
#include <voxelforge/types.hpp>
#include <vulkan/vulkan.h>

namespace vf {

const std::vector<const char *>& get_required_instance_extensions();
i64 score_device(VkPhysicalDevice& dev);
}
