# PriorityQueue
Priority Queue implemented with a binary heap in a C++ template class

pqheap.hpp - The `PQheap` template class.  There is nothing to compile, just include the .hpp (it includes the .cpp).

    template <typename T, typename C = std::less<T>, int S = 1> class PQheap;

Template parameters:
  1. `T` - the data type of each element.  Can be any type for which the comparator specified in the next parameter is defined.
  2. `C` - A functor: `std::less<T>` or `std::greater<T>`.  To create a min-oriented queue (top is smallest in N largest elements), use `std::greater<T>`. To create a max-oriented queue (top is largest in N smallest elements), use `std::less<T>`.
  3. `S` - A stride between elements in the heap. This does not affect the functionality of the class.  Don't worry about it.

by Jonathan Chappelow (chappjc)
