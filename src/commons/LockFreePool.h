//
// Created by Yeo Shu Heng on 3/7/25.
//

#ifndef LOCKFREEPOOL_H
#define LOCKFREEPOOL_H
#include <functional>
#include <memory>
#include <boost/lockfree/stack.hpp>

template<typename T, size_t init_size, size_t capacity>
class LockFreePool {
    boost::lockfree::stack<T*> object_pool{capacity};
    void return_object(T* obj_ptr);
public:
    LockFreePool();
    ~LockFreePool();

    /**
     * Borrows a object from the pool.
     * @return Unique pointer to the object that has a custom deleter to return the object to the pool once out of scope.
     */
    std::unique_ptr<T, std::function<void(T*)>> borrow_object();
};

#endif //LOCKFREEPOOL_H
