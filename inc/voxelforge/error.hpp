#pragma once

#include <any>
#include <optional>
#include <string>

namespace vforge {

struct Error {
	std::string message;
	std::optional<std::any> data;
};
}
