#include "serversock.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace ace {

ServerSocket::ServerSocket() : Socket() {}

ServerSocket::~ServerSocket() {
  if (valid()) {
    for (std::list<Socket>::iterator itr = this->clients.begin();
         itr != this->clients.end(); itr++) {
      if (itr->valid()) {
        on_disconnect(*itr);
      }
    }
    this->clients.clear();
  }
}

int ServerSocket::start(int port, addr_family family) {
  return create(port, family);
}

int ServerSocket::poll(int timeout) {
  // set of socket descriptors.
  fd_set readfds;

  // clear the socket set.
  FD_ZERO(&readfds);

  // add master socket to set.
  FD_SET(id, &readfds);

  int max_sd = id;

  for (std::list<Socket>::iterator itr = this->clients.begin();
       itr != this->clients.end(); itr++) {
    if (itr->valid()) {
      FD_SET(itr->id,
             &readfds); // if valid socket descriptor then add to read list.
      max_sd = max(max_sd, itr->id); // highest file descriptor number, need it
                                     // for the select function.
    }
  }

  timeval select_timeout;
  select_timeout.tv_sec = timeout / 1000;
  select_timeout.tv_usec = (timeout % 1000) * 1000;
  int activity = select(max_sd + 1, &readfds, NULL, NULL, &select_timeout);

  if (activity < 0) {
#if (ACE_BUILD_CONF & ACE_BUILD_WITH_IO)
    printf("%s/select failed : %s\n", __FUNCTION__,
           gai_strerrorA(WSAGetLastError()));
#endif
    return ACE_SOCK_SEL_FAILED;
  }

  int ret;

  // If something happened on the master socket,
  // then its an incoming connection.
  if (FD_ISSET(id, &readfds)) {
    Socket new_socket = this->accept();
    if (new_socket.valid()) {
      this->clients.push_back(new_socket);
#if (ACE_BUILD_CONF & ACE_BUILD_WITH_DBG_IO)
      printf("ace::server-socket : peer %s connected.\n",
             new_socket.name().c_str());
#endif
      on_connect(new_socket);
    }
  }

  char *buffer = (char *)malloc(65536);

  for (std::list<Socket>::iterator itr = this->clients.begin();
       itr != this->clients.end();) {
    std::list<Socket>::iterator next = itr;
    next++;
    if (itr->valid()) {
      if (FD_ISSET(itr->id, &readfds)) {
        // Check if it was for closing , and also read the
        // incoming message.
        int ret = recv(itr->id, buffer, 65536, 0);
        if (ret == 0) {
#if (ACE_BUILD_CONF & ACE_BUILD_WITH_DBG_IO)
          printf("ace::server-socket : peer %s disconnected.\n",
                 itr->name().c_str());
#endif
          on_disconnect(*itr);
          itr->shutdown();
          itr->close();
          this->clients.erase(itr);
        } else if (ret > 0) {
#if (ACE_BUILD_CONF & ACE_BUILD_WITH_DBG_IO)
          printf("ace::server-socket : received %d bytes from client %s.\n",
                 ret, itr->name().c_str());
#endif
          this->messages.push({*this, *itr, ByteBuffer(buffer, buffer + ret)});
        } else {
          int err = WSAGetLastError();
          if (err != WSAESHUTDOWN) {
#if (ACE_BUILD_CONF & ACE_BUILD_WITH_IO)
            printf("%s/recv failed : %s\n", __FUNCTION__, gai_strerrorA(err));
#endif
          }
#if (ACE_BUILD_CONF & ACE_BUILD_WITH_DBG_IO)
          printf("ace::server-socket : connection with %s closed.\n",
                 itr->name().c_str());
#endif
          on_disconnect(*itr);
          itr->close();
          this->clients.erase(itr);
          // DO NOT RETURN HERE THERE ARE MORE CLIENTS TO SERVE.
        }
      }
    }
    itr = next;
  }

  free(buffer);
  return ACE_SOCK_OK;
}

int ServerSocket::disconnect(Socket &client) {
  on_disconnect(client);
  ACE_SOCK_ASSERT_FUNC(client.shutdown());
  return ACE_SOCK_OK;
}

bool ServerSocket::has_messages() const { return this->messages.size(); }

ServerSocket::Message ServerSocket::peek_message() const {
  return this->messages.front();
}
ServerSocket::Message ServerSocket::get_message() {
  ServerSocket::Message msg = this->messages.front();
  this->messages.pop();
  return msg;
}

} // namespace ace
