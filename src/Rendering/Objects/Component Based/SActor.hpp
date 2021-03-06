//
//  SActor.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/8/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SActor_hpp
#define SActor_hpp

#include "Rendering/Objects/SObject.hpp"
#include "Rendering/Objects/Component Based/SComponent.hpp"

/******************************************************************************
 *  Definition for actor                                                      *
 ******************************************************************************/

class SActor : public SObject {

    SCLASS(SActor)

    public:
    
        SActor();
    
        virtual void render(double interpolation);
        virtual void update(const SEvent& event);
    
        virtual void onMoveToSceneGraph(SPhysicsGraph* physics_graph);
        virtual void onRemoveFromSceneGraph(SPhysicsGraph* physics_graph);
    
    //protected:
    
        SComponent* root_component;
    
};

#endif /* SActor_hpp */
