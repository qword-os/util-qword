# Globals and files to compile.
SOURCEDIR := src

# User options.
PREFIX  = /usr/bin
DESTDIR =

CC = gcc

CFLAGS = -O2 -pipe -Wall -Wextra

CHARDFLAGS := $(CFLAGS) -I$(SOURCEDIR)

.PHONY: all clean install uninstall

all:
	$(CC) $(CHARDFLAGS) $(SOURCEDIR)/mount.c -o mount

clean:
	rm -f mount

install:
	install -d $(PREFIX)/bin
	install -s mount $(DESTDIR)$(PREFIX)/mount

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/mount