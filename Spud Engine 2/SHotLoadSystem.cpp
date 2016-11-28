//
//  SHotLoadSystem.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/17/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SHotLoadSystem.hpp"

bool SHotLoadSystem::should_check_for_updates = true;
boost::thread SHotLoadSystem::update_thread;

/******************************************************************************
 *  Functions for resource hot load manager                                   *
 ******************************************************************************/

void SHotLoadSystem::startup() {

    // Launch a thread that we can do the hot reloading on
    update_thread = boost::thread(&SHotLoadSystem::checkForUpdates);
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SHotLoadSystem startup");
    
}

void SHotLoadSystem::shutdown() {

    // Tell the hot reload system that we are done and then wait for it
    should_check_for_updates = false;
    update_thread.join();
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SHotLoadSystem shutdown");
    
}

void SHotLoadSystem::checkForUpdates() {
    
    while (should_check_for_updates) {
    
        // Go through all of the files loaded
        for (std::map<size_t, SResource*>::iterator i = SResourceManager::loaded_resources.begin();
             i != SResourceManager::loaded_resources.end();
             i++) {
        
            SResource* resource = i->second;
        
            if (resource) {
            
                std::vector <SPath>& paths = resource->paths;
                
                for (int j = 0; j < paths.size(); j++) {
                
                    // Get the time it was modified
                    SPath resource_path = paths[j];
                    long new_time = SResourceManager::getModifiedTimeForFileAtPath(resource_path.getPathAsAbsolutePath().c_str());
            
                    if (new_time != resource->modified_times[j] && SFileSystem::fileExitsAtPath(resource_path)) {
                
                        // Tell the file that it needs to be reloaded
                        SLog::verboseLog(SVerbosityLevel::Warning, "Hot reloading %s", resource_path.getPathAsAbsolutePath().c_str());
                        resource->modified_times[j] = new_time;
                
                        // Hotload the resource
                        resource->hotload(resource_path);
                
                    }
            
                }
                
            }
        
        }
        
        // We dont need to constantly check, wait a bit
        sleep(1);
        
    }

}
