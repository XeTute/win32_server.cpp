> win32_server.cpp is still new, and it may have some small bugs which we may did not expirience while testing. Please test your application before publishing.

# win32_server.cpp
A simple, efficient and single-threaded TCP server for systems running Windows. May become multi-threaded in the future.

# Understanding the library
win32_server.cpp is a headers-only library for Windows system.<br>
To create a server, you only need a free port number and a function which handles the input(char* function(char* input)).

# Example code
This should compile without any errors, and you should be able to see a new used port on your system:
```cpp
#include <malloc.h>

#include "TCP_server.hpp"

char* function(char* i)
{
	char* o = i;
	return o;
}

int main()
{
	TCP_server _socket;

	bool* run = (bool*)malloc(sizeof(bool));
	*run = true;

	_socket.init(4301);
	_socket._listen();
	_socket._accept(function, run);
	_socket.kill();

	return 0;
}
```

# License
The code found in this repo is licensed under the MIT license, but a note (something like "Built with win32_server.cpp by XeTute" is always appreciated =)

# Star History
[![Star History Chart](https://api.star-history.com/svg?repos=XeTute/win32_server.cpp&type=Date)](https://star-history.com/#XeTute/win32_server.cpp&Date)
