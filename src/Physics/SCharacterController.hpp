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
#define CHARACTER_GROUND_EPSILON 0.01f
#define CHARACTER_COLLISION_EPSILON 0.001f
#define CHARACTER_SLIDE_EPSILON 0.0001f

/******************************************************************************
 *  Definition for character controller                                       *
 ******************************************************************************/

class SCharacterController {
    
    public:
	
		SCharacterController(SPhysicsGraph* _physics_graph, physx::PxGeometry* _shape, glm::vec2 size, STransform* _parent_transform);
		SCharacterController(SPhysicsGraph* _physics_graph, glm::vec2 size, STransform* _parent_transform);
	
		~SCharacterController();
	
        virtual void prePhysicsUpdate(const SEvent& event);
        virtual void postPhysicsUpdate(const SEvent& event);
    
        void setMoveDirection(glm::vec3 direction);
        bool isOnGround();
		bool isOnGround(physx::PxVec3& normal);
    
        void jump();
        void setJumpHeight(float _jump_height);
    
        float movement_speed = 5.0;
    
    protected:
    
        STransform* parent_transform;
		physx::PxTransform transform;
		SPhysicsGraph* physics_graph;
	
		physx::PxConvexMesh* cylinder_mesh;
		physx::PxGeometry* shape;
		void createCylinder(glm::vec2& size);
	
		void performMoveSweep(physx::PxVec3 movement_direction, int itterations = 5);
	
		physx::PxVec3 walking_direction = physx::PxVec3(0.0, 0.0, 0.0);
        float jump_vel = sqrtf(0.5) * SQRT_2G;
		float y_velocity = 0.0;
    
        SEventListener event_listener;
	
    
    
};

#endif /* SCharacterController_hpp */
