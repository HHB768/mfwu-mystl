#ifndef __PRIORITY_QUEUE_HPP__
#define __PRIORITYTQUEUE_HPP__

#include "heap.hpp"

namespace mfwu {

template <typename T, typename Container=mfwu::vector<T>, 
    typename CmpFunctor=mfwu::less<T>>
using priority_queue = mfwu::heap<T, Container, CmpFunctor>;

}  // endof namespace mfwu


#endif  // __PRIORITY_QUEUE_HPP__