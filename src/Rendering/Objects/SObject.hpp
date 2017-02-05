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
#include "SShader.hpp"

#include "SPhysicsSystem.hpp"

/******************************************************************************
 *  Definition for generic renderable object                                  *
 ******************************************************************************/

class SObject {
    
    public:
    
        SObject();
    
        virtual ~SObject() { /* intentionally empty */ }
    
        virtual void render(bool render_material, double interpolation) = 0;
        virtual void update(const SEvent& event) = 0;
    
        virtual bool shouldBeRendered(const glm::mat4& projection_view_matrix) { return true; }
    
        virtual void onMoveToSceneGraph(SPhysicsGraph* physics_graph) { /* intentionally empty */ }
        virtual void onRemoveFromSceneGraph(SPhysicsGraph* physics_graph) { /* intentionally empty */ }
    
        STransform transform;
    
    protected:
    
        SEventListener event_listener;
    
    
};

#endif /* SObject_hpp */
