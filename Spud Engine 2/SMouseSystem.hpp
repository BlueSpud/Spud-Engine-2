//
//  SMouseSystem.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/17/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SMouseSystem_hpp
#define SMouseSystem_hpp

#include "SEventSystem.hpp"
#include "SGL.hpp"

/******************************************************************************
 *  Definition for mouse events                                               *
 ******************************************************************************/

#define EVENT_MOUSE_MOVE 120

struct SEventMouseMove : public SEvent {
    
    glm::vec2 mouse_vel;
    
};



/******************************************************************************
 *  Definition for mouse system                                               *
 ******************************************************************************/

class SMouseSystem : public SSubsystem {
    
    public:
    
        static void startup();
        static void shutdown();
    
        static void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
    
        static glm::vec2 last_mouse_pos;
    
};


#endif /* SMouseSystem_hpp */
