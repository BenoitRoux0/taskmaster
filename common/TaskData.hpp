#ifndef COMMON_H
#define COMMON_H
#include <string>

struct TaskData {
	std::string name;
	int         index;
	int         status;
	int         exitStatus;
	std::string cmd;
};

#endif // COMMON_H
