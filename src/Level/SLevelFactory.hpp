//
//  SLevelFactory.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 3/20/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SLevelFactory_hpp
#define SLevelFactory_hpp

#include <iostream>
#include <map>

#include "SHash.hpp"

// Forward declaration
class SLevelFactory;

/******************************************************************************
 *  Definition for allocator     											  *
 ******************************************************************************/

struct SLevelAllocatorBase { virtual void* alloc() = 0;};

template <class T>
struct SLevelAllocator : public SLevelAllocatorBase {
	
	virtual void* alloc() { return new T(); }
	
};

/******************************************************************************
 *  Definition for factory registry											  *
 ******************************************************************************/

class SLevelFactoryRegistry {
	
	friend class SLevelFactory;
	
	public:
	
		static SLevelFactoryRegistry* instance();
	
		template <class T>
		bool registerClass(const std::string& name) {
		
			size_t hash = SHash::hashString(name);
			
			// Create a new factory with the name and template
			SLevelAllocator<T>* allocator = new SLevelAllocator<T>();
			allocators[hash] = allocator;
			
			hashes[SHash::hashString(typeid(T).name())] = hash;
			
			return true;
		
		}
	
	private:
	
		std::map<size_t, SLevelAllocatorBase*> allocators;
		std::map<size_t, size_t> hashes;
	
};

/******************************************************************************
 *  Definition for factory      											  *
 ******************************************************************************/

class SLevelFactory {
	
	public:
	
		template <class T>
		static T* allocate(const std::string& name) {
		
			// Hash the class name
			size_t hash = SHash::hashString(name);
			return allocate<T>(hash);
		
		}
	
		template <class T>
		static T* allocate(size_t hash) {
		
			// Make sure that we have an allocator for this class
			if (SLevelFactoryRegistry::instance()->allocators.count(hash))
				return (T*)SLevelFactoryRegistry::instance()->allocators[hash]->alloc();
		
			return nullptr;
		
		}
	
		template <class T>
		static size_t getClassHash() {
		
			// Get the hash of the class name
			size_t class_hash = SHash::hashString(typeid(T).name());
			
			// Get the appropriate hash
			return SLevelFactoryRegistry::instance()->hashes[class_hash];
		
		}
	
};

#define REGISTER_CLASS(a) bool a##_registered = SLevelFactoryRegistry::instance()->registerClass<a>(#a);

#endif /* SLevelFactory_hpp */
