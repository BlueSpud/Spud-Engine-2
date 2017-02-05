//
//  SRigidBody.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/22/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SRigidBody.hpp"

/******************************************************************************
 *  Implementation for rigid body                                             *
 ******************************************************************************/

SRigidBody::SRigidBody(float _mass, physx::PxGeometry* _collision_geometry, physx::PxMaterial* _material, STransform* _parent_transform) {
    
    mass = _mass;
    parent_transform = _parent_transform;
    physx_material = _material;
    
    // Create the PhysX rigid body
    physx_shape = PxGetPhysics().createShape(*_collision_geometry, *physx_material);
    
    // Create a rigid body, static or dynamic, with an identity transform
    physx::PxTransform transform;
    
    if (mass) {
        
        // Dynamic needs to be cast
        rigid_body = PxGetPhysics().createRigidDynamic(transform);
        physx::PxRigidBodyExt::updateMassAndInertia(*(physx::PxRigidDynamic*)rigid_body, mass);
        
    } else rigid_body = PxGetPhysics().createRigidStatic(transform);
    
    // Attatch the shape and calculate inertia
    rigid_body->attachShape(*physx_shape);
    
    // Move the rigid body to the parent
    moveRigidBodyToParent(0.0);
    
    // Have the event listener listen to the physics ticks
    event_listener.listenToEvent(EVENT_PHYSICS_PREUPDATE, EVENT_MEMBER(SRigidBody::prePhysicsUpdate));
    event_listener.listenToEvent(EVENT_PHYSICS_POSTUPDATE, EVENT_MEMBER(SRigidBody::postPhysicsUpdate));
    
}

SRigidBody::~SRigidBody() {
    
    // Unload the assets
    rigid_body->release();
    physx_material->release();
    physx_shape->release();
    
}

void SRigidBody::prePhysicsUpdate(const SEvent& event) {
    
    // Update the static rigid body
    if (!mass) {
        
        // Move it to the parent transform
        const SEventPhysicsUpdate& event_p = (const SEventPhysicsUpdate&)event;
        moveRigidBodyToParent(event_p.interpolation);
        
    }
    
}

void SRigidBody::postPhysicsUpdate(const SEvent& event) {
    
    // Update the parent transform for the dynamic rigid body
    if (mass)
        SPhysicsSystem::PxTransformToSTransform(rigid_body->getGlobalPose(), *parent_transform);
    
}

void SRigidBody::moveRigidBodyToParent(double interpolation) {
    
    // Create a new transform for the body
    physx::PxTransform new_transform = SPhysicsSystem::STransformToPxTransform(*parent_transform, interpolation);
    rigid_body->setGlobalPose(new_transform);
    
}

void SRigidBody::addToPhysicsGraph(SPhysicsGraph* physics_graph) { physics_graph->addActor(rigid_body); }
void SRigidBody::removeFromPhysicsGraph(SPhysicsGraph* physics_graph) { physics_graph->removeActor(rigid_body); }
