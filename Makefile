prefix=/usr/local
bindir=$(prefix)/bin
datarootdir=$(prefix)/share

CC = gcc
EXEC = pacman
CFLAGS = -lncurses -l usb-1.0 -Wall -Werror -Wextra -pedantic

all:
	$(CC) main.c usb.c pacman-1.3/pacman.c -o $(EXEC) -DDATAROOTDIR=\"$(datarootdir)\" $(CFLAGS)

install:    all
	mkdir -p $(DESTDIR)$(datarootdir)/pacman/Levels
	cp -r pacman-1.3/Levels/ $(DESTDIR)$(datarootdir)/pacman/Levels

uninstall:
	rm -f -r $(DESTDIR)$(datarootdir)/pacman

clean:	uninstall
	rm -f pacman
