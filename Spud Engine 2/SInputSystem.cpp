//
//  SInputSystem.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/23/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SInputSystem.hpp"

SInputMode SInputSystem::input_mode;

glm::vec2 SInputSystem::last_mouse_pos;
glm::vec2 SInputSystem::mouse_pos;

SInputListener* SInputSystem::current_input_listener;
SEventListener SInputSystem::event_listener;

/******************************************************************************
 *  Functions for input listener                                              *
 ******************************************************************************/

void SInputListener::bind(boost::function<void(int)> function, int key, int action) {
    
    // Decide which map to add the funciton to and then add it
    if (action == INPUT_ACTION_DOWN)
        down_funcs[key] = function;
    
    if (action == INPUT_ACTION_UP)
        up_funcs[key] = function;
    
}

void SInputListener::unbind(int key, int action) {
    
    // Rempve the key function for the correct action
    if (action == INPUT_ACTION_DOWN)
        up_funcs.erase(key);
    
    if (action == INPUT_ACTION_UP)
        up_funcs.erase(key);
    
}

void SInputListener::setHasFocus() { SInputSystem::current_input_listener = this; }

void SInputListener::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    
    // Send the action to the corret map and function
    if (action == INPUT_ACTION_DOWN || (action == INPUT_ACTION_REPEAT && repeats)) {
        
        if (down_funcs.count(key)) {
            
            // Fire off the function
            down_funcs[key](key);
            
        }
        
    }
    
    if (action == INPUT_ACTION_UP) {
        
        if (up_funcs.count(key)) {
            
            // Fire off the function
            up_funcs[key](key);
            
        }
        
    }
}

/******************************************************************************
 *  Functions for input system                                                *
 ******************************************************************************/

void SInputSystem::startup() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SInputSystem startup");
    
    // Set the key callbacks
    SGL::setKeyCallback(keyCallback);
    SGL::setCharCallback(charCallback);
    
    // Set the mouse button callback
    SGL::setMouseCallback(mouseCallback);
    
    // Set the mouse mode and get the initial position and listen to the tick event for mouse movement
    SGL::setMouseInputMode(GLFW_CURSOR_DISABLED);
    event_listener.listenToEvent(EVENT_TICK, &moveMouse);
    
    SGL::getMousePos(&last_mouse_pos.x, &last_mouse_pos.y);

}

void SInputSystem::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SInputSystem shutdown");
    
    // Stop listening for mouse movement
    event_listener.stopListeningToEvent(EVENT_TICK);

}

void SInputSystem::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    
    // Send out the key message to the client that currently has input
    if (input_mode == SInputModeGame) {
        
        if (current_input_listener)
            current_input_listener->keyCallback(window, key, scancode, action, mods);
        
    } else { SUI::keyCallback(window, key, scancode, action, mods); }
    
}

void SInputSystem::charCallback(GLFWwindow* window, unsigned int unicode_value) {
    
    // Send out the key message to the client that currently has input
    if (input_mode == SInputModeGame) {
        
        if (current_input_listener && current_input_listener->char_func)
            current_input_listener->char_func(unicode_value);
        
    } else { SUI::charCallback(window, unicode_value); }
    
}

void SInputSystem::mouseCallback(GLFWwindow* window, int button, int action, int mods) {
    
    // Send out the mouse message to the client that currently has input
    if (input_mode == SInputModeGame) {
        
        // We just call the key funciton with the mosue button
        if (current_input_listener)
            current_input_listener->keyCallback(window, button, 0, action, mods);
        
    } else { SUI::mouseCallback(window, button, action, mods); }
    
}

void SInputSystem::moveMouse(const SEvent& event) {
    
    // Save the old position as the last position
    last_mouse_pos = mouse_pos;
    
    // Save the new position
    SGL::getMousePos(&mouse_pos.x, &mouse_pos.y);
    
    // Call the move mouse callback on the input listener
    if (input_mode == SInputModeGame) {
        
        if (current_input_listener && current_input_listener->mouse_move_func)
            current_input_listener->mouse_move_func(mouse_pos - last_mouse_pos);
        
    } else { /* Send input to UI */ }
    
}
