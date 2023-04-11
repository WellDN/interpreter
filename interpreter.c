#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#define int long long

enum { LEA, IMM, JMP, CALL, JZ, JNZ, ENT, ADJ, LEV, LI, LC, SI, SC, PUSH,
       OR, XOR, AND, EQ, NE, LT, GT, LE, GE, SHL, SHR, ADD, SUB, DIV, MUL, MOD,
       OPEN, READ, CLOS, PRTF, MALC, MSET, MCMP, EXIT };

int token;
char *src, *old_src;
int poolsize;
int line;

int *text,
    *old_text,
    *stack;
char *data;

int *pc, *sp, *bp, ax, cycle; // vm registers

void next() {
    token = *src++;
    next();
}

void expression(int level) {

}

void program() {
    next();
    while(token > 0) {
        printf("token is: %c\n", token);
        next();
    }
}

void eval() {
    return 0;
}

int main(int argc, char **argv) {
    int i, fd;

    argc--;
    argv++;

    poolsize = 256 * 1024;
    line = 1;

    if ((fd = open(*argv, 0)) < 0) {
        printf("could not open(%s)\n", *argv);
        return -1;
    }

    if (!(src = old_src = malloc(poolsize))) {
        printf("could not malloc(%d)\n", poolsize)
            return -1;
    }

    if ((i = read(fd, src, poolsize-1)) <= 0) {
        printf("read() returned %s\n", i);
        return -1;
    }

    src[i] = 0;
    close(fd);

    if (!(text = old_text = malloc(poolsize))) {
        printf("could not malloc(%d) find text\n", poolsize);
        return -1;
    }

    if (!(data = malloc(poolsize))) {
        printf("could not malloc(%d) find data\n", poolsize);
        return -1;
    }

    if (!(stack = malloc(poolsize))) {
        printf("could not malloc(%d) find stack\n", poolsize);
        return -1;
    }

    memset(text, 0, poolsize);
    memset(data, 0, poolsize);
    memset(stack, 0, poolsize);

    bp = sp = (int *)((int)(stack + poolsize));
    ax = 0;

    program();
    return eval();
}
