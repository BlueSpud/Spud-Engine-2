//
//  SGL.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/5/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SGL.hpp"
#include "Resource/Resources/Rendering/Shader/SShader.hpp"

GLFWwindow* SGL::window;
glm::mat4 SGL::matrices[3] = {glm::mat4(1.0), glm::mat4(1.0), glm::mat4(1.0)};

const char* SGL::matrix_names[3] = {"mat_model", "mat_view", "mat_projection"};

GLuint SGL::rect_id;
GLuint SGL::rect_buffers[2];

bool SGL::window_focused = true;

/******************************************************************************
 *  Implementation for transform                                              *
 ******************************************************************************/

void STransform::update() {
	
	// Add the velocities on
	translation += translation_velocity;
	rotation += rotation_velocity;
	scale += scale_velocity;
	
}

glm::vec3 STransform::getForwardVector(double interpolation) const {
	
	// Calculate a few things we need more than once
	float modified_yaw = (rotation.y + rotation_velocity.y * interpolation) - M_PI_2;
	float cos_pitch = cos(rotation.x + rotation_velocity.x * interpolation);
	
	// Calculate the vector
	return glm::normalize(glm::vec3(cos(modified_yaw) * cos_pitch,
									sin(rotation.x + rotation_velocity.x * interpolation),
									sin(modified_yaw) * cos_pitch));
	
}

void STransform::serialize(SSerializer& serializer) {
	
	// Transform should never be written out alone, so it doesnt need a class hash
	
	// Add all of the various vectors we store
	serializer.addItem(&translation);
	serializer.addItem(&translation_velocity);
	
	serializer.addItem(&rotation);
	serializer.addItem(&rotation_velocity);
	
	serializer.addItem(&scale);
	serializer.addItem(&scale_velocity);
	
}


void STransform::deserialize(SDeserializer& deserializer) {
	
	// Get back all of what we serialized in order
	deserializer.deserialize(&translation);
	deserializer.deserialize(&translation_velocity);
	
	deserializer.deserialize(&rotation);
	deserializer.deserialize(&rotation_velocity);
	
	deserializer.deserialize(&scale);
	deserializer.deserialize(&scale_velocity);
	
}


/******************************************************************************
 *  Implementation for frustum						     					  *
 ******************************************************************************/

SFrustum::SFrustum(const glm::mat4 projection_view_matrix) {
	
	// Extract a frustum's planes from the projection view matrix
	planes[0] = glm::normalize(glm::vec4(projection_view_matrix[0][3] + projection_view_matrix[0][0],
										 projection_view_matrix[1][3] + projection_view_matrix[1][0],
										 projection_view_matrix[2][3] + projection_view_matrix[2][0],
										 projection_view_matrix[3][3] + projection_view_matrix[3][0]));
		
	planes[1] = glm::normalize(glm::vec4(projection_view_matrix[0][3] - projection_view_matrix[0][0],
										 projection_view_matrix[1][3] - projection_view_matrix[1][0],
										 projection_view_matrix[2][3] - projection_view_matrix[2][0],
										 projection_view_matrix[3][3] - projection_view_matrix[3][0]));
		
	planes[2] = glm::normalize(glm::vec4(projection_view_matrix[0][3] + projection_view_matrix[0][1],
										 projection_view_matrix[1][3] + projection_view_matrix[1][1],
										 projection_view_matrix[2][3] + projection_view_matrix[2][1],
										 projection_view_matrix[3][3] + projection_view_matrix[3][1]));
		
	planes[3] = glm::normalize(glm::vec4(projection_view_matrix[0][3] - projection_view_matrix[0][1],
										 projection_view_matrix[1][3] - projection_view_matrix[1][1],
										 projection_view_matrix[2][3] - projection_view_matrix[2][1],
										 projection_view_matrix[3][3] - projection_view_matrix[3][1]));
		
	planes[4] = glm::normalize(glm::vec4(projection_view_matrix[0][2],
										 projection_view_matrix[1][2],
										 projection_view_matrix[2][2],
										 projection_view_matrix[3][2]));
		
	planes[5] = glm::normalize(glm::vec4(projection_view_matrix[0][3] - projection_view_matrix[0][2],
										 projection_view_matrix[1][3] - projection_view_matrix[1][2],
										 projection_view_matrix[2][3] - projection_view_matrix[2][2],
										 projection_view_matrix[3][3] - projection_view_matrix[3][2]));
	
}

/******************************************************************************
 *  Implementation for OpenGL system                                          *
 ******************************************************************************/

void SGL::startup() {

    SLog::verboseLog(SVerbosityLevel::Debug, "SGL startup");
    
    // Initialize GLFW
    if (!glfwInit())
        exit(0);
    
    // Create a window
    createWindow();

    // Load the rect we use
    loadRect();
    
    // Set some basic parameters for OpenGL
	glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glCullFace(GL_BACK);
    
    glEnable(GL_DEPTH_TEST);
    
}

