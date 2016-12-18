//
//  SRigidBody.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/17/16.
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
    
        SRigidBody(float _mass, btCollisionShape* _collision_shape, STransform* _parent_transform);
        ~SRigidBody();
    
        void prePhysicsUpdate(const SEvent& event);
        void postPhysicsUpdate(const SEvent& event);
    
        void addToPhysicsGraph(SPhysicsGraph* physics_graph);
        void removeFromPhysicsGraph(SPhysicsGraph* physics_graph);
    
        void moveRigidBodyToParent(double interpolation);
    
        void setMass(float _mass);
    
    private:
    
        float mass;
        STransform* parent_transform;
    
        btRigidBody* bullet_rigid_body;
        btCollisionShape* collision_shape;
    
        SEventListener event_listener;
    
    
};

#endif /* SRigidBody_hpp */
