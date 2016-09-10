//
//  object.cpp
//  GLFW Test
//
//  Created by Logan Pazol on 8/16/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "object.hpp"
#include "SEvents.hpp"

object::object() : scale_driver(&scale, 0.25, 20.0, 1.0) {

    // Listen to events
    event_listener.listenToEvent(EVENT_TICK, boost::bind(&object::update, this, _1));

}

void object::update(const SEvent& event) {
    
    position_x += velocity_x;
    position_y += velocity_y;
    
    scale_driver.updateValue();
    
}

void object::render(double interpolation) {

        
}
