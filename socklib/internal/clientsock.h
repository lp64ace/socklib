#pragma once

#include "socket.h"

namespace ace {

	class ClientSocket : private Socket {
	public:
		ClientSocket ( );
		virtual ~ClientSocket ( );

		int ConnectTCP ( const char *ip , int port , addr_family family = ACE_AF_INET );

		int PollEvents ( int timeout );

		inline int Send ( const void *buf , int len ) { return Socket::send ( buf , len ); }
		inline int SendInt ( int var ) { return Socket::send ( &var , sizeof ( var ) ); }
		inline int SendSizeT ( size_t var ) { return Socket::send ( &var , sizeof ( var ) ); }
		inline int SendStr ( const char *str ) { return Socket::send ( str , strlen ( str ) + 1 ); }

		inline int ToInt ( const void *buffer ) { return *( const int * ) buffer; }
		inline size_t ToSizeT ( const void *buffer ) { return *( const size_t * ) buffer; }
		inline const char *ToStr ( const void *buffer ) { return ( const char * ) buffer; }

		int Disconnect ( ) const;

		virtual void OnConnect ( ) { }
		virtual void OnMessage ( const void *buffer , int len ) { }
		virtual void OnDisconnect ( ) { }
	};

}

