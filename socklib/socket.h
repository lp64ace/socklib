#pragma once

#include "socklib.h"

namespace ace {

	class Socket {
		unsigned int id;
	public:
		Socket ( );
		Socket ( unsigned int id );
		virtual ~Socket ( );

		Socket accept ( );

		int create ( int port );
		int connect_tcp ( const char *ip , int port );
		int close ( );
		int shutdown ( );

		bool valid ( ) const;

		friend class ServerSocket;
	};

}
