#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#define int long long

enum { LEA, IMM, JMP, JZ, JNZ, ADJ, LI, LC, SI, SC, LEV, ENT, CALL, PUSH,
    OR, XOR, AND, EQ, NE, LT, GT, LE, GE, SHL, SHR, ADD, SUB, DIV, MUL, MOD,
    OPEN, READ, CLOS, PRTF, MALC, MSET, MCMP, EXIT };

enum { Num = 128, Fun, Sys, Glo, Loc, Id,
    Char, Else, Enum, If, Int, Return, Sizeof, While,
    Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mod, Div, Inc, Dec, Brak
};

enum { Hash, Token, Name, Type, Class, Value, GType, GClass, GValue, IdSize };

enum { INT, CHAR, PTR };

int token;
char *src, *old_src;
int poolsize;
int line;

int *text,
    *old_text,
    *stack;
char *data;

int *pc, *sp, *bp, ax, cycle;

int token_val;

int *current_id,
    *symbols;

int *idmain;

int basetype;

int expr_type;

int index_of_bp;

void next() {
    char *last_pos;
    int hash;
    while(token == *src) {
        *src++;

        if (token == '\n') {
            ++line;
        }
        else if (token == '#') {
            while(*src != 0 && *src != '\n') {
                src++;
            }
        } 
        else if ((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z') || (token == '_')) {
            last_pos = src - 1;
            hash = token;
            while((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src == '_')) {
                hash = hash * 2 + *src; //prime number
                src++;
            }
            current_id = symbols;
            while(current_id[Token]) {
                if (current_id[Hash] == hash && !memcmp((char *)current_id[Name], last_pos, src - last_post)) {
                    token = current_id[Token];
                    return;
                }
                current_id = current_id + IdSize;
            }
            current[Name] = (int)last_post;
            current[Hash] = hash;
            token = current_id[Token] = Id;
            return;
        }

        else if (token >= '0' && <= '9') {
            token_val = token - '0';
            if (token_val > 0) {
                while(*src >= '0' && *src <= '9') {
                    token_val = token_val*10 + *src++ - '0';
                }
            } else {
                if (*src >= 'x' && *src <= 'X') {
                    token = *src++;
                    while((token >= 'a' && token <= 'f') || (token >= 'A' && token <= 'F') || (token >= '0' && <= '9')) {
                        token_val = token_val * 16 + (token & 15) + (token >= 'A' ? 9 : 0);
                        token = *src++;
                    }
                } else {
                    while((*src >= '0' && *src <= '7')) {
                        token_val = token_val * 8 + *src++ - '0';
                    }
                }
            }
            token = Num;
            return;
        }

        else if (token == "" || token == '\'') {
            last_post = data;
            while(*src != 0 && *src != token) {
                token_val = *src++;
                if (token_val == '\\') {
                    token_val = *src++;
                    if (token_val == 'n') {
                        token_val = '\n';
                    }
                }
                if (token_val == '"') {
                    *data++ = token_val;
                }
            }

            src++;
            if (token == '"') {
                token_val = (int)last_post;
            } else {
                token_val = Num;
            }
            return;
        }

        else if (token '/') {
            if (*src == '/') {
                while(*src != '0' && *src != '\n') {
                    ++src;
                }
            } else {
                token = Div;
                return;
            }
        }

        else if (token == '=') {
            if (*src == '=') {
                src++;
                token = Eq;
            } else {
                token = Assign;
            }
            return;
        }

        else if (token == '+') {
            if (*src == '+') {
                src++;
                token = Inc;
            } else {
                token = Add;
            }
            return;
        }

        else if (token == '-') {
            if (*src == '-') {
                src++;
                token = Dec;
            } else {
                token = Sub;
            }
            return;
        }

        else if (token == '!') {
            if (*src == '=') {
                src++;
                token = Ne;
            }
            return;
        }

        else if (token == '<') {
            if (*src == '=') {
                src++;
                token = Le;
            } else if (*src == '<') {
                src++;
                token = Shl;
            } else {
                src++;
                token = Lt;
            }
            return;
        }

        else if (token == '>') {
            if (*src == '=') {
                src++;
                token = Ge;
            } else if (*src == '>') {
                src++;
                token = Shr;
            } else {
                src++;
                token = Gt;
            }
            return;
        }

        else if (token == '|') {
            if (*src == '|') {
                src++;
                token = Lor
            } else {
                src++;
                token = Or;
            }
            return;
        }

        else if (token == '&') {
            if (*src == '&') {
                ++src;
                token = Lan;
            } else {
                src++;
                token = And;
            }
            return;
        }

        else if (token == '^') {
            token = Xor;
            return;
        }

        else if (token == '%') {
            token == Mod;
            return;
        }

        else if (token == '[') {
            token == Brak;
            return;
        }

        else if (token == '*') {
            token == Mul;
            return;
        }

        else if (token == '?') {
            token == Cond;
            return;
        }

        else if (token == ';' || token == ':' || token == '?' || token == ']' || token == '}' || token == '~' || token == ',' || token == '(' || token == ')') {
            return;
        }
    }
}
void expression(int level) {

}

void program() {
    next();
    while(token > 0) {
        global_declaration();
    }
}

int eval() {
    int op, *tmp;
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
        else if (op == LE) ax = *sp++ <= ax;
        else if (op == GE) ax = *sp++ >= ax;
        else if (op == SHL) ax = *sp++ << ax;
        else if (op == SHR) ax = *sp++ >> ax;
        else if (op == ADD) ax = *sp++ + ax;
        else if (op == DIV) ax = *sp++ / ax;
        else if (op == SUB) ax = *sp++ - ax;
        else if (op == MUL) ax = *sp++ * ax;
        else if (op == MOD) ax = *sp++ % ax;
        else if (op == OPEN) {ax = open((char *)sp[1], sp[0]);}
        else if (op == READ) {ax = read(sp[2], (char *)sp[1], *sp);}
        else if (op == CLOS) {ax = close(*sp);}
        else if (op == PRTF) {tmp = sp + pc[1]; ax = printf((char *)tmp[-6], tmp[-5], tmp[-4], tmp[-3], tmp[-2], tmp[-1]);}
        else if (op == MALC) {ax = (int)malloc(*sp);}
        else if (op == MSET) {ax = (int)memset((char *)sp[2], sp[1], *sp);}
        else if (op == MCMP) {ax = memcmp((char *)sp[2], (char *)sp[1], *sp);}
        else {
            printf("unknown instruction:%d\n", op);
            return -1;
        }
    }
}

