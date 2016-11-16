//
//  SConsole.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/15/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SConsole.hpp"

SShader* SConsole::color_shader;
SFont* SConsole::console_font;

bool SConsole::console_active = false;

/******************************************************************************
 *  Functions for console                                                     *
 ******************************************************************************/

void SConsole::startup() {
    
    // Load up the shader that we use for text rendering
    SLog::verboseLog(SVerbosityLevel::Debug, "SConsole startup");
    
    color_shader = (SShader*)SResourceManager::getResource(SPath("Shader/simple/simple_color.glsl"));
    console_font = (SFont*)SResourceManager::getResource(SPath("Font/Arial.font"));
    
}

void SConsole::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SConsole shutdown");
    
}

void SConsole::activate(int arg) {
    
    // Give control of the keyboard to the text field
    
    
    // Show that the console is active
    console_active = true;
    
}

void SConsole::render() {
    
    // If the console is not active, then we show nothing
    if (console_active) {
    
        // First render an entirely black quad
        glm::vec2 console_size = SGL::getWindowFramebufferSize();
        console_size.y = console_size.y * 0.75;
    
        color_shader->bind();
    
        glm::vec4 black_color = glm::vec4(0.0, 0.0, 0.0, 1.0);
        color_shader->bindUniform(&black_color, "color", UNIFORM_VEC4, 1);
    
        SGL::drawRect(glm::vec2(0.0), console_size);
    
        // Draw some text over it
        STextRenderer::renderText(SLog::getLogAsString(), console_font, 15.0, glm::vec2(10.0, 10.0));
        
    }
    
}
