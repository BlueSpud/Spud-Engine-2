//
//  SUI.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/17/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SUI.hpp"

SShader* SUI::simple_shader_color;
SShader* SUI::simple_shader_texture;

/******************************************************************************
 *  Functions main ui class                                                   *
 ******************************************************************************/

void SUI::startup() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SUI startup");
    
    // Aquire the shaders that the UI might need to use
    simple_shader_color = (SShader*)SResourceManager::getResource(SPath("Shader/simple/simple_color.glsl"));
    simple_shader_texture = (SShader*)SResourceManager::getResource(SPath("Shader/simple/simple_texture.glsl"));
    
}

void SUI::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SUI shutdown");
    
}

void SUI::drawRect(SUIRect& rect, glm::vec4 color) {
    
    // Bind the simple color shader and the color
    simple_shader_color->bind();
    SUI::simple_shader_color->bindUniform(&color, "color", UNIFORM_VEC4, 1);
    
    // Draw the rect with opengl
    SGL::drawRect(rect.origin, rect.size);
    
}

void SUI::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {}

void SUI::charCallback(GLFWwindow* window, unsigned int unicode_value) {}

void SUI::mouseCallback(GLFWwindow* window, int button, int action, int mods) {}
