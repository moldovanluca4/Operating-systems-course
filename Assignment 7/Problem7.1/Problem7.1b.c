#include <stdio.h>

extern void h(void);

int x=1;
static double y = 1;
static char z = 'A';

static void g(void){
    puts("b.c: g()");
    h();
}

void f(void){
    puts("b.c: f()");
    g();
}