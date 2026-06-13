#include "kernel/stat.h"
#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define BUFFER_SIZE (512)
#define STDIN (0)
#define MAX_WORD_LENGTH (32)

int
main(int argc, char *argv[]) {
    int last_read;
    char *p;
    char buffer[BUFFER_SIZE];
    int copy_length;

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
    
    for (int i = 0; i < copy_length; i++) {
        printf("%d, ", buffer[i]);
    }
    printf("\nBUFFER: %s\n", buffer);
}