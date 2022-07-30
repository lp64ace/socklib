#pragma once

#include "socket.h"

#include <list>

namespace ace {

	class ServerSocket : protected Socket {
	public:
		ServerSocket ( );
		~ServerSocket ( );

		int Start ( int port );

		int PollEvents ( int timeout );

		virtual void OnConnect ( Socket& client ) { }
		virtual void OnMessage ( Socket& from , const void *buffer , int len ) { }
		virtual void OnDisconnect ( Socket& client ) { }
	private:
		std::list<Socket> clients;
	};

}
