all: 
	gcc main.c -o main.out 

run: 
	./main.out 

clean: 
	-rm *.out 

