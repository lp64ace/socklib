#include "clientsock.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace ace {

ClientSocket::ClientSocket() : Socket() {}

ClientSocket::~ClientSocket() {
  if (this->valid()) {
    close();
  }
}

int ClientSocket::connect(const char *ip, int port, addr_family family) {
  ACE_SOCK_ASSERT_FUNC(this->connect_tcp(ip, port, family));
#if (ACE_BUILD_CONF & ACE_BUILD_WITH_DBG_IO)
  printf("ace::client-socket : connected to %s.\n", this->name().c_str());
#endif
  return ACE_SOCK_OK;
}

int ClientSocket::poll(int timeout) {
  if (!valid()) {
    return ACE_SOCK_DISCONNECTED;
  }

  // set of socket descriptors.
  fd_set readfds;

  // clear the socket set.
  FD_ZERO(&readfds);

  // add master socket to set.
  FD_SET(id, &readfds);

  timeval select_timeout;
  select_timeout.tv_sec = timeout / 1000;
  select_timeout.tv_usec = (timeout % 1000) * 1000;
  int activity = select(id + 1, &readfds, NULL, NULL, &select_timeout);

  if (activity < 0) {
#if (ACE_BUILD_CONF & ACE_BUILD_WITH_IO)
    printf("%s/select failed : %s\n", __FUNCTION__,
           gai_strerrorA(WSAGetLastError()));
#endif
    return ACE_SOCK_SEL_FAILED;
  }

  int ret;

  char *buffer = (char *)malloc(65536);

  if (FD_ISSET(id, &readfds)) {
    // Check if it was for closing , and also read the
    // incoming message.
    int ret = recv(id, buffer, 65536, 0);
    if (ret == 0) {
#if (ACE_BUILD_CONF & ACE_BUILD_WITH_DBG_IO)
      printf("ace::client-socket : peer %s disconnected.\n",
             this->name().c_str());
#endif
      this->shutdown();
      this->close();

      free(buffer);

      return ACE_SOCK_DISCONNECTED;
    } else if (ret > 0) {
#if (ACE_BUILD_CONF & ACE_BUILD_WITH_DBG_IO)
      printf("ace::client-socket : received %d bytes from client %s.\n", ret,
             this->name().c_str());
#endif
      this->messages.push(
          ByteBuffer((unsigned char *)buffer, (unsigned char *)buffer + ret));
    } else {
      int err = WSAGetLastError();
      if (err != WSAESHUTDOWN) {
#if (ACE_BUILD_CONF & ACE_BUILD_WITH_IO)
        printf("%s/recv failed : %s\n", __FUNCTION__, gai_strerrorA(err));
#endif
      }
#if (ACE_BUILD_CONF & ACE_BUILD_WITH_DBG_IO)
      printf("ace::client-socket : connection with %s closed.\n",
             this->name().c_str());
#endif
      this->close();

      free(buffer);

      return ACE_SOCK_RECV_FAILED;
    }
  }

  free(buffer);

  return ACE_SOCK_OK;
}

int ClientSocket::disconnect() const {
  ACE_SOCK_ASSERT_FUNC(shutdown());
  return ACE_SOCK_OK;
}

bool ClientSocket::has_messages ( ) const { return this->messages.size(); }

ClientSocket::Message ClientSocket::peek_message() const {
  return this->messages.front();
}
ClientSocket::Message ClientSocket::get_message ( ) {
  ClientSocket::Message msg = this->messages.front();
  this->messages.pop();
  return msg;
}

} // namespace ace
