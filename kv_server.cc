// the extent server implementation

#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


#include "lang/verify.h"
#include "kv_server.h"
using namespace std;

kv_server::kv_server() 
{
	pthread_mutex_init(&map_locker, NULL);
}

kv_server::~kv_server()
{
	pthread_mutex_destroy(&map_locker);
}

/* The RPC reply argument "val" should contain 
 * the retrieved val together with its current version 
 */
int 
kv_server::get(std::string key, kv_protocol::versioned_val &val)
{
	// You fill this in for Lab 1.
	pthread_mutex_lock(&map_locker);
	map<string, versioned_val>::iterator it = server_map.find(key);
	if(it != server_map.end()) {
		if(it->second.delete_flag) {
			pthread_mutex_unlock(&map_locker);
			return kv_protocol::NOEXIST;
		} else {
			val.buf = it->second.buf;
			val.version = it->second.version;
			pthread_mutex_unlock(&map_locker);
			return kv_protocol::OK;
		}

	} else {
		pthread_mutex_unlock(&map_locker);
		return kv_protocol::NOEXIST;
	}

	//return kv_protocol::IOERR;
}


/* the server should store the key-value entry, increment its version if appropriate.
   and put the new version of the stored entry in the RPC reply (i.e. the last argument)
*/
int 
kv_server::put(std::string key, std::string buf, int &new_version)
{
	// You fill this in for Lab 1.
	pthread_mutex_lock(&map_locker);
	map<string, versioned_val>::iterator it = server_map.find(key);
	if(it != server_map.end()) {
		it->second.version++;
		it->second.buf = buf;
		it->second.delete_flag = false;
		new_version = it->second.version;
		pthread_mutex_unlock(&map_locker);
        return kv_protocol::OK;
	} else {
		versioned_val temp;
		temp.buf = buf;
		temp.version = 1;
		temp.delete_flag = false;
		server_map.insert(pair<string,versioned_val>(key,temp));
		new_version = 1;
		pthread_mutex_unlock(&map_locker);
		return kv_protocol::OK;
	}

	//return kv_protocol::IOERR;
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
	pthread_mutex_lock(&map_locker);
	map<string, versioned_val>::iterator it = server_map.find(key);
	if (it != server_map.end()) {
		it->second.version++;
		if (it->second.delete_flag) {
			pthread_mutex_unlock(&map_locker);
			return kv_protocol::NOEXIST;
		} else {
			it->second.delete_flag = true;
			new_version = it->second.version;
			pthread_mutex_unlock(&map_locker);
			return kv_protocol::OK;
		}
	} else {
		versioned_val temp;
		temp.version = 1;
		temp.delete_flag = true;
		new_version = it->second.version;
		server_map.insert(pair<string,versioned_val>(key,temp));
		pthread_mutex_unlock(&map_locker);
		return kv_protocol::NOEXIST;
	}
	//return kv_protocol::IOERR;
}

int 
kv_server::stat(int x, std::string &msg)
{
	msg = "Server says: I am kicking";	
	return kv_protocol::OK;
}
