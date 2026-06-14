#include "kernel/stat.h"
#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define BUFFER_SIZE (512)
#define STDIN (0)
#define MAX_WORD_LENGTH (32)

int
main(int argc, char *argv[]) {
    char buffer[BUFFER_SIZE];

    while (strlen(gets(buffer, BUFFER_SIZE))) {
        for (int i = 0; buffer[i]; i++) {
            printf("%d, ", buffer[i]);
        }
    }
    printf("\nBUFFER: %s\n", buffer);
}