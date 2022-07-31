#pragma once

#include "socket.h"

#include <list>

namespace ace {

	class ServerSocket : private Socket {
	public:
		ServerSocket ( );
		~ServerSocket ( );

		int Start ( int port );

		int PollEvents ( int timeout );

		inline int Send ( const Socket &client , const void *buf , int len ) { return Socket::send ( client , buf , len ); }

		virtual void OnConnect ( Socket& client ) { }
		virtual void OnMessage ( Socket& from , const void *buffer , int len ) { }
		virtual void OnDisconnect ( Socket& client ) { }
	private:
		std::list<Socket> clients;
	};

}
