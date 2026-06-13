#include "kernel/stat.h"
#include "kernel/types.h"
#include "user/user.h"

#define UPPER_BOUND (35) 
#define LOWER_BOUND (2)
#define INT_SIZE (4)

int
main(int argc, char *argv[]) {
    int parent_to_child[2];
    int buffer[1];
    int child_prime;
    int is_child;
    int reader;


    pipe(parent_to_child);

    is_child = fork();
    while (is_child == 0) {
        reader = parent_to_child[0];
        close(parent_to_child[1]);
        if (read(reader, buffer, INT_SIZE) == 0) { // final child fills condition
            close(reader);
            exit(0);
        }

        pipe(parent_to_child);
        is_child = fork();
        if (is_child == 0) {
            continue;
        }

        child_prime = buffer[0];
        printf("prime %d\n", child_prime);

        while (read(reader, buffer, INT_SIZE) != 0) {
            if (buffer[0] % child_prime != 0) {
                write(parent_to_child[1], buffer, INT_SIZE);
            }
        }

        close(reader);
        close(parent_to_child[1]);
        wait(0);
        exit(0);
    }
    
    for (int i = LOWER_BOUND; i <= UPPER_BOUND; i++) {
        buffer[0] = i;
        write(parent_to_child[1], buffer, INT_SIZE);
    }
    close(parent_to_child[1]);
    
    wait(0);
    exit(0);
}