#ifndef LIST_H
#define LIST_H
#include <iostream>
#include <cstdlib>
using namespace std;

template <typename T> struct ListNode;
template <typename T> using ListNodePosi = ListNode<T>*; //列表节点位置
typedef int Rank; // 补充Rank类型定义，代表“秩”（索引）
template <typename T> class List { // 补充List类定义
private:
    Rank _size; // 列表规模（需提前定义Rank类型）
    ListNodePosi<T> head, tail; // 头、尾哨兵节点
    void init(); // 初始化
    void copyNodes(ListNodePosi<T> p, Rank n); // 复制节点
public:
    // 构造函数
    List() { init(); } // 默认构造
    List(ListNodePosi<T> p, Rank n) { copyNodes(p, n); } // 区间构造
    List(List<T> const& L) { copyNodes(L.first(), L._size); } // 拷贝构造
    List(List<T> const& L, Rank r, Rank n); // 区间拷贝构造（声明）
    
    // 析构函数
    ~List(); // 析构（声明）
    
    // 访问操作
    Rank size() const { return _size; } // 规模
    bool empty() const { return _size == 0; } // 是否为空
    ListNodePosi<T> first() const { return head->succ; } // 首节点
    ListNodePosi<T> last() const { return tail->pred; } // 尾节点
    
    // 插入操作
    ListNodePosi<T> insertFirst(T const& e); // 插入首节点
    ListNodePosi<T> insertLast(T const& e); // 插入尾节点
    ListNodePosi<T> insert(ListNodePosi<T> p, T const& e); // 插入p之后
    ListNodePosi<T> insert(T const& e, ListNodePosi<T> p); // 插入p之前
    // 补充测试函数中调用的插入接口
    ListNodePosi<T> insertAfter(ListNodePosi<T> p, T const& e) { return insert(p, e); }
    ListNodePosi<T> insertBefore(ListNodePosi<T> p, T const& e) { return insert(e, p); }
    
    // 删除操作
    T remove(ListNodePosi<T> p); // 删除节点
    Rank clear(); // 清空列表
    
    // 查找与去重
    ListNodePosi<T> find(T const& e, Rank n, ListNodePosi<T> p) const; // 无序查找
    Rank dedup(); // 去重（无序）
    Rank uniquify(); // 去重（有序）
    
    // 排序操作
    void sort(ListNodePosi<T> p, Rank n); // 区间排序
    void insertionSort(ListNodePosi<T> p, Rank n); // 插入排序
    void selectionSort(ListNodePosi<T> p, Rank n); // 选择排序
    ListNodePosi<T> selectMax(ListNodePosi<T> p, Rank n); // 选最大节点
    ListNodePosi<T> merge(ListNodePosi<T> p, Rank n, List<T>& L, ListNodePosi<T> q, Rank m); // 归并
    void mergeSort(ListNodePosi<T>& p, Rank n); // 归并排序
    // 暂不实现radixSort，避免额外依赖，修改sort函数默认分支
    // void radixSort(ListNodePosi<T> p, Rank n); // 基数排序（声明，暂不实现）
    
    // 遍历与访问
    void traverse(void (*visit)(T&)); // 函数指针遍历
    template <typename VST> void traverse(VST& visit); // 函数对象遍历
    ListNodePosi<T> operator[](Rank r) const; // 下标访问
    ListNodePosi<T> search(T const& e, Rank n, ListNodePosi<T> p) const; // 有序查找
    
    // 补充测试函数中调用的print接口
    void print() const {
        for (ListNodePosi<T> p = first(); p != tail; p = p->succ) {
            cout << p->data << " ";
        }
        cout << endl;
    }
};

template <typename T> struct ListNode { //列表节点模板类（以双向链表形式实现）

   T data; 
   ListNodePosi<T> pred, succ; //数值、前驱、后继

   ListNode() {} //针对head和tail的构造
   ListNode ( T const& e, ListNodePosi<T> p = NULL, ListNodePosi<T> s = NULL )
      : data( e ), pred( p ), succ( s ) {} //默认构造器（类T须定义复制方法）

   ListNodePosi<T> insertPred( T const& e ); //紧靠当前节点之前插入新节点
   ListNodePosi<T> insertSucc( T const& e ); //紧随当前节点之后插入新节点
};


