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

#include "SSubsystem.hpp"
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
        static void registerAllocators();
    
    protected:
    
        virtual void load(const SPath& path) = 0;
        virtual void unload() = 0;
    
        virtual void upload();
    
};

/******************************************************************************
 *  Definition for resource manager                                           *
 ******************************************************************************/

class SResourceManager : public SSubsystem {
    
    public:
    
        static void registerAllocatorForExtension(std::string extension, SResource*(*allocator)());
        static SResource* getResource(const SPath& resource_path);
    
    private:
    
        static std::map<size_t, SResource*>loaded_resources;
        static std::map<size_t, SResource*(*)()>allocators;
    
        static std::hash<std::string>hasher;
    
    
};

#endif /* SResourceManager_hpp */
