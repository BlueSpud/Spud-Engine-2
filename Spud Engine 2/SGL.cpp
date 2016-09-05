//
//  SGL.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/5/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SGL.hpp"

GLFWwindow* SGL::window;

/******************************************************************************
 *  Functions for OpenGL system                                               *
 ******************************************************************************/

void SGL::startup() {

    SLog::verboseLog(SVerbosityLevel::Debug, "SGL startup");
    
    /* Initialize the library */
    if (!glfwInit())
        exit(0);
    
    // Check if the window was able to be made
    if (!(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Spud Engine 2", NULL, NULL))) {
        
        glfwTerminate();
        exit(0);
    }
    
    // Make the current context
    glfwMakeContextCurrent(window);

    
}

void SGL::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SGL startup");
    
    // Shut down OpenGL
    glfwTerminate();

}

bool SGL::windowGood() { return !glfwWindowShouldClose(window); }

void SGL::swapBuffers() { glfwSwapBuffers(window); }

void SGL::setKeyCallback(GLFWkeyfun func) { glfwSetKeyCallback(window, func); }
