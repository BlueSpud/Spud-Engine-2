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
    
    // Initialize GLFW
    if (!glfwInit())
        exit(0);
    
    // Create a window
    createWindow();
    
    // Make the current context
    glfwMakeContextCurrent(window);

    
}

void SGL::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SGL startup");
    
    // Shut down OpenGL
    glfwTerminate();

}

void SGL::createWindow() {
    
    // Set parameters of the window and the context
    if (REQUEST_OPENGL_32) {
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
    }
    
    // Turn of VSync by default
    glfwSwapInterval(0);
    
    if (!(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Spud Engine 2", NULL, NULL))) {
        
        if (REQUEST_OPENGL_32) {
            
            // Make the context earlier, did not have a GPU to supprot OpenGL 3.2+
            SLog::verboseLog(SVerbosityLevel::Warning, "Failed to create OpenGL 3.2 context, GPU does not support it");
        
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
            
            if (!(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Spud Engine 2", NULL, NULL))) {
                
                // Window creation failed
                SLog::verboseLog(SVerbosityLevel::Critical, "Failed to create OpenGL context");
                shutdown();
                
            }
            
        } else {
            
            // Window creation failed
            SLog::verboseLog(SVerbosityLevel::Critical, "Failed to create OpenGL context");
            shutdown();
            
        }
    }

    
    SLog::verboseLog(SVerbosityLevel::Debug, "Created a window with an OpenGL context version of: %i.%i",
                     glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR),
                     glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR));
    
}

bool SGL::windowIsGood() { return !glfwWindowShouldClose(window); }

void SGL::swapBuffers() { glfwSwapBuffers(window); }

void SGL::setKeyCallback(GLFWkeyfun func) { glfwSetKeyCallback(window, func); }

/******************************************************************************
 *  Functions for operations pertaining to graphics math                      *
 ******************************************************************************/

glm::mat4 SGL::transformToMatrix(const STransform& transform) {
    
    glm::mat4 to_return = glm::mat4(1);
    
    //Perform the operations on the matrix
    glm::translate(to_return, transform.translation);
    glm::rotate(to_return, transform.rotation.y, x_axis);
    glm::rotate(to_return, transform.rotation.x, y_axis);
    glm::rotate(to_return, transform.rotation.z, z_axis);
    glm::scale(to_return, transform.scale);
    
    return to_return;
    
}
