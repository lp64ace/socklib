#pragma once

#include "socket.h"

#include <list>

namespace ace {

	class ServerSocket : private Socket {
	public:
		ServerSocket ( );
		~ServerSocket ( );

		int Start ( int port , addr_family family = ACE_AF_INET );

		int PollEvents ( int timeout );

		inline int Send ( const Socket &client , const void *buf , int len ) { return Socket::send ( client , buf , len ); }
		inline int SendInt ( const Socket &client , int var ) { return Socket::send ( client , &var , sizeof ( var ) ); }
		inline int SendSizeT ( const Socket &client , size_t var ) { return Socket::send ( client , &var , sizeof ( var ) ); }
		inline int SendStr ( const Socket &client , const char *str ) { return Socket::send ( client , str , strlen ( str ) + 1 ); }

		inline int ToInt ( const void *buffer ) { return *( const int * ) buffer; }
		inline size_t ToSizeT ( const void *buffer ) { return *( const size_t * ) buffer; }
		inline const char *ToStr ( const void *buffer ) { return ( const char * ) buffer; }

		int Disconnect ( Socket& client );

		virtual void OnConnect ( Socket& client ) { }
		virtual void OnMessage ( Socket& from , const void *buffer , int len ) { }
		virtual void OnDisconnect ( Socket& client ) { }
	private:
		std::list<Socket> clients;
	};

}
