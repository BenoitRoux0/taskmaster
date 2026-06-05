#include <cctype>

namespace stackixx {
	void skipWhitespaces(const char*& ptr) {
		while (isspace(*ptr))
			++ptr;
	}
} // namespace stackixx
