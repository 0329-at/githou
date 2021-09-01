//
// Created by Administrator on 2021/8/31.
//

#ifndef GITHOU_PAGECACHE_H
#define GITHOU_PAGECACHE_H
#pragma once

#include "Common.h"

//对于Page Cache也要设置为单例，对于Central Cache获取span的时候
//每次都是从同一个page数组中获取span
//单例模式
class PageCache
{
public:
    static PageCache* GetInstence()
    {
        return &_inst;
    }

    Span* AllocBigPageObj(size_t size);
    void FreeBigPageObj(void* ptr, Span* span);

    Span* _NewSpan(size_t n);
    Span* NewSpan(size_t n);//获取的是以页为单位

    //获取从对象到span的映射
    Span* MapObjectToSpan(void* obj);

    //释放空间span回到PageCache，并合并相邻的span
    void ReleaseSpanToPageCache(Span* span);

private:
    SpanList _spanlist[NPAGES];
    //std::map<PageID, Span*> _idspanmap;
    std::unordered_map<PageID, Span*> _idspanmap;

    std::mutex _mutex;
private:
    PageCache(){}

    PageCache(const PageCache&) = delete;
    static PageCache _inst;
};
/*
 * 申请内存：

当Central Cache向page cache申请内存时，Page Cache先检查对应位置有没有span，如果没有则向更大页寻找一个span，如果找到则分裂成两个。比如：申请的是4page，4page后面没有挂span，则向后面寻找更大的span，假设在10page位置找到一个span，则将10page span分裂为一个4page span和一个6page span。
如果找到128 page都没有合适的span，则向系统使用mmap、brk或者是VirtualAlloc等方式申请128page span挂在自由链表中，再重复1中的过程。
释放内存：

如果Central Cache释放回一个span，则依次寻找span的前后_pageid的span，看是否可以合并，如果合并继续向前寻找。这样就可以将切小的内存合并收缩成大的span，减少内存碎片。
 */
#endif //GITHOU_PAGECACHE_H
