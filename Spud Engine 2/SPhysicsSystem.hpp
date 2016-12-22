//
//  SPhysicsSystem.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SPhysicsSystem_hpp
#define SPhysicsSystem_hpp

#define NDEBUG

#include <PxPhysicsAPI.h>

#include "SEventSystem.hpp"
#include "SGL.hpp"

// Forward declarations
class SPhysicsGraph;

/******************************************************************************
 *  Definition for physics update event                                        *
 ******************************************************************************/

#define EVENT_PHYSICS_PREUPDATE 35
#define EVENT_PHYSICS_POSTUPDATE 36

struct SEventPhysicsUpdate : public SEvent {

    double interpolation;

};

/******************************************************************************
 *  Definition for physics system                                             *
 ******************************************************************************/

class SPhysicsSystem : public SSubsystem {
    
    friend class SPhysicsGraph;
    
    public:
    
        static void startup();
        static void shutdown();
    
        static void updatePhysics(double time_elapsed, double interpolation, int max_updates, double time_per_tick);
    
        static SPhysicsGraph* current_physics_graph;
    
    private:
    
        static physx::PxFoundation* physx_foundation;
        static physx::PxPhysics* physx_SDK;
        static physx::PxDefaultErrorCallback physx_error_callback;
        static physx::PxDefaultAllocator physx_allocator;
    
};

/******************************************************************************
 *  Definition for physics graph                                              *
 ******************************************************************************/

class SPhysicsGraph {
    
    friend class SPhysicsSystem;
    
    public:
    
        SPhysicsGraph();
        ~SPhysicsGraph();
    
        void addActor(physx::PxActor* actor);
        void removeActor(physx::PxActor* actor);
    
    private:
    
        physx::PxScene* physx_scene;
        physx::PxDefaultCpuDispatcher* cpu_dispatcher;
    
};

#endif /* SPhysicsSystem_hpp */
