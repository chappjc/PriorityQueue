// pqheap.hpp - The PQheap template class.
//
// Template parameters:
//    1. typename T - the data type of each element.  Can be any type for which
//       the comparator specified in the next parameter is defined.
//    2. typename C - A functor: std::less<T> or std::greater<T>.  To create a
//       min-oriented queue (top is smallest in N largest elements), use 
//       std::greater<T>. To create a max-oriented queue (top is largest in N
//       smallest elements), use std::less<T>.
//    3. int S - A stride between elements in the heap. This does not affect 
//       the functionality of the class.  Don't worry about it.
//
// by Jonathan Chappelow (chappjc)

#ifndef PQHEAP_H_
#define PQHEAP_H_

#include <functional>

#ifdef __CUDACC__
#pragma message("CUDA!")
#define CUDA_CALLABLE __host__ __device__
#define CUDA_INLINE __inline__
#else
#pragma message("NOT CUDA!")
#define CUDA_CALLABLE
#define CUDA_INLINE inline
#endif 

template <typename T, typename C = std::less<T>, int S = 1>
class PQheap
{
public:
    CUDA_CALLABLE
    PQheap();
    CUDA_CALLABLE
    ~PQheap();

    CUDA_CALLABLE
    int init(unsigned int maxSize);
    CUDA_CALLABLE
    PQheap(unsigned int maxSize);

    CUDA_CALLABLE
    PQheap(T* bindTo, unsigned int capacity);

    typedef typename T element_type;

public:

    CUDA_CALLABLE
    int bind(T* a, unsigned int capacity);
    CUDA_CALLABLE
    int unbind();

    CUDA_CALLABLE
    int insert(const T& t);

    CUDA_CALLABLE
    int extractTop(T& t);
    CUDA_CALLABLE
    T extractTop();

    CUDA_CALLABLE
    int peakTop(const T* t) const;
    CUDA_CALLABLE
    T peakTop() const;

    CUDA_CALLABLE
    void discardTop();

    CUDA_CALLABLE CUDA_INLINE
    int size() const { return N; }
    CUDA_CALLABLE CUDA_INLINE
    int capacity() const { return cap; }

    CUDA_CALLABLE CUDA_INLINE
    bool empty() const { return N == 0; }

    T const* data() { return heap; }

public:

    static CUDA_CALLABLE CUDA_INLINE
        int left_child(int index) { return /*index << 1*/ 2 * index; }
    static CUDA_CALLABLE CUDA_INLINE
        int right_child(int index) { return /*(index << 1) + 1*/ 2 * index + 1; }
    static CUDA_CALLABLE CUDA_INLINE
        int parent(int index) { return index/2 /*index >> 1*/; }

private:

    CUDA_CALLABLE
    int heapUp();
    CUDA_CALLABLE
    int heapDown();
    CUDA_CALLABLE
        int heapDownQuick();

    // duplicate code for speed, no default arguments
    CUDA_CALLABLE
    int heapUp(int i);
    CUDA_CALLABLE
    int heapDown(int i); // no default, use overload

    CUDA_CALLABLE
    int heapify();

    static CUDA_CALLABLE CUDA_INLINE
    void swap(T& a, T& b);

private:

    T*  heap;

    int N;
    int cap;

    //int *aux;
    C cmp;
};

// actual implementation sepearate to keep this file short
#include "pqheap.cpp"

#endif
