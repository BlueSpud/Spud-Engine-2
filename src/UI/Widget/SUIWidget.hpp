//
//  SUIWidget.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/16/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SUIWidget_hpp
#define SUIWidget_hpp

#include "UI/SUI.hpp"
#include "Event/Input/SInputSystem.hpp"

/******************************************************************************
 *  Definition for generic ui widget                                          *
 ******************************************************************************/

class SUIWidget {
    
    friend class SUI;
    
    public:
    
        virtual void render(double interpolation) = 0;
        SUIRect frame;
    
        virtual ~SUIWidget() { /* intentionally empty */ }
    
    protected:
    
        // Booleans for events captured
        bool hovering = false;
    
        virtual void onHover() { /* intentionally empty */ }
        virtual void onPress(int button) { /* intentionally empty */ }
        virtual void onRelease(int button) { /* intentionally empty */ }

    
        SInputListener input_listener;
    
};

#endif /* SUIWidget_hpp */
