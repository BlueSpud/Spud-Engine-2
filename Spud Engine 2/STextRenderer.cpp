//
//  STextRenderer.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/15/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "STextRenderer.hpp"
#include "SUI.hpp"

SFont* STextRenderer::default_font;
SShader* STextRenderer::text_shader;

/******************************************************************************
 *  Functions for the text rendering                                          *
 ******************************************************************************/

void STextRenderer::startup() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "STextRenderer startup");
    
    // Load up the shader that we use for text rendering
    text_shader = (SShader*)SResourceManager::getResource(SPath("Shader/ui/text_simple.glsl"));
    
}

void STextRenderer::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "STextRenderer shutdown");
    
}

void STextRenderer::renderText(std::string text, SFont* font, float font_size, glm::vec2 screen_pos) {
    
    text_shader->bind();
    font->font_atlas->bind();
    
    glm::vec2 cursor_head = screen_pos * SGL::getScreenScale();
    
    float font_size_multiplier = 1.0 / font->font_size * font_size;
    
    for (int i = 0; i < text.length(); i++) {
        
        char& current_character = text.at(i);
        
        // If we were a new line we need to do a carriage return
        if (current_character == '\n') {
        
            cursor_head = glm::vec2(screen_pos.x * SGL::getScreenScale(), cursor_head.y + font->line_height * font_size_multiplier);
            
        } else {
        
            // Upload some uniforms
            text_shader->bindUniform(&font->characters[current_character].size, "size", UNIFORM_VEC2, 1);
            text_shader->bindUniform(&font->characters[current_character].position, "start", UNIFORM_VEC2, 1);
        
            SGL::drawRect(cursor_head + font->characters[current_character].offset * font_size_multiplier, font->characters[current_character].size * (glm::vec2)font->font_atlas->size * font_size_multiplier);
        
            // Add the x-stride to the cursor
            cursor_head.x = cursor_head.x + font->characters[current_character].x_advance * font_size_multiplier;
            
        }
        
    }
    
}

void STextRenderer::renderTextWithCursor(std::string text, int cursor_pos, SFont* font, float font_size, glm::vec2 screen_pos) {
    
    text_shader->bind();
    font->font_atlas->bind();
    
    glm::vec2 cursor_head = screen_pos * SGL::getScreenScale();
    bool has_drawn_cursor = false;
    
    float font_size_multiplier = 1.0 / font->font_size * font_size;
    
    for (int i = 0; i < text.length(); i++) {
        
        char& current_character = text.at(i);
        
        // If we were a new line we need to do a carriage return
        if (current_character == '\n') {
            
            cursor_head = glm::vec2(screen_pos.x * SGL::getScreenScale(), cursor_head.y + font->line_height * font_size_multiplier);
            
        } else {
            
            // Check if we need to draw the cursor
            if (i == cursor_pos)
                render_cursor(font, font_size_multiplier, cursor_head);
            
            text_shader->bind();
            
            // Upload some uniforms
            text_shader->bindUniform(&font->characters[current_character].size, "size", UNIFORM_VEC2, 1);
            text_shader->bindUniform(&font->characters[current_character].position, "start", UNIFORM_VEC2, 1);
            
            SGL::drawRect(cursor_head + font->characters[current_character].offset * font_size_multiplier, font->characters[current_character].size * (glm::vec2)font->font_atlas->size * font_size_multiplier);
            
            // Add the x-stride to the cursor
            cursor_head.x = cursor_head.x + font->characters[current_character].x_advance * font_size_multiplier;
            
        }
        
    }
    
    // If the cursor hasnt been drawn, draw it
    if (!has_drawn_cursor)
        render_cursor(font, font_size_multiplier, cursor_head);
    
}

void STextRenderer::render_cursor(SFont* font, float font_size_multiplier, const glm::vec2& position) {
    
    SUIRect cursor_frame;
    cursor_frame.origin = position / SGL::getScreenScale();
    cursor_frame.size = glm::vec2(CURSOR_WIDTH, font->line_height * font_size_multiplier / SGL::getScreenScale());
    
    SUI::drawRect(cursor_frame, glm::vec4(1.0));
    
}
