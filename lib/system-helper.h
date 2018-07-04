#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <core.h>
#include <carp_memory.h>

int System_getpid() {
  return getpid();
}

Array System_arguments() {
  pid_t pid = getpid();
  char filename[1024];
  memset(filename, 0, 1024);
  sprintf(filename, "/proc/%d/cmdline", pid);
  int fd = open(filename, O_RDONLY);
  char buf[1024];
  int r, total = 0;
  while ((r = read(fd, buf, 1024)) > 0) {
    total += r;
  }
  Array result;
  memset(&result, 0, sizeof(Array));
  if (total > 0) {
    for (int i = 0, j = 1; j < total; i ++, j ++) {
      if (buf[i] != 0 && buf[j] == 0) {
        result.len ++;
      } else if (buf[j] == 0 && buf[i] == 0) {
        break;
      }
    }
    result.capacity = result.len;
    result.data = CARP_MALLOC(sizeof(char *) * result.len);

    int k = 0;
    for (int i = 0, j = 0; i < total && k < result.len; i ++) {
      if (buf[i] == 0) {
        char * arg = CARP_MALLOC(i - j + 1);
        memcpy(arg, (void *)buf + j, (i - j + 1));
        *(((String *)result.data) + (k ++)) = arg;
        j = i + 1;
      }
    }
  }
  return result;
}

String System_strftime(String * fmt, long time) {
  struct tm t;
  gmtime_r(&time, &t);
  String result = CARP_MALLOC(1024);
  memset(result, 0, 1024);
  strftime(result, 1023, *fmt, &t);
  return result;
}
