//
//  SResourceManager.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/2/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SResourceManager.hpp"

std::map<size_t, std::shared_ptr<SResource>>SResourceManager::loaded_resources;

/******************************************************************************
 *  Implementation for generic resource                                       *
 ******************************************************************************/

SResource::~SResource() { /* intentionally empty */ }
void SResource::hotload(const SPath& path) { /* intentionally empty */ }
const SPath& SResource::getPath() const { return paths[0]; }
size_t SResource::getHash() const { return hash; }

/******************************************************************************
 *  Implementation for resource allocation manager                            *
 ******************************************************************************/

SResourceAllocatorManger* SResourceAllocatorManger::instance() {
    
    // Return a static instance of the manager
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
    for (std::map<size_t, std::shared_ptr<SResource>>::iterator i = loaded_resources.begin(); i != loaded_resources.end(); i++) {
        
        SResource* resource = i->second.get();
        
        if (resource) {
            
            // Close the file and delete it from memory
            loaded_resources[i->first]->unload();
			// Memory is deallocated when the pointer is destroyed
			//delete loaded_resources[i->first].get();
            
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

void SResourceManager::purge() {
	
	// Some resources may reference others, this is something that wont be called often, so it is ok ot do it like this
	bool removed = true;
	while (removed) {
	
		removed = false;
		
		// Iterate through all of the resources and check if they need to be unloaded
		std::map<size_t, std::shared_ptr<SResource>>::iterator i = loaded_resources.begin();
	
		while (i != loaded_resources.end()) {
		
			// Check if the resource had one reference, that will mean nothing is referencing it other than the storage
			if (i->second.use_count() <= 1) {
		
				// Remove it from the map, this will deallocate the memory
				SLog::verboseLog(SVerbosityLevel::Debug, "Unloading %s", i->second->getPath().getPathAsString().c_str());
				i->second->unload();
				i = loaded_resources.erase(i);
				removed = true;
		
			} else i++;
			
		}
			
	}
	
}
