## 1.概述：  
---
**vector**是一种可以改变大小的数组。

vector中的元素使用连续的存储位置，具有和数组一样高效的访问元素的方式。  

当vector需要增加大小时，其会重新分配一个更大的空间，然后复制原来的内容至新空间。当然不会在每次插入元素时都这样做，因为这将耗费大量时间。当其空余位置较少时(size快接近capacity),才会重新分配空间。  

vector以更多的内存换取了管理存储和动态增长的能力。而且其具有高效的元素访问效率，并且操作尾部元素也相对高效。但在其他位置插入、删除元素时效率较低。  
>关于vector的详细信息，请参阅[cplusplus.com](https://cplusplus.com/reference/vector/vector/)
## 2.类方法：  
---
### 2.1构造函数、析构函数：  
**构造函数**  

|  类型  |函数声明|示例|说明|
|--------|-------|----|----|
|default |ector (const allocator_type& alloc = allocator_type());|vector<int> first;|空的int数组|
|fill    |vector (size_type n, const value_type& val = value_type(), const allocator_type& alloc = allocator_type());	|vector<int> second (4,100);|4个int，值都为100|
|range   |template <class InputIterator> vector (InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type());|vector<int>  third (second.begin(),second.end());	|通过另一个序列容器构造|
|copy    |vector (const vector& x);|vector<int> fourth (third);|复制另一个vector的内容|


>次为c++98的标准，c++11起支持移动构造和列表初始化。[点此了解更多](https://cplusplus.com/reference/vector/vector/vector/)

**析构函数**  
　``~vector();``销毁vector对象。  

### 2.2访问方式：  

|    函数   |示例：vector<int> myvector (10);	|          说明         |
|-----------|--------------------------------|-----------------------|
|operator[]	|myvector[1] = 0;                |访问1号元素             |
|at         |myvector.at(1) = 0；            |访问1号元素             |
|front      |myvector.front()                |访问第一个元素          |
|back       |myvector.back()                 |访问最后一个元素        |
|data       |int* p = myvector.data();       |返回指向第一个元素的指针 |


### 2.3迭代器：  

|  函数  |      示例：vector<int> myvector (10);       |              说明                |
|-------|---------------------------------------------|----------------------------------|
|begin  |vector<int>::iterator it = myvector.begin()  |指向序列容器开头的迭代器。          |
|end    |vector<int>::iterator it = myvector.end()    |指向序列末尾之后的元素的迭代器。     |
|rbegin |vector<int>::iterator it = myvector.rbegin() |指向序列容器反向起始的反向迭代器。   |
|rend   |vector<int>::iterator it = myvector.rend()   |指向序列容器的反向末端的反向迭代器。 |
|cbegin |vector<int>::iterator it = myvector.cbegin() |指向序列容器开头的const迭代器。     |
|cend   |vector<int>::iterator it = myvector.cend()   |指向序列末尾之后的元素的const迭代器。|
|crbegin|vector<int>::iterator it = myvector.crbegin()|指向序列反向起始的const反向迭代器。  |
|crend  |vector<int>::iterator it = myvector.crend()  |指向序列反向末端的const反向迭代器。  |

>正向迭代器遍历vector``for (it = myvector.begin() ; it != myvector.end(); ++it)``将得到vector的正向序列。  
>反向迭代器遍历vector``for (it = myvector.rbegin() ; it != myvector.rend(); ++it)``将得到vector的反向序列。  
>对迭代器解引用``*it``将得到迭代器指向的元素。  

### 2.4与大小相关的：  

|    函数    |          函数声明          |示例：vector<int> myvector (10);|     说明            |
|------------|---------------------------|-----------------------|-----------------------------|
|size        |size_type size() const;    |myvector.size()        |返回vector中实际元素的数量     |
|max_size    |size_type max_size() const;|myvector.max_size()    |返回vector所能容纳的最大元素数 |
|resize      |void resize (size_type n, value_type val = value_type());|myvector.resize(8)；<br> myvector.resize(8,100);|调整容器大小使之包含n个元素|
|capacity    |size_type capacity() const;|myvector.capacity()    |返回已分配的存储容量大小       |
|empty       |bool empty() const;        |myvector.empty()       |检测vector是否为空            |
|reverse     |void reserve (size_type n);|myvector.reverse(100)  |请求更改容量                  |
|shirk_to_fit|void shrink_to_fit();      |myvector.shirk_to_fit()|请求容器减少其容量以适合其大小。|

### 2.5修改：  

|     函数   |    函数声明    |示例：vector<int> myvector (10);	|    说明    |
|------------|---------------|--------------------------------|------------|
|assign      |template <class InputIterator> void assign (InputIterator first, InputIterator last);|myvector.assign (it,  it+5);|将迭代器指向的区间 [it, it+5) 赋值给myvector|
|assign      |void assign (size_type n, const value_type& val);|myvector.assign (7,100);|给myvector赋值7个100|
|push_back   |void push_back (const value_type& val);|myvector.push_back(1)|在末尾添加元素1|
|pop_back    |void pop_back ();|myvector.pop_back()|删除最后一个元素|
|insert      |iterator insert (iterator position, const value_type& val);|auto it = myvector.insert (it, 200);|在位置it之前插入200|
|insert      |void insert (iterator position, size_type n, const value_type& val);|myvector.insert (it,2,300);|在位置it之前插入2个300|
|insert      |template <class InputIterator> void insert (iterator position, InputIterator first, InputIterator last);|myvector.insert (it,first.begin(),first.end());|在位置it之前插入 [ first.begin(),first.end() ) 之间的元素|
|erase       |iterator erase (iterator position);|myvector.erase(it)|删除位置it的元素|
|erase       |iterator erase (iterator first, iterator last);|myvector.erase(it，it+5)|删除区间 [it，it+5) 之间的元素|
|swap        |void swap (vector& x);|myvector.swap(first)|交换myvector和first的内容|
|clear       |void clear();|myvector.clear()|从vector中移除所有元素(这些元素被销毁)，使容器的大小为0。|
|emplace     |template <class... Args> iterator emplace (const_iterator position, Args&&... args);|myvector.emplace ( it, 200 );|就地构造元素并插入|
|emplace_back|template <class... Args> void emplace_back (Args&&... args);|myvector.emplace_back(200);|构造并在末尾插入元素|

>emplace和emplace_back要比insert和push_back效率高一点，因为少了**拷贝副本**及**类型转换**的时间。

### 2.6重载的一些运算符

|操作符|                            含义                           |
|------|----------------------------------------------------------|
|==    |size相等且相应位置的元素相等则为真                           |
|!=    |size不等或相应位置的元素存在不等则为真                       |
|<     |按相应位置从前往后比较，以第一个不等的相应位置的元素大小关系为准|
|<=    |按相应位置从前往后比较，以第一个不等的相应位置的元素大小关系为准|
|>     |按相应位置从前往后比较，以第一个不等的相应位置的元素大小关系为准|
|>=    |按相应位置从前往后比较，以第一个不等的相应位置的元素大小关系为准|
