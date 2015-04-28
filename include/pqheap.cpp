// pqheap.hpp - The PQheap class implementation (included by header).
// by Jonathan Chappelow (chappjc)

#include <cassert>

template <typename T, typename C, int S>
CUDA_CALLABLE
PQheap<T,C,S>::PQheap()
: heap(NULL), N(0), cap(0)
{
}

template <typename T, typename C, int S>
CUDA_CALLABLE
PQheap<T,C,S>::PQheap(unsigned int maxSize)
: heap(NULL), N(0), cap(maxSize)
{
    init(cap);
}

template <typename T, typename C, int S>
CUDA_CALLABLE
int PQheap<T,C,S>::init(unsigned int maxSize)
{
    if (maxSize == 0)
        return 0;

    N = 0;
    cap = maxSize;

    if (heap)
        delete[] heap;

    heap = new T[S*(cap + 1)];

    if (!heap) {
        cap = 0;
        return 1;
    }

    return 0;
}

template <typename T, typename C, int S>
CUDA_CALLABLE
PQheap<T,C,S>::PQheap(T* a, unsigned int capacity)
{
    bind(a, capacity);
    // you MUST call unbind() when finished, otherwise your buffer will be destroyed
}

template <typename T, typename C, int S>
CUDA_CALLABLE
int PQheap<T,C,S>::bind(T* a, unsigned int capacity)
{
    assert(heap == nullptr);
    assert(!(a == nullptr && capacity > 0));
    heap = a; // don't forget to unbind()!
    cap = capacity;
    N = 0;
    return 0;
}

template <typename T, typename C, int S>
CUDA_CALLABLE
int PQheap<T,C,S>::unbind()
{
    heap = nullptr;
    cap = 0;
    return 0;
}

template <typename T, typename C, int S>
CUDA_CALLABLE
PQheap<T,C,S>::~PQheap()
{
    // be sure to run unbind() first if using external buffer for heap
    if (heap) delete[] heap;
    N = 0;
    cap = 0;
}

template <typename T, typename C, int S>
CUDA_CALLABLE
int PQheap<T,C,S>::insert(const T& t)
{
    if (cap == 0)
        return -2;

    if (N == cap) {
        if (cmp(t, heap[S])) {
            // replace top and sift down
            heap[S] = t;
            return heapDownQuick();
        }
        else {
            return -1;
        }
    }

    // append bottom and bubble up
    heap[(++N)*S] = t;

    return N==1 ? 1 : heapUp();
}

template <typename T, typename C, int S>
CUDA_CALLABLE
int PQheap<T,C,S>::heapUp()
{
    return heapUp(N);
    /*assert(N > 1);

    unsigned int i = N;
    unsigned int p = parent(i);

    while (i > 1 && heap[p] > heap[i]) {
        swap(heap[p], heap[i]);
        i = p;
        p = parent(i);
    }

    return i;*/
}

template <typename T, typename C, int S>
CUDA_CALLABLE
int PQheap<T,C,S>::heapUp(int i)
{
    assert(N > 1);

    T bubble = heap[i*S];

    //unsigned int i = N;
    int p = parent(i);

    while (i > 1 && cmp(heap[p*S], bubble) /*heap[i]*/) {
        //swap(heap[p], heap[i]);
        heap[i*S] = heap[p*S];
        i = p;
        p = parent(i);
    }

    heap[i*S] = bubble;

    return i;
}

template <typename T, typename C, int S>
CUDA_CALLABLE
int PQheap<T,C,S>::extractTop(T& t)
{
    if (N < 1) return 1;

    t = heap[S];

    if (N > 1)
        heap[S] = heap[N*S];

#ifdef _DEBUG
    heap[S*N--] = T();
#else
    --N;
#endif

    return N <= 1 ? 0 : heapDownQuick();
}

template <typename T, typename C, int S>
CUDA_CALLABLE
T PQheap<T,C,S>::peakTop() const
{
    return N > 0 ? heap[S] : T();
}

