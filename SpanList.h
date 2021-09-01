//
// Created by Administrator on 2021/8/31.
//

#ifndef GITHOU_SPANLIST_H
#define GITHOU_SPANLIST_H
//和上面的Freelist一样，各个接口自己实现，双向带头循环的Span链表
class SpanList
{
public:
    Span* _head;
    std::mutex _mutex;

public:
    SpanList()
    {
        _head = new Span;
        _head->_next = _head;
        _head->_prev = _head;
    }

    ~SpanList()//释放链表的每个节点
    {
        Span * cur = _head->_next;
        while (cur != _head)
        {
            Span* next = cur->_next;
            delete cur;
            cur = next;
        }
        delete _head;
        _head = nullptr;
    }

    //防止拷贝构造和赋值构造，将其封死，没有拷贝的必要，不然就自己会实现浅拷贝
    SpanList(const SpanList&) = delete;
    SpanList& operator=(const SpanList&) = delete;

    //左闭右开
    Span* Begin()//返回的一个数据的指针
    {
        return _head->_next;
    }

    Span* End()//最后一个的下一个指针
    {
        return _head;
    }

    bool Empty()
    {
        return _head->_next == _head;
    }

    //在pos位置的前面插入一个newspan
    void Insert(Span* cur, Span* newspan)
    {
        Span* prev = cur->_prev;

        //prev newspan cur
        prev->_next = newspan;
        newspan->_next = cur;

        newspan->_prev = prev;
        cur->_prev = newspan;
    }

    //删除pos位置的节点
    void Erase(Span* cur)//此处只是单纯的把pos拿出来，并没有释放掉，后面还有用处
    {
        Span* prev = cur->_prev;
        Span* next = cur->_next;

        prev->_next = next;
        next->_prev = prev;
    }

    //尾插
    void PushBack(Span* newspan)
    {
        Insert(End(), newspan);
    }

    //头插
    void PushFront(Span* newspan)
    {
        Insert(Begin(), newspan);
    }

    //尾删
    Span* PopBack()//实际是将尾部位置的节点拿出来
    {
        Span* span = _head->_prev;
        Erase(span);

        return span;
    }

    //头删
    Span* PopFront()//实际是将头部位置节点拿出来
    {
        Span* span = _head->_next;
        Erase(span);

        return span;
    }

    void Lock()
    {
        _mutex.lock();
    }

    void Unlock()
    {
        _mutex.unlock();
    }
};



#endif //GITHOU_SPANLIST_H
