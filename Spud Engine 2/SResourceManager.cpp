//
//  SResourceManager.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/2/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SResourceManager.hpp"

std::map<size_t, SResource*>SResourceManager::loaded_resources;
std::hash<std::string>SResourceManager::hasher;

/******************************************************************************
 *  Functions  for generic resource                                           *
 ******************************************************************************/

SResource* SResource::allocate() { return nullptr; }
SResource::~SResource() { /* stub */ }

void SResource::upload() { uploaded = true; }

/******************************************************************************
 *  Functions  for resource allocation manager                                *
 ******************************************************************************/

SResourceAllocatorManger* SResourceAllocatorManger::instance() {
    
    // Return a static instace of the manager
    static SResourceAllocatorManger* _instance = NULL;
    if (_instance == NULL)
        _instance = new SResourceAllocatorManger();
    
    return _instance;
    
}

bool SResourceAllocatorManger::registerAllocatorForExtension(const std::string& extension,  SResource* (*allocator)()) {
    
    // Keep track of the allocator for this extension
    allocators[extension] = allocator;
    return true;
    
}

/******************************************************************************
 *  Functions  for resource manager                                           *
 ******************************************************************************/

void SResourceManager::startup() {

    SLog::verboseLog(SVerbosityLevel::Debug, "SResourceManager startup");

}

void SResourceManager::shutdown() {
    
    // Delete all the loaded resources
    std::map<size_t, SResource*>::iterator i = loaded_resources.begin();
    
    while (i != loaded_resources.end()) {
        
        SResource* resource = i->second;
        
        if (resource) {
            
            // Close the file and delete it from memory
            delete loaded_resources[i->first];
            
        }
        
        i++;
    }

    
    // Delete the instance of the allocation manager that we dynamically allocated
    delete SResourceAllocatorManger::instance();
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SEventSystem startup");

}

SResource* SResourceManager::getResource(const SPath& resource_path) {
    
    // Hash the name of the resource
    size_t hash = hasher(resource_path.getPathAsString());
    
    // Check for loaded resource
    if (!resource_path.getIsDirectory() && !loaded_resources.count(hash)) {
        
        // Check that we have the allocator that handles this type of resource
        if (SResourceAllocatorManger::instance()->allocators.count(resource_path.getExtension())) {
        
            // Load the resource, upload it and keep it
            SResource* resource = SResourceAllocatorManger::instance()->allocators[resource_path.getExtension()]();
            
            // Make sure we have an allocator
            if (!resource->load(resource_path)) {
                
                // Failed to load it
                SLog::verboseLog(SVerbosityLevel::Critical, "Coud not load resource: %s! Resource was returned but may behave unexpectadly", resource_path.getPathAsString().c_str());
                
            } else resource->upload();
            
            loaded_resources[hash] = resource;
         
            return resource;
            
        }
        
        SLog::verboseLog(SVerbosityLevel::Critical, "Coud not load resource: %s! Resource had an extension that was not valid", resource_path.getPathAsString().c_str());
            
    }
        
    return loaded_resources[hash];
    
}
