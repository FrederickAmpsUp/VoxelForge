#include <voxelforge/util.hpp>

namespace vf {
static const std::vector<const char *> instance_extensions = {

};

static const std::vector<const char *> device_extensions = {

};

const VkPhysicalDeviceFeatures& get_required_physical_device_features() {
	static VkPhysicalDeviceFeatures features = {};

	// we don't actually require any features ... yet!

	return features;
}

const std::vector<const char *>& get_required_instance_extensions() {
	return instance_extensions;
}

const std::vector<const char *>& get_required_device_extensions() {
	return device_extensions;
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

	u32 i = 0;

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

// rip
VkPhysicalDeviceFeatures combine_physical_device_features(const VkPhysicalDeviceFeatures& a, const VkPhysicalDeviceFeatures& b) {
    VkPhysicalDeviceFeatures combined_features = a;

    combined_features.robustBufferAccess |= b.robustBufferAccess;
    combined_features.fullDrawIndexUint32 |= b.fullDrawIndexUint32;
    combined_features.imageCubeArray |= b.imageCubeArray;
    combined_features.independentBlend |= b.independentBlend;
    combined_features.geometryShader |= b.geometryShader;
    combined_features.tessellationShader |= b.tessellationShader;
    combined_features.sampleRateShading |= b.sampleRateShading;
    combined_features.dualSrcBlend |= b.dualSrcBlend;
    combined_features.logicOp |= b.logicOp;
    combined_features.multiDrawIndirect |= b.multiDrawIndirect;
    combined_features.drawIndirectFirstInstance |= b.drawIndirectFirstInstance;
    combined_features.depthClamp |= b.depthClamp;
    combined_features.depthBiasClamp |= b.depthBiasClamp;
    combined_features.fillModeNonSolid |= b.fillModeNonSolid;
    combined_features.depthBounds |= b.depthBounds;
    combined_features.wideLines |= b.wideLines;
    combined_features.largePoints |= b.largePoints;
    combined_features.alphaToOne |= b.alphaToOne;
    combined_features.multiViewport |= b.multiViewport;
    combined_features.samplerAnisotropy |= b.samplerAnisotropy;
    combined_features.textureCompressionETC2 |= b.textureCompressionETC2;
    combined_features.textureCompressionASTC_LDR |= b.textureCompressionASTC_LDR;
    combined_features.textureCompressionBC |= b.textureCompressionBC;
    combined_features.occlusionQueryPrecise |= b.occlusionQueryPrecise;
    combined_features.pipelineStatisticsQuery |= b.pipelineStatisticsQuery;
    combined_features.vertexPipelineStoresAndAtomics |= b.vertexPipelineStoresAndAtomics;
    combined_features.fragmentStoresAndAtomics |= b.fragmentStoresAndAtomics;
    combined_features.shaderTessellationAndGeometryPointSize |= b.shaderTessellationAndGeometryPointSize;
    combined_features.shaderImageGatherExtended |= b.shaderImageGatherExtended;
    combined_features.shaderStorageImageExtendedFormats |= b.shaderStorageImageExtendedFormats;
    combined_features.shaderStorageImageMultisample |= b.shaderStorageImageMultisample;
    combined_features.shaderStorageImageReadWithoutFormat |= b.shaderStorageImageReadWithoutFormat;
    combined_features.shaderStorageImageWriteWithoutFormat |= b.shaderStorageImageWriteWithoutFormat;
    combined_features.shaderUniformBufferArrayDynamicIndexing |= b.shaderUniformBufferArrayDynamicIndexing;
    combined_features.shaderSampledImageArrayDynamicIndexing |= b.shaderSampledImageArrayDynamicIndexing;
    combined_features.shaderStorageBufferArrayDynamicIndexing |= b.shaderStorageBufferArrayDynamicIndexing;
    combined_features.shaderStorageImageArrayDynamicIndexing |= b.shaderStorageImageArrayDynamicIndexing;
    combined_features.shaderClipDistance |= b.shaderClipDistance;
    combined_features.shaderCullDistance |= b.shaderCullDistance;
    combined_features.shaderFloat64 |= b.shaderFloat64;
    combined_features.shaderInt64 |= b.shaderInt64;
    combined_features.shaderInt16 |= b.shaderInt16;
    combined_features.shaderResourceResidency |= b.shaderResourceResidency;
    combined_features.shaderResourceMinLod |= b.shaderResourceMinLod;
    combined_features.sparseBinding |= b.sparseBinding;
    combined_features.sparseResidencyBuffer |= b.sparseResidencyBuffer;
    combined_features.sparseResidencyImage2D |= b.sparseResidencyImage2D;
    combined_features.sparseResidencyImage3D |= b.sparseResidencyImage3D;
    combined_features.sparseResidency2Samples |= b.sparseResidency2Samples;
    combined_features.sparseResidency4Samples |= b.sparseResidency4Samples;
    combined_features.sparseResidency8Samples |= b.sparseResidency8Samples;
    combined_features.sparseResidency16Samples |= b.sparseResidency16Samples;
    combined_features.sparseResidencyAliased |= b.sparseResidencyAliased;
    combined_features.variableMultisampleRate |= b.variableMultisampleRate;
    combined_features.inheritedQueries |= b.inheritedQueries;

    return combined_features;
}
}
}
