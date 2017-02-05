//
//  SActor.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/8/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SActor_hpp
#define SActor_hpp

#include "SObject.hpp"
#include "SComponent.hpp"

/******************************************************************************
 *  Definition for actor                                                      *
 ******************************************************************************/

class SActor : public SObject {
    
    public:
    
        SActor();
    
        virtual void render(bool render_material, double interpolation);
        virtual void update(const SEvent& event);
    
        virtual void onMoveToSceneGraph(SPhysicsGraph* physics_graph);
        virtual void onRemoveFromSceneGraph(SPhysicsGraph* physics_graph);
    
    //protected:
    
        SComponent* root_component;
    
};

#endif /* SActor_hpp */
