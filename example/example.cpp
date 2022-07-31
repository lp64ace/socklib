#include <acesock.h>
#include <stdio.h>

#define TIMEOUT_MS			16

class bkServerSocket : public ace::ServerSocket {
public:
	void OnConnect ( ace::Socket &client ) { // A client connected to this server.
		this->Send ( client , "Greetings from Server." , 23 );
	}

	void OnMessage ( ace::Socket &from , const void *buffer , int len ) { // Received message from client.
		printf ( "SERVER RECEIVED : '%s'\n" , ( const char * ) buffer );
	}

	void OnDisconnect ( ace::Socket &client ) { // Lost connection to client.
	}
};

class bkClientSocket : public ace::ClientSocket {
public:
	void OnConnect ( ) { // Connected to server.
		this->Send ( "Greetings from Client." , 23 );
	}

	void OnMessage ( const void *buffer , int len ) { // Received message from server.
		printf ( "CLIENT RECEIVED : '%s'\n" , ( const char * ) buffer );
	}

	void OnDisconnect ( ) { // Lost connection to server.
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
