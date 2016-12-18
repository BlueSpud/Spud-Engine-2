//
//  SRigidBody.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/17/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SRigidBody.hpp"

/******************************************************************************
 *  Functions for rigid body                                                  *
 ******************************************************************************/

SRigidBody::SRigidBody(float _mass, btCollisionShape* _collision_shape, STransform* _parent_transform) {
    
    // Create a new motion state, using the parents transform
    mass = _mass;
    collision_shape = _collision_shape;
    parent_transform = _parent_transform;
    
    // Make an empty transform
    btTransform bullet_transform;
    
    // Calculate the inertia
    btVector3 inertia;
    collision_shape->calculateLocalInertia(mass, inertia);
    
    btMotionState* motion_state = new btDefaultMotionState(bullet_transform);
    btRigidBody::btRigidBodyConstructionInfo info = btRigidBody::btRigidBodyConstructionInfo(mass, motion_state, collision_shape, inertia);
    
    // Create the bullet rigid body
    bullet_rigid_body = new btRigidBody(info);
    
    // Move the rigidbody to the parent
    moveRigidBodyToParent(0.0);
    
    // Have the event listener listen to the physics ticks
    event_listener.listenToEvent(EVENT_PHYSICS_PREUPDATE, EVENT_MEMBER(SRigidBody::prePhysicsUpdate));
    event_listener.listenToEvent(EVENT_PHYSICS_POSTUPDATE, EVENT_MEMBER(SRigidBody::postPhysicsUpdate));
    
}

SRigidBody::~SRigidBody() {
    
    // Clean up
    delete bullet_rigid_body;
    delete collision_shape;
    
}

void SRigidBody::prePhysicsUpdate(const SEvent& event) {
    
    // Get the event so we can get interpolation
    const SEventPhysicsUpdate& event_p = (const SEventPhysicsUpdate&)event;
    
    // If we dont have a mass, that means that the body is static and therfore needs to take on the parent transform
    if (!mass)
        moveRigidBodyToParent(event_p.interpolation);
    
}

void SRigidBody::postPhysicsUpdate(const SEvent& event) {
    
    // If we have a mass, that means that the body is moving and therfore needs to change the parent transform
    if (mass) {
        
        btTransform bullet_transform;
        bullet_rigid_body->getMotionState()->getWorldTransform(bullet_transform);
        SPhysicsSystem::bulletTransformToSTransform(bullet_transform, *parent_transform);
        
        // Zero the velocities because bullet handles interpolation for us
        parent_transform->translation_velocity = glm::vec3(0.0);
        parent_transform->rotation_velocity = glm::vec3(0.0);
        
    }
    
}

void SRigidBody::addToPhysicsGraph(SPhysicsGraph* physics_graph) {
    
    // Add the bullet body to the physics graph
    physics_graph->addRigidBody(bullet_rigid_body);

}

void SRigidBody::removeFromPhysicsGraph(SPhysicsGraph* physics_graph) {

    // Add the bullet body to the physics graph
    physics_graph->addRigidBody(bullet_rigid_body);
    
}

void SRigidBody::moveRigidBodyToParent(double interpolation) {
    
    // Create a new motion state with the new transform
    btMotionState* new_motion_state = new btDefaultMotionState(SPhysicsSystem::STransformToBulletTransform(*parent_transform, interpolation));
    bullet_rigid_body->setMotionState(new_motion_state);
    
}

void SRigidBody::setMass(float _mass) {
    
    mass = _mass;
    
    // Calculate the new inertia
    btVector3 inertia;
    collision_shape->calculateLocalInertia(mass, inertia);
    
    bullet_rigid_body->setMassProps(mass, inertia);
    
}
