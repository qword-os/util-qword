#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <util/messages.h>
#include <qword/mount.h>

// Command line parsing.
extern int opterr;

int main(int argc, char *argv[]) {
    // First, parse the command line.
    opterr = 0; // Disable getopt warnings, we will use our own.
    int option = 0;
    char *mount_point = NULL;

    while ((option = getopt(argc, argv, "Vh")) != -1) {
        switch (option) {
            case 'V':
                puts("umount from util-qword");
                exit(0);
                break;
            case 'h':
                puts("Usage:");
                printf("\t%s <mount point>\n", argv[0]);
                puts("");
                puts("Options:");
                puts("\t-V\tPrints the version of the program");
                puts("\t-h\tDisplays this help message");
                exit(0);
                break;
            default:
                error("Unknown option");
        }
    }

    // Device and mount point.
    mount_point = argv[optind];

    // Check values.
    if (!mount_point) {
        error("Invalid program flags! Have you tried '-h'?");
    }

    // Start the actual program.
    printf("Unmounting '%s'", mount_point);
    if (umount(mount_point)) {
        error("Couldn't unmount device (syscall returned -1)");
    }
}
