#include <voxelforge/voxelforge.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <result/result.hpp>
#include <unordered_set>
#include <cstring>
#include <algorithm>
#include <optional>
#include <set>

// if this was rust, we'd have the ? operator
#define TRY(res) ({auto _res = res; if (!_res) {return cpp::fail(_res.error());} _res.value(); })

class TestApplication {
public:
	cpp::result<void, vf::Error> create_window() {
		if (!glfwInit()) {
			return cpp::fail(vf::Error{"Failed to initialize GLFW."});
		}

		if (!glfwVulkanSupported()) {
			return cpp::fail(vf::Error{"GLFW does not support Vulkan."});
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		this->window = glfwCreateWindow(800, 600, "Voxelforge Test Window", NULL, NULL);

		if (!this->window) {
			return cpp::fail(vf::Error{"Failed to create window."});
		}

		spdlog::info("Created window.");

		return {};
	}

	cpp::result<void, vf::Error> create_instance() {
		VkApplicationInfo app_info = {};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pApplicationName = "Voxelforge Test Application";
		app_info.pEngineName = "voxelforge";
		app_info.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo inst_info = {};
		inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		inst_info.pApplicationInfo = &app_info;

		this->instance_extensions = std::vector<const char *>();

		u32 n_glfw_exts = 0;
		const char **glfw_exts;

		glfw_exts = glfwGetRequiredInstanceExtensions(&n_glfw_exts);
		spdlog::info("GLFW requiring {} instance extensions.", n_glfw_exts);
		if (glfw_exts && n_glfw_exts != 0) this->instance_extensions.insert(this->instance_extensions.end(), glfw_exts, glfw_exts + n_glfw_exts);
		
		const auto& vf_extensions = vf::get_required_instance_extensions();
		this->instance_extensions.insert(this->instance_extensions.end(), vf_extensions.begin(), vf_extensions.end());

		spdlog::info("Using {} instance extensions:", this->instance_extensions.size());
		for (const char *ext : this->instance_extensions) {
			spdlog::info("\t{}", ext);
		}

		inst_info.enabledExtensionCount = this->instance_extensions.size();
		inst_info.ppEnabledExtensionNames = this->instance_extensions.data();

#ifdef NDEBUG
		const std::vector<const char *> layers = {
		};
#else
		std::unordered_set<const char *> layers = {
			"VK_LAYER_KHRONOS_validation",
		};

		unsigned int n_supported_layers = 0;
		vkEnumerateInstanceLayerProperties(&n_supported_layers, nullptr);

		const std::vector<VkLayerProperties> supported_layers(n_supported_layers);
		vkEnumerateInstanceLayerProperties(&n_supported_layers, (VkLayerProperties*)supported_layers.data());

		for (const char *layer_name : layers) {
			bool failed = true;
			for (const auto& layer : supported_layers) {
				if (strcmp(layer_name, layer.layerName) == 0) {
					failed = false;
					break;
				}
			}

			if (failed) {
				spdlog::warn("Validation layer \"{}\" requested but not supported!", layer_name);
				layers.erase(layer_name);
				break;
			}
		}
#endif

		this->validation_layers = std::vector<const char *>(layers.begin(), layers.end());

		spdlog::info("Using {} validation layers:", this->validation_layers.size());
		for (const char *layer : this->validation_layers) {
			spdlog::info("\t{}", layer);
		}

		inst_info.enabledLayerCount = this->validation_layers.size();
		inst_info.ppEnabledLayerNames = this->validation_layers.data();

		if (vkCreateInstance(&inst_info, nullptr, &this->instance) != VK_SUCCESS) {
			return cpp::fail(vf::Error{"Failed to create Vulkan instance."});
		}

		spdlog::info("Created Vulkan instance.");

		return {};
	}

	cpp::result<void, vf::Error> create_window_surface() {
		if (glfwCreateWindowSurface(this->instance, this->window, nullptr, &this->window_surface) != VK_SUCCESS) {
			return cpp::fail(vf::Error{"Failed to create window surface."});
		}

		spdlog::info("Created window surface.");

		return {};
	}

	static std::optional<u64> find_present_family(const VkPhysicalDevice& dev, const VkSurfaceKHR& window_surface) {
		u32 n_queue_families = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(dev, &n_queue_families, nullptr);

		std::vector<VkQueueFamilyProperties> queue_families(n_queue_families);
		vkGetPhysicalDeviceQueueFamilyProperties(dev, &n_queue_families, queue_families.data());

		u32 i = 0;
		for (const auto& family : queue_families) {
			VkBool32 present_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, window_surface, &present_support);
			
			if (present_support) return i;

			++i;
		}	

		return std::nullopt;
	}

	i64 score_device(const VkPhysicalDevice& dev, const VkSurfaceKHR& window_surface) {
		i64 vf_score = vf::score_device(dev);

		if (vf_score < 0) return vf_score; // if VoxelForge doesn't support the GPU, we don't either

		if (!find_present_family(dev, window_surface).has_value()) return -1; // we need a GPU with present support for this app
		
		u32 n_exts = 0;
		vkEnumerateDeviceExtensionProperties(dev, nullptr, &n_exts, nullptr);

		std::vector<VkExtensionProperties> exts(n_exts);
		vkEnumerateDeviceExtensionProperties(dev, nullptr, &n_exts, exts.data());

		bool all_exts_found = true;
		for (const char *required_ext : this->device_extensions) {
			bool ext_found = false;

			for (const auto& ext : exts) {
				if (strcmp(ext.extensionName, required_ext) == 0) {
					ext_found = true;
					break;
				}
			}

			if (!ext_found) {
				all_exts_found = false;
				break;
			}
		}

		if (!all_exts_found) return -1; // device is unsuitable if it doesn't support the required extensions
	
		return 1; // no scoring yet...
	}

