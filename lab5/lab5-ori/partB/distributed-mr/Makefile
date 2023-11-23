all:
	echo 'nothing'

counter-mr.so: 
	gcc -g -shared -fPIC -o ./counter-mr.so ../util/counter-mr.c ../util/key-value.c ../lib/list.c

mr-server:
	# server
	export SRCDIR=.
	gcc -c -shared -fPIC mr-server.c -o libmr-server.so
	go run mr-server.go mr-rpcs.go

mr-client:
	# client
	go build -buildmode=c-archive mr-client.go mr-rpcs.go
	gcc mr-client.c mr.c ../lib/list.c ../util/key-value.c mr-rpcs.a -o mr-client.out
	#example: ./mr-client.out ./counter-mr.so

clean:
	rm -rf *.i *.s *.o *.a *.so *-rpcs.h *.out core.* *.txt ./tmp ./output 