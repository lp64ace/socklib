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
	if ( ace::SockInit ( ) == ACE_SOCK_OK ) {
		bkServerSocket server;
		if ( server.Start ( 8080 ) != ACE_SOCK_OK ) {
			return -1;
		}
		bkClientSocket client;
		if ( client.ConnectTCP ( "127.0.0.1" , 8080 ) != ACE_SOCK_OK ) {
			return -1;
		}
		while ( server.PollEvents ( TIMEOUT_MS ) == ACE_SOCK_OK && client.PollEvents ( TIMEOUT_MS ) == ACE_SOCK_OK ) {
		}
		ace::SockQuit ( );
	}
	return 0;
}
