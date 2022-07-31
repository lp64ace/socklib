#include "socket.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

namespace ace {

	Socket::Socket ( ) : id ( INVALID_SOCKET ) {

	}

	Socket::Socket ( unsigned int id ) : id ( id ) {

	}

	Socket::~Socket ( ) {

	}

	int Socket::create ( int port , addr_family family , sock_type type ) {
		if ( SockStatus ( ) != ACE_SOCK_OK ) {
			ACE_SOCK_ASSERT_FUNC ( SockInit ( ) );
		}

		if ( id != INVALID_SOCKET ) {
			ACE_SOCK_ASSERT_FUNC ( this->shutdown ( ) );
			ACE_SOCK_ASSERT_FUNC ( this->close ( ) );
		}

		int ret;

		struct addrinfo *result = NULL , hints;

		ZeroMemory ( &hints , sizeof ( hints ) );
		switch ( family ) {
			case ace::ACE_AF_INET: hints.ai_family = AF_INET; break;
			case ace::ACE_AF_INET6: hints.ai_family = AF_INET6; break;
			case ace::ACE_AF_UNSPEC: hints.ai_family = AF_UNSPEC; break;
			default: hints.ai_family = AF_INET; break;
		}
		switch ( type ) {
			case ace::ACE_SOCK_STREAM: hints.ai_socktype = SOCK_STREAM; break;
			case ace::ACE_SOCK_DATAGRAM: hints.ai_socktype = SOCK_DGRAM; break;
			default: hints.ai_socktype = SOCK_STREAM; break;
		}
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		char service [ 64 ]; sprintf_s ( service , 64 , "%d" , port );

		// Resolve the server address and port
		ret = getaddrinfo ( NULL , service , &hints , &result );
		if ( ret != 0 ) {
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
			printf ( "%s/getaddrinfo failed : %s\n" , __FUNCTION__ , gai_strerrorA ( WSAGetLastError ( ) ) );
#endif
			return ACE_SOCK_GETADDR_FAILED;
		}

		// Create a SOCKET for connecting to server
		id = socket ( result->ai_family , result->ai_socktype , result->ai_protocol );
		if ( id == INVALID_SOCKET ) {
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
			printf ( "%s/socket failed : %s\n" , __FUNCTION__ , gai_strerrorA ( WSAGetLastError ( ) ) );
#endif
			return ACE_SOCK_GET_FAILED;
		}

		// Setup the TCP listening socket
		ret = bind ( id , result->ai_addr , ( int ) result->ai_addrlen );
		if ( ret == SOCKET_ERROR ) {
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
			printf ( "%s/bind failed : %s\n" , __FUNCTION__ , gai_strerrorA ( WSAGetLastError ( ) ) );
#endif
			ACE_SOCK_ASSERT_FUNC ( close ( ) );
			return ACE_SOCK_BIND_FAILED;
		}

		freeaddrinfo ( result );

		ret = listen ( id , SOMAXCONN );
		if ( ret == SOCKET_ERROR ) {
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
			printf ( "%s/listen failed : %s\n" , __FUNCTION__ , gai_strerrorA ( WSAGetLastError ( ) ) );
#endif
			ACE_SOCK_ASSERT_FUNC ( close ( ) );
			return ACE_SOCK_LISTEN_FAILED;
		}

		return ACE_SOCK_OK;
	}

