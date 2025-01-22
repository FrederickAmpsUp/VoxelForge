#pragma once

#include <vector>
#include <voxelforge/types.hpp>
#include <vulkan/vulkan.h>
#include <optional>

namespace vf {
namespace vk_util {

/**
* A list of queue family indices for all (non-extended) Vulkan queue types.
*/
struct QueueFamilyIndices {
	// see https://docs.vulkan.org/guide/latest/queues.html
	std::optional<u32> graphics;
	std::optional<u32> compute;
	std::optional<u32> transfer;
	std::optional<u32> sparse_binding;
	std::optional<u32> protected_mem;

	/**
	* Check if all (non-extension) queue types are supported.
	* @return `true` if all queue types are supported, else `false`
	*/
	bool is_complete() const;
	
	/**
	* Check if enough queue families are supported for Voxelforge to run on the device.
	* @return `true` if the supported queues are sufficient for Voxelforge, else `false`
	*/
	bool is_vf_suitable() const;
	
	/**
	* Find queue family indices for the given `VkPhysicalDevice`.
	* @param device The `VkPhysicalDevice` to find queue family indices for
	* @return The `QueueFamilyIndices` supported by the `VkPhysicalDevice`
	*/
	static QueueFamilyIndices find(const VkPhysicalDevice& device);
};

/**
* Combines 2 `VkPhysicalDeviceFeatures`.
* @param a A `VkPhysicalDeviceFeatures`
* @param b A `VkPhysicalDeviceFeatures`
* @return A `VkPhysicalDeviceFeatures` with all features required by either `a` or `b`
*/
VkPhysicalDeviceFeatures combine_physical_device_features(const VkPhysicalDeviceFeatures& a, const VkPhysicalDeviceFeatures& b);
}

/**
* Get a list of Vulkan instance extensions Voxelforge requires.
* @return A `std::vector` of instance extension names required by Voxelforge
*/
const std::vector<const char *>& get_required_instance_extensions();

/**
* Get a list of Vulkan device extensions Voxelforge requires.
* @return A `std::vector` of device extension names required by Voxelforge
*/
const std::vector<const char *>& get_required_device_extensions();

/**
* Get a list of Vulkan physical device features Voxelforge requires.
* @return A `VkPhysicalDeviceFeatures` representing features required by Voxelforge
*/
const VkPhysicalDeviceFeatures& get_required_physical_device_features();

/**
* Score a `VkPhysicalDevice` based on expected performance for Voxelforge.
* @return A "score" value, larger being better score, and `<0` being completely unsuitable for Voxelforge.
*/
i64 score_device(const VkPhysicalDevice& device);
}