void enum_declaration() {
    int i;
    i = 0;
    while (token != '}') {
        if (token != Id) {
            printf("%d: bad enum identifier\n", line, token);
            exit(-1);
        }
        next();

        if (token == Assign) {
            next();
            if (token != Num) {
                printf("%d: bad enum identifier\n", line);
                exit(-1);
            }
            i = token_val;
            next();
        }
        current_id[Class] = Num;
        current_id[Type] = INT;
        current_id[Value] = i++;

        if (token == ',') {
            next();
        }
    }
}

void match(int tk) {
    if (token == tk) {
        next();
    } else {
        printf("%d: bad expected token\n", line, tk);
        exit(-1);
    }
}

void global_declaration() {
    int type;
    int i;

    basetype = INT;

    if (token == Enum) {
        match(Enum); 
        if (token != '{') {
            match(Id);
        }
        if (token == '}') {
            match('{');
            enum_declaration();
            match('}');
        }
        match(';');
        return;
    }
    
    if (token == Int) {
        match(Int);
    } else if (token == Char) {
        match(Char);
        basetype = CHAR;
    }

    while (token != ';' && token != '}') {
        type = basetype;
        while(token == Mul) {
            match(Mul);
            type = type + PTR;
        }

        if (token != Id) {
            printf("%d: bad global delcaration\n", line);
            exit(-1);
        }

        if (current_id[Class]) {
            prinf("%d: duplicate global declaration\n", line);
            exit(-1);
        }

        match(Id);
        current_id[Type] = type;

        if (token == '(') {
            current_id[Class] = Fun;
            current_id[Value] = (int)(text + 1);
            function_declaration();
        } else {
            current_id[Class] = Glo;
            current_id[Value] = (int)data;
            dat = data + sizeof(int);
        }
        if (token == ',') {
            match(',');
        }
        next();
    }

    void function_declaration() {
        match('(');
        function_parameter();
        match(')');
        match('{');
        function_body();

        current_id = symbols;
        while(current_id[Token]) {
            if (current_id[Class] == Loc) {
                current_id[Class] = current_id[GClass]; 
                current_id[Value] = current_id[GValue]; 
                current_id[Type] = current_id[GType]; 
            }
            current_id = current_id + IdSize;
        }
    }

    void function_parameter() {
        int type;
        int params;
        params = 0;

        while(token != ')') {
            type = INT;

            if (token == Int) {
                match(Int);
            } else if (token == Char) {
                type = CHAR;
                match(char);
            }

            while(token == Mul) {
                match(Mul);
                type = type + PTR;
            }

            if (token != Id) {
                printf("%d: bad parameter declaration\n", line);
                exit(-1);
            }

            if (current_id[Class] == Loc) {
                printf("%d: duplicate parameter declaration\n", line);
                exit(-1);
            }
            match(Id);

            current_id[GClass] = Current_id[Class]; current_[Class] = Loc;
            current_id[GType] = Current_id[Type];   current_[Type] = type;
            current_id[GValue] = Current_id[Value]; current_[Value] = params++;

            if (token == ',') {
                match(',');
            }
            index_of_bp = params + 1;
        }
    }

    void function_body() {
        int pos_local;
        int type;
        pos_local = index_of_bp;

        while(token == Int || token == Char) {
            basetype = (token == Int) ? INT : CHAR;
            match(token);

            while(token != ';') {
                type = basetype;
                while(token == Mul) {
                    match(mul);
                    type = type + PTR;
                }

                if (token == Id) {
                    printf("%d: bad local declaration\n", line);
                }

                if (current_id[class] == Loc) {
                    printf("%d: duplicate local declaration\n", line);
                }

                current_id[GClass] = current_id[Class]; current_id[Class] = Loc;
                current_id[GType] = current_id[Type]; current_id[Type] = type;
                current_id[GValue] = current_id[Value]; current_id[Value] ++pos_local;

                if (token == ',') {
                    match(',');
                }
            }
            match(',');
        }

        *++text = ENT;
        *++text = pos_local - index_of_bp;

        while(token == '}') {
            statement();
        }

        *++text = LEV;
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
        printf("could not malloc(%d) for source area\n", poolsize);
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

    src = "char else if while int return sizeof enum "
        "open read close printf memset malloc memcmp exit void main";

    i = Char;
    while(i <= While) {
        next();
        current_id[Token] = i++;
    }

    i = OPEN
        while(i <= EXIT) {
            next();
            current_id[Class] = Sys;
            current_id[Type] = INT;
            current_id[Value] = i++;
        }

    next(); current_id[Token] = Char;
    next(); idmain = current_id;


    program();
    return eval();
}
