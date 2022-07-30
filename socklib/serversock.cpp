#include "serversock.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

namespace ace {

	ServerSocket::ServerSocket ( ) {
	}

	ServerSocket::~ServerSocket ( ) {
		if ( valid ( ) ) {
			shutdown ( );
			close ( );
		}
	}

	int ServerSocket::Start ( int port ) {
		return create ( port );
	}

	int ServerSocket::PollEvents ( int timeout ) {
		//set of socket descriptors.
		fd_set readfds;

		//clear the socket set.
		FD_ZERO ( &readfds );

		//add master socket to set.
		FD_SET ( id , &readfds );

		int max_sd = id;

		for ( std::list<Socket>::iterator itr = this->clients.begin ( ); itr != this->clients.end ( ); itr++ ) {
			if ( itr->valid ( ) ) {
				FD_SET ( itr->id , &readfds ); // if valid socket descriptor then add to read list.
				max_sd = max ( max_sd , itr->id ); // highest file descriptor number, need it for the select function.
			}
		}

		timeval select_timeout; select_timeout.tv_sec = timeout / 1000; select_timeout.tv_usec = ( timeout % 1000 ) * 1000;
		int activity = select ( max_sd + 1 , &readfds , NULL , NULL , &select_timeout );

		if ( activity < 0 ) {
			printf ( "%s/select failed : %s\n" , __FUNCTION__ , gai_strerrorA ( WSAGetLastError ( ) ) );
			return ACE_SOCK_SEL_FAILED;
		}

		int ret;

		// If something happened on the master socket, 
		// then its an incoming connection.
		if ( FD_ISSET ( id , &readfds ) ) {
			Socket new_socket = this->accept ( );
			if ( new_socket.valid ( ) ) {
				this->clients.push_back ( new_socket );

				sockaddr_in addr;
				socklen_t serv_len = sizeof ( addr );
				ret = getpeername ( new_socket.id , ( struct sockaddr * ) &addr , &serv_len );
				if ( ret != 0 ) {
					printf ( "%s/getpeername warning : %s.\n" , __FUNCTION__ , gai_strerrorA ( WSAGetLastError ( ) ) );
				} else {
					printf ( "connection established with %s::%d.\n" , inet_ntoa ( addr.sin_addr ) , ntohs ( addr.sin_port ) );
				}
			}
		}

		char *buffer = ( char * ) malloc ( 1024 );

		for ( std::list<Socket>::iterator itr = this->clients.begin ( ); itr != this->clients.end ( ); ) {
			std::list<Socket>::iterator next = itr; next++;
			if ( itr->valid ( ) ) {
				if ( FD_ISSET ( itr->id , &readfds ) ) {
					// Check if it was for closing , and also read the 
					// incoming message.
					int ret = recv ( itr->id , buffer , 1024 , 0 );
					if ( ret == 0 ) {
						sockaddr_in addr;
						socklen_t serv_len = sizeof ( addr );
						ret = getpeername ( itr->id , ( struct sockaddr * ) &addr , &serv_len );
						if ( ret != 0 ) {
							printf ( "%s/getpeername warning : %s.\n" , __FUNCTION__ , gai_strerrorA ( WSAGetLastError ( ) ) );
						} else {
							printf ( "client %s::%d disconnected.\n" , inet_ntoa ( addr.sin_addr ) , ntohs ( addr.sin_port ) );
						}

						itr->shutdown ( );
						itr->close ( );
						this->clients.erase ( itr );
					} else if ( ret > 0 ) {
						sockaddr_in addr;
						socklen_t serv_len = sizeof ( addr );
						ret = getpeername ( itr->id , ( struct sockaddr * ) &addr , &serv_len );
						if ( ret != 0 ) {
							printf ( "%s/getpeername warning : %s.\n" , __FUNCTION__ , gai_strerrorA ( WSAGetLastError ( ) ) );
						} else {
							printf ( "received %d bytes from client %s::%d.\n" , ret , inet_ntoa ( addr.sin_addr ) , ntohs ( addr.sin_port ) );
						}

						printf ( "received %d bytes from client %d.\n" , ret , itr->id );
					}
				}
			}
			itr = next;
		}

		free ( buffer );
	}

}
