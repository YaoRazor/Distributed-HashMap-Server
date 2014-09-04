#include <stdlib.h>
#include <stdio.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#include "kv_protocol.h"
#include "kv_client.h"
#include "rpc.h"
#include "slock.h"
#include "lang/verify.h"
#include "rpc/jsl_log.h"

#define NTHREADS 10
#define NREPEAT 100

int current_test = 0;
kv_client *kvc[NTHREADS];

std::string key1;
std::string key2;

void
verify_version(int lineno, int version, int expected)
{
	if (version != expected) {
		printf("\t line-%d: ERROR: incorrect version, should be %d instead of %d\n", lineno, expected, version);
		exit(1);
	}
}

void
verify_value(int lineno, std::string val, std::string expected)
{
	if (val!= expected) {
		printf("\t line-%d: ERROR: incorrect value, should be %s instead of %s\n", lineno, expected.c_str(), val.c_str());
		exit(1);
	}
}
void
verify_ret(int lineno, int ret, int expected)
{
	if (ret != expected) {
		printf("\t line-%d: ERROR: incorrect return status, should be %d instead of %d\n", lineno, expected, ret);
		exit(1);
	}
}

void
test1(void)
{
	int ver;
	std::string buf;

    printf ("\t put get remove get put put put get remove \n");
    verify_ret(__LINE__, kvc[0]->put(key1, "a1", &ver), 0);
	verify_version(__LINE__,ver, 1);

	verify_ret(__LINE__, kvc[0]->get(key1, buf, &ver), 0);
    verify_version(__LINE__, ver, 1);
	verify_value(__LINE__, buf, "a1");


	verify_ret(__LINE__, kvc[0]->remove(key1, &ver), 0);
	verify_ret(__LINE__, kvc[0]->get(key1, buf, &ver), kv_protocol::NOEXIST);

	verify_ret(__LINE__, kvc[0]->put(key1, "a1", &ver), 0);
	verify_version(__LINE__, ver, 3);

	verify_ret(__LINE__, kvc[0]->put(key1, "a2", &ver), 0);
	verify_version(__LINE__, ver, 4);

	verify_ret(__LINE__, kvc[0]->put(key1, "a3", &ver), 0);
	verify_version(__LINE__, ver, 5);

	verify_ret(__LINE__, kvc[0]->get(key1, buf, &ver), 0);
    verify_version(__LINE__, ver, 5);
	verify_value(__LINE__, buf, "a3");

	verify_ret(__LINE__, kvc[0]->remove(key1, &ver), 0);
    verify_version(__LINE__, ver, 6);
}

void *
test23_run(void *x)
{
	int i = * (int *) x;
	if (current_test == 3)
		i = 0;

	char tmp[1000];
	int version;
	std::string buf;
	std::string key;

	for (int j = 0; j < 2*NREPEAT; j++) {
		sprintf(tmp, "%d", (int) random() % 1000);
		if ((j % 2) == 0)
			key = key1;
		else
			key = key2;

		verify_ret(__LINE__, kvc[i]->put(key, tmp), 0);

		if (kvc[i]->get(key, buf, &version) == 0) {
			if (version > (NTHREADS * (NREPEAT + 1))|| version <= 0) {
				//since each thread issues NREPEAT/2 puts and 1 remove to either key1 or key2
				//key1 or key2 cannot have a version number greater than NTHREADS * NREPEAT/2
				printf("\t line-%d ERROR: wrong version number %d\n", __LINE__, version);
				exit(1);
			}
		}
	}
	kvc[i]->remove(key1, &version);
	kvc[i]->remove(key2, &version);

	return 0;
}

void test23()
{
	pthread_t th[NTHREADS];

	for (int i = 0; i < NTHREADS; i++) {
		int *a = new int (i);
		VERIFY(pthread_create(&th[i], NULL, test23_run, (void *) a) == 0);
	}
	for (int i = 0; i < NTHREADS; i++) {
		pthread_join(th[i], NULL);
	}

	int ver;
	std::string buf;
	verify_ret(__LINE__, kvc[0]->get(key1, buf, &ver), kv_protocol::NOEXIST);
	verify_ret(__LINE__, kvc[0]->get(key2, buf, &ver), kv_protocol::NOEXIST);

	verify_ret(__LINE__, kvc[0]->put(key1, "test23done"), 0);
	verify_ret(__LINE__, kvc[0]->get(key1, buf, &ver), 0);
	verify_version(__LINE__, ver, NTHREADS*(NREPEAT+1) + 1);
	verify_value(__LINE__, buf, "test23done");

	verify_ret(__LINE__, kvc[0]->put(key2, "test23done"), 0);
	verify_ret(__LINE__, kvc[0]->get(key2, buf, &ver), 0);
	verify_version(__LINE__, ver, NTHREADS*(NREPEAT + 1)+1);
	verify_value(__LINE__, buf, "test23done");

}


int
main(int argc, char *argv[])
{
	int test = 0;

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	srandom(getpid());


	//uncomment the following line to print a lot of debug messages
	//jsl_set_debug(JSL_DBG_4);

	if(argc < 2) {
		fprintf(stderr, "Usage: %s [host:]port [test]\n", argv[0]);
		exit(1);
	}

	if (argc > 2) {
		test = atoi(argv[2]);
		if(test < 1 || test > 3) {
			printf("Test number must be between 1 and 3\n");
			exit(1);
		}
	}

	for (int i = 0; i < NTHREADS; i++)  {
		kvc[i] = new kv_client(argv[1]);
		printf("created RPC client %d\n", i);
	}

	char tmp[1000];

	if(!test || test == 1) {
		current_test = 1;

		sprintf(tmp, "%ld", random());
		key1 = std::string(tmp);

		printf("---test1: single client thread issuing requests (using key %s) ....\n", key1.c_str());
		test1();
	}

	if(!test || test == 2){
		current_test = 2;

		sprintf(tmp, "%ld", random());
		key1 = std::string(tmp);
		sprintf(tmp, "%ld", random());
		key2 = std::string(tmp);

		printf("---test2: %d RPC clients issuing concurrent put/get requests (using keys %s %s)...\n",
				NTHREADS, key1.c_str(), key2.c_str());
		test23();
	}

	if(!test || test == 3){
		current_test = 3;

		sprintf(tmp, "%ld", random());
		key1 = std::string(tmp);
		sprintf(tmp, "%ld", random());
		key2 = std::string(tmp);

		printf("---test3: %d threads issuing concurrent requests from same RPC client (using keys %s %s)...\n",
				NTHREADS, key1.c_str(), key2.c_str());
		test23();
	}

    printf ("%s: passed all tests successfully\n", argv[0]);

}
