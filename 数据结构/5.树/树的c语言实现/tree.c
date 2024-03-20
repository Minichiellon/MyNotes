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
            if (!(T = (BiNode*)malloc(sizeof(BiNode)))) //�ڴ�����ʧ��
                return NULL;//T=new BiTNode
            T->data = ch[count];  // ���ɸ����
            count++;
            T->lchild = CreateBiTree();//����������
            T->rchild = CreateBiTree();//����������
        }
    }
    return T;
} // CreateBiTree

bool PreOrderTraverse(BiTree T) {
    if (!T) return OK; //�ն�����
    else {
        visit(T); //���ʸ����
        PreOrderTraverse(T->lchild);//�ݹ����������
        PreOrderTraverse(T->rchild);//�ݹ����������
    }
}

bool InOrderTraverse(BiTree T) {
    if (!T) return OK; //�ն�����
    else {
        InOrderTraverse(T->lchild);//�ݹ����������
        visit(T); //���ʸ����
        InOrderTraverse(T->rchild);//�ݹ����������
    }
}

bool PostOrderTraverse(BiTree T) {
    if (!T) return OK; //�ն�����
    else {
        PostOrderTraverse(T->lchild);//�ݹ����������
        PostOrderTraverse(T->rchild);//�ݹ����������
        visit(T);//���ʸ����
    }
}
/*
void LevelOrder(const BiTree* T) {
    BTNode* p;
    SqQueue* qu;//��ʼ������
    InitQueue(qu);//�����ָ��������
    enQueue(qu, T);
    while (!QueueEmpty(qu)) {//�Ӳ�Ϊ�գ���ѭ��
        deQueue(qu, p);//���ӽ��p
        printf("%c", p->data);//���ʽ��p
        if (p->lchild != NULL) enQueue(qu, p->lchild);//������ʱ�������
        if (p->rchild != NULL) enQueue(qu, p->rchild);//���Һ���ʱ�������
    }
}

int Copy(BiTree T, BiTree* NewT) {
    if (T == NULL) { //����ǿ�������0
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
    if (T == NULL) return 0;//����ǿ�������0
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
int LeafCount(BiTree T) {//����ǿ�������0
    if (T == NULL) return 0;
    if (T->lchild == NULL && T->rchild == NULL)//�����Ҷ�ӽ�㷵��1
        return 1;
    else
        return LeafCount(T->lchild) + LeafCount(T->rchild);
}

void CreatHuffmanTree(HuffmanTree HT, int n) { //�����������--�������㷨
    if (n <= 1) return;
    m = 2 * n - 1;//���鹲2n-1��Ԫ��
    HT = new HTNode[m + 1]; //0�ŵ�Ԫδ�ã�HT[m]��ʾ�����
    for (i = 1; i <= m; ++i) { //��2n-1��Ԫ�ص�lch��rch��parent��Ϊ0
        HT[il.lch = 0;
        HT[i].rch = 0;
        HT[i].parent = 0;
    }
    for (i = 1; i <= n; ++i) cin >> HT[i].weight; //����ǰn��Ԫ�ص�weightֵ
    //��ʼ�����������濪ʼ������������
    for (i = n + 1; i <= m; i++) {  //�ϲ�����n-1�����--����Huffman��
        Select(HT, i - 1, s1, s2); //��HT[k](1��k��i-1)��ѡ��������˫����Ϊ0
        //��Ȩֵ��С�Ľ��,������������HT�е����s1��s2
        HT[s1].parent = i;  HT[s2].parent = i;  //��ʾ��F��ɾ��s1,s2
        HT[i].lch = s1;     HT[i].rch = s2;    //s1,s2�ֱ���Ϊi�����Һ���
        HT[i].weight = HT[s1].weight + HT[s2].weight; //i��ȨֵΪ���Һ���Ȩֵ֮��
    }
}

void CreatHuffmanCode(HuffmanTree HT, HuffmanCode& HC, int n) {
    //��Ҷ�ӵ���������ÿ���ַ��Ĺ��������룬�洢�ڱ����HC��
    HC = new char* [n + 1];    //����n���ַ������ͷָ��ʸ��
    cd = new char[n];       //������ʱ��ű���Ķ�̬����ռ�
    cd[n - 1] = '\0';          //���������
    for (i = 1; i <= n; ++i) {     //����ַ������������
        start = n - 1; c = i; f = HT[i].parent;
        while (f != 0) {    //��Ҷ�ӽ�㿪ʼ���ϻ��ݣ�ֱ�������
            --start;      //����һ��start��ǰָһ��λ��
            if (HT[f].lchild == c) cd[start] = '0';    //���c��f�����ӣ������ɴ���0
            else                  cd[start] = '1';    //���c��f���Һ��ӣ������ɴ���1
            c = f; f = HT[f].parent;    //�������ϻ���
        }                               //�����i���ַ��ı���
        HC[i] = new char[n - start];     // Ϊ��i ���ַ����������ռ�
        strcpy(HC[i], &cd[start]);      //����õı������ʱ�ռ�cd���Ƶ�HC�ĵ�ǰ����
        delete cd;    //�ͷ���ʱ�ռ�
    }
}// CreatHuffanCode
*/