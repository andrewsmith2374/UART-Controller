all: controller

CFLAGS=-std=gnu99 -Wall -g

controller: controller.o
	gcc $(CFLAGS) -pthread -o $@ $^

%.o : %.c controller.h
	gcc $(CFLAGS) -c $<

clean : 
	rm -f *.o controller *~
