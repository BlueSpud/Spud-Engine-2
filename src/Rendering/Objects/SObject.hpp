//
//  SObject.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SObject_hpp
#define SObject_hpp

#include "SEventSystem.hpp"
#include "SGbufferShader.hpp"

#include "SPhysicsSystem.hpp"

/******************************************************************************
 *  Definition for generic renderable object                                  *
 ******************************************************************************/

class SObject {
    
    public:
    
        SObject();
    
        virtual ~SObject() { /* intentionally empty */ }
    
        virtual void render(double interpolation) = 0;
		virtual void render(SGbufferShader* shader, double interpolation);
        virtual void update(const SEvent& event) = 0;
    
        virtual bool shouldBeRendered(const SFrustum& frustum) { return true; }
    
        virtual void onMoveToSceneGraph(SPhysicsGraph* physics_graph) { /* intentionally empty */ }
        virtual void onRemoveFromSceneGraph(SPhysicsGraph* physics_graph) { /* intentionally empty */ }
    
        STransform transform;
    
    protected:
    
        SEventListener event_listener;
    
    
};

#endif /* SObject_hpp */
