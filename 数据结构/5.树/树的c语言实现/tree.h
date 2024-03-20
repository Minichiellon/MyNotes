#define TRUE 1
#define FALSE 0
#define OK 1
#define MAX_NODE_SIZE 20
typedef enum { false = 0, true } bool;
typedef char TElemType;

//二叉树结点结构
typedef struct BiNode {
    TElemType data;
    struct BiNode* lchild, * rchild; //左右孩子指针
}BiNode, * BiTree;
/*
//线索二叉树
typedef struct BiThrNode {
    int data;
    int ltag, rtag;
    struct BiThrNode* lchild, rchild;
}BiThrNode, * BiThrTree;

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
BiTree CreateBiTree();
bool PreOrderTraverse(const BiTree T);
bool InOrderTraverse(const BiTree T);
bool PostOrderTraverse(const BiTree T);
/*
void LevelOrder(const BiTree* T);
int Copy(BiTree T, BiTree* NewT);
int Depth(BiTree T);
int NodeCount(BiTree T);
int LeafCount(BiTree T);
void CreatHuffmanTree(HuffmanTree HT, int n);
void CreatHuffmanCode(HuffmanTree HT, HuffmanCode& HC, int n);
*/