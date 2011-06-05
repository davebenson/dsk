social-network-daemon: social-network-daemon.c generated/socnet.pb-c.c
	gcc -o $@ $^


generated/socnet.pb-c.c: socnet.proto
	protoc-c --c_out=generated socnet.proto
