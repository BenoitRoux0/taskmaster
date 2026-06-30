#ifndef COMMON_H
#define COMMON_H
#include <string>

#include "State.hpp"

struct TaskData {
	std::string name;
	int         index;
	int         status;
	int         exitStatus;
	State       state;
	std::string cmd;
};

#endif // COMMON_H
