#include "kernel/stat.h"
#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define BUFFER_SIZE (512)
#define STDIN (0)
#define MAX_WORD_LENGTH (32)

int
make_delim(char *buffer, char delim, char end) {
    // splits and returns split count. end = 0 for no custom ending
    int changed;

    changed = 1;
    while (*buffer != end && *buffer != '\0') {
        if (*buffer == delim) {
            *buffer = '\0';

            if (*(buffer + 1) != '\0') {
                changed++;
            }
        }
        buffer++;
    }
    return changed;
}

char*
init_copied(char *p) {
    static char copy[MAX_WORD_LENGTH];
    strcpy(copy, p);
    return copy;
}

int
main(int argc, char *argv[]) {

    char buffer[BUFFER_SIZE];
    char *p;
    char *args[MAXARG];
    int copy_length;
    int last_read;

    int line_count;
    int word_count;

    
    for (int i = 0; i < argc - 1; i++) {
        args[i] = argv[i + 1];
                                                // printf("%d: %s\n", i, args[i]);
    }

    p = buffer;
    copy_length = 0;
    last_read = read(STDIN, p, BUFFER_SIZE - copy_length); 
    while (last_read > 0) {
        copy_length = copy_length + last_read;
        p = p + last_read;
        while (*p != '\0') {
            p--;
        }
        last_read = read(STDIN, p, BUFFER_SIZE - copy_length);
    }
    
                                                // for (int i = 0; i < copy_length; i++) {
                                                //     printf("%d, ", buffer[i]);
                                                // }
                                                // printf("\nBUFFER: %s\n", buffer);
    *(buffer + copy_length) = 0;

    line_count = make_delim(buffer, '\n', 0);
    p = buffer;

                                                // printf("LINE COUNT: %d\n", line_count);
    for (int i = 0; i < line_count; i++) {
        word_count = make_delim(p, ' ', 0);
                                                // printf("WORD COUNT: %d\n", word_count);

        for (int j = 0; j < word_count; j++) {
            args[j + argc - 1] = init_copied(p);
            p = p + strlen(p) + 1;
        }
        args[word_count + argc - 1] = 0;        

        if (fork() == 0) {
            exec(args[0], args);
        }
        wait(0);
    }

    exit(0);
}