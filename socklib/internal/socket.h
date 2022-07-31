#pragma once

#include "socklib.h"

#include <string>

namespace ace {

	enum addr_family : int {
		ACE_AF_INET, // This field specifies the ip4 family addresses.
		ACE_AF_INET6, // This field specifies the ip6 family addresses.
		ACE_AF_UNSPEC, // The value ACE_AF_UNSPEC indicates either ipv4 or ipv6 address families.
	};

	class Socket {
		unsigned int id;
	public:
		Socket ( );
		Socket ( unsigned int id );
		virtual ~Socket ( );

		std::string name ( ) const;

		Socket accept ( );

		int create ( int port , addr_family family = ACE_AF_INET );
		int connect_tcp ( const char *ip , int port , addr_family family = ACE_AF_INET );
		int close ( );
		int shutdown ( );

		int send ( const void *buffer , int len );
		int send ( const Socket& to , const void *buffer , int len );

		bool valid ( ) const;

		friend class ServerSocket;
		friend class ClientSocket;
	};

}
