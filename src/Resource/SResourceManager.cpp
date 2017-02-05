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
 *  Implementation for generic resource                                       *
 ******************************************************************************/

SResource::~SResource() { /* intentionally empty */ }
void SResource::hotload(const SPath& path) { /* intentionally empty */ }
SResource* SResource::resource() { return this; }

/******************************************************************************
 *  Implementation for resource allocation manager                            *
 ******************************************************************************/

SResourceAllocatorManger* SResourceAllocatorManger::instance() {
    
    // Return a static instace of the manager
    static SResourceAllocatorManger* _instance = NULL;
    if (_instance == NULL)
        _instance = new SResourceAllocatorManger();
    
    return _instance;
    
}

bool SResourceAllocatorManger::registerClassForExtension(const std::string& class_name, const std::string& extension) {
    
    // Check if we need to create a new vector for the extensions
    if (!supported_extensions.count(class_name))
        supported_extensions[class_name] = std::vector<std::string>();
    
    supported_extensions[class_name].push_back(extension);
    
    return true;
    
}

/******************************************************************************
 *  Implementation for resource manager                                       *
 ******************************************************************************/

void SResourceManager::startup() {

    SLog::verboseLog(SVerbosityLevel::Debug, "SResourceManager startup");

}

void SResourceManager::shutdown() {
    
    // Delete all the loaded resources
    for (std::map<size_t, SResource*>::iterator i = loaded_resources.begin(); i != loaded_resources.end(); i++) {
        
        SResource* resource = i->second;
        
        if (resource) {
            
            // Close the file and delete it from memory
            loaded_resources[i->first]->unload();
            delete loaded_resources[i->first];
            
        }
    }

    
    // Delete the instance of the allocation manager that we dynamically allocated
    delete SResourceAllocatorManger::instance();
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SResourceManager shutdown");

}

long SResourceManager::getModifiedTimeForFileAtPath(const char* path) {
    
    // Get the statistics for the file
    struct stat stats;
    stat(path, &stats);
    
    // Return the time in seconds
    return stats.st_mtimespec.tv_sec;
    
}
