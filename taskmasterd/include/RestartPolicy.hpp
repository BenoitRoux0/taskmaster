#ifndef RESTART_POLICY_HPP
#define RESTART_POLICY_HPP

enum RestartPolicy {
	always = 1,
	never,
	unexpected
};

inline RestartPolicy restartPolicyFromStr(const std::string& policy) {
	if (policy == "always") {
		return always;
	}
	if (policy == "unexpected") {
		return unexpected;
	}
	return never;
}

#endif // RESTART_POLICY_HPP