	Socket Socket::accept ( ) {
		int sock = ::accept ( id , NULL , NULL );
		if ( sock == INVALID_SOCKET ) {
			printf ( "%s/accept failed : " , __FUNCTION__ );
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
			switch ( WSAGetLastError ( ) ) {
				case WSANOTINITIALISED: printf ( "A successful SockInit call must occur before using this function.\n" ); break;
				case WSAENETDOWN: printf ( "The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.\n" ); break;
				case WSAEINPROGRESS: printf ( "A blocking Windows Sockets 1.1 operation is in progress.\n" ); break;
				case WSAECONNRESET: printf ( "An incoming connection was indicated, but was subsequently terminated by the remote peer prior to accepting the call.\n" ); break;
				case WSAEFAULT: printf ( "The addrlen parameter is too small or addr is not a valid part of the user address space.\n" ); break;
				case WSAEINTR: printf ( "A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall.\n" ); break;
				case WSAEINVAL: printf ( "The listen function was not invoked prior to accept.\n" ); break;
				case WSAEMFILE: printf ( "The queue is nonempty upon entry to accept and there are no descriptors available.\n" ); break;
				case WSAENOBUFS: printf ( "No buffer space is available.\n" ); break;
				case WSAENOTSOCK: printf ( "The descriptor is not a socket.\n" ); break;
				case WSAEOPNOTSUPP: printf ( "The referenced socket is not a type that supports connection-oriented service.\n" ); break;
				case WSAEWOULDBLOCK: printf ( "The socket is marked as nonblocking and no connections are present to be accepted.\n" ); break;
				default: printf ( "Unknown error.\n" ); break;
			}
#endif
			close ( );
		}
		return Socket ( sock );
	}

	int Socket::connect_tcp ( const char *ip , int port , addr_family family , sock_type type ) {
		if ( SockStatus ( ) != ACE_SOCK_OK ) {
			ACE_SOCK_ASSERT_FUNC ( SockInit ( ) );
		}

		if ( id != INVALID_SOCKET ) {
			ACE_SOCK_ASSERT_FUNC ( this->shutdown ( ) );
			ACE_SOCK_ASSERT_FUNC ( this->close ( ) );
		}

		int ret;

		struct addrinfo *result = NULL , hints;

		ZeroMemory ( &hints , sizeof ( hints ) );
		switch ( family ) {
			case ace::ACE_AF_INET: hints.ai_family = AF_INET; break;
			case ace::ACE_AF_INET6: hints.ai_family = AF_INET6; break;
			case ace::ACE_AF_UNSPEC: hints.ai_family = AF_UNSPEC; break;
			default: hints.ai_family = AF_INET; break;
		}
		switch ( type ) {
			case ace::ACE_SOCK_STREAM: hints.ai_socktype = SOCK_STREAM; break;
			case ace::ACE_SOCK_DATAGRAM: hints.ai_socktype = SOCK_DGRAM; break;
			default: hints.ai_socktype = SOCK_STREAM; break;
		}
		hints.ai_protocol = IPPROTO_TCP;

		char service [ 64 ]; sprintf_s ( service , 64 , "%d" , port );

		// Resolve the server address and port
		ret = getaddrinfo ( ip , service , &hints , &result );
		if ( ret != 0 ) {
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
			printf ( "%s/getaddrinfo failed : %s\n" , __FUNCTION__ , gai_strerrorA ( WSAGetLastError ( ) ) );
#endif
			return ACE_SOCK_GETADDR_FAILED;
		}

		// Attempt to connect to an address until one succeeds
		for ( struct addrinfo *ptr = result; ptr != NULL; ptr = ptr->ai_next ) {
			// Create a SOCKET for connecting to server
			id = socket ( ptr->ai_family , ptr->ai_socktype , ptr->ai_protocol );
			if ( id == INVALID_SOCKET ) {
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
				printf ( "%s/socket failed : %s\n" , __FUNCTION__ , gai_strerrorA ( WSAGetLastError ( ) ) );
#endif
				return ACE_SOCK_GET_FAILED;
			}

			// Connect to server.
			ret = connect ( id , ptr->ai_addr , ( int ) ptr->ai_addrlen );
			if ( ret == SOCKET_ERROR ) {
				this->close ( );
				continue;
			}
			
			break;
		}

		freeaddrinfo ( result );

		if ( id == INVALID_SOCKET ) {
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
			printf ( "unable to connect to server.\n" );
#endif
			return ACE_SOCK_CONN_FAILED;
		}

		return ACE_SOCK_OK;
	}

