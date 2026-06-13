#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
    int parent_to_child[2];
    int child_to_parent[2];
    char buf[1];
    int pid;
    
    pipe(parent_to_child);
    pipe(child_to_parent);

    if (fork() == 0) {
        pid = getpid();
        read(parent_to_child[0], buf, 1);
        printf("%d: received ping\n", pid);
        write(child_to_parent[1], buf, 1);

        close(parent_to_child[1]);
        close(child_to_parent[1]);
        close(parent_to_child[0]);
        close(child_to_parent[0]);
    }
    else {
        buf[1] = 67;
        pid = getpid();
        write(parent_to_child[1], buf, 1);
        read(child_to_parent[0], buf, 1);
        printf("%d: received pong\n", pid);

        close(parent_to_child[1]);
        close(child_to_parent[1]);
        close(parent_to_child[0]);
        close(child_to_parent[0]);
    }
    exit(0);
}