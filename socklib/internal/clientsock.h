#pragma once

#include "socket.h"

namespace ace {

class ClientSocket : private Socket {
public:
  using Message = ByteBuffer;

public:
  ClientSocket();
  virtual ~ClientSocket();

  int connect(const char *ip, int port, addr_family family = ACE_AF_INET);

  int poll(int timeout);

  inline int send(const void *buf, int len) { return Socket::send(buf, len); }
  inline int send_i(int var) { return Socket::send(&var, sizeof(var)); }
  inline int send_z(size_t var) { return Socket::send(&var, sizeof(var)); }
  inline int send_s(const char *str) {
    return Socket::send(str, strlen(str) + 1);
  }

  inline int as_i(const void *buffer) { return *(const int *)buffer; }
  inline size_t as_z(const void *buffer) { return *(const size_t *)buffer; }
  inline const char *as_s(const void *buffer) { return (const char *)buffer; }

  int disconnect() const;

  bool has_messages() const;
  ClientSocket::Message peek_message() const;
  ClientSocket::Message get_message();

private:
  std::queue<ClientSocket::Message> messages;
};

} // namespace ace
