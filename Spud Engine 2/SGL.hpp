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
#include <GLFW/glfw3.h>

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
    
};

/******************************************************************************
 *  Definition for unform locations                                           *
 ******************************************************************************/

#define MAT_MODEL_MATRIX "mat_model"
#define MAT_VIEW_MATRIX "mat_view"
#define MAT_PROJECTION_MATRIX "mat_projection"

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
    
        static void renderRect(glm::vec2 position, glm::vec2 size);
    
/******************************************************************************
*  Definition for the matrix operations                                       *
******************************************************************************/
    
        static void loadMatrix(const glm::mat4& mat, const char* mat_name);
        static void mulMatrix(const glm::mat4& mat, const char* mat_name);
        static glm::mat4 getMatrix(const char* mat_name);
        static void clearMatrix(const char* mat_name);
    
        static void flushMatrix(const char* mat_name);
    
    private:
    
        static std::map<const char*, glm::mat4>matrices;
    
        static GLFWwindow* window;
    
        static void loadRect();
        static void unloadRect();
    
        static GLuint rect_id;
        static GLuint rect_buffers[2];
    
    
};

static const glm::vec3 x_axis = glm::vec3(1, 0, 0);
static const glm::vec3 y_axis = glm::vec3(0, 1, 0);
static const glm::vec3 z_axis = glm::vec3(0, 0, 1);

#endif /* SGL_hpp */
