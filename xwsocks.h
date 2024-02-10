#ifndef XWSOCKS_H
#define XWSOCKS_H

/*
  xwSocks -- Cross platform Wrapper for sockets
  % Implements a wrapper for sockets function making different platform behave the same
  % Unix Sockets that is arguments, and -1 on error
  % Wrapper types for compatibility
  % Just one function outside standard
  
  Types:
    xwSocket -> SOCKET on Windows | int on Unix compilant
    xwSocklen -> int on WIndows | socklen_t on Unix compilant
    xwSockaddr -> windows(struct sockaddr) | unix(struct sockaddr)
    xwSockaddr_in -> windows(struct sockaddr_in) | unix(struct sockaddr_in) 

  Functions:
    int xwSock_init(void) -> Should be called once when the program starts

    xwSocket xwSocks_socket(int domain, int type, int protocol)
  int xwSocks_setsockopt(xwSocket socket, int level, int option_name, const void *option_value, xwSocklen option_length)
  int xwSocks_listen(xwSocket socket, int backlog)
  int xwSocks_bind(xwSocket socket, xwSockaddr *addr, xwSocklen addr_len)
  xwSocket xwSocks_accept(xwSocket socket, xwSockaddr *addr, xwSocklen *addr_len)
  int xwSocks_recv(xwSocket socket, char *buff, size_t len, int flags)
  int xwSocks_recvfrom(xwSocket socket, char *buff, size_t len, int flags, xwSockaddr *addr, xwSocklen *addr_len)
  int xwSocks_send(xwSocket socket, char *buff, size_t len, int flags)
  int xwSocks_sendto(xwSocket socket, char *buff, size_t len, int flags, xwSockaddr *addr, xwSocklen addr_len)
  int xwSocks_close(xwSocket socket)
*/

// Set Underlying System Arch
#if defined(__unix__) && !defined(USYS_WINDOWS)
  #define USYS_UNIX
#elif defined(_WIN32) && !defined(USYS_UNIX)
  #define USYS_WINDOWS
#else
  #error "Couldn't classify your system into UNIX or WINDOWS compilant"
#endif

// Include headers according to system
#if defined(USYS_UNIX)
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#elif defined(USYS_WINDOWS)
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Types according to underlying system
#if defined(USYS_UNIX)
typedef int xwSocket;
typedef socklen_t xwSocklen;
typedef struct sockaddr xwSockaddr;
typedef struct sockaddr_in xwSockaddr_in;

#elif defined(USYS_WINDOWS)
typedef SOCKET xwSocket;
typedef int xwSocklen;
typedef struct sockaddr xwSockaddr;
typedef struct sockaddr_in xwSockaddr_in;

#endif

int xwSocks_init(void);

xwSocket xwSocks_socket(int domain, int type, int protocol);
int xwSocks_setsockopt(xwSocket socket, int level, int option_name, const void *option_value, xwSocklen option_length);
int xwSocks_listen(xwSocket socket, int backlog);
int xwSocks_bind(xwSocket socket, xwSockaddr *addr, xwSocklen addr_len);
xwSocket xwSocks_accept(xwSocket socket, xwSockaddr *addr, xwSocklen *addr_len);
int xwSocks_recv(xwSocket socket, char *buff, size_t len, int flags);
int xwSocks_recvfrom(xwSocket socket, char *buff, size_t len, int flags, xwSockaddr *addr, xwSocklen *addr_len);
int xwSocks_send(xwSocket socket, char *buff, size_t len, int flags);
int xwSocks_sendto(xwSocket socket, char *buff, size_t len, int flags, xwSockaddr *addr, xwSocklen addr_len);
int xwSocks_close(xwSocket socket);

#ifdef XWSOCKS_IMPLEMENTATION

int xwSocks_init(void)
{
#if defined(USYS_UNIX)
  return 0;
#elif defined(USYS_WINDOWS)
  WSADATA wsad;
  if (WSAStartup(MAKEWORD(2, 2), &wsad) != 0) return -1;
  return 0;
#endif
}

