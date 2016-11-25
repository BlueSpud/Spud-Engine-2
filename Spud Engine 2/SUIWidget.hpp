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
#include "SInputSystem.hpp"

/******************************************************************************
 *  Definition for generic ui widget                                          *
 ******************************************************************************/

class SUIWidget {
    
    friend class SUI;
    
    public:
    
        virtual void render(double interpolation) = 0;
        SUIRect frame;
    
    protected:
    
        // Booleans for events captured
        bool has_event_pressed = false;
        bool has_event_released = false;
        bool has_event_hover = false;
    
        bool captures_pressed = false;
    
        SInputListener input_listener;
    
};

#endif /* SUIWidget_hpp */
