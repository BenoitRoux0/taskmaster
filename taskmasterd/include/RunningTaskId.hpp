#ifndef RUNNING_TASKID_HPP
#define RUNNING_TASKID_HPP

#include <format>
#include <string>

class RunningTaskId {
public:
	RunningTaskId(std::string name, int index);

	int operator<=>(const RunningTaskId&) const;
	bool operator==(const RunningTaskId&) const;

	std::string _name;
	int         _index;
};

#endif // RUNNING_TASKID_HPP
