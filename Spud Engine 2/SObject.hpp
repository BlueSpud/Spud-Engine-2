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
#include "SGL.hpp"

/******************************************************************************
 *  Definition for generic renderable object                                  *
 ******************************************************************************/

class SObject {
    
    public:
    
        virtual void render(double interpolation) = 0;
        virtual void update() = 0;
    
        virtual bool shouldBeRendered(const glm::mat4& projection_view_matrix) { return true; }
    
        virtual ~SObject() { /* stub */ }
    
        STransform transform;
    
    private:
    
        SEventListener event_listener;
    
    
};

#endif /* SObject_hpp */
