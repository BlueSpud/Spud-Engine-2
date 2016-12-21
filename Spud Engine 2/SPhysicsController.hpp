//
//  SPhysicsController.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/18/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SPhysicsController_hpp
#define SPhysicsController_hpp

#include "SRigidBody.hpp"

#define SQRT_2G 5.4221766847f

/******************************************************************************
 *  Definition for physics controller                                         *
 ******************************************************************************/

class SPhysicsController : public SRigidBody {
    
    public:
    
        SPhysicsController(float _mass, btCollisionShape* _collision_shape, STransform* _parent_transform);
    
        virtual void prePhysicsUpdate(const SEvent& event);
        virtual void postPhysicsUpdate(const SEvent& event);
    
        void setWalkingDirection(glm::vec3 direction);
        void jump();
    
        bool ignore_rotation = true;
    
        float jump_height = 2.2;
    
    protected:
    
        btVector3 walking_direction;
    
};

#endif /* SPhysicsController_hpp */
