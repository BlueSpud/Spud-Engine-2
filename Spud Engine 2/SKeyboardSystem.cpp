//
//  SKeyboardSystem.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SKeyboardSystem.hpp"

SKeyboardMode SKeyboardSystem::keyboard_mode;

SKeyboardListener* SKeyboardSystem::current_keyboard_listener;
SKeyboardListener* SKeyboardSystem::current_keyboard_listener_ui;

/******************************************************************************
 *  Functions for keyboard listener                                           *
 ******************************************************************************/

void SKeyboardListener::bind(boost::function<void(int)> function, int key, int action) {
    
    // Decide which map to add the funciton to and then add it
    if (action == KEY_ACTION_DOWN)
        key_down_funcs[key] = function;
    
    if (action == KEY_ACTION_UP)
        key_up_funcs[key] = function;
    
}

void SKeyboardListener::unbind(int key, int action) {
    
    // Rempve the key function for the correct action
    if (action == KEY_ACTION_DOWN)
        key_down_funcs.erase(key);
    
    if (action == KEY_ACTION_UP)
        key_down_funcs.erase(key);
    
}

void SKeyboardListener::setHasFocus() { SKeyboardSystem::current_keyboard_listener = this; }
void SKeyboardListener::setHasFocusUI() { SKeyboardSystem::current_keyboard_listener_ui = this; }

void SKeyboardListener::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    
    // Send the action to the corret map and function
    if (action == KEY_ACTION_DOWN || (action == KEY_ACTION_REPEAT && repeats)) {
        
        if (key_down_funcs.count(key)) {
            
            // Fire off the function
            key_down_funcs[key](key);
            
        }
        
    }
    
    if (action == KEY_ACTION_UP) {
        
        if (key_up_funcs.count(key)) {
            
            // Fire off the function
            key_up_funcs[key](key);
            
        }
        
    }
}

void SKeyboardListener::setCharCallback(boost::function<void(int)> _char_func) { char_func = _char_func; }

/******************************************************************************
 *  Functions for keyboard system                                             *
 ******************************************************************************/

void SKeyboardSystem::startup() {

    SLog::verboseLog(SVerbosityLevel::Debug, "SKeyboardSystem startup");
    
    // Set the key callbacks
    SGL::setKeyCallback(keyCallback);
    SGL::setCharCallback(charCallback);

}

void SKeyboardSystem::shutdown() {

    SLog::verboseLog(SVerbosityLevel::Debug, "SKeyboardSystem startup");

}

void SKeyboardSystem::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    
    // Send out the key message to the client that currently has input
    if (keyboard_mode == SKeyboardModeGame) {
        
        if (current_keyboard_listener)
            current_keyboard_listener->keyCallback(window, key, scancode, action, mods);
    } else  if (current_keyboard_listener_ui)
            current_keyboard_listener_ui->keyCallback(window, key, scancode, action, mods);
    
}

void SKeyboardSystem::charCallback(GLFWwindow* window, unsigned int unicode_value) {
    
    // Send out the key message to the client that currently has input
    if (keyboard_mode == SKeyboardModeGame) {
        
    if (current_keyboard_listener && current_keyboard_listener->char_func)
        current_keyboard_listener->char_func(unicode_value);
        
    } else if (current_keyboard_listener_ui && current_keyboard_listener_ui->char_func)
        current_keyboard_listener_ui->char_func(unicode_value);
    
}
