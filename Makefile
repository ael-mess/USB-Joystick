CC = gcc
EXEC = pacman
CFLAGS = -lncurses -l usb-1.0 -Wall -Werror -Wextra -pedantic

all:
	$(CC) main.c usb.c pacman-1.3/pacman.c -o $(EXEC) $(CFLAGS)

clean:
	rm -f pacman
