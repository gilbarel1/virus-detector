all: lab3_bubblesort hexaPrint AntiVirus

lab3_bubblesort: lab3_bubblesort.o
	gcc -m32 -g -Wall -o lab3_bubblesort lab3_bubblesort.o

hexaPrint: hexaPrint.o
	gcc -m32 -g -Wall -o hexaPrint hexaPrint.o

lab3_bubblesort.o: lab3_bubblesort.c
	gcc -g -Wall -m32 -c -o lab3_bubblesort.o lab3_bubblesort.c

hexaPrint.o: hexaPrint.c
	gcc -g -Wall -m32 -c -o hexaPrint.o hexaPrint.c

AntiVirus: AntiVirus.o
	gcc -m32 -g -Wall -o AntiVirus AntiVirus.o

AntiVirus.o: AntiVirus.c
	gcc -g -Wall -m32 -c -o AntiVirus.o AntiVirus.c


    
.PHONY: clean 

clean:
	rm -f *.o lab3_bubblesort hexaPrint