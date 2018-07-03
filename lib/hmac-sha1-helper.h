#include <string.h>
#include <core.h>
#include <carp_memory.h>
#include <carp_string.h>
#include <openssl/hmac.h>

static char encode_hex(unsigned char data) {
  if (data < 10) {
    return data + 0x30;
  } else if (data < 16) {
    return data + 0x57;
  } else {
    return ' ';
  }
}

String HmacSha1_digest(String * key, String * data) {
  unsigned char md[HMAC_MAX_MD_CBLOCK];
  unsigned int len = 0;
  memset(md, 0, HMAC_MAX_MD_CBLOCK);
  HMAC(EVP_sha1(), *key, strlen(*key), (unsigned char *)(*data), strlen(*data), md, &len);
  String result = CARP_MALLOC(len * 2 + 1);
  memset(result, 0, len * 2 + 1);
  for (int i = 0; i < len; i ++) {
    unsigned char msb = ((md[i] >> 4) & 0x0F);
    unsigned char lsb = ((md[i] >> 0) & 0x0F);
    result[i * 2] = encode_hex(msb);
    result[i * 2 + 1] = encode_hex(lsb);
  }
  return result;
}
