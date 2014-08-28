//
// A demo program to test the skeleton of K/V service
//

#include "kv_protocol.h"
#include "kv_client.h"
#include "rpc.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>

std::string dst;
kv_client *kvc;

int
main(int argc, char *argv[])
{
  int r;

  if(argc != 2){
    fprintf(stderr, "Usage: %s [host:]port\n", argv[0]);
    exit(1);
  }

  dst = argv[1];
  kvc = new kv_client(dst);

  std::string msg;
  r = kvc->stat(msg);
  printf("RPC %s, stat: %s\n", r?"failure":"success", msg.c_str());
}
