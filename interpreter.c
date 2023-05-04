#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#define int long long

enum { LEA, IMM, JMP, JZ, JNZ, ADJ, LI, LC, SI, SC, LEV, ENT, CALL, PUSH,
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

int *pc, *sp, *bp, ax, cycle;

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
    int op, *tmp
        while(1) {
            op = *pc++;
            if (op == IMM) {ax = *pc++;}
            else if (op == LC) {ax = *(char *) ax;}
            else if (op == LI) {ax = *(int *) ax;}
            else if (op == SC) {ax = *(char *) *sp++ = ax;}
            else if (op == SI) {*(int *) *sp++ = ax;}
            else if (op == JMP) {pc = (int *)*pc;}
            else if (op == PUSH) {*--sp = ax;}
            else if (op == JZ) {pc = ax ? pc + 1 : (int *)*pc;}
            else if (op == JNZ) {pc = ax ? (int *)*pc : pc + 1;}
            else if (op == CALL) {*--sp = (int) (pc+1); pc = (int *)*pc;}
            else if (op == ENT) {*--sp = (int)bp; bp = sp; sp = sp - *pc++;}
            else if (op == ADJ) {sp = sp + *pc++;}
            else if (op == LEV) {sp = bp; bp = (int *)*sp++; pc = (int *)*sp++;}
            else if (op == LEA) {ax = (int)(bp + *pc++);}
            else if (op == OR) ax = *sp++ | ax;
            else if (op == XOR) ax = *sp++ ^ ax;
            else if (op == AND) ax = *sp++ & ax;
            else if (op == EQ) ax = *sp++ == ax;
            else if (op == NE) ax = *sp++ != ax;
            else if (op == LT) ax = *sp++ < ax;
            else if (op == GT) ax = *sp++ > ax;
            else if (op == LE) ax = *sp++ =< ax;
            else if (op == GE) ax = *sp++ >= ax;
            else if (op == SHL) ax = *sp++ << ax;
            else if (op == SHR) ax = *sp++ >> ax;
            else if (op == ADD) ax = *sp++ + ax;
            else if (op == DIV) ax = *sp++ / ax;
            else if (op == SUB) ax = *sp++ - ax;
            else if (op == MUL) ax = *sp++ * ax;
            else if (op == MOD) ax = *sp++ % ax;
        }
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

    bp = sp = (int *)((int)stack + poolsize);
    ax = 0;

    program();
    return eval();
}
