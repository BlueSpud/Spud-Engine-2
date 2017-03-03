//
//  SPhysicsSystem.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SPhysicsSystem.hpp"

SPhysicsGraph* SPhysicsSystem::current_physics_graph;

physx::PxFoundation* SPhysicsSystem::physx_foundation;
physx::PxPhysics* SPhysicsSystem::physx_SDK;
physx::PxDefaultErrorCallback SPhysicsSystem::physx_error_callback;
physx::PxDefaultAllocator SPhysicsSystem::physx_allocator;
physx::PxCooking* SPhysicsSystem::physx_cooking;

/******************************************************************************
 *  Implementation for physics system                                         *
 ******************************************************************************/

void SPhysicsSystem::startup() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SPhysicsSystem startup");
    
    // Open up PhysX
    physx_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, physx_allocator, physx_error_callback);
    physx_SDK = PxCreatePhysics(PX_PHYSICS_VERSION, *physx_foundation, physx::PxTolerancesScale());
    physx_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *physx_foundation, physx::PxCookingParams(physx::PxTolerancesScale()));

}

void SPhysicsSystem::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SPhysicsSystem shutdown");
    
    // If we have a physics graph, delete it
    if (current_physics_graph)
        delete current_physics_graph;
    
    // Close down PhysX
    physx_cooking->release();
    physx_SDK->release();
    physx_foundation->release();

}

void SPhysicsSystem::updatePhysics(double time_elapsed, double interpolation) {
    
    // If there is a current physics graph, we update it
    if (current_physics_graph) {
        
        // Pre-physics update
        SEventPhysicsUpdate event;
        event.interpolation = interpolation;
        event.time_elapsed = time_elapsed;
        SEventSystem::postEvent(EVENT_PHYSICS_PREUPDATE, event);
        
        // PhysX update
        current_physics_graph->physx_scene->simulate(time_elapsed);
        current_physics_graph->physx_scene->fetchResults(true);
        
        // post-physics update
        event = SEventPhysicsUpdate();
        SEventSystem::postEvent(EVENT_PHYSICS_POSTUPDATE, event);
        
    }
    
}

physx::PxTransform SPhysicsSystem::STransformToPxTransform(const STransform& transform, double interpolation) {
    
    // Create a quaternian to represent rotation
    glm::quat rotation;
    rotation = glm::rotate(rotation, transform.rotation.z + transform.rotation_velocity.z * (float)interpolation, z_axis);
    rotation = glm::rotate(rotation, transform.rotation.y + transform.rotation_velocity.y * (float)interpolation, y_axis);
    rotation = glm::rotate(rotation, transform.rotation.x + transform.rotation_velocity.x * (float)interpolation, x_axis);
    
    // Create a PhysX transform
    return physx::PxTransform(physx::PxVec3(transform.translation.x + transform.translation_velocity.x * (float)interpolation,
                                            transform.translation.y + transform.translation_velocity.y * (float)interpolation,
                                            transform.translation.z + transform.translation_velocity.z * (float)interpolation),
                              physx::PxQuat(-rotation.z, rotation.y, -rotation.x, rotation.w));
    
}

void SPhysicsSystem::PxTransformToSTransform(const physx::PxTransform& physx_transform, STransform& transform) {
    
    // Get rotation, set velocity to 0
    physx::PxQuat rotation = physx_transform.q;
    transform.rotation = glm::eulerAngles(glm::quat(-rotation.z, rotation.y, -rotation.x, rotation.w));
    transform.rotation_velocity = glm::vec3(0.0);
    
    // Get translation, set velocity to 0
    physx::PxVec3 translation = physx_transform.p;
    transform.translation = glm::vec3(translation.x, translation.y, translation.z);
    transform.translation_velocity = glm::vec3(0.0);
    
}

physx::PxCooking* SPhysicsSystem::getCooking() { return physx_cooking; }

/******************************************************************************
 *  Implementation for physics graph                                          *
 ******************************************************************************/

SPhysicsGraph::SPhysicsGraph() {
    
    // Create thr PhysX scene descriptor, gravity and the like
    physx::PxSceneDesc scene_desc = physx::PxSceneDesc(SPhysicsSystem::physx_SDK->getTolerancesScale());
    scene_desc.gravity = physx::PxVec3(0.0, -PHYSICS_G, 0.0);
    cpu_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    scene_desc.cpuDispatcher = cpu_dispatcher;
    scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;
    
    // Create the actual scene
    physx_scene = SPhysicsSystem::physx_SDK->createScene(scene_desc);
    
    // Create the characer manager
    character_manager = PxCreateControllerManager(*physx_scene);
    character_manager->setOverlapRecoveryModule(true);
    
}

SPhysicsGraph::~SPhysicsGraph() {
    
    // Destroy the PhysX scene
    character_manager->release();
    physx_scene->release();
    cpu_dispatcher->release();
    
}

void SPhysicsGraph::addActor(physx::PxActor* actor) { physx_scene->addActor(*actor); }
void SPhysicsGraph::removeActor(physx::PxActor* actor) { physx_scene->removeActor(*actor); }

physx::PxController* SPhysicsGraph::createCharacterController(const physx::PxControllerDesc& description) {
    
    // Create a character controller with the given description
    return character_manager->createController(description);
    
}

