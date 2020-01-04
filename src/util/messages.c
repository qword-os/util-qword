#include <util/messages.h>
#include <stdio.h>
#include <stdlib.h>

#define CMAGENTA "\e[1;35m"
#define CRED     "\e[1;31m"
#define CRESET   "\e[0m"

void warning(const char *msg) {
    fprintf(stderr, "%sWarning%s: %s.\n", CMAGENTA, CRESET, msg);
}

void error(const char *msg) {
    fprintf(stderr, "%sError%s: %s.\n", CRED, CRESET, msg);
    exit(1);
}