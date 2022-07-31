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
#include <thread>

#define TIMEOUT_MS			16

class bkServerSocket : public ace::ServerSocket {
public:
	void OnConnect ( ace::Socket &client ) {
		this->Send ( client , "Greetings from Server." , 23 );
	}

	void OnMessage ( ace::Socket &from , const void *buffer , int len ) {
		printf ( "SERVER RECEIVED : '%s'\n" , ( const char * ) buffer );
	}

	void OnDisconnect ( ace::Socket &client ) {
	}
};

class bkClientSocket : public ace::ClientSocket {
public:
	void OnConnect ( ) {
		this->Send ( "Greetings from Client." , 23 );
	}

	void OnMessage ( const void *buffer , int len ) {
		printf ( "CLIENT RECEIVED : '%s'\n" , ( const char * ) buffer );
	}

	void OnDisconnect ( ) {
	}
};

int main ( void ) {
	std::thread server ( [ ] ( ) {
		if ( ace::SockInit ( ) == ACE_SOCK_OK ) {
			bkServerSocket server;
			if ( server.Start ( 80 ) == ACE_SOCK_OK ) {
				while ( server.PollEvents ( TIMEOUT_MS ) == ACE_SOCK_OK ) {
				}
			} else {
				printf ( "failed to start server.\n" );
			}
			ace::SockQuit ( );
		}
	} );

	std::thread client ( [ ] ( ) {
		if ( ace::SockInit ( ) == ACE_SOCK_OK ) {
			bkClientSocket client;
			if ( client.ConnectTCP ( "127.0.0.1" , 80 ) == ACE_SOCK_OK ) {
				while ( client.PollEvents ( TIMEOUT_MS ) == ACE_SOCK_OK ) {
				}
			} else {
				printf ( "failed to start server.\n" );
			}
			ace::SockQuit ( );
		}
	} );

	server.join ( );
	client.join ( );
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
