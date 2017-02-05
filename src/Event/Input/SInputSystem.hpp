//
//  SInputSystem.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/23/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SInputSystem_hpp
#define SInputSystem_hpp

#include "SEventSystem.hpp"
#include "SUI.hpp"

#define INPUT_ACTION_DOWN GLFW_PRESS
#define INPUT_ACTION_REPEAT GLFW_REPEAT
#define INPUT_ACTION_UP GLFW_RELEASE


// Forward declarations
class SInputSystem;

/******************************************************************************
 *  Definition for input modes                                                *
 ******************************************************************************/

enum SInputMode {
    
    SInputModeGame,
    SInputModeUI
    
};

/******************************************************************************
 *  Definition for input listener                                             *
 ******************************************************************************/

class SInputListener {
    
    friend class SInputSystem;
    friend class SUI;
    
    public:
    
        void bind(boost::function<void(int)> function, int button, int action);
        void unbind(int button, int action);
    
        void setHasFocus();
    
        bool repeats = false;
        boost::function<void(int)> char_func;
        boost::function<void(glm::vec2)> mouse_move_func;
    
    protected:
    
        std::map<int, boost::function<void(int)>>down_funcs;
        std::map<int, boost::function<void(int)>>up_funcs;
    
        virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
};

/******************************************************************************
 *  Definition for input system                                               *
 ******************************************************************************/

class SInputSystem : public SSubsystem {
    
    friend class SInputListener;
    
    public:
    
        static void startup();
        static void shutdown();
    
        static void setInputMode(SInputMode _input_mode);
    
        static void moveMouse();
    
    private:
    
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void charCallback(GLFWwindow* window, unsigned int unicode_value);
        static void mouseCallback(GLFWwindow* window, int button, int action, int mods);
    
        static glm::vec2 mouse_pos;
        static glm::vec2 last_mouse_pos;
    
        static SInputListener* current_input_listener;
    
        static SInputMode input_mode;
    
};

#endif /* SInputSystem_hpp */
