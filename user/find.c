#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"


#define STDERR (2)
#define O_RDONLY (0)
#define BUFFER_SIZE (512)

char *
fmtname(char *path) {
    static char buf[DIRSIZ + 1];
    char *p;
    int length;

    length = 0;

    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--) {
        length++;
    }
    p++;

    if(strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, length);
    
    return buf;
}

void 
find(char *path, char *query) {
    char buffer[BUFFER_SIZE];
    char *p;
    int fd;
    struct dirent de;
    struct stat st;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        fprintf(STDERR, "find: opening path <%s> failed\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(STDERR, "find: failed getting stat of <%s>\n", path);
        close(fd);
        return;
    }

    strcpy(buffer, path);

    switch (st.type) {

    case T_DEVICE:
    case T_FILE:
        if (strcmp(fmtname(buffer), query) == 0) {
            printf("%s\n", path);
        }
        break;

    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buffer){
            fprintf(STDERR, "find: path too long\n");
            break;
        }
        p = buffer + strlen(buffer);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            
            // strcpy(p, de.name);
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = '\0';
            find(buffer, query);
        }
        
    }
    close(fd);
    return;
}

int
main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(STDERR, "find: missing arguments (2 required)\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}