//
//  SCharacterController.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/22/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SCharacterController.hpp"

/******************************************************************************
 *  Implementation for character controller                                   *
 ******************************************************************************/

SCharacterController::SCharacterController(SPhysicsGraph* physics_graph,
                                           physx::PxMaterial* material,
                                           glm::vec2 size,
                                           float _step_size,
                                           float _slope_limit,
                                           STransform* _parent_transform) {
    
    parent_transform = _parent_transform;
    
    // Create a descriptor
    physx::PxCapsuleControllerDesc controller_desc;
    
    controller_desc.height = size.y;
    controller_desc.radius = size.x;
    
    // Stepping information
    controller_desc.climbingMode = physx::PxCapsuleClimbingMode::eCONSTRAINED;
    controller_desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
    controller_desc.slopeLimit = _slope_limit;
    controller_desc.stepOffset = _step_size;
    
    controller_desc.upDirection = physx::PxVec3(0.0, 1.0, 0.0);
    controller_desc.material = material;
    controller_desc.density = 0.0001;
    
    // Create the controller
    physx_controller = physics_graph->createCharacterController(controller_desc);
    physx_controller->setPosition(physx::PxExtendedVec3(parent_transform->translation.x,
                                                        parent_transform->translation.y,
                                                        parent_transform->translation.z));
    
    // Have the event listener listen to the physics ticks
    event_listener.listenToEvent(EVENT_PHYSICS_PREUPDATE, EVENT_MEMBER(SCharacterController::prePhysicsUpdate));
    event_listener.listenToEvent(EVENT_PHYSICS_POSTUPDATE, EVENT_MEMBER(SCharacterController::postPhysicsUpdate));
    
}

void SCharacterController::prePhysicsUpdate(const SEvent& event) {
    
    const SEventPhysicsUpdate& event_p = (const SEventPhysicsUpdate&)event;
    
    physx::PxVec3 movement_direction = walking_direction * movement_speed;
	
	// We do a raycast to determine the floor normal to slide across it
	// We have to remove the actor from the scene because if we dont we will hit it
	physx_controller->getScene()->removeActor(*physx_controller->getActor());

	physx::PxRaycastHit hit;
	physx_controller->getScene()->raycastSingle(physx::toVec3(physx_controller->getPosition()), physx::PxVec3(0.0, -1.0, 0.0), PHYSICS_G * 100.0, physx::PxHitFlag::eNORMAL, hit);
	floor_normal = hit.normal;
	
	physx_controller->getScene()->addActor(*physx_controller->getActor());
	
    // If we're on the ground we change the movement vector along the normal
    // Check the normal threshold to make sure we can actually climb it
    float dot = physx_controller->getUpDirection().dot(floor_normal);
    float normal_angle = acos(dot);
	
    if (isOnGround() && normal_angle < physx_controller->getSlopeLimit()) {
		
        // Do some math to get a rotation matrix
        physx::PxQuat quat;
        physx::PxVec3 cross_product = physx_controller->getUpDirection().cross(floor_normal);
        quat.x = cross_product.x;
        quat.y = cross_product.y;
        quat.z = cross_product.z;
        
        // We can use 1.0 because we the vectors are normalized so mag1^2 * mag2^2 = 1.0
        quat.w = 1.0 + dot;
        quat.normalize();
        
        // Rotate it alone the normal
        physx::PxMat33 rotation_matrix = physx::PxMat33(quat);
        movement_direction = rotation_matrix * movement_direction;
        
    }
    
    // Set the walking direction to the y-velocity
    movement_direction.y = movement_direction.y + y_velocity;
	
    // Update the controller walking
    physx_controller->move(movement_direction * event_p.time_elapsed, 0.0, event_p.time_elapsed, physx::PxControllerFilters());
    
    // Calculate new y-velocity
    if (!isOnGround())
        y_velocity = y_velocity - PHYSICS_G * event_p.time_elapsed;
    else y_velocity = 0.0;
    
}

void SCharacterController::postPhysicsUpdate(const SEvent& event) {
    
    // Set the parent transform translation, velocity is 0
    // Make sure we're sane
    physx::PxExtendedVec3 position = physx_controller->getPosition();
    parent_transform->translation = glm::vec3(position.x, position.y, position.z);
    parent_transform->translation_velocity = glm::vec3(0.0);

}
void SCharacterController::setMoveDirection(glm::vec3 direction) {

    physx::PxVec3 new_walking_direction = physx::PxVec3(direction.x, 0.0, direction.z);
    
    // Check if the direction has a magnitude, if so normalize
	if (glm::length(direction)) {
		
		// Make sure that its normalized
        new_walking_direction.normalize();
		
	} else {
		
		// Fade out the velocity
		if (walking_direction.magnitude() > 0.01)
			new_walking_direction = walking_direction * 0.65;
		
	}
    
    walking_direction = new_walking_direction;

}

bool SCharacterController::isOnGround() {
    
    // Move the capsule down and see it it ended up in the same position
    physx::PxExtendedVec3 last_position = physx_controller->getPosition();
    physx_controller->move(physx::PxVec3(0.0, -PHYSICS_G * 100.0, 0.0), 0.0, 1.0, physx::PxControllerFilters());
    float delta_y = physx_controller->getPosition().y - last_position.y;
    
    if (delta_y < -CHARACTER_GROUND_EPSILON) {
        
        // Reset position
        physx_controller->setPosition(last_position);
        return false;
        
    }
    
    return true;
}

void SCharacterController::jump() { y_velocity += jump_vel; }
void SCharacterController::setJumpHeight(float _jump_height) { jump_vel = sqrtf(_jump_height) * SQRT_2G; }
