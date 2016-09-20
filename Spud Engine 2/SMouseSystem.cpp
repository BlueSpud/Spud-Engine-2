//
//  SMouseSystem.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/17/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SMouseSystem.hpp"

glm::vec2 SMouseSystem::last_mouse_pos;

/******************************************************************************
 *  Functions for mouse system                                                *
 ******************************************************************************/

void SMouseSystem::startup() {
    
    SGL::setMouseMoveCallback(&SMouseSystem::mouseMoveCallback);
    SGL::setMouseInputMode(GLFW_CURSOR_DISABLED);
    
    SGL::getMousePos(&last_mouse_pos.x, &last_mouse_pos.y);
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SMouseSystem startup");
    
}

void SMouseSystem::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SMouseSystem shutdown");
    
}

void SMouseSystem::mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    
    // Calculate mouse velocity
    SEventMouseMove event;
    event.mouse_vel = glm::vec2(xpos, ypos) - last_mouse_pos;
    
    // Save the new position as the last position
    last_mouse_pos.x = xpos;
    last_mouse_pos.y = ypos;
    
    // Send out a mouse move event
    SEventSystem::postEvent(EVENT_MOUSE_MOVE, event);
    
}
