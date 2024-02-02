mandelbrodt: mandel.o gui_element.o
	gcc -o mandelbrodt mandel.o gui_element.o

mandel.o: mandel.c
	gcc -c mandel.c

gui_element.o: gui_element.c
	gcc -c gui_element.c
	
clean:
	rm -f mandel.o
	rm -f gui_element.o
