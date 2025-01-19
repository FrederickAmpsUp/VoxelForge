#include <voxelforge/voxelforge.hpp>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <result/result.hpp>

class TestApplication {
public:
	cpp::result<void, vforge::Error> run() {
		glfwInit();
		this->window = glfwCreateWindow(800, 600, "Voxelforge Test Window", NULL, NULL);
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

		// TODO: validation layers
		
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
