lab1_src := $(common_src) $(rpclib) $(kv_server) $(kv_demo) $(kv_tester) $(hfiles1) $(hfilesrpc)

l1: $(lab1_src) 
	./mklab.pl 1 0 l1 $^

l1-sol: $(lab1_src)
	./mklab.pl 1 1 l1-sol $^

lab2_src := $(common_src) $(rpclib) $(kv_server) $(kv_demo) $(kv_tester) $(hfiles1) $(hfilesrpc) $(hfilesrsm) rsm_tester.pl $(rsm_tester)

l2: $(lab2_src)
	./mklab.pl 2 0 l2 $^

l2-sol: $(lab2_src)
	./mklab.pl 2 2 l2-sol $^


lab3_src :=  $(common_src) $(rpclib) $(kv_server) $(kv_demo) $(kv_tester) $(hfiles1) $(hfilesrpc) $(hfilesrsm) rsm_tester.pl $(rsm_tester)


l3: $(lab3_src)
	./mklab.pl 3 0 l3 $^

l3-sol: $(lab3_src)
	./mklab.pl 3 3 l3-sol $^
