#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "core.h"
#include "carp_memory.h"

int Socket_buf_MINUS_size = 1024;

typedef struct {
  int socket;
  struct sockaddr_in them;
} Socket;

Socket Socket_setup_MINUS_client(String * node, int port) {
  Socket ret;
  struct addrinfo hints, * addr;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  int port_len = snprintf(NULL, 0, "%d", port) + 1;
  char port_buf[port_len];
  snprintf(port_buf, port_len, "%d", port);
  port_buf[port_len - 1] = 0;
  int result = getaddrinfo(* node, port_buf, &hints, &addr);
  if (result != 0) {
    printf("%s\n", gai_strerror(result));
    exit(result);
  }

  ret.socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
  if (ret.socket == -1) {
    perror("socket");
    exit(-1);
  }
  if (connect(ret.socket, addr->ai_addr, addr->ai_addrlen) == -1) {
    freeaddrinfo(addr);
    printf("%s\n", strerror(errno));
    exit(-1);
  }
  freeaddrinfo(addr);

  return ret;
}

void Socket_send(Socket * sock, String * msg) {
  send(sock->socket, *msg, strlen(*msg), 0);
}

String Socket_read(Socket * sock) {
  String buf = CARP_MALLOC(Socket_buf_MINUS_size);
  memset(buf, 0, Socket_buf_MINUS_size);
  int val = read(sock->socket, buf, Socket_buf_MINUS_size);
  return buf;
}

Socket Socket_setup_MINUS_server(String * node, int port) {
  Socket ret;
  int opt = 1;
  struct addrinfo hints, * addr;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  int port_len = snprintf(NULL, 0, "%d", port) + 1;
  char port_buf[port_len];
  snprintf(port_buf, port_len, "%d", port);
  port_buf[port_len - 1] = 0;
  int result = getaddrinfo(* node, port_buf, &hints, &addr);
  if (result != 0) {
    printf("%s\n", gai_strerror(result));
    exit(result);
  }

  ret.socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
  setsockopt(ret.socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
  if (ret.socket == -1) {
    perror("socket");
    exit(-1);
  }
  if (bind(ret.socket, addr->ai_addr, addr->ai_addrlen) == -1) {
    freeaddrinfo(addr);
    printf("%s\n", strerror(errno));
    exit(-1);
  }
  freeaddrinfo(addr);

  return ret;
}

void Socket_listen(Socket * sock) {
  listen(sock->socket, 3);
}

Socket Socket_accept(Socket * sock) {
  Socket ret;
  int size = sizeof(ret.them);
  ret.socket = accept(sock->socket, (struct sockaddr *)&sock->them, (socklen_t*)&size);
  return ret;
}

void Socket_close(Socket * sock) {
  close(sock->socket);
}
