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

SPhysicsController::SPhysicsController(btConvexShape* _collision_shape, STransform* _parent_transform) {

    parent_transform = _parent_transform;
    collision_shape = _collision_shape;
    
    // Create a ghost object
    ghost_body = new btPairCachingGhostObject();
    moveControllerToParent(0.0);
    ghost_body->setCollisionShape(collision_shape);
    ghost_body->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
    
    // Create the character controller
    controller = new btKinematicCharacterController(ghost_body, collision_shape, 0.1f);
    controller->setUseGhostSweepTest(false);
    
    // Only listen to post-physics tick
    event_listener.listenToEvent(EVENT_PHYSICS_POSTUPDATE, EVENT_MEMBER(SPhysicsController::postPhysicsUpdate));
    
}

SPhysicsController::~SPhysicsController() {

    // Clean up
    delete controller;
    delete ghost_body;
    delete collision_shape;

}

void SPhysicsController::postPhysicsUpdate(const SEvent& event) {

    // Set the position of the parent transform to the position of the ghost body
    // TEMP, might need to add in yaw in future
    btTransform ghost_transform = ghost_body->getWorldTransform();
    btVector3 ghost_position = ghost_transform.getOrigin();
    
    std::cout << ghost_position.x() << " " << ghost_position.y() << " " << ghost_position.z() << std::endl;
    
    parent_transform->translation = glm::vec3(ghost_position.x(), ghost_position.y(), ghost_position.z());
    parent_transform->translation_velocity = glm::vec3(0.0);

}

void SPhysicsController::moveControllerToParent(double interpolation) {
    
    // Move the ghost body to the parent transform
    // TEMP, might need to add in yaw in future
    btTransform transform;
    transform.setOrigin(btVector3(parent_transform->translation.x + parent_transform->translation_velocity.x * interpolation,
                                  parent_transform->translation.y + parent_transform->translation_velocity.y * interpolation,
                                  parent_transform->translation.z + parent_transform->translation_velocity.z * interpolation));
    
    ghost_body->setWorldTransform(transform);
    
}

void SPhysicsController::addToPhysicsGraph(SPhysicsGraph* physics_graph) { physics_graph->addPhysicsController(ghost_body, controller); }
void SPhysicsController::removeFromPhysicsGraph(SPhysicsGraph* physics_graph) { physics_graph->removePhysicsController(ghost_body, controller); }

void SPhysicsController::setWalkingDirection(glm::vec3 direction) { controller->setWalkDirection(btVector3(direction.x, direction.y, direction.z)); }