void SGL::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SGL shutdown");
    
    // Unload the rect we use for rendering
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
        SLog::verboseLog(SVerbosityLevel::Critical, "Failed to create an OpenGL context of at least version 3.2. Your GPU may not support this version of OpenGL, try updating your drivers");
        shutdown();
  
    }

    SLog::verboseLog(SVerbosityLevel::Debug, "Created an OpenGL context version: %i.%i",
                     glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR),
                     glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR));
    
    // Turn of VSync by default
    glfwMakeContextCurrent(window);
	//glfwSwapInterval(0);
	
	// Set the window callback to know when we have focus
	glfwSetWindowFocusCallback(window, windowFocusCallback);
	
}

bool SGL::windowIsGood() { return !glfwWindowShouldClose(window); }

void SGL::swapBuffers() { glfwSwapBuffers(window); }

void SGL::setKeyCallback(GLFWkeyfun func) { glfwSetKeyCallback(window, func); }
void SGL::setCharCallback(GLFWcharfun func) { glfwSetCharCallback(window, func); }
void SGL::setMouseCallback(GLFWmousebuttonfun func) { glfwSetMouseButtonCallback(window, func); }

void SGL::windowFocusCallback(GLFWwindow* window, int state) { window_focused = (bool)state; }
bool SGL::getWindowFocused() { return window_focused; }

int SGL::getKeyState(int key) { return glfwGetKey(window, key); }

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

float SGL::getScreenScale() { return getWindowFramebufferSize().x / WINDOW_WIDTH; }

/******************************************************************************
 *  Implementation for operations pertaining to graphics math                 *
 ******************************************************************************/

glm::mat4 SGL::transformToMatrix(const STransform& transform) {
    
    glm::mat4 to_return = glm::mat4(1.0);
    
    //Perform the operations on the matrix
    to_return = glm::translate(to_return, transform.translation);
    to_return = glm::rotate(to_return, transform.rotation.z, z_axis);
    to_return = glm::rotate(to_return, transform.rotation.y, y_axis);
    to_return = glm::rotate(to_return, transform.rotation.x, x_axis);
    to_return = glm::scale(to_return, transform.scale);
    
    return to_return;
    
}

glm::mat4 SGL::transformToMatrix(const STransform& transform, double interpolation) {
    
    glm::mat4 to_return = glm::mat4(1.0);
    
    //Perform the operations on the matrix
    to_return = glm::translate(to_return, transform.translation + transform.translation_velocity * (float)interpolation);
    to_return = glm::rotate(to_return, transform.rotation.z + transform.rotation_velocity.z * (float)interpolation, z_axis);
    to_return = glm::rotate(to_return, transform.rotation.y + transform.rotation_velocity.y * (float)interpolation, y_axis);
    to_return = glm::rotate(to_return, transform.rotation.x + transform.rotation_velocity.x * (float)interpolation, x_axis);
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

void SGL::renderRect(const glm::vec2& position, const glm::vec2& size) {
    
    // Make the matrix for the rect
    glm::mat4 matrix = glm::mat4(1.0);
    matrix = glm::translate(matrix, glm::vec3(position, 1.0));
    matrix = glm::scale(matrix, glm::vec3(size.x, size.y, 1.0));
    
    // Load the modelview and flush the matricies
    loadMatrix(matrix, MAT_MODEL);
    clearMatrix(MAT_VIEW);
    
    flushMatrix(MAT_PROJECTION);
    flushMatrix(MAT_VIEW);
    flushMatrix(MAT_MODEL);
    
    // render the array
    glBindVertexArray(rect_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
	//glBindVertexArray(0);
    
}

/******************************************************************************
 *  Implementation for the matricies                                          *
 ******************************************************************************/

void SGL::loadMatrix(const glm::mat4& mat, int matrix) {
    
    // Load the matrix into the storage
    matrices[matrix] = mat;

}

void SGL::mulMatrix(const glm::mat4& mat, int matrix) {

	// Multiply it and upload it
	const glm::mat4& mat_current = matrices[matrix];
	matrices[matrix] = mat_current * mat;

}

glm::mat4 SGL::getMatrix(int matrix) {
 
    // See if there is a matrix that exists by that name, otherwise return identity
	return matrices[matrix];
}

void SGL::clearMatrix(int matrix) {

	// Load identity matrix
    loadMatrix(glm::mat4(1.0), matrix);
    
}

void SGL::flushMatrix(int matrix) {
    
    // Get the currently bound shader
    SShader* shader = SShader::getBoundShader();
    if (shader) {
        
        // Now tell it to upload a uniform of type matrix
        shader->bindUniform(&matrices[matrix], matrix_names[matrix], UNIFORM_MAT4, 1);
        
    }
    
}

/******************************************************************************
 *  Arrays for simple quad rendering                                          *
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
    
    // Create a simple 1x1 square that we can use to render rects to the screen
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
    
	//glBindVertexArray(0);

}

void SGL::unloadRect() {

    // Delete the buffers and array
    glDeleteBuffers(2, rect_buffers);
    glDeleteVertexArrays(1, &rect_id);

}
