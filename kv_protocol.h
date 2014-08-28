// extent wire protocol

#ifndef kv_protocl_h
#define kv_protocl_h

#include "rpc.h"

class kv_protocol {
 public:
  typedef struct {
	  int version;
	  std::string buf;
  } versioned_val;
  
  enum xxstatus { OK, NOEXIST, VERSION_MISMATCH, IOERR };
  enum rpc_numbers {
    put = 0x6001,
    get,
    remove,
	stat
  };
};

inline unmarshall &
operator>>(unmarshall &u, kv_protocol::versioned_val &a)
{
  u >> a.version;
  u >> a.buf;
  return u;
}

inline marshall &
operator<<(marshall &m, kv_protocol::versioned_val a)
{
  m << a.version;
  m << a.buf;
  return m;
}


#endif 
