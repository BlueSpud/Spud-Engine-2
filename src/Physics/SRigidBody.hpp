//
//  SRigidBody.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/22/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SRigidBody_hpp
#define SRigidBody_hpp

#include "SPhysicsSystem.hpp"

/******************************************************************************
 *  Definition for rigid body                                                 *
 ******************************************************************************/

class SRigidBody {
    
    public:
    
        SRigidBody(float _mass, physx::PxGeometry* _collision_geometry, physx::PxMaterial* _material, STransform* _parent_transform);
        ~SRigidBody();
    
        virtual void prePhysicsUpdate(const SEvent& event);
        virtual void postPhysicsUpdate(const SEvent& event);
    
        void addToPhysicsGraph(SPhysicsGraph* physics_graph);
        void removeFromPhysicsGraph(SPhysicsGraph* physics_graph);
    
    private:
    
        void moveRigidBodyToParent(double interpolation);
    
        float mass;
        STransform* parent_transform;
    
        physx::PxRigidActor* rigid_body;
        physx::PxShape* physx_shape;
        physx::PxMaterial* physx_material;
    
        SEventListener event_listener;
    
};

#endif /* SRigidBody_hpp */
