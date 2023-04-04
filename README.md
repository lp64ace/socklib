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

### Virtual Functions

#### Virtual Server Functions

* `void OnConnect ( Socket& client );` called once a new client connects.
    * client: The client that just connected.
* `void OnMessage ( Socket& from , const void *buffer , int len )` called per message received.
    * from: Who send the message.
    * buffer: The data of the message.
    * len: The length of the data.
* `void OnDisconnect ( Socket& client )` called once a client disconnects.
    * client: The client that just disconnected.

#### Virtual Client Functions

* `void OnConnect ( )` called once the connection to the server is established.
* `void OnMessage ( const void *buffer , int len )` called per message received.
    * buffer: The data of the message.
    * len: The length of the data.
* `void OnDisconnect ( ) { }` called once the conncetion to the server is lost.

## TODO :
  * Add more configurations.
  * Add a method to get notifications for data without virtual function.
