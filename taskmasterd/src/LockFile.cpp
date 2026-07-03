#include "LockFile.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>

LockFile::LockFile(const std::string& path) {
	_fd = open(path.c_str(), O_CREAT, 0666);
}

LockFile::~LockFile() {
	unlock();
	close(_fd);
}

bool LockFile::isLocked() const {
	return _locked;
}

void LockFile::lock() {
	if (_locked)
		return;

	if (flock(_fd, LOCK_EX | LOCK_NB) == -1) {
		_locked = false;
		return;
	}

	_locked = true;
}

void LockFile::unlock() {
	if (!_locked)
		return;

	flock(_fd, LOCK_UN | LOCK_NB);

	_locked = false;
}
