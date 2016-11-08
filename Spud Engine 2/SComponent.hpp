//
//  SComponent.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/8/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SComponent_hpp
#define SComponent_hpp

#include "SObject.hpp"

/******************************************************************************
 *  Definition for generic component                                          *
 ******************************************************************************/

class SComponent : public SObject {
    
    public:
    
        virtual void render(bool render_material, double interpolation);
        virtual void update();
    
        void attatchComponent(SComponent* new_child_componenet);
    
    private:
    
    protected:
    
        SComponent* parent_component = nullptr;
    
        std::vector<SComponent*> child_components;
    
    
};

#endif /* SComponent_hpp */
