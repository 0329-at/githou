//
// Created by Administrator on 2021/8/31.
//

#ifndef GITHOU_THREADCACHE_H
#define GITHOU_THREADCACHE_H
#pragma once

#include "Common.h"

class ThreadCache
{
private:
    Freelist _freelist[NLISTS];//自由链表

public:
    //申请和释放内存对象
    void* Allocate(size_t size);
    void Deallocate(void* ptr, size_t size);

    //从中心缓存获取对象
    void* FetchFromCentralCache(size_t index, size_t size);

    //释放对象时，链表过长时，回收内存回到中心堆
    void ListTooLong(Freelist* list, size_t size);
};

//静态的，不是所有可见
//每个线程有个自己的指针, 用(_declspec (thread))，我们在使用时，每次来都是自己的，就不用加锁了
//每个线程都有自己的tlslist
_declspec (thread) static ThreadCache* tlslist = nullptr;

/*申请内存：

当Thread Cache中没有内存时，就会批量向Central Cache申请一些内存对象，Central Cache也有一个哈希映射的freelist，freelist中挂着span，从span中取出对象给Thread Cache，这个过程是需要加锁的。
Central Cache中没有非空的span时，则将空的span链在一起，向Page Cache申请一个span对象，span对象中是一些以页为单位的内存，切成需要的内存大小，并链接起来，挂到span中。
Central Cache的span中有一个_usecount，分配一个对象给Thread Cache，就++_usecount。
释放内存：

当Thread Cache过长或者线程销毁，则会将内存释放回Central Cache中的，释放回来时- -_usecount。
当_usecount减到0时则表示所有对象都回到了span，则将Span释放回Page Cache，Page Cache中会对前后相邻的空闲页进行合并。
*/
#endif //GITHOU_THREADCACHE_H
