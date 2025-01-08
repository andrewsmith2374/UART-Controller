all: controller

CFLAGS=-std=gnu99 -Wall -g

controller: controller.o control_loop.o libuart-0.2.0.1/uart.o
	gcc $(CFLAGS) -pthread -o $@ $^

%.o : %.c controller.h
	gcc $(CFLAGS) -c $<

clean : 
	rm -f *.o controller *~
