#include <stdio.h>

extern int x;
int y;

static void f(void){
    static char z = 'Z';
    puts("a.c: f()");
}

void g(void){
    puts("a.c: g()");
    f();
}

void h(void){
    puts("a.c: h()");
    g();
}