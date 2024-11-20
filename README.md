> win32_server.cpp is still new, and it may have some small bugs which we may did not expirience while testing. Please test your application before publishing and issue bugs when they happen.

# win32_server.cpp
A simple, efficient and 🤞 single-threaded TCP server for systems running Windows. May become 🙌 multi-threaded in the future.

# Understanding the library
You can read our 📖 short doc / tutorial [here](https://github.com/XeTute/win32_server.cpp/wiki).

# Example code 🖥️
This should compile without any errors, and you should be able to see a new used port on your system:
```cpp
#include "win32_server.hpp"

std::string function(std::string i) { return i; }
int main()
{
	TCP_server _socket;

	bool run = true;

	_socket.init(4301);
	_socket._listen();
	_socket._accept(function, &run);
	_socket.kill();

	return 0;
}
```

# License 📎
The code found in this repo is licensed under the MIT license, but a note (something like "Built with win32_server.cpp by XeTute") is always appreciated =)

# Star History 🌟
[![Star History Chart](https://api.star-history.com/svg?repos=XeTute/win32_server.cpp&type=Date)](https://star-history.com/#XeTute/win32_server.cpp&Date)
