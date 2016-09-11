//
//  SKeyboardSystem.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SKeyboardSystem_hpp
#define SKeyboardSystem_hpp

#include <boost/function.hpp>

#include "SSubsystem.hpp"
#include "SGL.hpp"

#define KEY_ACTION_DOWN GLFW_PRESS
#define KEY_ACTION_UP GLFW_RELEASE

// Forward declarations
class SKeyboardSystem;

/******************************************************************************
 *  Definition for keyboard listener                                          *
 ******************************************************************************/

class SKeyboardListener {
    
    friend class SKeyboardSystem;
    
    public:
    
        void bind(boost::function<void(int)> function, int key, int action);
        void unbind(int key, int action);
    
        void setHasFocus();
    
    private:
    
        std::map<int, boost::function<void(int)>>key_down_funcs;
        std::map<int, boost::function<void(int)>>key_up_funcs;
    
        void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

};

/******************************************************************************
 *  Definition for keyboard system                                            *
 ******************************************************************************/

class SKeyboardSystem : public SSubsystem {
    
    friend class SKeyboardListener;
    
    public:
    
        static void startup();
        static void shutdown();
    
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
    private:
    
        static SKeyboardListener* current_keyboard_listener;
    
    
};

#endif /* SKeyboardSystem_hpp */
