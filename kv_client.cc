// RPC stubs for clients to talk to kv_server

#include "kv_client.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <time.h>


kv_client::kv_client(std::string dst)
{
  sockaddr_in dstsock;
  make_sockaddr(dst.c_str(), &dstsock);
  cl = new rpcc(dstsock);
  if (cl->bind() != 0) {
    printf("kv_client: bind failed\n");
  }
}

int
kv_client::get(std::string key, std::string &buf, int *version)
{
	kv_protocol::versioned_val val;
	int ret = cl->call(kv_protocol::get, key, val);
	buf = val.buf;
	if (version)
		*version = val.version;
	return ret;
}

int
kv_client::put(std::string key, const std::string & buf, int *version)
{
  int new_version; 
  int ret = cl->call(kv_protocol::put, key, buf, new_version);
  if (version)
	  *version = new_version;
  return ret;
}

int
kv_client::remove(std::string key, int *version)
{
  int new_version;
  int ret = cl->call(kv_protocol::remove, key, new_version);
  if (version)
	  *version = new_version;
  return ret;
}

int
kv_client::stat(std::string &msg)
{
  int r; //un-used rpc argument
  return cl->call(kv_protocol::stat, r, msg);
}
