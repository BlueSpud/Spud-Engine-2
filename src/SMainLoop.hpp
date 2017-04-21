//
//  SMainLoop.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/12/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SMainLoop_hpp
#define SMainLoop_hpp

#include "Rendering/SRenderSystem.hpp"
#include "UI/SUI.hpp"
#include "Physics/SPhysicsSystem.hpp"
#include "Sound/SSoundSystem.hpp"

#include "Event/Input/SInputSystem.hpp"

#include "Engine/Utility/STime.hpp"

#define MAX_UPDATES_BEFORE_RENDER 5

/******************************************************************************
 *  Definition for main loop                                                  *
 ******************************************************************************/

class SMainLoop {
    
    public:
    
        static int loop();
    
};

#endif /* SMainLoop_hpp */
