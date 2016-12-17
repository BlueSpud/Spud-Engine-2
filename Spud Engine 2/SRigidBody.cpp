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
    
    // Cannot use scale for transform, TEMP needs solution
    btTransform bullet_transform;
    bullet_transform.setIdentity();
    bullet_transform.setOrigin(btVector3(parent_transform->translation.x, parent_transform->translation.y, parent_transform->translation.z));
    bullet_transform.setRotation(btQuaternion(parent_transform->rotation.x, parent_transform->rotation.y, parent_transform->rotation.z));
    
    // Calculate the inertia
    btVector3 inertia;
    collision_shape->calculateLocalInertia(mass, inertia);
    
    btMotionState* motion_state = new btDefaultMotionState(bullet_transform);
    btRigidBody::btRigidBodyConstructionInfo info = btRigidBody::btRigidBodyConstructionInfo(mass, motion_state, collision_shape, inertia);
    
    bullet_rigid_body = new btRigidBody(info);
    
    // Have the event listener listen to the physics ticks
    event_listener.listenToEvent(EVENT_PHYSICS_PREUPDATE, EVENT_MEMBER(SRigidBody::prePhysicsUpdate));
    event_listener.listenToEvent(EVENT_PHYSICS_POSTUPDATE, EVENT_MEMBER(SRigidBody::postPhysicsUpdate));
    
}

void SRigidBody::prePhysicsUpdate(const SEvent& event) {
    
    // Get the event so we can get interpolation
    const SEventPhysicsUpdate& event_p = (const SEventPhysicsUpdate&)event;
    
    // If we dont have a mass, that means that the body is static and therfore needs to take on the parent transform
    if (!mass) {
        
        btTransform bullet_transform;
        bullet_transform.setIdentity();
        
        // Set the transform, making sure to take in account velocity
        bullet_transform.setOrigin(btVector3(parent_transform->translation.x + parent_transform->translation_velocity.x * event_p.interpolation,
                                             parent_transform->translation.y + parent_transform->translation_velocity.y * event_p.interpolation,
                                             parent_transform->translation.z + parent_transform->translation_velocity.z * event_p.interpolation));
        
        bullet_transform.setRotation(btQuaternion(parent_transform->rotation.x + parent_transform->rotation_velocity.x * event_p.interpolation,
                                                  parent_transform->rotation.y + parent_transform->rotation_velocity.y * event_p.interpolation,
                                                  parent_transform->rotation.z + parent_transform->rotation_velocity.z * event_p.interpolation));
    
        bullet_rigid_body->getMotionState()->setWorldTransform(bullet_transform);
        
    }
    
}

void SRigidBody::postPhysicsUpdate(const SEvent& event) {
    
    // If we have a mass, that means that the body is moving and therfore needs to change the parent transform
    if (mass) {
        
        STransform temp_transform;
        SPhysicsSystem::rigidBodyTransformToSTransform(bullet_rigid_body, temp_transform);
        
        // Set the parent transforms rotation and translation, making sure to zero velocities
        parent_transform->translation = temp_transform.translation;
        parent_transform->translation_velocity = glm::vec3();
        
        parent_transform->rotation = temp_transform.rotation;
        parent_transform->rotation_velocity = glm::vec3();
        
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

void SRigidBody::setMass(float _mass) {
    
    mass = _mass;
    
    // Calculate the new inertia
    btVector3 inertia;
    collision_shape->calculateLocalInertia(mass, inertia);
    
    bullet_rigid_body->setMassProps(mass, inertia);
    
}
