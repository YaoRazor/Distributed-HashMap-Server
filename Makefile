LAB=1
SOL=1
RPC=./rpc
LAB2GE=$(shell expr $(LAB) \>\= 2)
LAB3GE=$(shell expr $(LAB) \>\= 3)
CXXFLAGS =  -g  -std=c++11  -MMD -Wall -I. -I$(RPC) -DLAB=$(LAB) -DSOL=$(SOL) -D_FILE_OFFSET_BITS=64
#CXXFLAGS =  -g  -MMD -Wall -I. -I$(RPC) -DLAB=$(LAB) -DSOL=$(SOL) -D_FILE_OFFSET_BITS=64

#LDLIBS += $(shell test -f `gcc -print-file-name=librt.so` && echo -lrt)
#LDLIBS += $(shell test -f `gcc -print-file-name=libdl.so` && echo -ldl)
LDLIBS += -lrt -lpthread

CC = g++
CXX = g++

lab:  lab$(LAB)
lab1: rpc/rpctest kv_server kv_demo kv_tester
lab2: rpc/rpctest kv_server kv_demo kv_tester rsm_tester
lab3: rpc/rpctest kv_server kv_demo kv_tester rsm_tester

#the hfiles* are for make l1 etc.
common_src := .gitignore GNUmakefile  
hfilesrpc=rpc/fifo.h rpc/connection.h rpc/rpc.h rpc/marshall.h rpc/method_thread.h\
	rpc/thr_pool.h rpc/pollmgr.h rpc/jsl_log.h rpc/slock.h\
	lang/verify.h lang/algorithm.h 
hfiles1= kv_server.h kv_client.h kv_protocol.h
hfilesrsm=handle.h tprintf.h paxos.h paxos_protocol.h rsm_state_transfer.h rsmtest_client.h rsm_client.h
rsm_files = rsm.cc paxos.cc config.cc log.cc handle.cc

rpclib=rpc/rpc.cc rpc/connection.cc rpc/pollmgr.cc rpc/thr_pool.cc rpc/jsl_log.cc rpc/rpctest.cc
rpc/librpc.a: $(patsubst %.cc,%.o,$(rpclib))
	rm -f $@
	ar cq $@ $^
	ranlib rpc/librpc.a

rpctest=rpc/rpctest.cc
rpc/rpctest: $(patsubst %.cc,%.o,$(rpctest)) rpc/librpc.a

kv_demo=kv_demo.cc kv_client.cc
ifeq ($(LAB3GE),1)
  kv_demo+=rsm_client.cc handle.cc
endif
kv_demo : $(patsubst %.cc,%.o,$(kv_demo)) rpc/librpc.a

kv_tester=kv_tester.cc kv_client.cc
ifeq ($(LAB3GE),1)
  kv_tester+=rsm_client.cc handle.cc
endif
kv_tester : $(patsubst %.cc,%.o,$(kv_tester)) rpc/librpc.a

kv_server=kv_server.cc kv_smain.cc
ifeq ($(LAB2GE),1)
  kv_server+= $(rsm_files)
endif

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
	@bash -c "cd ../; tar -X <(tr ' ' '\n' < <(echo '$(handin_ignore)')) -czvf $(handin_file) $(labdir); mv $(handin_file) $(labdir); cd $(labdir)"
