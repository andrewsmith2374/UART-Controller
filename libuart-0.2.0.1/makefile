TARGET 	= libuart.so.0.1
RM 	= rm -rf
CC 	= gcc
CFLAGS 	= -Wall -fPIC
LDFLAGS = -shared -Wl,-soname,libuart.so.0

OBJ = uart.o error.o error_uart.o

all: main

debug: CFLAGS += -g

debug: main

main: $(OBJ)
	$(CC) -o $(TARGET) $(LDFLAGS) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJ) *~
