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

void SResource::upload() { uploded = true; }

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
    SResource* resource = loaded_resources[hash];
    
    if (!resource) {
        
        // Get the allocator that handles this type of resource
        SResource*(*allocator)()  = SResourceAllocatorManger::instance()->allocators[resource_path.getExtension()];
        
        if (allocator) {
        
            // Load the resource, upload it and keep it
            resource = allocator();
            if (resource->load(resource_path)) {
                
                resource->upload();
                loaded_resources.insert(std::pair<size_t, SResource*>(hash, resource));
                
                return resource;
                
            }
            
            // Couldnt load the resource
            delete resource;
            
        }
            
    }
    
    SLog::verboseLog(SVerbosityLevel::Critical, "Coud not load resource: %s", resource_path.getPathAsString().c_str());
    
    return resource;
    
}
