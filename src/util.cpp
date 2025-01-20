#include <voxelforge/util.hpp>

namespace vf {
static const std::vector<const char *> instance_extensions = {

}; // nothing here!

const std::vector<const char *>& get_required_instance_extensions() {
	return instance_extensions;
}

i64 score_device(const VkPhysicalDevice& dev) {
	const vk_util::QueueFamilyIndices indices = vk_util::QueueFamilyIndices::find(dev);

	if (!indices.is_vf_suitable()) return -1;

	return 1; // no real scoring yet
}

namespace vk_util {
bool QueueFamilyIndices::is_complete() const {
	return  graphics.has_value() &&
           	compute.has_value() &&
           	transfer.has_value() &&
           	sparse_binding.has_value() &&
           	protected_mem.has_value();
}

bool QueueFamilyIndices::is_vf_suitable() const {
	return  graphics.has_value() &&
		compute.has_value();
}

QueueFamilyIndices QueueFamilyIndices::find(const VkPhysicalDevice& dev) {
	QueueFamilyIndices indices;

	u32 n_queue_families = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &n_queue_families, nullptr);

	std::vector<VkQueueFamilyProperties> queue_families(n_queue_families);
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &n_queue_families, queue_families.data());

	i32 i = 0;

	for (const auto& queue_family : queue_families) {
		if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphics = i;
		}
		if (queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			indices.compute = i;
		}
		if (queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT) {
			indices.transfer = i;
		}
		if (queue_family.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
			indices.sparse_binding = i;
		}
		if (queue_family.queueFlags & VK_QUEUE_PROTECTED_BIT) {
			indices.protected_mem = i;
		}

		if (indices.is_complete()) {
			break;
		}

		++i;
	}

	return indices;
}
}
}
