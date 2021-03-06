//
//  SComponent.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/8/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SComponent_hpp
#define SComponent_hpp

#include "Rendering/Objects/SObject.hpp"

/******************************************************************************
 *  Definition for generic component                                          *
 ******************************************************************************/

class SComponent : public SObject {
    
    public:
    
        virtual void render(double interpolation);
        virtual void update(const SEvent& event);
    
        virtual void onMoveToSceneGraph(SPhysicsGraph* physics_graph);
        virtual void onRemoveFromSceneGraph(SPhysicsGraph* physics_graph);
    
        void attatchComponent(SComponent* new_child_componenet);
    
    private:
    
    protected:
    
        SComponent* parent_component = nullptr;
    
        std::vector<SComponent*> child_components;
    
    
};

#endif /* SComponent_hpp */
