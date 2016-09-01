//
//  SPool.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/1/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SPool_hpp
#define SPool_hpp

#include <iostream>
#include <list>

template <class T>
class SPool {
    
    private:
    
        std::list<T*>used_objects;
        std::list<T*>avaliable_objects;
    
    public:
    
        SPool(int capacity);
    
        T& getObject();
        void returnObject(T& object);
    
    protected:
    
        virtual void recycle();
        T* getObjectToRecycle();
    
};

/******************************************************************************
 *  Functions to for default object pool                                      *
 ******************************************************************************/

template <class T>
SPool<T>::SPool(int capacity) {
    
    // Allocate the number of objects that were specified
    for (int i = 0; i < capacity; i++) {
        
        T* new_object = new T();
        avaliable_objects.push_back(new_object);
        
    }
    
}

template <class T>
T& SPool<T>::getObject() {
    
    // Check if there is something in the avaliable pool we can get
    if (avaliable_objects.size()) {
        
        // Grab the object that we already have in the pool
        T* object = avaliable_objects.front();
        avaliable_objects.erase(avaliable_objects.begin());
        used_objects.push_back(object);
        
        return *object;
        
    } else {
        
        // Allocate a new object
        T* new_object = new T();
        used_objects.push_back(new_object);
        return *new_object;
        
    }
    
}

template <class T>
void SPool<T>::returnObject(T& object) {
    
    T* object_ptr = &object;
    
    // Return the object to the pool
    used_objects.remove(object_ptr);
    avaliable_objects.push_back(object_ptr);
    
    recycle();
    
}

/******************************************************************************
 *  Functions to handle recycling of objects                                  *
 ******************************************************************************/

template <class T>
void SPool<T>::recycle() { /* stub */ }

template <class T>
T* SPool<T>::getObjectToRecycle() { return avaliable_objects.back(); }

#endif /* SPool_hpp */
