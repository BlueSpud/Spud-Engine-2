//
//  SGL.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/5/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SGL_hpp
#define SGL_hpp

#include <iostream>
#include <GLFW/glfw3.h>

#include "SSubsystem.hpp"

#define WINDOW_WIDTH 1440
#define WINDOW_HEIGHT 800

/******************************************************************************
 *  Definition for OpenGL subsystem                                           *
 ******************************************************************************/

class SGL : public SSubsystem {
    
    public:
    
        static void startup();
        static void shutdown();
    
        static bool windowGood();
    
        static void swapBuffers();
    
        static void setKeyCallback(GLFWkeyfun func);
    
    
    private:
    
        static GLFWwindow* window;
    
};

#endif /* SGL_hpp */
