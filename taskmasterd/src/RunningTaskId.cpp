#include "RunningTaskId.hpp"

#include <cstring>

RunningTaskId::RunningTaskId(std::string name, int index): _name(std::move(name)), _index(index) {
}

int RunningTaskId::operator<=>(const RunningTaskId& rhs) const {
	if (_name == rhs._name) {
		return _index - rhs._index;
	}

	return strcmp(_name.c_str(), rhs._name.c_str());
}