template <typename T, typename C, int S>
CUDA_CALLABLE
int PQheap<T,C,S>::peakTop(const T* t) const
{
    if (N < 1) return 1;

    t = &heap[S];

    return 0;
}

template <typename T, typename C, int S>
CUDA_CALLABLE
void PQheap<T,C,S>::discardTop()
{
    if (N < 1) return;
    if (N > 1) {
        heap[S] = heap[S*N--];
        heapDownQuick();
    }
    else
        --N;
}

template <typename T, typename C, int S>
CUDA_CALLABLE
T PQheap<T,C,S>::extractTop()
{
    T top;
    extractTop(top);
    return top;
}

//template <typename T>
//CUDA_CALLABLE
//int PQheap<T>::heapDown()
//{
//    //return heapDown(1);
//    assert(N > 1);
//
//    int i = 1;
//    int left = 2;  // left_child(1)
//    int right = 3; // right_child(1)
//
//    if (right <= N && heap[left] > heap[right])
//        left = right;
//
//    while (left <= N && heap[i] > heap[left]) {
//        swap(heap[left], heap[i]);
//
//        i = left;
//        left = left_child(i);
//        right = left + 1;
//
//        if (right <= N && heap[left] > heap[right])
//            left = right;
//    }
//
//    return i;
//}

template <typename T, typename C, int S>
CUDA_CALLABLE
int PQheap<T,C,S>::heapDown()
{
    //return heapDown(1);
    assert(N > 1);

    int i = 1;
    int next = 2;  // left_child(1)

    T rock = heap[i*S];

    if (next < N && cmp(heap[next*S] , heap[(next+1)*S]))
        ++next; // go right

    while (next <= N && cmp(rock , heap[next*S])) {
        heap[i*S] = heap[next*S];

        i = next;
        next = left_child(i);

        if (next < N && cmp(heap[next*S] , heap[(next + 1)*S]))
            ++next;
    }

    heap[i*S] = rock;

    return i;
}

template <typename T, typename C, int S>
CUDA_CALLABLE
int PQheap<T,C,S>::heapDownQuick()
{
    //return heapDown(1);
    assert(N > 1);

    int i = 1;
    int next = 2;  // left_child(1)

    T rock = heap[i*S];

    while (i <= N/2) {
        next = left_child(i);

        if (next < N && cmp(heap[next*S] , heap[(next + 1)*S]))
            ++next;
        if (!cmp(rock , heap[next*S]))
            break;

        heap[i*S] = heap[next*S];
        i = next;
    }

    heap[i*S] = rock;

    return i;
}


template <typename T, typename C, int S>
CUDA_CALLABLE
int PQheap<T,C,S>::heapDown(int i)
{
    assert(N > 1);

    int left = left_child(i);
    int right = right_child(i);

    if (right <= N && cmp(heap[left*S] , heap[right*S]))
        left = right;

    while (left <= N && cmp(heap[i*S] , heap[left*S])) {
        swap(heap[left*S], heap[i*S]);

        i = left;
        left = left_child(i);
        right = left + 1;

        if (right <= N && cmp(heap[left*S] , heap[right*S]))
            left = right;
    }

    return i;
}

template <typename T, typename C, int S>
CUDA_CALLABLE
int PQheap<T,C,S>::heapify()
{
    return heapDown();
}

#if _MSC_VER < 1800
template <typename T, typename C, int S>
CUDA_CALLABLE CUDA_INLINE
void PQheap<T,C,S>::swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}
#else
// #if __cplusplus >= 201103L
#define XSTR(x) STR(x)
#define STR(x) #x
#pragma message("C++: " XSTR(__cplusplus) ", MSVC: " XSTR(_MSC_VER))
template <typename T, typename C, int S>
CUDA_CALLABLE CUDA_INLINE
void PQheap<T,C,S>::swap(T& a, T& b) {
    T tmp(std::move(a));
    a = std::move(b);
    b = std::move(tmp);
}
#endif