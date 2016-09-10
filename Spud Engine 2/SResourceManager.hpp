//
//  SResourceManager.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/2/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SResourceManager_hpp
#define SResourceManager_hpp

#include <iostream>
#include <map>

#include "SFileSystem.hpp"

// Forward declarations
class SResourceManager;

/******************************************************************************
 *  Definition for generic resource                                           *
 ******************************************************************************/

class SResource {
    
    friend class SResourceManager;
    
    public:
    
        static SResource* allocate();
        virtual ~SResource();
    
    protected:
    
        virtual bool load(const SPath& path) = 0;
        virtual void unload() = 0;
    
        virtual void upload();
    
        bool uploaded = false;
    
};

/******************************************************************************
 *  Definition for resource allocation manager                                *
 ******************************************************************************/

class SResourceAllocatorManger {
    
    public:
    
        std::map<std::string, SResource* (*)()> allocators;
        bool registerAllocatorForExtension(const std::string& extension,  SResource* (*allocator)());
    
        static SResourceAllocatorManger* instance();
    
};

// Define a macro that can be used to register a resource class
#define REGISTER_RESOURCE_CLASS(E, T) bool isRegistered_##T_##E =  SResourceAllocatorManger::instance()->registerAllocatorForExtension(#E, T::allocate)

/******************************************************************************
 *  Definition for resource manager                                           *
 ******************************************************************************/

class SResourceManager : public SSubsystem {
    
    public:
    
        static void startup();
        static void shutdown();
    
        static bool registerAllocatorForExtension(std::string extension, SResource*(*allocator)());
        static SResource* getResource(const SPath& resource_path);
    
    private:
    
        static std::map<size_t, SResource*>loaded_resources;
        static std::hash<std::string>hasher;
    
    
};

#endif /* SResourceManager_hpp */
