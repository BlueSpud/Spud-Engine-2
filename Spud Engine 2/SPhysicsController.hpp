//
//  SPhysicsController.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/18/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SPhysicsController_hpp
#define SPhysicsController_hpp

#include "SPhysicsSystem.hpp"

/******************************************************************************
 *  Definition for physics controller                                         *
 ******************************************************************************/

class SPhysicsController {
    
    public:
    
        SPhysicsController(btConvexShape* _collision_shape, STransform* _parent_transform);
        ~SPhysicsController();
    
        void postPhysicsUpdate(const SEvent& event);
    
        void moveControllerToParent(double interpolation);
    
        void addToPhysicsGraph(SPhysicsGraph* physics_graph);
        void removeFromPhysicsGraph(SPhysicsGraph* physics_graph);
    
        void setWalkingDirection(glm::vec3 direction);
    
    private:
    
        STransform* parent_transform;
    
        btConvexShape* collision_shape;
        btKinematicCharacterController* controller;
        btPairCachingGhostObject* ghost_body;
    
        SEventListener event_listener;
    
        glm::vec3 last_walk;
    
};

#endif /* SPhysicsController_hpp */
