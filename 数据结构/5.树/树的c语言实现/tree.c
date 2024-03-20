#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void visit(BiTree T) {
    printf("%c", T->data);
}

BiTree CreateBiTree() {
    static int count = 0;
    BiTree T = NULL;
    static char ch[MAX_NODE_SIZE] = "\0";
    if(count == 0){
        printf("Enter the sequence of the data, \"#\" means null\n");
        scanf_s("%s", ch, MAX_NODE_SIZE);//ch = getchar();
    }
    if (ch[count] != '\n'){
        if (ch[count] == '#')
            return NULL;
        else {
            if (!(T = (BiNode*)malloc(sizeof(BiNode)))) //内存申请失败
                return NULL;//T=new BiTNode
            T->data = ch[count];  // 生成根结点
            count++;
            T->lchild = CreateBiTree();//构造左子树
            T->rchild = CreateBiTree();//构造右子树
        }
    }
    return T;
} // CreateBiTree

bool PreOrderTraverse(BiTree T) {
    if (!T) return OK; //空二叉树
    else {
        visit(T); //访问根结点
        PreOrderTraverse(T->lchild);//递归遍历左子树
        PreOrderTraverse(T->rchild);//递归遍历右子树
    }
}

bool InOrderTraverse(BiTree T) {
    if (!T) return OK; //空二叉树
    else {
        InOrderTraverse(T->lchild);//递归遍历左子树
        visit(T); //访问根结点
        InOrderTraverse(T->rchild);//递归遍历右子树
    }
}

bool PostOrderTraverse(BiTree T) {
    if (!T) return OK; //空二叉树
    else {
        PostOrderTraverse(T->lchild);//递归遍历左子树
        PostOrderTraverse(T->rchild);//递归遍历右子树
        visit(T);//访问根结点
    }
}
/*
void LevelOrder(const BiTree* T) {
    BTNode* p;
    SqQueue* qu;//初始化队列
    InitQueue(qu);//根结点指针进入队列
    enQueue(qu, T);
    while (!QueueEmpty(qu)) {//队不为空，则循环
        deQueue(qu, p);//出队结点p
        printf("%c", p->data);//访问结点p
        if (p->lchild != NULL) enQueue(qu, p->lchild);//有左孩子时将其进队
        if (p->rchild != NULL) enQueue(qu, p->rchild);//有右孩子时将其进队
    }
}

int Copy(BiTree T, BiTree* NewT) {
    if (T == NULL) { //如果是空树返回0
        New T = NULL;
        return 0;
    else {
        NewT = new BiTNode;
        NewT->data = T->data;
        Copy(T->lChild, NewT->lchild);
        Copy(T->rChild, NewT->rchild);
    }
}

int Depth(BiTree T) {
    if (T == NULL) return 0;//如果是空树返回0
    else {
        m = Depth(T->IChild);
        n = Depth(T->rChild);
        if (m > n) return(m + 1);
        else return(n + 1);
    }
}

int NodeCount(BiTree T) {
    if (T == NULL)
        return 0;
    else
        return NodeCount(T->lchild) + NodeCount(T->rchild) + 1;
}
int LeafCount(BiTree T) {//如果是空树返回0
    if (T == NULL) return 0;
    if (T->lchild == NULL && T->rchild == NULL)//如果是叶子结点返回1
        return 1;
    else
        return LeafCount(T->lchild) + LeafCount(T->rchild);
}

void CreatHuffmanTree(HuffmanTree HT, int n) { //构造哈夫曼树--哈夫曼算法
    if (n <= 1) return;
    m = 2 * n - 1;//数组共2n-1个元素
    HT = new HTNode[m + 1]; //0号单元未用，HT[m]表示根结点
    for (i = 1; i <= m; ++i) { //将2n-1个元素的lch、rch、parent置为0
        HT[il.lch = 0;
        HT[i].rch = 0;
        HT[i].parent = 0;
    }
    for (i = 1; i <= n; ++i) cin >> HT[i].weight; //输入前n个元素的weight值
    //初始化结束，下面开始建立哈夫曼树
    for (i = n + 1; i <= m; i++) {  //合并产生n-1个结点--构造Huffman树
        Select(HT, i - 1, s1, s2); //在HT[k](1≤k≤i-1)中选择两个其双亲域为0
        //且权值最小的结点,并返回它们在HT中的序号s1和s2
        HT[s1].parent = i;  HT[s2].parent = i;  //表示从F中删除s1,s2
        HT[i].lch = s1;     HT[i].rch = s2;    //s1,s2分别作为i的左右孩子
        HT[i].weight = HT[s1].weight + HT[s2].weight; //i的权值为左右孩子权值之和
    }
}

void CreatHuffmanCode(HuffmanTree HT, HuffmanCode& HC, int n) {
    //从叶子到根逆向求每个字符的哈夫曼编码，存储在编码表HC中
    HC = new char* [n + 1];    //分配n个字符编码的头指针矢量
    cd = new char[n];       //分配临时存放编码的动态数组空间
    cd[n - 1] = '\0';          //编码结束符
    for (i = 1; i <= n; ++i) {     //逐个字符求哈夫曼编码
        start = n - 1; c = i; f = HT[i].parent;
        while (f != 0) {    //从叶子结点开始向上回溯，直到根结点
            --start;      //回溯一次start向前指一个位置
            if (HT[f].lchild == c) cd[start] = '0';    //结点c是f的左孩子，则生成代码0
            else                  cd[start] = '1';    //结点c是f的右孩子，则生成代码1
            c = f; f = HT[f].parent;    //继续向上回溯
        }                               //求出第i个字符的编码
        HC[i] = new char[n - start];     // 为第i 个字符串编码分配空间
        strcpy(HC[i], &cd[start]);      //将求得的编码从临时空间cd复制到HC的当前行中
        delete cd;    //释放临时空间
    }
}// CreatHuffanCode
*/