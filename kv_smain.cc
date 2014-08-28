#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "rpc/jsl_log.h"

#include "kv_server.h"

int
main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	srandom(getpid());

	//uncomment the following line to print a lot of debug info
	//jsl_set_debug(JSL_DBG_4);

	if(argc != 2){
		fprintf(stderr, "Usage: %s port\n", argv[0]);
		exit(1);
	}


#ifndef RSM
	kv_server kvs;
	rpcs server(atoi(argv[1]));
	server.reg(kv_protocol::stat, &kvs, &kv_server::stat);
#endif


	while(1)
		sleep(1000);
}