	cpp::result<void, vf::Error> pick_physical_device() {
		unsigned int n_devices = 0;
		vkEnumeratePhysicalDevices(this->instance, &n_devices, nullptr);

		if (n_devices == 0) {
			return cpp::fail(vf::Error{"No GPUs found with Vulkan support."});
		}

		std::vector<VkPhysicalDevice> devices(n_devices);
		vkEnumeratePhysicalDevices(this->instance, &n_devices, devices.data());

		this->device_extensions = std::vector<const char *>();

		this->device_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		const auto& vf_extensions = vf::get_required_device_extensions();
		this->device_extensions.insert(this->device_extensions.end(), vf_extensions.begin(), vf_extensions.end());

		std::sort(devices.begin(), devices.end(), [this](VkPhysicalDevice& a, VkPhysicalDevice& b) {
			return score_device(a, this->window_surface) > score_device(b, this->window_surface);
		});

		auto& best_device = devices[0];

		if (score_device(best_device, this->window_surface) < 0) {
			return cpp::fail(vf::Error{"No suitable GPUs found."});
		}

		this->physical_device = best_device;

		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(best_device, &props);

		spdlog::info("Picking device {}", props.deviceName);

		return {};
	}

	cpp::result<void, vf::Error> create_logical_device() {
		auto indices = vf::vk_util::QueueFamilyIndices::find(this->physical_device);
		u32 present_index = find_present_family(this->physical_device, this->window_surface).value();

		std::vector<VkDeviceQueueCreateInfo> queue_infos;
		const float priority = 1.0f;

		std::set<u32> queue_families = { indices.graphics.value(), present_index };

		for (u32 family : queue_families) {
			VkDeviceQueueCreateInfo queue_info = {};

			queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_info.queueFamilyIndex = indices.graphics.value();
			queue_info.queueCount = 1;
			queue_info.pQueuePriorities = &priority;
		
			queue_infos.push_back(queue_info);
		}

		VkPhysicalDeviceFeatures device_features = {};
		VkPhysicalDeviceFeatures vf_features = vf::get_required_physical_device_features();

		device_features = vf::vk_util::combine_physical_device_features(device_features, vf_features);

		VkDeviceCreateInfo device_info = {};
		device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		device_info.pQueueCreateInfos = queue_infos.data();
		device_info.queueCreateInfoCount = queue_infos.size();

		device_info.pEnabledFeatures = &device_features;

		spdlog::info("Using {} device extensions:", this->device_extensions.size());
		for (const char *ext : this->device_extensions) {
			spdlog::info("\t{}", ext);
		}

		device_info.enabledExtensionCount = this->device_extensions.size();
		device_info.ppEnabledExtensionNames = this->device_extensions.data();

		device_info.enabledLayerCount = this->validation_layers.size();
		device_info.ppEnabledLayerNames = this->validation_layers.data();

		if (vkCreateDevice(this->physical_device, &device_info, nullptr, &this->device) != VK_SUCCESS) {
			return cpp::fail(vf::Error{"Failed to create Vulkan device."});
		}

		spdlog::info("Created Vulkan device.");

		return {};
	}

	cpp::result<void, vf::Error> get_device_queues() {
		auto indices = vf::vk_util::QueueFamilyIndices::find(this->physical_device);

		vkGetDeviceQueue(this->device, indices.graphics.value(), 0, &this->graphics_queue);

		u32 present_index = find_present_family(this->physical_device, this->window_surface).value();
		
		vkGetDeviceQueue(this->device, present_index, 0, &this->present_queue);
		return {};
	}

	cpp::result<void, vf::Error> run() {
		TRY(this->create_window());

		TRY(this->create_instance());
		
		TRY(this->create_window_surface());
		
		TRY(this->pick_physical_device());
		TRY(this->create_logical_device());
		TRY(this->get_device_queues());

		while (!glfwWindowShouldClose(this->window)) {
			glfwPollEvents();
		}

		this->cleanup();

		return {};
	}

	void cleanup() {
		vkDestroyDevice(this->device, nullptr);

		vkDestroySurfaceKHR(this->instance, this->window_surface, nullptr);
		vkDestroyInstance(this->instance, nullptr);
		
		glfwDestroyWindow(this->window);
		glfwTerminate();
	}

	std::vector<const char *> instance_extensions;
	std::vector<const char *> device_extensions;
	std::vector<const char *> validation_layers;

	GLFWwindow *window;

	VkSurfaceKHR window_surface;

	VkInstance instance;

	VkQueue graphics_queue = VK_NULL_HANDLE;
	VkQueue present_queue = VK_NULL_HANDLE;

	VkPhysicalDevice physical_device = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
};

int main(int argc, char **argv) {

	TestApplication app;

	auto res = app.run();
	if (res.has_error()) {
		spdlog::error("Error whilst running TestApp: \"{}\"!", res.error().message);
		return -1;
	}

	return 0;
}
