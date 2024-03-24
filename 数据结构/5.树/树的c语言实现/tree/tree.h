#ifndef TREE_H
#define TREE_H

#define OK 1
#define MAX_NODE_SIZE 50
typedef enum { FALSE = 0, TRUE } Bool;
typedef char TElemType;

//二叉树结点结构
typedef struct BiNode {
    TElemType data;
    struct BiNode* lchild, * rchild; //左右孩子指针
}BiNode, * BiTree;

//线索二叉树
typedef struct BiThrNode {
    int data;
    int ltag, rtag;
    struct BiThrNode* lchild, * rchild;
}BiThrNode, * BiThrTree;
/*
//树的存储----双亲表示法----结点结构
typedef struct PTNode {
    TElemType data;
    int parent;//双亲位置域
} PTNode;

//树的存储----双亲表示法----树结构
#define MAX_TREE_SIZE 100
typedef struct {
    PTNode nodes[MAX_TREE_SIZE];
    int r, n;// 根结点的位置和结点个数
} PTree;

//树的存储----孩子链表----孩子结点结构
typedef struct CTNode {
    int child;
    struct CTNode* next;
}*ChildPtr;

//树的存储----孩子链表----双亲结点结构
typedef struct {
    TElemType data;
    ChildPtr firstchild;// 孩子链表头指针
} CTBox;

//树的存储----孩子链表----树结构
typedef struct {
    CTBox nodes[MAX_TREE_SIZE];
    int n，r;//结点数和根结点的位置
}CTree;

//树的存储----孩子兄弟表示法----树结构
typedef struct CSNode {
    TElemType data;
    struct CSNode* firstchild, * nextsibling;
}CSNode, * CSTree;

//哈夫曼树结构
typedef struct {
    int weight;
    int parent, lch, rch;
}HTNode, * HuffmanTree;
*/
BiTree CreateBiTree();                      //创建二叉树，返回指向创建的二叉树的头结点的指针
void PreOrderTraverse(const BiTree T);      //前序遍历
void InOrderTraverse(const BiTree T);       //中序遍历
void PostOrderTraverse(const BiTree T);     //后序遍历
void visit(BiTree T);                       //访问节点
void LevelOrderTraverse(const BiTree T);           //层序遍历
BiTree Copy(const BiTree T);                //复制二叉树
int Depth(const BiTree T);                  //且二叉树深度
int NodeCount(const BiTree T);              //求二叉树结点数
int LeafCount(const BiTree T);              //求二叉树叶子结点树
BiThrTree BiTree2BiThrTree(const BiTree T);     //构造相应二叉树的二叉搜索树
/*
void CreatHuffmanTree(HuffmanTree HT, int n);
void CreatHuffmanCode(HuffmanTree HT, HuffmanCode& HC, int n);
*/
#endif
