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
    controller_desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
    controller_desc.slopeLimit = cosf(M_PI / 4.0);
    controller_desc.stepOffset = 0.2;
    
    controller_desc.upDirection = physx::PxVec3(0.0, 1.0, 0.0);
    controller_desc.material = material;
    
    // Set the callback to recieve collision
    controller_desc.reportCallback = this;
    
    // Create the controller
    physx_controller = physics_graph->createCharacterController(controller_desc);
    
    // Have the event listener listen to the physics ticks
    event_listener.listenToEvent(EVENT_PHYSICS_PREUPDATE, EVENT_MEMBER(SCharacterController::prePhysicsUpdate));
    event_listener.listenToEvent(EVENT_PHYSICS_POSTUPDATE, EVENT_MEMBER(SCharacterController::postPhysicsUpdate));
    
}

void SCharacterController::prePhysicsUpdate(const SEvent& event) {
    
    const SEventPhysicsUpdate& event_p = (const SEventPhysicsUpdate&)event;
    
    physx::PxVec3 movement_direction = walking_direction;
    
    // If we're on the ground we change the movement vector along the normal
    // Check the normal threshold to make sure we can actually climb it
    float dot = physx_controller->getUpDirection().dot(floor_normal);
    if (isOnGround() && acos(dot) < M_PI / 3.0) {
        
        // Do some math to get a rotation matrix
        physx::PxQuat quat;
        physx::PxVec3 cross_product = physx_controller->getUpDirection().cross(floor_normal);
        quat.x = cross_product.x;
        quat.y = cross_product.y;
        quat.z = cross_product.z;
        
        // We can use 1.0 because we normalize the vectors
        quat.w = 1.0 + dot;
        quat.normalize();
        
        // Rotate it alone the normal
        physx::PxMat33 rotation_matrix = physx::PxMat33(quat);
        movement_direction = rotation_matrix * movement_direction;
        
    }
    
    // Set the walking direction to the y-velocity
    movement_direction.y = movement_direction.y + y_velocity;
    
    // Update the controller walking
    physx_controller->move(movement_direction * event_p.time_elapsed, 0.01f, event_p.time_elapsed, physx::PxControllerFilters());
    
    // Calculate new y-velocity
    if (!isOnGround())
        y_velocity = y_velocity - PHYSICS_G * event_p.time_elapsed;
    else y_velocity = 0.0;
    
}

void SCharacterController::postPhysicsUpdate(const SEvent& event) {
    
    // Set the parent transform translation, velocity is 0
    physx::PxExtendedVec3 position = physx_controller->getPosition();
    parent_transform->translation = glm::vec3(position.x, position.y, position.z);
    parent_transform->translation_velocity = glm::vec3(0.0);

}

void SCharacterController::onShapeHit(const physx::PxControllerShapeHit& hit) {
    
    // If the motion direction was the up direction, take the floor normal
    if (hit.dir == -physx_controller->getUpDirection())
        floor_normal = hit.worldNormal.getNormalized();
    
}

void SCharacterController::onControllerHit(const physx::PxControllersHit& hit) { /* intentionally blank */ }
void SCharacterController::onObstacleHit(const physx::PxControllerObstacleHit& hit) { /* intentionally blank */ }

void SCharacterController::setMoveDirection(glm::vec3 direction) { walking_direction = physx::PxVec3(direction.x, 0.0, direction.z); }

bool SCharacterController::isOnGround() {
    
    // Move the capsule down and see it it ended up in the same position
    physx::PxExtendedVec3 last_position = physx_controller->getPosition();
    physx_controller->move(physx::PxVec3(0.0, -PHYSICS_G * 100.0, 0.0), 0.01f, 1.0, physx::PxControllerFilters());
    float delta_y = physx_controller->getPosition().y - last_position.y;
    
    if (delta_y < -CHARACTER_VELOCITY_EPSILON) {
        
        // Reset position
        physx_controller->setPosition(last_position);
        return false;
        
    }
    
    return true;
}

void SCharacterController::jump() {
    
    // Add jump velocity in case we want to implement jumping while falling / double jump
    float delta_v_y = sqrt_jump_height * SQRT_2G;
    y_velocity += delta_v_y;
    
}

void SCharacterController::setJumpHeight(float _jump_height) { sqrt_jump_height = sqrtf(_jump_height); }
