#include "clientsock.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

namespace ace {

	ClientSocket::ClientSocket ( ) {

	}

	ClientSocket::~ClientSocket ( ) {
		if ( this->valid ( ) ) {
			OnDisconnect ( );
		}
	}

	int ClientSocket::ConnectTCP ( const char *ip , int port ) {
		ACE_SOCK_ASSERT_FUNC ( this->connect_tcp ( ip , port ) );
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
		printf ( "CLIENT : connected to %s.\n" , this->name ( ).c_str ( ) );
#endif
		OnConnect ( );
		return ACE_SOCK_OK;
	}

	int ClientSocket::PollEvents ( int timeout ) {
		//set of socket descriptors.
		fd_set readfds;

		//clear the socket set.
		FD_ZERO ( &readfds );

		//add master socket to set.
		FD_SET ( id , &readfds );

		timeval select_timeout; select_timeout.tv_sec = timeout / 1000; select_timeout.tv_usec = ( timeout % 1000 ) * 1000;
		int activity = select ( id + 1 , &readfds , NULL , NULL , &select_timeout );

		if ( activity < 0 ) {
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
			printf ( "%s/select failed : %s\n" , __FUNCTION__ , gai_strerrorA ( WSAGetLastError ( ) ) );
#endif
			return ACE_SOCK_SEL_FAILED;
		}

		int ret;

		char *buffer = ( char * ) malloc ( 65536 );

		if ( FD_ISSET ( id , &readfds ) ) {
			// Check if it was for closing , and also read the 
			// incoming message.
			int ret = recv ( id , buffer , 65536 , 0 );
			if ( ret == 0 ) {
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
				printf ( "CLIENT : peer %s disconnected.\n" , this->name ( ).c_str ( ) );
#endif
				OnDisconnect ( );
				this->shutdown ( );
				this->close ( );

				free ( buffer );

				return ACE_SOCK_DISCONNECTED;
			} else if ( ret > 0 ) {
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
				printf ( "CLIENT : received %d bytes from client %s.\n" , ret , this->name ( ).c_str ( ) );
#endif
				OnMessage ( buffer , ret );
			} else {
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
				printf ( "%s/recv failed : %s\n" , __FUNCTION__ , gai_strerrorA ( WSAGetLastError ( ) ) );
				printf ( "CLIENT : connection with %s closed.\n" , this->name ( ).c_str ( ) );
#endif
				OnDisconnect ( );
				this->close ( );

				free ( buffer );

				return ACE_SOCK_RECV_FAILED;
			}
		}

		free ( buffer );

		return ACE_SOCK_OK;
	}

}
