#include "kernel/stat.h"
#include "kernel/types.h"
#include "user/user.h"

#define STDERR 2

int
main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(STDERR,"sleep: no argument passed\n");
        exit(1);
    }
    int n;
    n = atoi(argv[1]);
    sleep(n);
    exit(0);
}