template <typename T> void List<T>::init() { //列表初始化，在创建列表对象时统一调用
   head = new ListNode<T>; tail = new ListNode<T>; //创建头、尾哨兵节点
   head->succ = tail; head->pred = NULL; //向前链接
   tail->pred = head; tail->succ = NULL; //向后链接
   _size = 0;
}

template <typename T> //重载下标操作符，以通过秩直接访问列表节点（O(r)效率，虽方便，勿多用）
ListNodePosi<T> List<T>::operator[]( Rank r ) const {
   ListNodePosi<T> p = first();
   while ( 0 < r-- ) p = p->succ;
   return p;
}

template <typename T> //在无序列表内节点p（可能是tail）的n个（真）前驱中，找到等于e的最后者
ListNodePosi<T> List<T>::find( T const& e, Rank n, ListNodePosi<T> p ) const {
   while ( 0 < n-- )
      if ( e == ( p = p->pred )->data ) return p;
   return NULL;
}

template <typename T> ListNodePosi<T> List<T>::insertFirst( T const& e )
   { _size++; return head->insertSucc( e ); } // e当作首节点插入

template <typename T> ListNodePosi<T> List<T>::insertLast( T const& e )
   { _size++; return tail->insertPred( e ); } // e当作末节点插入

template <typename T> ListNodePosi<T> List<T>::insert( ListNodePosi<T> p, T const& e )
   { _size++; return p->insertSucc( e ); } // e当作p的后继插入

template <typename T> ListNodePosi<T> List<T>::insert( T const& e, ListNodePosi<T> p )
   { _size++; return p->insertPred( e ); } // e当作p的前驱插入

template <typename T> //将e紧靠当前节点之前插入于当前节点所属列表（设有哨兵head）
ListNodePosi<T> ListNode<T>::insertPred( T const& e ) {
   ListNodePosi<T> x = new ListNode( e, pred, this );
   pred->succ = x; pred = x;
   return x;
}

template <typename T> //将e紧随当前节点之后插入于当前节点所属列表（设有哨兵tail）
ListNodePosi<T> ListNode<T>::insertSucc( T const& e ) {
   ListNodePosi<T> x = new ListNode( e, this, succ );
   succ->pred = x; succ = x;
   return x;
}

template <typename T> //列表内部方法：复制列表中自位置p起的n项
void List<T>::copyNodes( ListNodePosi<T> p, Rank n ) {
   init();
   while ( n-- ) { insertLast( p->data ); p = p->succ; }
}

template <typename T> //复制列表中自位置p起的n项（assert: p为合法位置，且至少有n-1个后继节点）
List<T>::List( ListNodePosi<T> p, Rank n ) { copyNodes( p, n ); }

template <typename T> //整体复制列表L
List<T>::List( List<T> const& L ) { copyNodes( L.first(), L._size ); }

template <typename T> //复制L中自第r项起的n项（assert: r+n <= L._size）
List<T>::List( List<T> const& L, Rank r, Rank n ) {
   ListNodePosi<T> p = L.first();
   while ( 0 < r-- ) p = p->succ;
   copyNodes ( p, n );
}

template <typename T> T List<T>::remove( ListNodePosi<T> p ) { //删除合法节点p
   T e = p->data;
   p->pred->succ = p->succ; p->succ->pred = p->pred;
   delete p; _size--;
   return e;
}

template <typename T> List<T>::~List() //列表析构器
{ clear();
  delete head;
  delete tail; 
}

template <typename T> Rank List<T>::clear() { //清空列表
   Rank oldSize = _size;
   while ( 0 < _size ) remove ( head->succ );
   return oldSize;
}

