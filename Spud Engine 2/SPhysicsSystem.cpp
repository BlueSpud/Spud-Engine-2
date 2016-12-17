//
//  SPhysicsSystem.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SPhysicsSystem.hpp"

SPhysicsGraph* SPhysicsSystem::current_physics_graph;

/******************************************************************************
 *  Functions for physics system                                              *
 ******************************************************************************/

void SPhysicsSystem::startup() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SPhysicsSystem startup");

}

void SPhysicsSystem::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SPhysicsSystem shutdown");
    
    // If we have a physics graph, delete it
    if (current_physics_graph)
        delete current_physics_graph;

}

void SPhysicsSystem::updatePhysics(double time_elapsed, double interpolation, int max_updates, double time_per_tick) {
    
    // If there is a current physics graph, we update it
    if (current_physics_graph) {
        
        // Pre-physics update
        SEventPhysicsUpdate event;
        event.interpolation = interpolation;
        SEventSystem::postEvent(EVENT_PHYSICS_PREUPDATE, event);
        
        current_physics_graph->bullet_world->stepSimulation(time_elapsed, max_updates, time_per_tick);
        
        // post-physics update
        event = SEventPhysicsUpdate();
        SEventSystem::postEvent(EVENT_PHYSICS_POSTUPDATE, event);
        
    }
    
}

void SPhysicsSystem::rigidBodyTransformToSTransform(btRigidBody* rigid_body, STransform& transform) {
    
    // Get the entire transform of the object
    btMotionState* motionStatePlane = rigid_body->getMotionState();
    btTransform bullet_transform;
    motionStatePlane->getWorldTransform(bullet_transform);
    
    // Get position and rotation
    btVector3 rigid_body_origin = bullet_transform.getOrigin();
    btQuaternion rigid_body_rotation = bullet_transform.getRotation();
    
    // eulerAngles() returns pitch, yaw, roll, we have to swap it to be in yaw pitch roll
    glm::vec3 rotation = glm::eulerAngles(glm::quat(rigid_body_rotation.y(),
                                                    rigid_body_rotation.x(),
                                                    rigid_body_rotation.z(),
                                                    rigid_body_rotation.w()));
    
    transform.rotation = glm::vec3(rotation.x, rotation.y, rotation.z + M_PI);
    
    transform.translation = glm::vec3(rigid_body_origin.x(),
                                      rigid_body_origin.y(),
                                      rigid_body_origin.z());
    
}

/******************************************************************************
 *  Functions for physics graph                                               *
 ******************************************************************************/

SPhysicsGraph::SPhysicsGraph() {
    
    // Create the necessary components for a bullet physics world
    bullet_broadphase = new btDbvtBroadphase();
    
    bullet_collision_configuration = new btDefaultCollisionConfiguration();
    bullet_collision_dispatcher = new btCollisionDispatcher(bullet_collision_configuration);
    
    bullet_constraint_solver = new btSequentialImpulseConstraintSolver();
    
    // Create the bullet world
    bullet_world = new btDiscreteDynamicsWorld(bullet_collision_dispatcher,
                                               bullet_broadphase,
                                               bullet_constraint_solver,
                                               bullet_collision_configuration);
    
    // Set gravity, by default it is -9.8 m/s in the y direction
    bullet_world->setGravity(btVector3(0.0, -9.8, 0.0));
    
    
}

SPhysicsGraph::~SPhysicsGraph() {
    
    // Delete everything in reverse order
    delete bullet_world;
    delete bullet_constraint_solver;
    delete bullet_collision_dispatcher;
    delete bullet_collision_configuration;
    delete bullet_broadphase;
    
}

void SPhysicsGraph::addRigidBody(btRigidBody* rigid_body) { bullet_world->addRigidBody(rigid_body); }
void SPhysicsGraph::removeRigidBody(btRigidBody* rigid_body) { bullet_world->removeRigidBody(rigid_body); }