xwSocket xwSocks_socket(int domain, int type, int protocol)
{
#if defined(USYS_UNIX)
  return socket(domain, type, protocol);
#elif defined(USYS_WINDOWS)
  xwSocket s = socket(domain, type, protocol);
  if (s == INVALID_SOCKET) return -1;
  return s;
#endif
}

int xwSocks_setsockopt(xwSocket socket, int level, int option_name, const void *option_value, xwSocklen option_length)
{
#if defined(USYS_UNIX)
  return setsockopt(socket, level, option_name, option_value, option_length);
#elif defined(USYS_WINDOWS)
  int ret = setsockopt(socket, level, option_name, (const char*)option_value, option_length);
  if (ret == WSANOTINITIALISED || ret == WSAENETDOWN || ret == WSAEFAULT
    || ret == WSAEINPROGRESS || ret == WSAEINVAL || ret == WSAENETRESET 
    || ret == WSAENOPROTOOPT || ret == WSAENOTCONN || ret == WSAENOTSOCK
  ) return -1;
  return 0;
#endif
}

int xwSocks_listen(xwSocket socket, int backlog)
{
#if defined(USYS_UNIX)
  return listen(socket, backlog);
#elif defined(USYS_WINDOWS)
  int ret = listen(socket, backlog);
  if (ret == WSANOTINITIALISED || ret == WSAENETDOWN || ret == WSAEADDRINUSE 
    || ret == WSAEINPROGRESS || ret == WSAEINVAL || ret == WSAEISCONN 
    || ret == WSAEMFILE || ret == WSAENOBUFS || ret == WSAENOTSOCK 
    || ret == WSAEOPNOTSUPP
  ) return -1;
  return 0;
#endif  
}

int xwSocks_bind(xwSocket socket, xwSockaddr *addr, xwSocklen addr_len)
{
#if defined(USYS_UNIX)
  return bind(socket, addr, addr_len);
#elif defined(USYS_WINDOWS)
  int ret = bind(socket, addr, addr_len);
  if (ret == WSANOTINITIALISED || ret == WSAENETDOWN || ret == WSAEACCES
    || ret == WSAEADDRINUSE || ret == WSAEADDRNOTAVAIL || ret == WSAEFAULT
    || ret == WSAEINPROGRESS || ret == WSAEINVAL || ret == WSAENOBUFS 
    || ret == WSAENOTSOCK
  ) return -1;
  return 0;
#endif
}

xwSocket xwSocks_accept(xwSocket socket, xwSockaddr *addr, xwSocklen *addr_len)
{
#if defined(USYS_UNIX)
  return accept(socket, addr, addr_len);
#elif defined(USYS_WINDOWS)
  int ret = accept(socket, addr, addr_len);
  if (ret == WSANOTINITIALISED || ret == WSAECONNRESET || ret == WSAEFAULT
    || ret == WSAEINTR || ret == WSAEINVAL || ret == WSAEINPROGRESS
    || ret == WSAEMFILE || ret == WSAENETDOWN || ret == WSAENOBUFS 
    || ret == WSAENOTSOCK || ret == WSAEOPNOTSUPP || ret == WSAEWOULDBLOCK
  ) return -1;
  return ret;
#endif
}

int xwSocks_recv(xwSocket socket, char *buff, size_t len, int flags)
{
#if defined(USYS_UNIX)
  return recv(socket, buff, len, flags);
#elif defined(USYS_WINDOWS)
  int ret = recv(socket, buff, len, flags);
  if (ret == WSANOTINITIALISED || ret == WSAENETDOWN || ret == WSAEFAULT
    || ret == WSAENOTCONN || ret == WSAEINTR || ret == WSAEINPROGRESS
    || ret == WSAENOTCONN || ret == WSAEINTR || ret == WSAEINPROGRESS 
    || ret == WSAENETRESET || ret == WSAENOTSOCK || ret == WSAEOPNOTSUPP
    || ret == WSAESHUTDOWN || ret == WSAEWOULDBLOCK || ret == WSAEMSGSIZE
    || ret == WSAEINVAL || ret == WSAECONNABORTED || ret == WSAETIMEDOUT
    || ret == WSAECONNRESET
  ) return -1;
  return 0;
#endif
}

