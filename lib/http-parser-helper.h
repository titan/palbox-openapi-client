#include <core.h>
#include <carp_memory.h>
#include "picohttpparser.c"

typedef struct {
  String name;
  String value;
} HttpHeader;

typedef struct {
  String method;
  String path;
  int version;
  Array headers;
} HttpRequest;

typedef struct {
  int minor_version;
  int status;
  String msg;
  Array headers;
} HttpResponse;

String HttpHeader_prn(HttpHeader x) {
  int size = snprintf(NULL, 0, "HttpHeader(name=%s, value=%s)", x.name, x.value) + 1;
  String buffer = CARP_MALLOC(size);
  snprintf(buffer, size, "HttpHeader(name=%s, value=%s)", x.name, x.value);
  return buffer;
}

HttpRequest HttpParser_parse_MINUS_request(Array * buf) {
  HttpRequest request;
  return request;
}

HttpResponse HttpParser_parse_MINUS_response(Array * buf) {
  int minor_version = 0;
  int status = 0;
  const char msgbuf[1024];
  const char * msg = msgbuf;
  size_t msg_len = 0;
  struct phr_header headers[16];
  size_t num_headers = 0;
  int last_len = 0;
  HttpResponse response;

  memset((void *)msgbuf, 0, sizeof(char) * 1024);
  memset(headers, 0, sizeof(struct phr_header) * 16);
  memset(&response, 0, sizeof(HttpResponse));

  if (phr_parse_response(buf->data, buf->len, &response.minor_version, &response.status, &msg, &msg_len, headers, &num_headers, last_len) != -1) {
    if (msg_len != 0) {
      response.msg = CARP_MALLOC(msg_len + 1);
      memset(response.msg, 0, msg_len + 1);
      memcpy(response.msg, msg, msg_len);
    }
    if (num_headers > 0) {
      response.headers.len = num_headers;
      response.headers.capacity = num_headers;
      for (int i = 0; i < num_headers; i ++) {
        ((HttpHeader **)response.headers.data)[i] = (HttpHeader *)CARP_MALLOC(sizeof(HttpHeader));
        ((HttpHeader **)response.headers.data)[i]->name = CARP_MALLOC(headers[i].name_len + 1);
        memcpy(((HttpHeader **)response.headers.data)[i]->name, headers[i].name, headers[i].name_len);
        ((HttpHeader **)response.headers.data)[i]->name[headers[i].name_len] = 0;
        ((HttpHeader **)response.headers.data)[i]->value = CARP_MALLOC(headers[i].value_len + 1);
        memcpy(((HttpHeader **)response.headers.data)[i]->value, headers[i].value, headers[i].value_len);
        ((HttpHeader **)response.headers.data)[i]->value[headers[i].value_len] = 0;
      }
    }
  }
  return response;
}
