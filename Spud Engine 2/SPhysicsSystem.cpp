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

/******************************************************************************
 *  Functions for physics system                                              *
 ******************************************************************************/

void SPhysicsSystem::startup() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SPhysicsSystem startup");
    
    // Open up PhysX
    physx_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, physx_allocator, physx_error_callback);
    physx_SDK = PxCreatePhysics(PX_PHYSICS_VERSION, *physx_foundation, physx::PxTolerancesScale());

}

void SPhysicsSystem::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SPhysicsSystem shutdown");
    
    // If we have a physics graph, delete it
    if (current_physics_graph)
        delete current_physics_graph;
    
    // Close down PhysX
    physx_SDK->release();
    physx_foundation->release();

}

void SPhysicsSystem::updatePhysics(double time_elapsed, double interpolation, int max_updates, double time_per_tick) {
    
    // If there is a current physics graph, we update it
    if (current_physics_graph) {
        
        // Pre-physics update
        SEventPhysicsUpdate event;
        event.interpolation = interpolation;
        SEventSystem::postEvent(EVENT_PHYSICS_PREUPDATE, event);
        
        // PhysX update
        current_physics_graph->physx_scene->simulate(time_elapsed);
        current_physics_graph->physx_scene->fetchResults(true);
        
        // post-physics update
        event = SEventPhysicsUpdate();
        SEventSystem::postEvent(EVENT_PHYSICS_POSTUPDATE, event);
        
    }
    
}

/******************************************************************************
 *  Functions for physics graph                                               *
 ******************************************************************************/

SPhysicsGraph::SPhysicsGraph() {
    
    // Create thr PhysX scene descriptor, gravity and the like
    physx::PxSceneDesc scene_desc = physx::PxSceneDesc(SPhysicsSystem::physx_SDK->getTolerancesScale());
    scene_desc.gravity = physx::PxVec3(0.0, -9.81, 0.0);
    cpu_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    scene_desc.cpuDispatcher = cpu_dispatcher;
    scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;
    
    // Create the actual scene
    physx_scene = SPhysicsSystem::physx_SDK->createScene(scene_desc);
    
    
}

SPhysicsGraph::~SPhysicsGraph() {
    
    // Destroy the PhysX scene
    physx_scene->release();
    cpu_dispatcher->release();
    
}

void SPhysicsGraph::addActor(physx::PxActor* actor) { physx_scene->addActor(*actor); }
void SPhysicsGraph::removeActor(physx::PxActor* actor) { physx_scene->removeActor(*actor); }
