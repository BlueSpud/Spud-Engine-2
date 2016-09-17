//
//  SHotLoadSystem.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/17/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SHotLoadSystem_hpp
#define SHotLoadSystem_hpp

#include <boost/thread.hpp>

#include "SResourceManager.hpp"

/******************************************************************************
 *  Definition for resource hot load manager                                  *
 ******************************************************************************/

class SHotLoadSystem : public SSubsystem {
    
    public:
    
        static void startup();
        static void shutdown();

    private:
    
        static boost::thread update_thread;
        static bool should_check_for_updates;
        static void checkForUpdates();
    
    
};

#endif /* SHotLoadSystem_hpp */
