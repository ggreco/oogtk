#ifndef TCP_COMMON_H
#define TCP_COMMON_H
#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ioctl.h>
#define ERRNO errno
#define closesocket(x) close(x)
#define SOCKET_ERROR -1
#define STRERROR strerror
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
typedef size_t len_t;

#define initialize_tcpip() 
#define msleep(x) usleep(x*1000)

#else
#include <winsock2.h>
#include <sys/time.h>
#ifndef EINPROGRESS
#define EINPROGRESS WSAEWOULDBLOCK
#endif
#ifndef EWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOCK
#endif
#define ECONNABORTED WSAECONNABORTED
#define EALREADY WSAEALREADY
#define EISCONN WSAEISCONN
#define O_NONBLOCK 1234657
#define ECONNRESET   WSAECONNRESET
#define MSG_NOSIGNAL 0
#define ERRNO WSAGetLastError()
#define ioctl(x,y,z) ioctlsocket(x,y,(u_long*)(z))
#define STRERROR tor_socket_strerror
#define msleep(x) Sleep(x)

#define initialize_tcpip() do {  WSADATA wsaData; \
        if (WSAStartup(MAKEWORD( 2, 0 ), &wsaData) != 0) { \
            OutputDebugString("Impossibile inizializzare winsock!"); \
            exit(0); \
        } \
        } while (0)


typedef int len_t;
typedef unsigned long in_addr_t;
const char *tor_socket_strerror(int);
#define inet_ntop(a, ip, ipstr, len) do { strncpy(ipstr, inet_ntoa(*ip), len); } while(0)
#ifdef NEED_GETTIMEOFDAY
extern "C" int gettimeofday(struct timeval *, void *);
#endif
#define localtime_r(x,y) localtime(x)
#endif

#endif
