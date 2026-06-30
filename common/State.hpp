#ifndef STATE_HPP
#define STATE_HPP

enum class State {
	stopped,
	starting,
	running,
	backOff,
	stopping,
	exited,
	fatal
};

#endif // STATE_HPP
