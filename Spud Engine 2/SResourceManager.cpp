//
//  SResourceManager.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/2/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SResourceManager.hpp"

std::map<size_t, SResource*>SResourceManager::loaded_resources;
std::map<size_t, SResource*(*)()>SResourceManager::allocators;

std::hash<std::string>SResourceManager::hasher;

/******************************************************************************
 *  Functions  for generic resource                                           *
 ******************************************************************************/

SResource* SResource::allocate() { return nullptr; }
void SResource::registerAllocators() { /* stub */ }

void SResource::upload() { /* stub */ }

/******************************************************************************
 *  Functions  for resource manager                                           *
 ******************************************************************************/

void SResourceManager::registerAllocatorForExtension(std::string extension, SResource*(*allocator)()) {
    
    // Add it to the collection of allocators
    allocators.insert(std::pair<size_t, SResource*(*)()>(hasher(extension), allocator));
    
}

SResource* SResourceManager::getResource(const SPath& resource_path) {
    
    // Hash the name of the resource
    size_t hash = hasher(resource_path.getPathAsString());
    
    // Check for loaded resource
    SResource* resource = loaded_resources[hash];
    
    if (!resource) {
        
        size_t extension_hash = hasher(resource_path.getExtension());
        SResource*(*allocator)()  = allocators[extension_hash];
        
        if (allocator) {
        
            // Load the resource, upload it and keep it
            resource = allocator();
            resource->load(resource_path);
            resource->upload();
            
            loaded_resources.insert(std::pair<size_t, SResource*>(hash, resource));
            
        }
            
    }
    
    return resource;
    
}
