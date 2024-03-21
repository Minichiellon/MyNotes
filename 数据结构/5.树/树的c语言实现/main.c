#include "tree.h"
#include <stdio.h>
int main(void) {
    //BiTree tr[7];
    BiTree tr;
    tr = CreateBiTree();

    PreOrderTraverse(tr);
    printf("\n");
    InOrderTraverse(tr);
    printf("\n");
    PostOrderTraverse(tr);
    return 0;
}
