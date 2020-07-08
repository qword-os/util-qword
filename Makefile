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
	$(CC) $(CHARDFLAGS) $(SOURCEDIR)/util/messages.c $(SOURCEDIR)/mount.c  -o mount
	$(CC) $(CHARDFLAGS) $(SOURCEDIR)/util/sha256.c   $(SOURCEDIR)/qetty.c  -o qetty
	$(CC) $(CHARDFLAGS) $(SOURCEDIR)/qfetch.c -o qfetch
	$(CC) $(CHARDFLAGS) $(SOURCEDIR)/util/messages.c $(SOURCEDIR)/umount.c -o umount

clean:
	rm -f mount
	rm -f qetty
	rm -f qfetch
	rm -f umount

install:
	install -d $(DESTDIR)$(PREFIX)/bin
	install -s mount  $(DESTDIR)$(PREFIX)/bin
	install -s qetty  $(DESTDIR)$(PREFIX)/bin
	install -s qfetch $(DESTDIR)$(PREFIX)/bin
	install -s umount $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/mount
	rm -f $(DESTDIR)$(PREFIX)/bin/qetty
	rm -f $(DESTDIR)$(PREFIX)/bin/qfetch
	rm -f $(DESTDIR)$(PREFIX)/bin/umount
