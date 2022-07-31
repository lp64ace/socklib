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
				printf ( "failed to start client.\n" );
			}
			ace::SockQuit ( );
		}
	} );

	server.join ( );
	client.join ( );
	return 0;
}
