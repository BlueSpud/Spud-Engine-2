//
//  object.cpp
//  GLFW Test
//
//  Created by Logan Pazol on 8/16/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "object.hpp"

object::object() : scale_driver(&scale, 0.25, 20.0, 1.0) {

    // Listen to events
    event_listener.listenToEvent(0, boost::bind(&object::update, this, _1));

}

object::~object() {
    
    std::cout << "Object dealloc\n";
    
}

void object::update(const SEvent& event) {
    
    position_x += velocity_x;
    position_y += velocity_y;
    
    scale_driver.updateValue();
    
}

void object::render(double interpolation) {

    glLoadIdentity();
    
    double position_int_x = position_x + velocity_x * interpolation;
    double position_int_y = position_y + velocity_y * interpolation;
    
    double scale_int = scale_driver.getRenderValue(interpolation);
    
    glTranslated(position_int_x, position_int_y, 0);
    glScalef(scale_int, scale_int, scale_int);
    
    glColor4f(1.0, 1.0, 1.0, 1.0);
    
    glBegin(GL_QUADS);
        glVertex2f(-2.5, -2.5);
        glVertex2f(2.5, -2.5);
        glVertex2f(2.5, 2.5);
        glVertex2f(-2.5, 2.5);
    glEnd();
    
}
