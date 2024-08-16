all:
	cc main.c -o main

debug:
	cc -D DEBUG main.c -o main
