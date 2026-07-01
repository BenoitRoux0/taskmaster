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

inline std::string to_string(State e) {
	switch (e) {
		case State::stopped: return "stopped";
		case State::starting: return "starting";
		case State::running: return "running";
		case State::backOff: return "backOff";
		case State::stopping: return "stopping";
		case State::exited: return "exited";
		case State::fatal: return "fatal";
		default: return "unknown";
	}
}

#endif // STATE_HPP
