// the extent server implementation

#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include "lang/verify.h"
#include "kv_server.h"


kv_server::kv_server() 
{
}

/* The RPC reply argument "val" should contain 
 * the retrieved val together with its current version 
 */
int 
kv_server::get(std::string key, kv_protocol::versioned_val &val)
{
	// You fill this in for Lab 1.
	return kv_protocol::IOERR;
}


/* the server should store the key-value entry, increment its version if appropriate.
   and put the new version of the stored entry in the RPC reply (i.e. the last argument)
*/
int 
kv_server::put(std::string key, std::string buf, int &new_version)
{
	// You fill this in for Lab 1.
	return kv_protocol::IOERR;
}

/* "remove" the existing key-value entry
 * do not actually delete it from your storage
 * treat it as a special type of put and 
 * increments the key-value pair's version number
 * like regular puts.
 * Set a delete flag to the "removed" key-value 
 * entry so a later get does not see it.
*/
int 
kv_server::remove(std::string key, int &new_version)
{
	// You fill this in for Lab 1.
	return kv_protocol::IOERR;
}

int 
kv_server::stat(int x, std::string &msg)
{
	msg = "Server says: I am kicking";	
	return kv_protocol::OK;
}

std::string
kv_server::marshal_state()
{
	//Lab3: You fill this function to marshal the state of the kv_server
	//Hint: you may want to use ostringstream to marshal your state
	std::ostringstream ost;
	return ost.str();
}

void
kv_server::unmarshal_state(std::string state)
{
	//Lab3: You fill this function to unmarshal the transferred state into kv_server
	//Hint: use istringstream to extract stuff out of the state string
  	std::istringstream ist(state);
}
