//
//  SKeyboardSystem.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SKeyboardSystem_hpp
#define SKeyboardSystem_hpp

#include "SEventSystem.hpp"
#include "SGL.hpp"

#define KEY_ACTION_DOWN GLFW_PRESS
#define KEY_ACTION_REPEAT GLFW_REPEAT
#define KEY_ACTION_UP GLFW_RELEASE

enum SKeyboardMode {
    
    SKeyboardModeGame,
    SKeyboardModeUI
    
};

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
    
        void setCharCallback(boost::function<void(int)> function);
    
        void setHasFocus();
        void setHasFocusUI();
    
        bool repeats = false;
    
    protected:
    
        std::map<int, boost::function<void(int)>>key_down_funcs;
        std::map<int, boost::function<void(int)>>key_up_funcs;
    
        virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
        boost::function<void(int)> char_func;

};

/******************************************************************************
 *  Definition for keyboard system                                            *
 ******************************************************************************/

class SKeyboardSystem : public SSubsystem {
    
    friend class SKeyboardListener;
    
    public:
    
        static void startup();
        static void shutdown();
    
        static SKeyboardMode keyboard_mode;
    
    private:
    
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void charCallback(GLFWwindow* window, unsigned int unicode_value);
    
        static SKeyboardListener* current_keyboard_listener;
        static SKeyboardListener* current_keyboard_listener_ui;
    
};

#endif /* SKeyboardSystem_hpp */
