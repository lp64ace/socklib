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

#define TIMEOUT_MS			16

class bkServerSocket : public ace::ServerSocket {
public:
	void OnConnect ( ace::Socket &client ) { // A client connected to this server.
		this->SendStr ( client , "Greetings from Server." );
	}

	void OnMessage ( ace::Socket &from , const void *buffer , int len ) { // Received message from client.
		printf ( "server : '%s'\n" , ( const char * ) buffer );
		this->Disconnect ( from );
	}

	void OnDisconnect ( ace::Socket &client ) { // Lost connection to client.
		printf ( "server : %s disconnected.\n" , client.name ( ).c_str ( ) );
	}
};

class bkClientSocket : public ace::ClientSocket {
public:
	void OnConnect ( ) { // Connected to server.
		this->SendStr ( "Greetings from Client." );
	}

	void OnMessage ( const void *buffer , int len ) { // Received message from server.
		printf ( "client : '%s'\n" , ToStr ( buffer ) );
	}

	void OnDisconnect ( ) { // Lost connection to server.
		printf ( "client : disconnected.\n" );
	}
};

int main ( void ) {
	if ( ace::SockInit ( ) == ACE_SOCK_OK ) { // Init the socket data.
		bkServerSocket server; // Create the server object.
		if ( server.Start ( 8080 ) != ACE_SOCK_OK ) { // Start the server on port 8080.
			return -1;
		}
		bkClientSocket client; // Create teh client object.
		if ( client.ConnectTCP ( "127.0.0.1" , 8080 ) != ACE_SOCK_OK ) { // Connect the client to our server.
			return -1;
		}
		// Handle events for our server and client for 16ms each until there is an error.
		while ( server.PollEvents ( TIMEOUT_MS ) == ACE_SOCK_OK && client.PollEvents ( TIMEOUT_MS ) == ACE_SOCK_OK ) {
		}
		ace::SockQuit ( ); // Cleanup the socket data.
	}
	return 0;
}
```

### Functions

#### Virtual Server Functions

* `void OnConnect ( Socket& client );` called once a new client connects.
    * client: The client that just connected.
* `void OnMessage ( Socket& from , const void *buffer , int len )` called per message received.
    * from: Who send the message.
    * buffer: The data of the message.
    * len: The length of the data.
* `void OnDisconnect ( Socket& client )` called once a client disconnects.
    * client: The client that just disconnected.

#### Client Functions

* `void OnConnect ( )` called once the connection to the server is established.
* `void OnMessage ( const void *buffer , int len )` called per message received.
    * buffer: The data of the message.
    * len: The length of the data.
* `void OnDisconnect ( ) { }` called once the conncetion to the server is lost.

## TODO :
  * Add more configurations.
  * Add a method to get notifications for data without virtual function.
