#define TRUE 1
#define FALSE 0
#define OK 1
#define MAX_NODE_SIZE 20
typedef enum { false = 0, true } bool;
typedef char TElemType;

//���������ṹ
typedef struct BiNode {
    TElemType data;
    struct BiNode* lchild, * rchild; //���Һ���ָ��
}BiNode, * BiTree;
/*
//����������
typedef struct BiThrNode {
    int data;
    int ltag, rtag;
    struct BiThrNode* lchild, rchild;
}BiThrNode, * BiThrTree;

//���Ĵ洢----˫�ױ�ʾ��----���ṹ
typedef struct PTNode {
    TElemType data;
    int parent;//˫��λ����
} PTNode;

//���Ĵ洢----˫�ױ�ʾ��----���ṹ
#define MAX_TREE_SIZE 100
typedef struct {
    PTNode nodes[MAX_TREE_SIZE];
    int r, n;// ������λ�úͽ�����
} PTree;

//���Ĵ洢----��������----���ӽ��ṹ
typedef struct CTNode {
    int child;
    struct CTNode* next;
}*ChildPtr;

//���Ĵ洢----��������----˫�׽��ṹ
typedef struct {
    TElemType data;
    ChildPtr firstchild;// ��������ͷָ��
} CTBox;

//���Ĵ洢----��������----���ṹ
typedef struct {
    CTBox nodes[MAX_TREE_SIZE];
    int n��r;//������͸�����λ��
}CTree;

//���Ĵ洢----�����ֵܱ�ʾ��----���ṹ
typedef struct CSNode {
    TElemType data;
    struct CSNode* firstchild, * nextsibling;
}CSNode, * CSTree;

//���������ṹ
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