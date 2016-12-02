//
//  SSoundSystem.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/30/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SSoundSystem_hpp
#define SSoundSystem_hpp

#include <AL/al.h>
#include <AL/alc.h>

#include "SSubsystem.hpp"
#include "SGL.hpp"

/******************************************************************************
 *  Definition for sound system                                               *
 ******************************************************************************/

class SSoundSystem : public SSubsystem {
    
    public:
    
        static void startup();
        static void shutdown();
    
        static void updateListenerPosition(double interpolation);
    
    private:
    
        static ALCdevice* device;
        static ALCcontext* context;
    
        static glm::vec3 listener_position;
        static glm::vec3 listener_velocity;
    
};

#endif /* SSoundSystem_hpp */
