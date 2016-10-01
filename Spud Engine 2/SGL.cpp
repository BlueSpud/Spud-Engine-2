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


GLuint SGL::rect_id;
GLuint SGL::rect_buffers[2];

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

    // Load the rect we use
    loadRect();
    
    
}

void SGL::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SGL shutdown");
    
    // Unload the rect we use for drawing
    unloadRect();
    
    // Shut down OpenGL
    glfwTerminate();

}

void SGL::createWindow() {
    
    // Set parameters of the window and the context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    if (!(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Spud Engine 2", NULL, NULL))) {
        
        // For some reason we couldnt make the context, log it and then shutdown
        SLog::verboseLog(SVerbosityLevel::Warning, "Failed to create an OpenGL context of at least version 3.2. Your GPU may not support this version of OpenGL, try updating your drivers");
        shutdown();
  
    }

    SLog::verboseLog(SVerbosityLevel::Debug, "Created a window with an OpenGL context version of: %i.%i",
                     glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR),
                     glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR));
    
    // Turn of VSync by default
    glfwSwapInterval(0);
    
}

bool SGL::windowIsGood() { return !glfwWindowShouldClose(window); }

void SGL::swapBuffers() { glfwSwapBuffers(window); }

void SGL::setKeyCallback(GLFWkeyfun func) { glfwSetKeyCallback(window, func); }

void SGL::setMouseInputMode(int mode) { glfwSetInputMode(window, GLFW_CURSOR, mode); }

void SGL::getMousePos(float* x_pos, float* y_pos) {

    // Get the curor pos into doubles first
    double x_d, y_d;
    glfwGetCursorPos(window, &x_d, &y_d);
    
    // Set the actual values we passed in
    *x_pos = (float)x_d;
    *y_pos = (float)y_d;

}

glm::ivec2 SGL::getWindowFramebufferSize() {
    
    // Get the size of the windows framebuffer
    glm::ivec2 size;
    glfwGetFramebufferSize(window, &size.x, &size.y);
    
    return size;
    
}

/******************************************************************************
 *  Functions for operations pertaining to graphics math                      *
 ******************************************************************************/

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

glm::mat4 SGL::transformToMatrix(const STransform& transform, double interpolation) {
    
    glm::mat4 to_return = glm::mat4(1.0);
    
    //Perform the operations on the matrix
    to_return = glm::translate(to_return, transform.translation + transform.translation_velocity * (float)interpolation);
    to_return = glm::rotate(to_return, transform.rotation.x + transform.rotation_velocity.x * (float)interpolation, x_axis);
    to_return = glm::rotate(to_return, transform.rotation.y + transform.rotation_velocity.y * (float)interpolation, y_axis);
    to_return = glm::rotate(to_return, transform.rotation.z + transform.rotation_velocity.z * (float)interpolation, z_axis);
    to_return = glm::scale(to_return, transform.scale + transform.scale_velocity * (float)interpolation);
    
    return to_return;
    
}

glm::mat4 SGL::getProjectionMatrix2D(const SViewport& viewport) { return glm::ortho(0.0f,
                                                                                    viewport.screen_size.x,
                                                                                    viewport.screen_size.y,
                                                                                    0.0f, -100.0f, 100.0f); }

glm::mat4 SGL::getProjectionMatrix3D(const SViewport3D& viewport) { return glm::perspective(viewport.field_of_view,
                                                                                          viewport.screen_size.x / viewport.screen_size.y,
                                                                                          viewport.planes.x,
                                                                                          viewport.planes.y); }

void SGL::setUpViewport(const SViewport& viewport) { glViewport(viewport.screen_pos.x,
                                                                  viewport.screen_pos.y,
                                                                  viewport.screen_size.x,
                                                                  viewport.screen_size.y); }

void SGL::drawRect(glm::vec2 position, glm::vec2 size) {
    
    // Make the matrix for the rect
    glm::mat4 matrix = glm::mat4(1.0);
    matrix = glm::translate(matrix, glm::vec3(position, 1.0));
    matrix = glm::scale(matrix, glm::vec3(size.x, size.y, 1.0));
    
    // Load the modelview and flush the matricies
    loadMatrix(matrix, MAT_MODEL_MATRIX);
    clearMatrix(MAT_VIEW_MATRIX);
    
    flushMatrix(MAT_PROJECTION_MATRIX);
    flushMatrix(MAT_VIEW_MATRIX);
    flushMatrix(MAT_MODEL_MATRIX);
    
    // Draw the array
    glBindVertexArray(rect_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
}

/******************************************************************************
 *  Functions for the matrix stack                                            *
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

glm::mat4 SGL::getMatrix(const char* mat_name) {
 
    // See if there is a matrix that exists by that name, otherwise return identity
    if (matrices.count(mat_name))
        return matrices[mat_name];
    else return glm::mat4(1.0);
    
}

void SGL::clearMatrix(const char* mat_name) {
    
    glm::mat4 mat_new = glm::mat4(1.0);
    loadMatrix(mat_new, mat_name);
    
}

void SGL::flushMatrix(const char* mat_name) {
    
    // Get the currently bound shader
    SShader* shader = SShader::getBoundShader();
    if (shader) {
        
        // Now tell it to upload a uniform of type matrix
        shader->bindUniform(&matrices[mat_name], mat_name, UNIFORM_MAT4, 1);
        
    }
    
}

/******************************************************************************
 *  Arrays for simple quad drawing                                            *
 ******************************************************************************/

float rect_position[] = { 0.0, 1.0, 0.0,
                          1.0, 1.0, 0.0,
                          0.0, 0.0, 0.0,
    
                          0.0, 0.0, 0.0,
                          1.0, 0.0, 0.0,
                          1.0, 1.0, 0.0 };

float rect_tex_coord[] = { 0.0, 0.0,
                           1.0, 0.0,
                           0.0, 1.0,
    
                           0.0, 1.0,
                           1.0, 1.0,
                           1.0, 0.0};

void SGL::loadRect() {
    
    // Create a simple 1x1 square that we can use to draw rects to the screen
    glGenVertexArrays(1, &rect_id);
    
    // Create the position buffer
    glGenBuffers(2, rect_buffers);
    glBindVertexArray(rect_id);
    
    glBindBuffer(GL_ARRAY_BUFFER, rect_buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18, rect_position, GL_STATIC_DRAW);
    
    // Tell OpenGL how to read this
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glBindBuffer(GL_ARRAY_BUFFER, rect_buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, rect_tex_coord, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glBindVertexArray(0);

}

void SGL::unloadRect() {

    // Delete the buffers and array
    glDeleteBuffers(2, rect_buffers);
    glDeleteVertexArrays(1, &rect_id);

}
