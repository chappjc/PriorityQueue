// pqheapTestMEX.cpp - Test library (MATLAB MEX-file) for the PQheap class.
// by Jonathan Chappelow (chappjc)

#include "stdafx.h"

#include "mex.h"

//#include "..\include\mexHelpers.hpp"
#include "..\include\pqheap.hpp"

//#include <typeinfo>       // operator typeid
//#include <typeindex>      // std::type_index
//#include <unordered_map>  // std::unordered_map
//#include <string>         // std::string

#include <algorithm>

template <typename T> T PQtests(const mxArray *mx, unsigned int heapMaxLength);

template class PQheap<double>; // test

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

    if (nlhs != 0 || nrhs > 2 || nrhs < 2)
        mexErrMsgTxt("pqheapTestMEX(array, heapMaxLength)");

    const mxArray *mx = prhs[0];
    unsigned int N = (unsigned int)mxGetScalar(prhs[1]);

    mxClassID arrayClass = mxGetClassID(mx);
    switch (arrayClass)  {
    case mxINT8_CLASS:   PQtests<char>(mx, N);   break;
    case mxUINT8_CLASS:  PQtests<unsigned char>(mx, N);  break;
    case mxINT16_CLASS:  PQtests<short>(mx, N);  break;
    case mxUINT16_CLASS: PQtests<unsigned short>(mx, N); break;
    case mxINT32_CLASS:  PQtests<int>(mx, N);  break;
    case mxUINT32_CLASS: PQtests<unsigned>(mx, N); break;
    case mxINT64_CLASS:  PQtests<long long>(mx, N);  break;
    case mxUINT64_CLASS: PQtests<unsigned long long>(mx, N); break;
    case mxSINGLE_CLASS: PQtests<float>(mx, N); break;
    case mxDOUBLE_CLASS: PQtests<double>(mx, N); break;
    default: break;
    }
    
}

#include <chrono>
#include <ctime>

template <typename T>
T PQtests(const mxArray *mx, unsigned int heapMaxLength)
{
    int numel = (int)mxGetNumberOfElements(mx);
    T* sorted = new T[heapMaxLength];

    auto t_start = std::chrono::high_resolution_clock::now();
    //std::clock_t c_start = std::clock();

    const int heapStride = 1;
    PQheap<T, std::greater<T>, heapStride> pq(heapMaxLength);
    const T* heapData = pq.data();

    T* e = (T*)mxGetData(mx);

    for (int i = 0; i < numel; ++i){

        pq.insert(e[i]);
#ifdef _DEBUG
        if (heapMaxLength <= 20 && numel<=20) {
            mexPrintf("Inserting %g:\n",e[i]);
            for (int j = 0; j < std::min<int>(i+1, heapMaxLength); ++j)
                mexPrintf("| %g ", heapData[(j + 1)*heapStride]);
            mexPrintf("|\n\n");
        }
#endif
    }

    //while (!pq.empty()) {
    //    T top; pq.extractTop(top);
    //    mexPrintf("| %g ", top);
    //}
    for (unsigned i = 0; i < heapMaxLength; ++i){
        pq.extractTop(sorted[i]);
    }

    //std::clock_t c_end = std::clock();
    auto t_end = std::chrono::high_resolution_clock::now();

#ifdef _DEBUG
    if (heapMaxLength <= 20) {
        mexPrintf("\nPulling elements off top (heapsort):\n");
        for (unsigned i = 0; i < heapMaxLength; ++i){
            mexPrintf("| %g ", sorted[i]);
        }
        mexPrintf("|\n");
    }
#endif

    mexPrintf("Wall clock time passed: %g ms\n", std::chrono::duration<double, std::milli>(t_end - t_start).count());
    //mexPrintf("CPU time used: %g ms\n", 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC );
    mexPrintf("Sorted? %d\n", (char)std::is_sorted(sorted, sorted + heapMaxLength));

    return T();
}
