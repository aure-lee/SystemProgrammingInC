all:
	gcc -g main.c task.c mr.c ../lib/list.c ../util/key-value.c -o mthd-mr.out
	#example: ./mthd-mr.out counter-mr.so 8 8 a.txt b.txt c.txt d.txt

counter-mr.so: 
	gcc -g -shared -fPIC -o ./counter-mr.so ../util/counter-mr.c ../util/key-value.c ../lib/list.c

clean:
	rm -rf *.i *.s *.o *.out core.* *.so *.txt ./output 