int xwSocks_recvfrom(xwSocket socket, char *buff, size_t len, int flags, xwSockaddr *addr, xwSocklen *addr_len)
{
#if defined(USYS_UNIX)
  return recvfrom(socket, buff, len, flags, addr, addr_len);
#elif defined(USYS_WINDOWS)
  int ret = recvfrom(socket, buff, len, flags, addr, addr_len);
  if (ret == WSANOTINITIALISED || ret == WSAENETDOWN || ret == WSAEFAULT
    || ret == WSAEINTR || ret == WSAEINPROGRESS || ret == WSAEINVAL
    || ret == WSAEISCONN || ret == WSAENETRESET || ret == WSAENOTSOCK 
    || ret == WSAEOPNOTSUPP || ret == WSAESHUTDOWN || ret == WSAEWOULDBLOCK
    || ret == WSAEMSGSIZE || ret == WSAETIMEDOUT || ret == WSAECONNRESET
  ) return -1;
  return 0;
#endif
}

int xwSocks_send(xwSocket socket, char *buff, size_t len, int flags)
{
#if defined(USYS_UNIX)
  return send(socket, buff, len, flags);
#elif defined(USYS_WINDOWS)
  int ret = send(socket, buff, len, flags);
  if (ret == WSANOTINITIALISED || ret == WSAENETDOWN || ret == WSAEACCES
    || ret == WSAEINTR || ret == WSAEINPROGRESS || ret == WSAEFAULT
    || ret == WSAENETRESET || ret == WSAENOBUFS || ret == WSAENOTCONN 
    || ret == WSAENOTSOCK || ret == WSAEOPNOTSUPP || ret == WSAESHUTDOWN
    || ret == WSAEWOULDBLOCK || ret == WSAEMSGSIZE || ret == WSAEHOSTUNREACH
    || ret == WSAEINVAL || ret == WSAECONNABORTED || ret == WSAECONNRESET
    || ret == WSAETIMEDOUT
  ) return -1;
  return 0;
#endif
}

int xwSocks_sendto(xwSocket socket, char *buff, size_t len, int flags, xwSockaddr *addr, xwSocklen addr_len)
{
#if defined(USYS_UNIX)
  return sendto(socket, buff, len, flags, addr, addr_len);
#elif defined(USYS_WINDOWS)
  int ret = sendto(socket, buff, len, flags, addr, addr_len);
  if (ret == WSANOTINITIALISED || ret == WSAENETDOWN || ret == WSAEACCES
    || ret == WSAEINVAL || ret == WSAEINTR || ret == WSAEINPROGRESS
    || ret == WSAEFAULT || ret == WSAENETRESET || ret == WSAENOBUFS 
    || ret == WSAENOTCONN || ret == WSAENOTSOCK || ret == WSAEOPNOTSUPP
    || ret == WSAESHUTDOWN || ret == WSAEWOULDBLOCK || ret == WSAEMSGSIZE
    || ret == WSAEHOSTUNREACH || ret == WSAECONNABORTED || ret == WSAECONNRESET
    || ret == WSAEADDRNOTAVAIL || ret == WSAEAFNOSUPPORT || ret == WSAEDESTADDRREQ
    || ret == WSAENETUNREACH || ret == WSAETIMEDOUT
  ) return -1;
  return 0;
#endif
}

int xwSocks_close(xwSocket socket)
{
#if defined(USYS_UNIX)
  return close(socket);
#elif defined(USYS_WINDOWS)
  int ret = closesocket(socket);
  if (ret == WSANOTINITIALISED || ret == WSAENETDOWN || ret == WSAENOTSOCK
    || ret == WSAEINPROGRESS || ret == WSAEINTR || ret == WSAEWOULDBLOCK
  ) return -1;
  return 0;
#endif
}

#endif // XWSOCKS_IMPLEMENTATION
#endif // XWSOCKS_H