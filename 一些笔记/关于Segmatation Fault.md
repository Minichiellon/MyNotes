**SegmentationFault** 在我实现数据结构的过程中不知道出现了多少次！所以在此记录一下。  
该错误出现的根本原因是 **对内存的错误操作** 。  下面两篇文章很详细的解释了这个错误。  
[WikiPedia_内存分段错误](https://zh.wikipedia.org/wiki/%E8%A8%98%E6%86%B6%E9%AB%94%E5%8D%80%E6%AE%B5%E9%8C%AF%E8%AA%A4)  
[StackOverFlow_What is a segmentation fault?](https://stackoverflow.com/questions/2346806/what-is-a-segmentation-fault)  

StackOverFlow最高赞回答给出会触发segmentation fault的三种情况为：  

**示例1**  
```cpp
int *p = NULL;
*p = 1;
```
>指针p不指向任何有效地址，但``*p = 1``却尝试修改p指向的地址上的内容，显然这是不合理的。

<br/>

**示例2**  
```cpp
char *str = "Foo"; // Compiler marks the constant string as read-only
*str = 'b'; // Which means this is illegal and results in a segfault
```
>指针str指向常量字符串，常量字符串是只读的，因此修改会触发segmentation fault。

<br/>

**示例3**  
```cpp
char *p = NULL;
{
    char c;
    p = &c;
}
// Now p is dangling
*p = A;
```
>在块内定义变量``c``且令指针``p``指向``c``，当程序执行到块外时，变量``c``被释放，此时指针``p``则会悬空，不指向任何有效地址，尝试修改p指向的地址上的内容则会触发segmentation fault。

<br/>

以下是维基百科上指出的会引发段错误的行为：  
- 引用空指针
- 引用未初始化的野指针
- 引用已经被调用free()函数释放了的悬空指针
- 缓冲区溢出
- 堆栈溢出(一种方法是无限递归)
- 运行未正确编译的程序（尽管存在编译时错误，某些编译器依然会输出可执行文件）

```cpp
char *p1 = NULL;           // 空指针
char *p2;                  // 野指针：未被初始化的指针
char *p3  = malloc(10 * sizeof(char));  // 获取动态内存并初始化指针(假设malloc函数没有出错)
free(p3);                  // p3所指向的动态内存被释放掉，p3变成悬空指针

char c1 = *p1;             // 试图访问空指针所指向的内存总是会导致储存器段错误
char c2 = *p2;             // 试图访问野指针所指向的内存会导致随机数据
char c3 = *p3;             // 试图访问悬空指针所指向的内存可能会导致随机数据
```
另外， **数组访问越界** 也会触发该错误。
