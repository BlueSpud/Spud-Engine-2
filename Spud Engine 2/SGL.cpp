//
//  SGL.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/5/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SGL.hpp"
#include "SShader.hpp"

GLFWwindow* SGL::window;
std::map<const char*, glm::mat4>SGL::matrices;

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Turn of VSync by default
    glfwSwapInterval(0);
    
    if (!(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Spud Engine 2", NULL, NULL))) {
        
        // For some reason we couldnt make the context, log it and then shutdown
        SLog::verboseLog(SVerbosityLevel::Warning, "Failed to create an OpenGL context of at least version 3.2. Your GPU may not support this version of OpenGL, try updating your drivers");
        shutdown();
  
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

void SGL::uploadMatrix(const glm::mat4& mat, const char* mat_name) {
    
    // Get the shader if it exists
    SShader* shader = SShader::getBoundShader();
    if (shader) {
        
        // Upload the matrix
        glUniformMatrix4fv(SShader::getUniformLocation(shader, mat_name), 1, GL_FALSE, &mat[0][0]);
        
    }

}

glm::mat4 SGL::transformToMatrix(const STransform& transform) {
    
    glm::mat4 to_return = glm::mat4(1.0);
    
    //Perform the operations on the matrix
    to_return = glm::translate(to_return, transform.translation);
    to_return = glm::rotate(to_return, transform.rotation.x, x_axis);
    to_return = glm::rotate(to_return, transform.rotation.y, y_axis);
    to_return = glm::rotate(to_return, transform.rotation.z, z_axis);
    to_return = glm::scale(to_return, transform.scale);
    
    return to_return;
    
}

glm::mat4 SGL::getProjectionMatrix(const SViewport3D& viewport) { return glm::perspective(viewport.field_of_view,
                                                                                          viewport.screen_size.x / viewport.screen_size.y,
                                                                                          viewport.planes.x,
                                                                                          viewport.planes.y); }

void SGL::setUpViewport(const SViewport3D& viewport) { glViewport(viewport.screen_pos.x,
                                                                  viewport.screen_pos.y,
                                                                  viewport.screen_size.x,
                                                                  viewport.screen_size.y); }

/******************************************************************************
 *  Functions for managing matrices on the GPU                                *
 ******************************************************************************/

void SGL::loadMatrix(const glm::mat4& mat, const char* mat_name) {
    
    // Load the matrix into the storage
    matrices[mat_name] = mat;

}

void SGL::mulMatrix(const glm::mat4& mat, const char* mat_name) {
    
    // Check if there is already a matrix with this name
    if (matrices.count(mat_name)) {
        
        // Multiply it and upload it
        const glm::mat4& mat_current = matrices[mat_name];
        matrices[mat_name] = mat_current * mat;
        
    } else {
        
        // Behave like a regular upload
        loadMatrix(mat, mat_name);
        
    }

}


void SGL::clearMatrix(const char* mat_name) {
    
    glm::mat4 mat_new = glm::mat4(1.0);
    loadMatrix(mat_new, mat_name);
    
}

void SGL::flushMatrix(const char* mat_name) {
    
    // Get the matrix and upload it to the shader
    uploadMatrix(matrices[mat_name], mat_name);
    
}
