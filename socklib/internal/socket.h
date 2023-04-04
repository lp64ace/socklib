#pragma once

#include "socklib.h"

#include <string>

#include <queue>
#include <vector>

typedef std::vector<unsigned char> ByteBuffer;

#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

typedef SOCKET SOCKHANDLE;
#else
#endif

namespace ace {

enum addr_family : int {
  ACE_AF_INET,   // This field specifies the ip4 family addresses.
  ACE_AF_INET6,  // This field specifies the ip6 family addresses.
  ACE_AF_UNSPEC, // The value ACE_AF_UNSPEC indicates either ipv4 or ipv6
                 // address families.
};

enum sock_type : int {
  ACE_SOCK_STREAM,
  ACE_SOCK_DATAGRAM,
};

class Socket {
  SOCKHANDLE id;

public:
  Socket();
  Socket(SOCKHANDLE id);
  virtual ~Socket();

  std::string name() const;

  Socket accept();

  int create(int port, addr_family family = ACE_AF_INET,
             sock_type type = ACE_SOCK_STREAM);
  int connect_tcp(const char *ip, int port, addr_family family = ACE_AF_INET,
                  sock_type type = ACE_SOCK_STREAM);
  int close();
  int shutdown() const;

  int send(const void *buffer, int len) const;
  int send(const Socket &to, const void *buffer, int len) const;

  bool valid() const;

  friend class ServerSocket;
  friend class ClientSocket;
};

} // namespace ace
