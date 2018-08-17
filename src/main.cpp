#include "blib/bun/bun.hpp"
#include <sstream>

namespace ushort {
	struct Users {
		std::string uname;
		std::string edit_key;
		std::string url;
		std::string short_key;
	};
}

SPECIALIZE_BUN_HELPER((ushort::Users, uname, edit_key, url, short_key));

int main() {

	return 1;
}
