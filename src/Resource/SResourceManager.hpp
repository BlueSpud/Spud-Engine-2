//
//  SResourceManager.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/2/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SResourceManager_hpp
#define SResourceManager_hpp

#include <sys/stat.h>
#include <typeinfo>
#include <iostream>
#include <map>

#include "SFileSystem.hpp"

// Forward declarations
class SResourceManager;
class SHotLoadSystem;

/******************************************************************************
 *  Definition for generic resource                                           *
 ******************************************************************************/

class SResource {
    
    friend class SResourceManager;
    friend class SHotLoadSystem;
    
    public:
    
        virtual ~SResource();
		const SPath& getPath() const;
		size_t getHash() const;
	
    protected:
	
		size_t hash;
	
        virtual bool load(const SPath& path) = 0;
        virtual void unload() = 0;
        virtual void hotload(const SPath& path);
        bool uploaded = false;
        std::vector<SPath> paths;
        std::vector<long> modified_times;
    
};

/******************************************************************************
 *  Definition for resource allocation manager                                *
 ******************************************************************************/

class SResourceAllocatorManger {
    
    public:
    
        std::map<std::string, std::vector<std::string>> supported_extensions;
        bool registerClassForExtension(const std::string& class_name, const std::string& extension);
    
        static SResourceAllocatorManger* instance();
    
};

// Define a macro that can be used to register a resource class
#define REGISTER_RESOURCE_CLASS(E, T) bool isRegistered_##T##E = SResourceAllocatorManger::instance()->registerClassForExtension(typeid(T).name(), #E);


/******************************************************************************
 *  Definition for resource manager                                           *
 ******************************************************************************/

class SResourceManager : public SSubsystem {
    
    friend class SHotLoadSystem;
    
    public:
    
        static void startup();
        static void shutdown();
    
        template <class T>
        static T* getResource(const SPath& resource_path);
	
		template <class T>
		static T* getResource(size_t hash);

	
    private:
    
        static long getModifiedTimeForFileAtPath(const char* path);
    
        static std::map<size_t, SResource*>loaded_resources;
        static std::hash<std::string>hasher;
    
    
};

/******************************************************************************
 *  Function for resource manager                                             *
 ******************************************************************************/

template <class T>
T* SResourceManager::getResource(const SPath& resource_path) {
    
    // Hash the name of the resource
    size_t hash = hasher(resource_path.getPathAsString());
    
    // Check for loaded resource
    if (!loaded_resources.count(hash)) {
        
        // Check that we have the allocator that handles this type of resource
        const std::vector<std::string>& extensions = SResourceAllocatorManger::instance()->supported_extensions[typeid(T).name()];
        bool supported = false;
        for (int i = 0; i < extensions.size(); i++) {
            
            if (!extensions[i].compare(resource_path.getExtension())) {
                
                // Extension was supported by class
                supported = true;
                break;
                
            }
            
        }
        
        // If the resource was supported, return
        if (supported) {
            
            // Load the resource, upload it and keep it
            SResource* resource = new T();
            
            // Save the path that the resource was loaded from
            resource->paths.push_back(resource_path);
            
            SLog::verboseLog(SVerbosityLevel::Debug, "Loading resource %s", resource_path.getPathAsString().c_str());
            
            // Make sure we have an allocator
            if (!resource->load(resource_path)) {
                
                // Failed to load it
                SLog::verboseLog(SVerbosityLevel::Critical, "Coud not load resource: %s! Resource was returned but may behave unexpectadly", resource_path.getPathAsString().c_str());
                
            }
            
            // Get the time it was modified
            for (int i = 0; i < resource->paths.size(); i++)
                resource->modified_times.push_back(getModifiedTimeForFileAtPath(resource->paths[i].getPathAsAbsolutePath().c_str()));
			
			resource->hash = hash;
            loaded_resources[hash] = resource;
            
        } else {
            
            // The class we asked this object to be didnt support the extension of the path
            SLog::verboseLog(SVerbosityLevel::Critical, "Coud not load resource: %s! Resource attempted to be loaded with invalid extension", resource_path.getPathAsString().c_str());
            return nullptr;
            
        }
        
    }
    
	return (T*)loaded_resources[hash];
	
}

template <class T>
T* SResourceManager::getResource(size_t hash) {
	
	// See if we have loaded a resource
	if (loaded_resources.count(hash))
		return (T*)loaded_resources[hash];
	else SLog::verboseLog(SVerbosityLevel::Critical, "Attempted to access resource by hash that was not loaded, nullptr was returned which will likely cause a crash");
	
	return nullptr;
	
}

#endif /* SResourceManager_hpp */
