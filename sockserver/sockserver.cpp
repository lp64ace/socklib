#include <acesock.h>

#include <stdio.h>

int main ( void ) {
	if ( ace::SockInit ( ) == ACE_SOCK_OK ) {
		ace::ServerSocket server;
		if ( server.Start ( 25565 ) == ACE_SOCK_OK ) {
			while ( true ) {
				server.PollEvents ( 16 );
			}
		} else {
			printf ( "failed to start server.\n" );
		}
		ace::SockQuit ( );
	}
	return 0;
}