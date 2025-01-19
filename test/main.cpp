#include <voxelforge/voxelforge.hpp>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <result/result.hpp>
#include <unordered_set>
#include <cstring>

class TestApplication {
public:
	cpp::result<void, vforge::Error> run() {
		glfwInit();
		this->window = glfwCreateWindow(800, 600, "Voxelforge Test Window", NULL, NULL);
	
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		if (!this->window) {
			return cpp::fail(vforge::Error{"Failed to create window."});
		}

		VkApplicationInfo app_info{};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pApplicationName = "Voxelforge Test Application";
		app_info.pEngineName = "voxelforge";
		app_info.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo inst_info{};
		inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		inst_info.pApplicationInfo = &app_info;

		u32 glfw_ext_count = 0;		
		const char **glfw_exts;

		glfw_exts = glfwGetRequiredInstanceExtensions(&glfw_ext_count);

		std::vector<const char *> extensions(glfw_exts, glfw_exts + glfw_ext_count);
		const auto& vf_extensions = vforge::get_required_instance_extensions();
		extensions.insert(extensions.end(), vf_extensions.begin(), vf_extensions.end());

		inst_info.enabledExtensionCount = extensions.size();
		inst_info.ppEnabledExtensionNames = extensions.data();

#ifdef NDEBUG
		const std::vector<const char *> layers = {
		};
#else
		std::unordered_set<const char *> layers = {
			"VK_LAYER_KHRONOS_validation",
			"validate deez nuts"
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

		std::vector<const char *> layer_vec(layers.begin(), layers.end());

		inst_info.enabledLayerCount = layer_vec.size();
		inst_info.ppEnabledLayerNames = layer_vec.data();

		if (vkCreateInstance(&inst_info, nullptr, &this->instance)) {
			return cpp::fail(vforge::Error{"Failed to create Vulkan instance."});
		}

		vkDestroyInstance(this->instance, nullptr);
		glfwDestroyWindow(this->window);
		glfwTerminate();

		return {};
	}

	GLFWwindow *window;
	VkInstance instance;
};

int main(int argc, char **argv) {

	TestApplication app;

	auto res = app.run();
	if (res.has_error()) {
		spdlog::error("Error whilst running TestApp: \"{}\"!", res.error().message);
	}

	return 0;
}
