#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

int main(int argc, char *argv) {
    symlink("abc", "abc");
    exit(0);
}