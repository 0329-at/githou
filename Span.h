//
// Created by Administrator on 2021/8/31.
//

#ifndef GITHOU_SPAN_H
#define GITHOU_SPAN_H

struct Span{
    PageID _pageid = 0;//页号
    size_t _npage = 0;//页数

    Span* _prev = nullptr;
    Span* _next = nullptr;

    void* _list = nullptr;//链接对象的自由链表，后面有对象就不为空，没有对象就是空
    size_t _objsize = 0;//对象的大小

    size_t _usecount = 0;//对象使用计数,
};
#endif //GITHOU_SPAN_H
