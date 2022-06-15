all: 
	gcc main.c -o main.out -lncurses

run: 
	./main.out 

clean: 
	-rm *.out 

