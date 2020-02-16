all:
	gcc -o part1.x part1.c
	gcc -o empty.x empty.c
	strace -o part1.log ./part1.x
	strace -o empty.log ./empty.x

clean:
	rm *.x *.log
