//
//  SCharacterController.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/22/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SCharacterController_hpp
#define SCharacterController_hpp

#include "SPhysicsSystem.hpp"

#define SQRT_2G sqrt(2 * PHYSICS_G)

/******************************************************************************
 *  Definition for character controller                                       *
 ******************************************************************************/

class SCharacterController {
    
    public:
    
    SCharacterController(SPhysicsGraph* physics_graph, physx::PxMaterial* material, STransform* _parent_transform);
    
        virtual void prePhysicsUpdate(const SEvent& event);
        virtual void postPhysicsUpdate(const SEvent& event);
    
        void setMoveDirection(glm::vec3 direction);
        bool isOnGround();
    
        void jump();
        void setJumpHeight(float _jump_height);
    
    protected:
    
        STransform* parent_transform;
        physx::PxController* physx_controller;
    
        physx::PxVec3 walking_direction;
        float y_velocity;
        float sqrt_jump_height = sqrtf(2.0);
    
        bool is_on_ground = false;
    
        SEventListener event_listener;
    
    
};

#endif /* SCharacterController_hpp */
