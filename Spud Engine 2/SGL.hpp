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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SSubsystem.hpp"

#define WINDOW_WIDTH 1440
#define WINDOW_HEIGHT 800

#define REQUEST_OPENGL_32 0

/******************************************************************************
 *  Definition for transform                                                  *
 ******************************************************************************/

struct STransform {
    
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale = glm::vec3(1.0);
    
};

/******************************************************************************
 *  Definition for 3D viewport                                                *
 ******************************************************************************/

struct SViewport3D {
    
    glm::vec2 screen_size;
    glm::vec2 screen_pos;
    
    float field_of_view;
    
    glm::vec2 planes;
    
    SViewport3D(glm::vec2 _screen_size, glm::vec2 _screen_pos, float _field_of_view, glm::vec2 _planes) {
        
        screen_size = _screen_size;
        screen_pos = _screen_pos;
        field_of_view = _field_of_view;
        planes = _planes;
        
    }
    
};

/******************************************************************************
 *  Definition for OpenGL subsystem                                           *
 ******************************************************************************/

class SGL : public SSubsystem {
    
    public:
    
        static void startup();
        static void shutdown();
    
        static void createWindow();
    
        static bool windowIsGood();
    
        static void swapBuffers();
    
        static void setKeyCallback(GLFWkeyfun func);
    
/******************************************************************************
*  Definition for operations pertaining to graphics math                      *
******************************************************************************/
    
        static glm::mat4 transformToMatrix(const STransform& transform);
    
        static glm::mat4 getProjectionMatrix(const SViewport3D& viewport);
        static void setUp3DViewport(const SViewport3D& viewport);
    
    private:
    
        static GLFWwindow* window;
    
};

static const glm::vec3 x_axis = glm::vec3(1, 0, 0);
static const glm::vec3 y_axis = glm::vec3(0, 1, 0);
static const glm::vec3 z_axis = glm::vec3(0, 0, 1);

#endif /* SGL_hpp */
