//
//  SCharacterController.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/22/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SCharacterController.hpp"

/******************************************************************************
 *  Functions for character controller                                        *
 ******************************************************************************/

SCharacterController::SCharacterController(SPhysicsGraph* physics_graph, physx::PxMaterial* material, STransform* _parent_transform) {
    
    parent_transform = _parent_transform;
    
    // Create a descriptor
    physx::PxCapsuleControllerDesc controller_desc;
    
    controller_desc.height = 2.0;
    controller_desc.radius = 0.2;
    controller_desc.position = physx::PxExtendedVec3(parent_transform->translation.x,
                                                     parent_transform->translation.y,
                                                     parent_transform->translation.z);
    
    // Stepping information
    controller_desc.climbingMode = physx::PxCapsuleClimbingMode::eCONSTRAINED;
    controller_desc.slopeLimit = cosf(M_PI / 4.0);
    controller_desc.stepOffset = 0.2;
    
    controller_desc.upDirection = physx::PxVec3(0.0, 1.0, 0.0);
    controller_desc.material = material;
    
    // Create the controller
    physx_controller = physics_graph->createCharacterController(controller_desc);
    
    // Have the event listener listen to the physics ticks
    event_listener.listenToEvent(EVENT_PHYSICS_PREUPDATE, EVENT_MEMBER(SCharacterController::prePhysicsUpdate));
    event_listener.listenToEvent(EVENT_PHYSICS_POSTUPDATE, EVENT_MEMBER(SCharacterController::postPhysicsUpdate));
    
}

void SCharacterController::prePhysicsUpdate(const SEvent& event) {
    
    const SEventPhysicsUpdate& event_p = (const SEventPhysicsUpdate&)event;
    
    // Set the walking direction to the y-velocity
    walking_direction.y = y_velocity;
    
    // Update the controller walking
    physx_controller->move(walking_direction * event_p.time_elapsed, 0.01f, event_p.time_elapsed, physx::PxControllerFilters());
    
    // Get the stats of the character
    physx::PxControllerState controller_state;
    physx_controller->getState(controller_state);
    
    // Return if it is on the ground
    is_on_ground = (controller_state.collisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN) != 0;
    
    // Calculate new y-velocity
    if (!is_on_ground)
        y_velocity = y_velocity - PHYSICS_G * event_p.time_elapsed;
    else y_velocity = 0.0;
    
}

void SCharacterController::postPhysicsUpdate(const SEvent& event) {
    
    // Set the parent transform translation, velocity is 0
    physx::PxExtendedVec3 position = physx_controller->getPosition();
    parent_transform->translation = glm::vec3(position.x, position.y, position.z);
    parent_transform->translation_velocity = glm::vec3(0.0);

}

void SCharacterController::setMoveDirection(glm::vec3 direction) { walking_direction = physx::PxVec3(direction.x, -PHYSICS_G, direction.z); }

bool SCharacterController::isOnGround() { return is_on_ground; }

void SCharacterController::jump() {
    
    // Add jump velocity in case we want to implement jumping while falling / double jump
    float delta_v_y = sqrt_jump_height * SQRT_2G;
    y_velocity += delta_v_y;
    
}

void SCharacterController::setJumpHeight(float _jump_height) { sqrt_jump_height = sqrtf(_jump_height); }