	int Socket::close ( ) {
		int status = closesocket ( id );
		if ( status != 0 ) {
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
			printf ( "%s failed : " , __FUNCTION__ );
			switch ( WSAGetLastError ( ) ) {
				case WSANOTINITIALISED: printf ( "A successful SockInit call must occur before using this function.\n" ); break;
				case WSAENETDOWN: printf ( "The network subsystem has failed.\n" ); break;
				case WSAENOTSOCK: printf ( "This object is not describing a socket.\n" ); break;
				case WSAEINPROGRESS: printf ( "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.\n" ); break;
				case WSAEINTR: printf ( "The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall.\n" ); break;
				case WSAEWOULDBLOCK: printf ( "The socket is marked as nonblocking, but the l_onoff member of the linger structure is set to nonzero and the l_linger member of the linger structure is set to a nonzero timeout value.\n" ); break;
				default: printf ( "Unknown error.\n" ); break;
			}
#endif
			return ACE_SOCK_CLOSE_FAILED;
		}
		id = INVALID_SOCKET;
		return ACE_SOCK_OK;
	}

	int Socket::shutdown ( ) const {
		int status = ::shutdown ( id , SD_BOTH );
		if ( status != 0 ) {
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
			printf ( "%s failed : " , __FUNCTION__ );
			switch ( WSAGetLastError ( ) ) {
				case WSANOTINITIALISED: printf ( "A successful SockInit call must occur before using this function.\n" ); break;
				case WSAECONNABORTED: printf ( "The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable. This error applies only to a connection - oriented socket.\n" ); break;
				case WSAECONNRESET: printf ( "The virtual circuit was reset by the remote side executing a hard or abortive close. The application should close the socket as it is no longer usable. This error applies only to a connection - oriented socket.\n" ); break;
				case WSAENOTSOCK: printf ( "This object is not describing a socket.\n" ); break;
				case WSAEINPROGRESS: printf ( "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.\n" ); break;
				case WSAEINTR: printf ( "The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall.\n" ); break;
				case WSAENOTCONN: printf ( "The socket is not connected. This error applies only to a connection-oriented socket.\n" ); break;
				case WSAENETDOWN: printf ( "The network subsystem has failed.\n" ); break;
				default: printf ( "Unknown error.\n" ); break;
			}
#endif
			return ACE_SOCK_SHUTDOWN_FAILED;
		}
		return ACE_SOCK_OK;
	}

	bool Socket::valid ( ) const {
		return this->id != INVALID_SOCKET;
	}

	int Socket::send ( const void *buffer , int len ) const {
		int ret = ::send ( id , ( const char * ) buffer , len , 0 );
		if ( ret < 0 ) {
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
			printf ( "%s/send failed : %s\n" , __FUNCTION__ , gai_strerrorA ( WSAGetLastError ( ) ) );
#endif
			return ACE_SOCK_SEND_FAILED;
		}
		return ACE_SOCK_OK;
	}

	int Socket::send ( const Socket &to , const void *buffer , int len ) const {
		int ret = ::send ( to.id , ( const char * ) buffer , len , 0 );
		if ( ret < 0 ) {
#if ( ACE_BUILD_CONF & ACE_BUILD_WITH_IO )
			printf ( "%s/send failed : %s\n" , __FUNCTION__ , gai_strerrorA ( WSAGetLastError ( ) ) );
#endif
			return ACE_SOCK_SEND_FAILED;
		}
		return ACE_SOCK_OK;
	}

	std::string Socket::name ( ) const {
		sockaddr_in addr;
		socklen_t serv_len = sizeof ( addr );
		int ret = getpeername ( this->id , ( struct sockaddr * ) &addr , &serv_len );
		if ( ret != 0 ) {
			return "unkown";
		}
		return std::string ( inet_ntoa ( addr.sin_addr ) ) + "::" + std::to_string ( ntohs ( addr.sin_port ) );
	}

}
