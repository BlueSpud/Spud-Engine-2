//
//  SUIWidget.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/16/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SUIWidget_hpp
#define SUIWidget_hpp

#include "SUI.hpp"

/******************************************************************************
 *  Definition for generic ui widget                                          *
 ******************************************************************************/

class SUIWidget {
    
    friend class SUI;
    
    public:
    
        virtual void render(double interpolation) = 0;
    
        SUIRect frame;
    
};

#endif /* SUIWidget_hpp */
