//
// Created by Yeo Shu Heng on 3/7/25.
//

#include "LockFreePool.h"

template<typename T, size_t init_size, size_t capacity>
LockFreePool<T, init_size, capacity>::LockFreePool() {
    for (size_t i = 0; i < init_size; i++) {
        T* obj = new T();
        object_pool.push_back(obj);
    }
}

template<typename T, size_t init_size, size_t capacity>
LockFreePool<T, init_size, capacity>::~LockFreePool() {
    T* obj;
    while (object_pool.pop(obj)) {
        delete obj;
    }
}

template<typename T, size_t init_size, size_t capacity>
std::unique_ptr<T, std::function<void(T*)>> LockFreePool<T, init_size, capacity>::borrow_object() {
    T* obj = nullptr;
    if (!object_pool.pop()) {
        obj = new T();
    }
    return std::unique_ptr<T, std::function<void(T*)>>(obj, [this](T* obj_ptr) {
        this->return_object(obj_ptr);
    });
}

template<typename T, size_t init_size, size_t capacity>
void  LockFreePool<T, init_size, capacity>::return_object(T* obj_ptr) {
    if (!object_pool.push(obj_ptr)) {
        delete obj_ptr;
    }
}


