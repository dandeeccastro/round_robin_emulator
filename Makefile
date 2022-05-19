rr:
	gcc main.c -o main.out

run: rr
	./main.out

clean: 
	rm *.out 
	rm vgcore.* 
