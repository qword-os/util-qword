# Globals and files to compile.
SOURCEDIR := src

# User options.
PREFIX  = /usr
DESTDIR =

CC = gcc

CFLAGS = -O2 -pipe -Wall -Wextra

CHARDFLAGS := $(CFLAGS) -masm=intel -I$(SOURCEDIR)

.PHONY: all clean install uninstall

all:
	$(CC) $(CHARDFLAGS) $(SOURCEDIR)/mount.c  -o mount
	$(CC) $(CHARDFLAGS) $(SOURCEDIR)/qfetch.c -o qfetch

clean:
	rm -f mount
	rm -f qfetch

install:
	install -d $(DESTDIR)$(PREFIX)/bin
	install -s mount  $(DESTDIR)$(PREFIX)/bin
	install -s qfetch $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/mount
	rm -f $(DESTDIR)$(PREFIX)/bin/qfetch
