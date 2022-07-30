#pragma once

#include "socket.h"

#include <list>

namespace ace {

	class ServerSocket : public Socket {
	public:
		ServerSocket ( );
		~ServerSocket ( );

		int Start ( int port );

		int PollEvents ( int timeout );
	private:
		std::list<Socket> clients;
	};

}
