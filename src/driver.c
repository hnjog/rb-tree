#include<stdio.h>
#include <stdlib.h>
#include "rbtree.h"

int main(int argc, char *argv[]) {
    rbtree *t = new_rbtree();

    free(t);

    return 0;
}