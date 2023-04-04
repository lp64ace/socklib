#include "socklib.h"

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace ace {

static int g_SockStatus = ACE_SOCK_NOT_INIT;

int SockInit(int major, int minor) {
  int ret;

  WSADATA wsadata;
  ret = WSAStartup(MAKEWORD(major, minor), &wsadata);
  if (ret != 0) {
#if (ACE_BUILD_CONF & ACE_BUILD_WITH_IO)
    printf("SockInit failed : ");
    int err = WSAGetLastError();
    switch (err) {
    case WSASYSNOTREADY:
      printf("The underlying network subsystem is not ready for network "
             "communication.\n");
      break;
    case WSAVERNOTSUPPORTED:
      printf("The version of Windows Sockets support requested is not provided "
             "by this particular Windows Sockets implementation.\n");
      break;
    case WSAEINPROGRESS:
      printf("A blocking Windows Sockets 1.1 operation is in progress.\n");
      break;
    case WSAEPROCLIM:
      printf("A limit on the number of tasks supported by the Windows Sockets "
             "implementation has been reached.\n");
      break;
    default:
      printf("Unknown error.\n");
      break;
    }
#endif
    return ACE_SOCK_INIT_FAILED;
  }
  g_SockStatus = ACE_SOCK_OK;
  return ACE_SOCK_OK;
}

int SockQuit() {
  int ret;

  ret = WSACleanup();
  if (ret != 0) {
#if (ACE_BUILD_CONF & ACE_BUILD_WITH_IO)
    printf("SockQuit failed : ");
    int err = WSAGetLastError();
    switch (err) {
    case WSANOTINITIALISED:
      printf("A successful SockInit call must occur before using this "
             "function.\n");
      break;
    case WSAENETDOWN:
      printf("The version of Windows Sockets support requested is not provided "
             "by this particular Windows Sockets implementation.\n");
      break;
    case WSAEINPROGRESS:
      printf("A blocking Windows Sockets 1.1 operation is in progress.\n");
      break;
    default:
      printf("Unknown error.\n");
      break;
    }
#endif
    return ACE_SOCK_QUIT_FAILED;
  }
  return ACE_SOCK_OK;
}

int SockStatus() { return g_SockStatus; }

} // namespace ace
