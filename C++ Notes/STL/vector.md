## 1.概述：  
---
**vector**是一种可以改变大小的数组。

vector中的元素使用连续的存储位置，具有和数组一样高效的访问元素的方式。  

当vector需要增加大小时，其会重新分配一个更大的空间，然后复制原来的内容至新空间。当然不会在每次插入元素时都这样做，因为这将耗费大量时间。当其空余位置较少时(size快接近capacity),才会重新分配空间。  

vector以更多的内存换取了管理存储和动态增长的能力。而且其具有高效的元素访问效率，并且操作尾部元素也相对高效。但在其他位置插入、删除元素时效率较低。  
>关于vector的详细信息，请参阅[cplusplus.com](https://legacy.cplusplus.com/reference/vector/vector/)
## 2.类方法：  
---
### 2.1构造函数、析构函数：  
**构造函数**  
<table>
  <tbody>
    <tr>
      <td>类型</td>
      <td>函数声明</td>
      <td>示例</td>
      <td>说明</td>
    </tr>
    <tr>
      <td>default</td>
      <td>vector (const allocator_type&amp; alloc = allocator_type());</td>
      <td>vector&lt;int&gt; first;</td>
      <td>空的int数组</td>
    </tr>
    <tr>
      <td>fill</td>
      <td><br>vector (size_type n, const value_type&amp; val = value_type(),<br>&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;const allocator_type&amp; alloc = allocator_type());</td>
      <td>vector&lt;int&gt; second (4,100);</td>
      <td>4个int，值都为100</td>
    </tr>
    <tr>
      <td>range</td>
      <td><br>template &lt;class InputIterator&gt;<br>&nbsp; &nbsp; &nbsp; &nbsp; &nbsp;vector (InputIterator first, InputIterator last,<br>&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;const allocator_type&amp; alloc = allocator_type());</td>
      <td>vector&lt;int&gt;&nbsp; third (second.begin(),second.end());</td>
      <td>通过另一个序列容器构造</td>
    </tr>
    <tr>
      <td>copy</td>
      <td>vector (const vector&amp; x);</td>
      <td>vector&lt;int&gt; fourth (third);</td>
      <td>复制另一个vector的内容</td>
    </tr>
  </tbody>
  <colgroup>
    <col style="width: 12.5556%;">
    <col style="width: 48.4444%;">
    <col style="width: 16.2222%;">
    <col style="width: 22.7778%;">
  </colgroup>
</table>

>次为c++98的标准，c++11起支持移动构造和列表初始化。[点此了解更多](https://legacy.cplusplus.com/reference/vector/vector/vector/)

**析构函数**
　``~vector();``销毁vector对象。  
### 2.2访问方式：  
<table>
  <tbody>
    <tr>
      <td>函数</td>
      <td>示例：vector&lt;int&gt; myvector (10);</td>
      <td>说明</td>
    </tr>
    <tr>
      <td>operator[]</td>
      <td>myvector[1] = 0;</td>
      <td>访问1号元素</td>
    </tr>
    <tr>
      <td>at</td>
      <td>myvector.at(1) = 0；</td>
      <td>访问1号元素</td>
    </tr>
    <tr>
      <td>front</td>
      <td>myvector.front()</td>
      <td>访问第一个元素</td>
    </tr>
    <tr>
      <td>back</td>
      <td>myvector.back()</td>
      <td>访问最后一个元素</td>
    </tr>
    <tr>
      <td>data</td>
      <td>int* p = myvector.data();</td>
      <td>返回指向第一个元素的指针</td>
    </tr>
  </tbody>
  <colgroup>
    <col>
    <col>
    <col>
  </colgroup>
</table>

### 2.3迭代器：  
<table>
  <tbody>
    <tr>
      <td>函数</td>
      <td>示例：vector&lt;int&gt; myvector (10);</td>
      <td>说明</td>
    </tr>
    <tr>
      <td>begin</td>
      <td>vector&lt;int&gt;::iterator it = myvector.begin()</td>
      <td>指向序列容器开头的迭代器。</td>
    </tr>
    <tr>
      <td>end</td>
      <td>vector&lt;int&gt;::iterator it = myvector.end()</td>
      <td>指向序列末尾之后的元素的迭代器。</td>
    </tr>
    <tr>
      <td>rbegin</td>
      <td>vector&lt;int&gt;::iterator it = myvector.rbegin()</td>
      <td>指向序列容器反向起始的反向迭代器。</td>
    </tr>
    <tr>
      <td>rend</td>
      <td>vector&lt;int&gt;::iterator it = myvector.rend()</td>
      <td>指向序列容器的反向末端的反向迭代器。</td>
    </tr>
    <tr>
      <td>cbegin</td>
      <td>vector&lt;int&gt;::iterator it = myvector.cbegin()</td>
      <td>指向序列容器开头的const迭代器。</td>
    </tr>
    <tr>
      <td>cend</td>
      <td>vector&lt;int&gt;::iterator it = myvector.cend()</td>
      <td>指向序列末尾之后的元素的const迭代器。</td>
    </tr>
    <tr>
      <td>crbegin</td>
      <td>vector&lt;int&gt;::iterator it = myvector.crbegin()</td>
      <td>指向序列反向起始的const反向迭代器。</td>
    </tr>
    <tr>
      <td>crend</td>
      <td>vector&lt;int&gt;::iterator it = myvector.crend()</td>
      <td>指向序列反向末端的const反向迭代器。</td>
    </tr>
  </tbody>
  <colgroup>
    <col style="width: 21.1111%;">
    <col style="width: 43.1111%;">
    <col style="width: 35.7778%;">
  </colgroup>
</table>

>正向迭代器遍历vector``for (it = myvector.begin() ; it != myvector.end(); ++it)``将得到vector的正向序列。
>反向迭代器遍历vector``for (it = myvector.rbegin() ; it != myvector.rend(); ++it)``将得到vector的反向序列。
>对迭代器解引用``*it``将得到迭代器指向的元素。

### 2.4与大小相关的：  
<table>
  <tbody>
    <tr>
      <td>函数</td>
      <td>函数声明</td>
      <td>示例：vector&lt;int&gt; myvector (10);</td>
      <td>说明</td>
    </tr>
    <tr>
      <td>size</td>
      <td>size_type size() const;</td>
      <td>myvector.size()</td>
      <td>返回vector中实际元素的数量</td>
    </tr>
    <tr>
      <td>max_size</td>
      <td>size_type max_size() const;</td>
      <td>myvector.max_size()</td>
      <td>返回vector所能容纳的最大元素数</td>
    </tr>
    <tr>
      <td>resize</td>
      <td>void resize (size_type n, value_type val = value_type());</td>
      <td>myvector.resize(8)；<br>myvector.resize(8,100);</td>
      <td>调整容器大小使之包含n个元素</td>
    </tr>
    <tr>
      <td>capacity</td>
      <td>size_type capacity() const;</td>
      <td>myvector.capacity()</td>
      <td>返回已分配的存储容量大小</td>
    </tr>
    <tr>
      <td>empty</td>
      <td>bool empty() const;</td>
      <td>myvector.empty()</td>
      <td>检测vector是否为空</td>
    </tr>
    <tr>
      <td>reverse</td>
      <td>void reserve (size_type n);</td>
      <td>myvector.reverse(100)</td>
      <td>请求更改容量</td>
    </tr>
    <tr>
      <td>shirk_to_fit</td>
      <td>void shrink_to_fit();</td>
      <td>myvector.shirk_to_fit()</td>
      <td>请求容器减少其容量以适合其大小。</td>
    </tr>
  </tbody>
  <colgroup>
    <col style="width: 12.2222%;">
    <col style="width: 25.8889%;">
    <col style="width: 27.8889%;">
    <col style="width: 34%;">
  </colgroup>
</table>

### 2.5修改：  
<table>
  <tbody>
    <tr>
      <td>函数</td>
      <td>函数声明</td>
      <td>示例：vector&lt;int&gt; myvector (10);</td>
      <td>说明</td>
    </tr>
    <tr>
      <td>assign</td>
      <td><br>template &lt;class InputIterator&gt;<br>&nbsp; void assign (InputIterator first, InputIterator last);</td>
      <td><br>myvector.assign (it,&nbsp; it+5);<br></td>
      <td>将迭代器指向的区间&nbsp; [it,&nbsp; it+5)&nbsp; 赋值给myvector</td>
    </tr>
    <tr>
      <td></td>
      <td>void assign (size_type n, const value_type&amp; val);</td>
      <td>myvector.assign (7,100);</td>
      <td>给myvector赋值7个100</td>
    </tr>
    <tr>
      <td>push_back</td>
      <td>void push_back (const value_type&amp; val);</td>
      <td>myvector.push_back(1)</td>
      <td>在末尾添加元素1</td>
    </tr>
    <tr>
      <td>pop_back</td>
      <td>void pop_back ();</td>
      <td><br>myvector.pop_back()</td>
      <td>删除最后一个元素</td>
    </tr>
    <tr>
      <td>insert</td>
      <td>iterator insert (iterator position, const value_type&amp; val);</td>
      <td>auto it = myvector.insert ( &nbsp;it, 200 );</td>
      <td>在位置it之前插入200</td>
    </tr>
    <tr>
      <td></td>
      <td>void insert (iterator position, size_type n, const value_type&amp; val);</td>
      <td>myvector.insert (it,2,300);</td>
      <td>在位置it之前插入2个300</td>
    </tr>
    <tr>
      <td></td>
      <td><br>template &lt;class InputIterator&gt;<br>&nbsp; &nbsp; void insert (iterator position, InputIterator first, InputIterator last);</td>
      <td>myvector.insert (it,first.begin(),first.end());</td>
      <td>在位置it之前插入<br>[&nbsp; &nbsp;first.begin(),first.end()&nbsp; )之间的元素</td>
    </tr>
    <tr>
      <td>erase</td>
      <td>iterator erase (iterator position);</td>
      <td>myvector.erase(it)</td>
      <td>删除位置it的元素</td>
    </tr>
    <tr>
      <td></td>
      <td>iterator erase (iterator first, iterator last);</td>
      <td>myvector.erase(it，it+5)</td>
      <td>删除区间&nbsp; [it，it+5)&nbsp; 之间的元素</td>
    </tr>
    <tr>
      <td>swap</td>
      <td>void swap (vector&amp; x);</td>
      <td>myvector.swap(first)</td>
      <td>交换myvector和first的内容</td>
    </tr>
    <tr>
      <td>clear</td>
      <td>void clear();</td>
      <td>myvector.clear()</td>
      <td>从vector中移除所有元素(这些元素被销毁)，使容器的大小为0。</td>
    </tr>
    <tr>
      <td>emplace</td>
      <td><br>template &lt;class... Args&gt;<br>iterator emplace (const_iterator position, Args&amp;&amp;... args);</td>
      <td>myvector.emplace ( it, 200 );</td>
      <td>就地构造元素并插入</td>
    </tr>
    <tr>
      <td>emplace_back</td>
      <td><br>template &lt;class... Args&gt;<br>&nbsp; void emplace_back (Args&amp;&amp;... args);</td>
      <td>myvector.emplace ( 200 );</td>
      <td>构造并在末尾插入元素</td>
    </tr>
  </tbody>
  <colgroup>
    <col style="width: 13.5556%;">
    <col style="width: 24.5556%;">
    <col style="width: 29.3333%;">
    <col style="width: 32.5556%;">
  </colgroup>
</table>

>emplace和emplace_back要比insert和push_back效率高一点，因为少了**拷贝副本**及**类型转换**的时间。

### 2.6重载的一些运算符
<table>
  <tbody>
    <tr>
      <td>操作符</td>
      <td>含义</td>
    </tr>
    <tr>
      <td>==</td>
      <td>size相等且相应位置的元素相等则为真</td>
    </tr>
    <tr>
      <td>！=</td>
      <td>size不等或相应位置的元素存在不等则为真</td>
    </tr>
    <tr>
      <td>&lt;</td>
      <td>按相应位置从前往后比较，以第一个不等的相应位置的元素大小关系为准</td>
    </tr>
    <tr>
      <td>&lt;=</td>
      <td>按相应位置从前往后比较，以第一个不等的相应位置的元素大小关系为准</td>
    </tr>
    <tr>
      <td>&gt;</td>
      <td>按相应位置从前往后比较，以第一个不等的相应位置的元素大小关系为准</td>
    </tr>
    <tr>
      <td>&gt;=</td>
      <td>按相应位置从前往后比较，以第一个不等的相应位置的元素大小关系为准</td>
    </tr>
  </tbody>
  <colgroup>
    <col style="width: 35.7778%;">
    <col style="width: 64.1111%;">
  </colgroup>
</table>
