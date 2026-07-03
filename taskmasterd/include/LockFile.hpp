#ifndef LOCKFILE_HPP
#define LOCKFILE_HPP
#include <string>

class LockFile {
public:
	LockFile() = delete;
	LockFile(const std::string& path);

	~LockFile();

	[[nodiscard]] bool isLocked() const;
	void lock();
	void unlock();
private:
	int _fd;
	bool _locked {false};
};

#endif // LOCKFILE_HPP
