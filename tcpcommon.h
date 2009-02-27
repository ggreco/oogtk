#ifndef TCP_COMMON_H
#define TCP_COMMON_H
#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#define ERRNO errno
#define closesocket(x) close(x)
#define SOCKET_ERROR -1
#define STRERROR strerror
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#else
#include <winsock2.h>
#define EWOULDBLOCK WSAEWOULDBLOCK
#define ECONNABORTED WSAECONNABORTED
#define O_NONBLOCK 1234657
#define ECONNRESET   WSAECONNRESET
#define MSG_NOSIGNAL 0
#define ERRNO WSAGetLastError()
#define STRERROR tor_socket_strerror
typedef unsigned long in_addr_t;
const char *tor_socket_strerror(int);
#endif

#endif
