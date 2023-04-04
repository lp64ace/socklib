#pragma once

#include "socket.h"

#include <list>

namespace ace {

class ServerSocket : private Socket {
public:
  struct Message {
    Socket receiver; // us!
    Socket sender;
    ByteBuffer message;
  };

public:
  ServerSocket();
  ~ServerSocket();

  int start(int port, addr_family family = ACE_AF_INET);

  int poll(int timeout);

  inline int send(const Socket &client, const void *buf, int len) {
    return Socket::send(client, buf, len);
  }
  inline int send_i(const Socket &client, int var) {
    return Socket::send(client, &var, sizeof(var));
  }
  inline int send_z(const Socket &client, size_t var) {
    return Socket::send(client, &var, sizeof(var));
  }
  inline int send_s(const Socket &client, const char *str) {
    return Socket::send(client, str, strlen(str) + 1);
  }

  inline int as_i(const void *buffer) { return *(const int *)buffer; }
  inline size_t as_z(const void *buffer) { return *(const size_t *)buffer; }
  inline const char *as_s(const void *buffer) { return (const char *)buffer; }

  int disconnect(Socket &client);

  virtual void on_connect(const Socket &client) {}
  virtual void on_disconnect(const Socket &client) {}

  bool has_messages() const;
  ServerSocket::Message peek_message() const;
  ServerSocket::Message get_message();

private:
  std::queue<ServerSocket::Message> messages;
  std::list<Socket> clients;
};

} // namespace ace
