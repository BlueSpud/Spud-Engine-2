//
//  SPhysicsController.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/18/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SPhysicsController.hpp"

/******************************************************************************
 *  Functions for physics controller                                          *
 ******************************************************************************/

SPhysicsController::SPhysicsController(float _mass, btCollisionShape* _collision_shape, STransform* _parent_transform) : SRigidBody(_mass, _collision_shape, _parent_transform) {
    
    // Set the properties of the rigid body that make it behave how we want it
    // Make it always stand up and never sleep
    bullet_rigid_body->setSleepingThresholds(0.0, 0.0);
    bullet_rigid_body->setAngularFactor(0.0);
    
}

void SPhysicsController::prePhysicsUpdate(const SEvent& event) {
    
    // Set the velocity of the rigid body to the walking direction, we ignore the y direction because you cant walk in that directions
    btVector3 new_velocity = bullet_rigid_body->getLinearVelocity();
    new_velocity.setX(walking_direction.x());
    new_velocity.setZ(walking_direction.z());
    
    bullet_rigid_body->setLinearVelocity(new_velocity);
    
}


void SPhysicsController::postPhysicsUpdate(const SEvent& event) {
    
    // Perform a similar uppdate as the rigid body, but we ignore rotation unless specified
    btTransform bullet_transform;
    bullet_rigid_body->getMotionState()->getWorldTransform(bullet_transform);
    
    STransform new_transform;
    SPhysicsSystem::bulletTransformToSTransform(bullet_transform, new_transform);
    
    parent_transform->translation = new_transform.translation;
    
    // Usually we ignore rotation, but if for some reason we dont that is done here
    if (!ignore_rotation) {
        
        parent_transform->rotation = new_transform.rotation;
        parent_transform->rotation_velocity = glm::vec3(0.0);
        
    }
    
    // Zero the velocities because bullet handles interpolation for us
    parent_transform->translation_velocity = glm::vec3(0.0);
    
}


void SPhysicsController::setWalkingDirection(glm::vec3 direction) { walking_direction = btVector3(direction.x,
                                                                                                  direction.y,
                                                                                                  direction.z); }

void SPhysicsController::jump() {
    
    // Calculate the needed initial velocity to reach the height we want
    float initial_velocity = sqrt(jump_height) * SQRT_2G;
    
    btVector3 new_velocity = bullet_rigid_body->getLinearVelocity();
    new_velocity.setY(initial_velocity);
    bullet_rigid_body->setLinearVelocity(new_velocity);
    
    
}
