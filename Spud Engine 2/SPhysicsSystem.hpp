//
//  SPhysicsSystem.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/14/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SPhysicsSystem_hpp
#define SPhysicsSystem_hpp

#include <btBulletDynamicsCommon.h>

#include "SEventSystem.hpp"
#include "SGL.hpp"

// Forward declarations
class SPhysicsGraph;

/******************************************************************************
 *  Definition for physics update event                                        *
 ******************************************************************************/

#define EVENT_PHYSICS_UPDATE 35

struct SEventPhysicsUpdate : public SEvent {};

/******************************************************************************
 *  Definition for physics system                                             *
 ******************************************************************************/

class SPhysicsSystem : public SSubsystem {
    
    friend class SPhysicsGraph;
    
    public:
    
        static void startup();
        static void shutdown();
    
        static void updatePhysics(double time_elapsed, int max_updates, double time_per_tick);
    
        static void rigidBodyTransformToSTransform(btRigidBody* rigid_body, STransform& transform);
    
        static SPhysicsGraph* current_physics_graph;
    
};

/******************************************************************************
 *  Definition for physics graph                                              *
 ******************************************************************************/

class SPhysicsGraph {
    
    friend class SPhysicsSystem;
    
    public:
    
        SPhysicsGraph();
        ~SPhysicsGraph();
    
    private:
    
        btBroadphaseInterface* bullet_broadphase;
        btDefaultCollisionConfiguration* bullet_collision_configuration;
        btCollisionDispatcher* bullet_collision_dispatcher;
        btSequentialImpulseConstraintSolver* bullet_constraint_solver;
        btDiscreteDynamicsWorld* bullet_world;
    
};

#endif /* SPhysicsSystem_hpp */