template <typename T> Rank List<T>::dedup() {
   Rank oldSize = _size; ListNodePosi<T> p = first();
   for ( Rank r = 0; p != tail; p = p->succ )
      if ( ListNodePosi<T> q = find(p->data, r, p) )
         remove(q);
      else r++;
   return oldSize - _size;
}

template <typename T> void List<T>::traverse( void ( *visit )( T& ) ) //借助函数指针机制遍历
{  for ( ListNodePosi<T> p = head->succ; p != tail; p = p->succ ) visit ( p->data );  }

template <typename T> template <typename VST>
void List<T>::traverse( VST& visit )
{  for ( ListNodePosi<T> p = head->succ; p != tail; p = p->succ ) visit ( p->data );  }

template <typename T> Rank List<T>::uniquify() { //成批剔除重复元素
   if ( _size < 2 ) return 0;
   Rank oldSize = _size;
   ListNodePosi<T> p = first(); ListNodePosi<T> q;
   while ( tail != ( q = p->succ ) )
      if ( p->data != q->data ) p = q;
      else remove( q );
   return oldSize - _size;
}

template <typename T> //在有序列表内节点p（可能是tail）的n个真前驱中，找到不大于e的最后者
ListNodePosi<T> List<T>::search( T const& e, Rank n, ListNodePosi<T> p ) const {
   do {
      p = p->pred; n--;
   } while ( ( -1 != n ) && ( e < p->data ) );
   return p;
}

template <typename T> void List<T>::sort ( ListNodePosi<T> p, Rank n ) { //列表区间排序
   switch ( rand() % 4 ) {
      case 1  : insertionSort( p, n ); break;
      case 2  : selectionSort( p, n ); break;
      case 3  :     mergeSort( p, n ); break;
      default :     radixSort( p, n ); break;
   }
}

template <typename T> //对列表中起始于位置p、宽度为n的区间做插入排序
void List<T>::insertionSort( ListNodePosi<T> p, Rank n ) {
   for ( Rank r = 0; r < n; r++ ) {
      insert( search( p->data, r, p ), p->data );
      p = p->succ; remove( p->pred );
   }
}

template <typename T> //对列表中起始于位置p、宽度为n的区间做选择排序
void List<T>::selectionSort( ListNodePosi<T> p, Rank n ) {
   ListNodePosi<T> h = p->pred;
   ListNodePosi<T> t = p; for ( Rank i = 0; i < n; i++ ) t = t->succ;
   while ( 1 < n ) {
      ListNodePosi<T> max = selectMax ( h->succ, n );
      insert( remove( max ), t );
      t = t->pred; n--;
   }
}

template <typename T> //从起始于位置p的n个元素中选出最大者
ListNodePosi<T> List<T>::selectMax( ListNodePosi<T> p, Rank n ) {
   ListNodePosi<T> max = p;
   for ( ListNodePosi<T> cur = p; 1 < n; n-- )
      if ( ! ( ( cur = cur->succ )->data < max->data ) )
         max = cur;
   return max;
}

template <typename T> //有序列表的归并：当前列表中自p起的n个元素，与列表L中自q起的m个元素归并
ListNodePosi<T> List<T>::merge( ListNodePosi<T> p, Rank n,
                    List<T>& L, ListNodePosi<T> q, Rank m ) {
   ListNodePosi<T> pp = p->pred;
   while ( ( 0 < m ) && ( q != p ) )
      if ( ( 0 < n ) && ( p->data <= q->data ) )
         { p = p->succ; n--; }
      else
         { insert( L.remove( ( q = q->succ )->pred ), p ); m--; }
   return pp->succ;
}

template <typename T> //列表的归并排序算法：对起始于位置p的n个元素排序
void List<T>::mergeSort( ListNodePosi<T>& p, Rank n ) {
   if ( n < 2 ) return;
   Rank m = n >> 1;
   ListNodePosi<T> q = p; for ( Rank i = 0; i < m; i++ ) q = q->succ;
   mergeSort( p, m ); mergeSort( q, n - m );
   p = merge( p, m, *this, q, n - m );
}



#endif  // LIST_H