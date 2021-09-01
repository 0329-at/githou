//
// Created by Administrator on 2021/8/31.
//

#ifndef GITHOU_SIZECLASS_H
#define GITHOU_SIZECLASS_H

/*申请内存:

当内存申请size<=64k时在Thread Cache中申请内存，计算size在自由链表中的位置，如果自由链表中有内存对象时，直接从FistList[i]中Pop一下对象，时间复杂度是O(1)，且没有锁竞争。
当FreeList[i]中没有对象时，则批量从Central Cache中获取一定数量的对象，插入到自由链表并返回一个对象。
释放内存：

当释放内存小于64k时将内存释放回Thread Cache，计算size在自由链表中的位置，将对象Push到FreeList[i].
当链表的长度过长，也就是超过一次向中心缓存分配的内存块数目时则回收一部分内存对象到Central Cache。
4.3 对齐大小的设计（对齐规则）
 */

//专门用来计算大小位置的类
class SizeClass{
public:
    //获取Freelist的位置
    inline static size_t _Index(size_t size, size_t align)
    {
        size_t alignnum = 1 << align;  //库里实现的方法
        return ((size + alignnum - 1) >> align) - 1;
    }

    inline static size_t _Roundup(size_t size, size_t align)
    {
        size_t alignnum = 1 << align;
        return (size + alignnum - 1)&~(alignnum - 1);
    }

public:
    // 控制在12%左右的内碎片浪费
    // [1,128]				8byte对齐 freelist[0,16)
    // [129,1024]			16byte对齐 freelist[16,72)
    // [1025,8*1024]		128byte对齐 freelist[72,128)
    // [8*1024+1,64*1024]	1024byte对齐 freelist[128,184)

    inline static size_t Index(size_t size)
    {
        assert(size <= MAX_BYTES);

        // 每个区间有多少个链
        static int group_array[4] = { 16, 56, 56, 56 };
        if (size <= 128)
        {
            return _Index(size, 3);
        }
        else if (size <= 1024)
        {
            return _Index(size - 128, 4) + group_array[0];
        }
        else if (size <= 8192)
        {
            return _Index(size - 1024, 7) + group_array[0] + group_array[1];
        }
        else//if (size <= 65536)
        {
            return _Index(size - 8 * 1024, 10) + group_array[0] + group_array[1] + group_array[2];
        }
    }

    // 对齐大小计算，向上取整
    static inline size_t Roundup(size_t bytes)
    {
        assert(bytes <= MAX_BYTES);

        if (bytes <= 128){
            return _Roundup(bytes, 3);
        }
        else if (bytes <= 1024){
            return _Roundup(bytes, 4);
        }
        else if (bytes <= 8192){
            return _Roundup(bytes, 7);
        }
        else {//if (bytes <= 65536){
            return _Roundup(bytes, 10);
        }
    }

    //动态计算从中心缓存分配多少个内存对象到ThreadCache中
    static size_t NumMoveSize(size_t size)
    {
        if (size == 0)
            return 0;

        int num = (int)(MAX_BYTES / size);
        if (num < 2)
            num = 2;

        if (num > 512)
            num = 512;

        return num;
    }

    // 根据size计算中心缓存要从页缓存获取多大的span对象
    static size_t NumMovePage(size_t size)
    {
        size_t num = NumMoveSize(size);
        size_t npage = num*size;
        npage >>= PAGE_SHIFT;
        if (npage == 0)
            npage = 1;
        return npage;
    }
};


#endif //GITHOU_SIZECLASS_H
