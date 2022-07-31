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

		virtual void OnConnect ( ) { }
		virtual void OnMessage ( const void *buffer , int len ) { }
		virtual void OnDisconnect ( ) { }
	};

}

