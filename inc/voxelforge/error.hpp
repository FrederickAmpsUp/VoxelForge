#pragma once

#include <any>
#include <optional>
#include <string>

namespace vf {

struct Error {
	std::string message;
	std::optional<std::any> data;
};
}
