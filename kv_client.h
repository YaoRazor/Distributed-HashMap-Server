// key-value client interface.
#ifndef kv_client_h
#define kv_client_h

#include <string>
#include "kv_protocol.h"
#include "rpc.h"

class kv_client {
 private:
  rpcc *cl;

 public:
  kv_client(std::string dst);

  int get(std::string key, std::string &buf, int *version = NULL);
  int put(std::string key, const std::string &buf, int *version = NULL);
  int remove(std::string key, int *version = NULL);
  int stat(std::string &);
};

#endif 

