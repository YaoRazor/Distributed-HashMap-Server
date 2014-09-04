LAB=1
SOL=1
RPC=./rpc
LAB2GE=$(shell expr $(LAB) \>\= 2)
LAB3GE=$(shell expr $(LAB) \>\= 3)
LAB4GE=$(shell expr $(LAB) \>\= 4)
LAB5GE=$(shell expr $(LAB) \>\= 5)
LAB6GE=$(shell expr $(LAB) \>\= 6)
LAB7GE=$(shell expr $(LAB) \>\= 7)
CXXFLAGS =  -g  -std=c++11  -MMD -Wall -I. -I$(RPC) -DLAB=$(LAB) -DSOL=$(SOL) -D_FILE_OFFSET_BITS=64

#LDLIBS += $(shell test -f `gcc -print-file-name=librt.so` && echo -lrt)
#LDLIBS += $(shell test -f `gcc -print-file-name=libdl.so` && echo -ldl)
LDLIBS += -lrt -lpthread

CC = g++
CXX = g++

lab:  lab$(LAB)
lab1: rpc/rpctest kv_server kv_demo kv_tester
lab2: rpc/rpctest ck_server lock_tester lock_demo yfs_client extent_server
lab3: yfs_client extent_server lock_server test-lab-3-b test-lab-3-c

lab4: yfs_client extent_server lock_server lock_tester test-lab-3-b\
	 test-lab-3-c
lab5: yfs_client extent_server lock_server test-lab-3-b test-lab-3-c
lab6: lock_server rsm_tester
lab7: lock_tester lock_server rsm_tester

#the hfiles* are for make l1 etc.
common_src := .gitignore GNUmakefile  
hfilesrpc=rpc/fifo.h rpc/connection.h rpc/rpc.h rpc/marshall.h rpc/method_thread.h\
	rpc/thr_pool.h rpc/pollmgr.h rpc/jsl_log.h rpc/slock.h\
	lang/verify.h lang/algorithm.h 
hfiles1= kv_server.h kv_client.h kv_protocol.h
hfiles2=yfs_client.h extent_client.h extent_protocol.h extent_server.h
hfiles3=lock_client_cache.h lock_server_cache.h handle.h tprintf.h
hfiles4=log.h rsm.h rsm_protocol.h config.h paxos.h paxos_protocol.h rsm_state_transfer.h rsmtest_client.h tprintf.h
hfiles5=rsm_state_transfer.h rsm_client.h
rsm_files = rsm.cc paxos.cc config.cc log.cc handle.cc

rpclib=rpc/rpc.cc rpc/connection.cc rpc/pollmgr.cc rpc/thr_pool.cc rpc/jsl_log.cc rpc/rpctest.cc
rpc/librpc.a: $(patsubst %.cc,%.o,$(rpclib))
	rm -f $@
	ar cq $@ $^
	ranlib rpc/librpc.a

rpc/rpctest=rpc/rpctest.cc
rpc/rpctest: $(patsubst %.cc,%.o,$(rpctest)) rpc/librpc.a

kv_demo=kv_demo.cc kv_client.cc
kv_demo : $(patsubst %.cc,%.o,$(kv_demo)) rpc/librpc.a

kv_tester=kv_tester.cc kv_client.cc
ifeq ($(LAB4GE),1)
  lock_tester += lock_client_cache.cc
endif
ifeq ($(LAB7GE),1)
  lock_tester+=rsm_client.cc handle.cc lock_client_cache_rsm.cc
endif
kv_tester : $(patsubst %.cc,%.o,$(kv_tester)) rpc/librpc.a

lock_server=kv_server.cc kv_smain.cc
ifeq ($(LAB4GE),1)
  lock_server+=lock_server_cache.cc handle.cc
endif
ifeq ($(LAB6GE),1)
  lock_server+= $(rsm_files)
endif
ifeq ($(LAB7GE),1)
  lock_server+= lock_server_cache_rsm.cc
endif

lock_server : $(patsubst %.cc,%.o,$(lock_server)) rpc/librpc.a

yfs_client=yfs_client.cc extent_client.cc fuse.cc
ifeq ($(LAB3GE),1)
  yfs_client += lock_client.cc
endif
ifeq ($(LAB7GE),1)
  yfs_client += rsm_client.cc lock_client_cache_rsm.cc
endif
ifeq ($(LAB4GE),1)
  yfs_client += lock_client_cache.cc
endif
yfs_client : $(patsubst %.cc,%.o,$(yfs_client)) rpc/librpc.a

kv_server=kv_server.cc kv_smain.cc 
kv_server : $(patsubst %.cc,%.o,$(kv_server)) rpc/librpc.a

rsm_tester=rsm_tester.cc rsmtest_client.cc
rsm_tester:  $(patsubst %.cc,%.o,$(rsm_tester)) rpc/librpc.a

%.o: %.cc 
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include mklab.inc

-include *.d
-include rpc/*.d

clean_files=rpc/rpctest rpc/*.o rpc/*.d rpc/librpc.a *.o *.d kv_server kv_client kv_demo kv_tester rsm_tester
.PHONY: clean handin
clean: 
	rm $(clean_files) -rf 

handin_ignore=$(clean_files) core* *log
handin_file=lab$(LAB).tgz
labdir=$(shell basename $(PWD))
handin: 
	@if test -f stop.sh; then ./stop.sh > /dev/null 2>&1 | echo ""; fi
	@bash -c "cd ../; tar -X <(tr ' ' '\n' < <(echo '$(handin_ignore)')) -czvf $(handin_file) $(labdir); mv $(handin_file) $(labdir); cd $(labdir)"
