#include "tree.h"
#include <stdio.h>
int main(void) {
    BiTree tr;
    //创建二叉树、复制二叉树
    tr = CreateBiTree();
    BiTree tr_copy = Copy(tr);
    printf("\n");
    //前序遍历
    printf("Pre order traverse:\t");
    PreOrderTraverse(tr_copy);
    printf("\n");
    //中序遍历
    printf("In order traverse:\t");
    InOrderTraverse(tr_copy);
    printf("\n");
    //后序遍历
    printf("Post order traverse:\t");
    PostOrderTraverse(tr_copy);
    printf("\n");
    
    //层序遍历
    printf("Level order traverse:\t");
    LevelOrderTraverse(tr_copy);
    printf("\n");
    
    //深度、结点数、叶子结点数
    
    printf("depth of tree tr = %d\n",Depth(tr));
    printf("nodes of tree tr = %d\n",NodeCount(tr));
    printf("leaf nodes of tree tr = %d\n",LeafCount(tr));
    
    return 0;
}
