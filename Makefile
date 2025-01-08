all: controller

CFLAGS=-std=gnu99 -Wall -g -Ilibuart-0.2.0.1
SRC=controller.o control_loop.o libuart-0.2.0.1/uart.o

controller: $(SRC)
	gcc $(CFLAGS) -pthread -o $@ $^

%.o : %.c controller.h
	gcc $(CFLAGS) -c $<

clean : 
	rm -f *.o controller *~
