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
#include <map>

#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "SSubsystem.hpp"

#define WINDOW_WIDTH 1440
#define WINDOW_HEIGHT 900

// Forward declarations
class SGLUploadSystem;
class SCursor;

/******************************************************************************
 *  Definition for transform                                                  *
 ******************************************************************************/

struct STransform {
    
    glm::vec3 translation = glm::vec3(0.0);
    glm::vec3 rotation = glm::vec3(0.0);
    glm::vec3 scale = glm::vec3(1.0);
    
    glm::vec3 translation_velocity = glm::vec3(0.0);
    glm::vec3 rotation_velocity = glm::vec3(0.0);
    glm::vec3 scale_velocity = glm::vec3(0.0);
    
    void update() {
        
        // Add the velocities on
        translation += translation_velocity;
        rotation += rotation_velocity;
        scale += scale_velocity;
        
    }
	
	glm::vec3 getForwardVector(double interpolation) {
		
		// Calculate a few things we need more than once
		float modified_yaw = (rotation.y + rotation_velocity.y * interpolation) - M_PI_2;
		float cos_pitch = cos(rotation.x + rotation_velocity.x * interpolation);
		
		// Calculate the vector
		return glm::normalize(glm::vec3(cos(modified_yaw) * cos_pitch,
										sin(rotation.x + rotation_velocity.x * interpolation),
										sin(modified_yaw) * cos_pitch));
		
	}
	
};

/******************************************************************************
 *  Definition for matrix locations                                           *
 ******************************************************************************/

#define MAT_MODEL 0
#define MAT_VIEW 1
#define MAT_PROJECTION 2

/******************************************************************************
 *  Definition for frustum													  *
 ******************************************************************************/

struct SFrustum {
	
	SFrustum(const glm::mat4 projection_view_matrix);
	glm::vec4 planes[6];
	
};

/******************************************************************************
 *  Definition for 2D viewport                                                *
 ******************************************************************************/

struct SViewport {
    
    glm::vec2 screen_size;
    glm::vec2 screen_pos;
    
    // Should never be called
    SViewport() { /* oops */ }
    SViewport(glm::vec2 _screen_size, glm::vec2 _screen_pos) {
        
        screen_size = _screen_size;
        screen_pos = _screen_pos;
        
    }
    
};

/******************************************************************************
 *  Definition for 3D viewport                                                *
 ******************************************************************************/

struct SViewport3D : public SViewport {
    
    float field_of_view;
    
    glm::vec2 planes;
    
    SViewport3D(glm::vec2 _screen_size, glm::vec2 _screen_pos, float _field_of_view, glm::vec2 _planes) : SViewport(_screen_size, _screen_pos) {
        
        field_of_view = _field_of_view;
        planes = _planes;
        
    }
    
};

/******************************************************************************
 *  Definition for OpenGL subsystem                                           *
 ******************************************************************************/

class SGL : public SSubsystem {
    
    friend class SGLUploadSystem;
    friend class SCursor;
    
    public:
    
        static void startup();
        static void shutdown();
    
        static void createWindow();
    
        static bool windowIsGood();
    
        static void swapBuffers();
    
        static void setKeyCallback(GLFWkeyfun func);
        static void setCharCallback(GLFWcharfun func);
        static void setMouseCallback(GLFWmousebuttonfun func);
	
		static void windowFocusCallback(GLFWwindow* window, int state);
		static bool getWindowFocused();
	
        static int getKeyState(int key);
    
        static void setMouseInputMode(int mode);
        static void getMousePos(float* x_pos, float* y_pos);
    
        static glm::ivec2 getWindowFramebufferSize();
        static float getScreenScale();
    
/******************************************************************************
*  Definition for operations pertaining to graphics math                      *
******************************************************************************/
    
        static glm::mat4 transformToMatrix(const STransform& transform);
        static glm::mat4 transformToMatrix(const STransform& transform, double interpolation);
    
        static glm::mat4 getProjectionMatrix2D(const SViewport& viewport);
        static glm::mat4 getProjectionMatrix3D(const SViewport3D& viewport);
        static void setUpViewport(const SViewport& viewport);
    
        static void renderRect(const glm::vec2& position, const glm::vec2& size);
    
/******************************************************************************
*  Definition for the matrix operations                                       *
******************************************************************************/
    
        static void loadMatrix(const glm::mat4& mat, int matrix);
        static void mulMatrix(const glm::mat4& mat, int matrix);
        static glm::mat4 getMatrix(int matrix);
        static void clearMatrix(int matrix);
    
        static void flushMatrix(int matrix);
    
    private:
    
		static glm::mat4 matrices[3];
    
        static GLFWwindow* window;
    
        static void loadRect();
        static void unloadRect();
    
        static GLuint rect_id;
        static GLuint rect_buffers[2];
	
		static bool window_focused;
	
		static const char* matrix_names[3];
	
    
};

static const glm::vec3 x_axis = glm::vec3(1, 0, 0);
static const glm::vec3 y_axis = glm::vec3(0, 1, 0);
static const glm::vec3 z_axis = glm::vec3(0, 0, 1);

#endif /* SGL_hpp */
