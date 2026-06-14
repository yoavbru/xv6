#include "kernel/stat.h"
#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define BUFFER_SIZE (512)
#define STDIN (0)
#define STDERR (2)
#define MAX_WORD_LENGTH (32)

int
make_delim(char *buffer, char delim, char end) {
    // splits and returns split count. end = 0 for no custom ending
    int changed;

    changed = 1;
    while (*buffer != end && *buffer != '\0') {
        if (*buffer == delim) {
            if (*(buffer + 1) != delim) {
                *buffer = '\0';
            }

            if (*(buffer + 1) != '\0' && *(buffer + 1) != delim) {
                changed++;
            }
        }
        buffer++;
    }
    return changed;
}

int
main(int argc, char *argv[]) {

    char buffer[BUFFER_SIZE];
    char *p;
    char *args[MAXARG];
    int word_count;

    
    for (int i = 0; i < argc - 1; i++) {
        args[i] = argv[i + 1];
    }

    while (strlen(gets(buffer, BUFFER_SIZE))) {
        buffer[strlen(buffer) - 1] = 0;
        p = buffer;
        word_count = make_delim(p, ' ', 0);

        if (word_count >= MAXARG) {
            fprintf(STDERR, "xargs: too many arguments");
            continue;
        }

        for (int j = 0; j < word_count; j++) {

            args[j + argc - 1] = malloc(MAX_WORD_LENGTH);
            strcpy(args[j + argc - 1], p);
            p = p + strlen(p) + 1;
        }
        args[word_count + argc - 1] = 0;   

        if (fork() == 0) {
            exec(args[0], args);
        }
        wait(0);

        for (int i = argc - 1; args[i]; i++) {
            free(args[i]);
        }
    }

    exit(0);
}