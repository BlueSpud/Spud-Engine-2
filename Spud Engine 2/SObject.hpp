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

/******************************************************************************
 *  Definition for generic renderable object                                  *
 ******************************************************************************/

class SObject {
    
    public:
    
        virtual void render(double interpolation) = 0;
        virtual void update() = 0;
    
    private:
    
        SEventListener event_listener;
    
    
};

#endif /* SObject_hpp */
