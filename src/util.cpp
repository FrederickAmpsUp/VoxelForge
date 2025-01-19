#include <voxelforge/util.hpp>

static const std::vector<const char *> instance_extensions = {

}; // nothing here!

const std::vector<const char *>& vf::get_required_instance_extensions() {
	return instance_extensions;
}

i64 score_device(VkPhysicalDevice& dev) {
	return 1; // no requirements atm
}
