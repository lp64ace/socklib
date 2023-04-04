# socklib
Socket library for C++ and Win32.

## Features
* Easy: Build the library, include `acesock.h` and link `socklib.lib` to your project, or add the library files to your project include `acesock.h`, link `Ws2_32.lib` and `Mswsock.lib` and run.

### Import:
```cpp
#include <acesock.h>
```

### Namespace

```cpp
using namespace ace;
```

### Usecase

* Socklib wraps the code for the server and client using virtual functions.

```cpp
#include <acesock.h>
#include <stdio.h>

#define TIMEOUT_MS 16

int main(void) {
	if (ace::SockInit() == ACE_SOCK_OK) { // Init the socket data.
		ace::ServerSocket server;
		if (server.start(8080) != ACE_SOCK_OK) {
			return -1;
		}
		ace::ClientSocket client;
		if (client.connect("127.0.0.1", 8080) != ACE_SOCK_OK) {
			return -1;
		}
		client.send_s("greetings from client!");
		while (server.poll(TIMEOUT_MS) == ACE_SOCK_OK && client.poll(TIMEOUT_MS) == ACE_SOCK_OK) {
			while (server.has_messages()) {
				ace::ServerSocket::Message msg = server.get_message();
				printf("[server] received %zu bytes '%s' from '%s'.\n", msg.message.size(), (const char *)msg.message.data(), msg.sender.name().c_str());
				server.send_s(msg.sender, "greetings from server!");
			}
			while (client.has_messages()) {
				ace::ClientSocket::Message msg = client.get_message();
				printf("[client] received %zu bytes '%s'.\n", msg.size(), msg.data());
				client.send_s("greetings from client!");
			}
		}
		ace::SockQuit(); // Cleanup the socket data.
	}
	return 0;
}
```

## TODO :
  * Add more configurations.
