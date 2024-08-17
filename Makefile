all:
	cc main.c -o main

debug:
	cc -g -D DEBUG main.c -o main
