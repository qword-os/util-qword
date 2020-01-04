#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <termios.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <util/sha256.h>

// Sizes.
#define MAX_LINE_LEN       2048
#define MAX_USERS          256
#define MAX_USER_LEN       256
#define MAX_SHELL_LEN      256
#define MAX_FULLUSER_LEN   1024
#define MAX_HOMEDIR_LEN    1024

// /etc/passwd fields.
size_t passwd_users;
char   passwd_line[MAX_LINE_LEN];

char   passwd_user[MAX_USER_LEN][MAX_USERS];
char   passwd_hash[SHA256_STR_LEN + 1][MAX_USERS];
uid_t  passwd_uid[MAX_USERS];
gid_t  passwd_gid[MAX_USERS];
char   passwd_fulluser[MAX_FULLUSER_LEN][MAX_USERS];
char   passwd_homedir[MAX_HOMEDIR_LEN][MAX_USERS];
char   passwd_shell[MAX_SHELL_LEN][MAX_USERS];

int split(const char *str, char c, char ***arr) {
    int count = 1;
    int token_len = 1;
    int i = 0;
    char *p;
    char *t;

    p = str;

    while (*p != '\0') {
        if (*p == c)
            count++;
        p++;
    }

    *arr = (char**) malloc(sizeof(char*) * count);

    if (*arr == NULL)
        exit(1);

    p = str;

    while (*p != '\0') {
        if (*p == c) {
            (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
            if ((*arr)[i] == NULL)
                exit(1);

            token_len = 0;
            i++;
        }
        p++;
        token_len++;
    }
    (*arr)[i] = (char*) malloc( sizeof(char) * token_len );

    if ((*arr)[i] == NULL)
        exit(1);

    i = 0;
    p = str;
    t = ((*arr)[i]);

    while (*p != '\0') {
        if (*p != c && *p != '\0') {
            *t = *p;
            t++;
        }
        else {
            *t = '\0';
            i++;
            t = ((*arr)[i]);
        }
        p++;
    }

    return count;
}

void parse_passwd(void) {
    // Parse /etc/passwd.
    FILE *fp = fopen("/etc/passwd", "r");
    passwd_users  = 0;
    char **tokens = NULL;

    while (fgets(passwd_line, MAX_LINE_LEN, fp)) {
        // TODO: Check for the availability of the fields instead of harcode.
        split(passwd_line, ':', &tokens);

        strcpy(passwd_user[passwd_users], tokens[0]);
        strcpy(passwd_hash[passwd_users], tokens[1]);
        passwd_uid[passwd_users] = atoi(tokens[2]);
        passwd_gid[passwd_users] = atoi(tokens[3]);
        strcpy(passwd_fulluser[passwd_users], tokens[4]);
        strcpy(passwd_homedir[passwd_users],  tokens[5]);
        strcpy(passwd_shell[passwd_users],    tokens[6]);
        passwd_users += 1;

        if (passwd_users == MAX_USERS)
            break;
    }

    fclose(fp);
}

int test_passwd(char *usr, char *pw) {
    // Find the user we are searching for.
    for (size_t i = 0; i < passwd_users; i++) {
        if (!strcmp(usr, passwd_user[i])) {
            // Make the password a SHA.
            struct sha256_ctx ctx;
            uint8_t password[SHA256_BLOCK_SIZE];
            sha256_init(&ctx);
            sha256_update(&ctx, pw, strlen(pw));
            sha256_final(&ctx, password);

            char userpwhash[SHA256_STR_LEN];

            // Put hash in a string.
            for (size_t x = 0; x < SHA256_BLOCK_SIZE; x++)
                sprintf(userpwhash + (x * 2), "%02x", password[x]);

            // Compare the password hashes.
            if (!strncmp(userpwhash, passwd_hash[i], SHA256_STR_LEN))
                return 0;
            else
                return 1;
        }
    }

    return 1;
}

char *passwd_getshell(char *usr) {
    // Find the user we are searching for.
    for (size_t i = 0; i < passwd_users; i++) {
        if (!strcmp(usr, passwd_user[i])) {
            return passwd_shell[i];
        }
    }

    return NULL;
}

uid_t passwd_getuid(char *user) {
    // Find the user we are searching for.
    for (size_t i = 0; i < passwd_users; i++) {
        if (!strcmp(user, passwd_user[i])) {
            return passwd_uid[i];
        }
    }

    return 0;
}

#define MAX_USER_LEN     256
#define MAX_PASSWORD_LEN 1024
#define MAX_HOSTNAME_LEN 1024
#define MAX_PASSWD_LEN   1024

// Input fields.
char user[MAX_USER_LEN];
char password[MAX_PASSWORD_LEN];
char hostname[MAX_HOSTNAME_LEN];

void set_global_data(void) {
    // Ask for login name.
    printf("%s login: ", hostname);
    fflush(stdout);
    fgets(user, MAX_USER_LEN, stdin);
    user[strlen(user) - 1] = '\0';

    // Ask for password.
    printf("Password: ");
    fflush(stdout);

    // Turn off echo and etc.
    struct termios orig;
    tcgetattr(STDIN_FILENO, &orig);
    struct termios new = orig;
    new.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new);

    fgets(password, MAX_PASSWORD_LEN, stdin);

    // Reset terminal properties.
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);

    putchar('\n');

    password[strlen(password) - 1] = '\0';
}

int main(void) {
    // OS and hostname detection.
    struct utsname buffer;
    uname(&buffer);
    gethostname(hostname, MAX_HOSTNAME_LEN);
    printf("%s (%s) (%s)\n\n", buffer.sysname, hostname,
        basename(ttyname(STDIN_FILENO)));

    // Prepare the info we need.
    parse_passwd();

    while (1) {
        // Get command line user and password.
        set_global_data();

        // Test against /etc/passwd.
        if (test_passwd(user, password)) {
            printf("Login incorrect.\n\n");
        } else {
            break;
        }
    }

    // Fork to execute the login shell.
    pid_t pid = fork();
    int status;

    if (!pid) {
        // Set the UID in the forked thread.
        uid_t uid = passwd_getuid(user);
        setuid(uid);

        // Launch!
        char *shell = passwd_getshell(user);
        char *args[] = {shell, NULL};
        execvp(shell, args);
    } else {
        waitpid(pid, &status, 0);
    }
}
