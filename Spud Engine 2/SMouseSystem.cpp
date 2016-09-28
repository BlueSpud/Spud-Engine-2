//
//  SMouseSystem.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/17/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SMouseSystem.hpp"

glm::vec2 SMouseSystem::last_mouse_pos;
glm::vec2 SMouseSystem::mouse_pos;

SEventListener SMouseSystem::event_listener;

/******************************************************************************
 *  Functions for mouse system                                                *
 ******************************************************************************/

void SMouseSystem::startup() {
    
    SGL::setMouseInputMode(GLFW_CURSOR_DISABLED);
    event_listener.listenToEvent(EVENT_TICK, &moveMouse);
    
    SGL::getMousePos(&last_mouse_pos.x, &last_mouse_pos.y);
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SMouseSystem startup");
    
}

void SMouseSystem::shutdown() {
    
    event_listener.stopListeningToEvent(EVENT_TICK);
    SLog::verboseLog(SVerbosityLevel::Debug, "SMouseSystem shutdown");
    
}

void SMouseSystem::moveMouse(const SEvent& event) {
    
    // Save the old position as the last position
    last_mouse_pos = mouse_pos;
    
    // Save the new position
    SGL::getMousePos(&mouse_pos.x, &mouse_pos.y);
    
    // Make an event and dispatch it
    SEventMouseMove event_m;
    event_m.mouse_vel = mouse_pos - last_mouse_pos;
    SEventSystem::postEvent(EVENT_MOUSE_MOVE, event_m);
    
}
