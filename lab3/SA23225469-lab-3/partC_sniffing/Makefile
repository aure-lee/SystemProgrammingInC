all:
	gcc promiscuous_main.c packet-analyze.c -o main.out
	# sniffing requires super user privilege
	# device name from "ifconfig" or "ip link show"
	sudo ./main.out eth0
	# sudo ./main.out eth0 > main.txt

sniff:
	gcc -o analysis.out filter.c main.c packet-process.c
	sudo ./analysis.out eth0
	# sudo ./analysis.out eth0 > analysis.txt

clean:
	rm -rf *.out *.txt

