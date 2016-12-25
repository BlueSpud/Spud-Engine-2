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

#define SQRT_2G sqrt(2.0f * PHYSICS_G)
#define CHARACTER_VELOCITY_EPSILON 0.005

/******************************************************************************
 *  Definition for character controller                                       *
 ******************************************************************************/

class SCharacterController : physx::PxUserControllerHitReport {
    
    public:
    
        SCharacterController(SPhysicsGraph* physics_graph,
                             physx::PxMaterial* material,
                             glm::vec2 size,
                             float _step_size,
                             float _slope_limit,
                             STransform* _parent_transform);
    
        virtual void prePhysicsUpdate(const SEvent& event);
        virtual void postPhysicsUpdate(const SEvent& event);
    
        virtual void onShapeHit(const physx::PxControllerShapeHit& hit);
        virtual void onControllerHit(const physx::PxControllersHit& hit);
        virtual void onObstacleHit(const physx::PxControllerObstacleHit& hit);
    
        void setMoveDirection(glm::vec3 direction);
        bool isOnGround();
    
        void jump();
        void setJumpHeight(float _jump_height);
    
        float movement_speed = 5.0;
    
    protected:
    
        STransform* parent_transform;
        physx::PxController* physx_controller;
    
        physx::PxVec3 walking_direction;
        physx::PxVec3 floor_normal;
        float y_velocity;
        float jump_vel = sqrtf(2.0) * SQRT_2G;
    
        bool is_on_ground = false;
    
        SEventListener event_listener;
    
    
};

#endif /* SCharacterController_hpp */
