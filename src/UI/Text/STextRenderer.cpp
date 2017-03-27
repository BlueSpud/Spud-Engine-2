//
//  STextRenderer.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/15/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "STextRenderer.hpp"
#include "SUI.hpp"

std::shared_ptr<SFont> STextRenderer::default_font;
std::shared_ptr<SShader> STextRenderer::text_shader;

/******************************************************************************
 *  Implementation for the text rendering                                     *
 ******************************************************************************/

void STextRenderer::startup() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "STextRenderer startup");
    
    // Load up the shader that we use for text rendering
    text_shader = SResourceManager::getResource<SShader>(SPath("Shader/ui/text_simple.glsl"));
    
}

void STextRenderer::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "STextRenderer shutdown");
    
}

void STextRenderer::renderText(std::string text, std::shared_ptr<SFont> font, float font_size, glm::vec2 screen_pos) {
    
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
        
			SGL::renderRect(cursor_head + font->characters[current_character].offset * font_size_multiplier, glm::abs(font->characters[current_character].size) * (glm::vec2)font->font_atlas->size * font_size_multiplier);
        
            // Add the x-stride to the cursor
            cursor_head.x = cursor_head.x + font->characters[current_character].x_advance * font_size_multiplier;
            
        }
        
    }
    
}

void STextRenderer::renderTextCentered(std::string text, std::shared_ptr<SFont> font, float font_size, glm::vec2 screen_pos, float width) {
    
    text_shader->bind();
    font->font_atlas->bind();
    
    glm::vec2 cursor_head = screen_pos * SGL::getScreenScale();
    float font_size_multiplier = 1.0 / font->font_size * font_size;
    
    // Create a place to store all the characters on the line so we can center it once we know its length
    std::vector<SCharacterDrawCall> character_draw_calls;
    float line_length = 0.0;
    
    for (int i = 0; i < text.length(); i++) {
        
        char& current_character = text.at(i);
        
        // If we were a new line we need to do a carriage return
        if (current_character == '\n') {
            
            cursor_head = glm::vec2(screen_pos.x * SGL::getScreenScale(), cursor_head.y + font->line_height * font_size_multiplier);
            
            // Render the centered text
            renderCenteredTextLine(character_draw_calls, font, font_size_multiplier, line_length, width);

            
        } else {
            
            // Create a new character render call
            character_draw_calls.push_back(SCharacterDrawCall());
            SCharacterDrawCall& character_draw_call = character_draw_calls.back();
            character_draw_call.character = current_character;
            character_draw_call.position = cursor_head + font->characters[current_character].offset * font_size_multiplier;
            
            line_length = line_length + font->characters[current_character].x_advance * font_size_multiplier;
            
            // Add the x-stride to the cursor
            cursor_head.x = cursor_head.x + font->characters[current_character].x_advance * font_size_multiplier;
            
        }
        
    }
    
    // If there was a line that didnt have a carriage return at the end, render it too
    if (character_draw_calls.size())
        renderCenteredTextLine(character_draw_calls, font, font_size_multiplier, line_length, width);

}

void STextRenderer::renderTextWithCursor(std::string text, int cursor_pos, std::shared_ptr<SFont> font, float font_size, glm::vec2 screen_pos) {
    
    text_shader->bind();
    font->font_atlas->bind();
    
    glm::vec2 cursor_head = screen_pos * SGL::getScreenScale();
    bool has_rendern_cursor = false;
    
    float font_size_multiplier = 1.0 / font->font_size * font_size;
    
    for (int i = 0; i < text.length(); i++) {
        
        char& current_character = text.at(i);
        
        // If we were a new line we need to do a carriage return
        if (current_character == '\n') {
            
            cursor_head = glm::vec2(screen_pos.x * SGL::getScreenScale(), cursor_head.y + font->line_height * font_size_multiplier);
            
        } else {
            
            // Check if we need to render the cursor
            if (i == cursor_pos) {
                
                renderCursor(font, font_size_multiplier, cursor_head);
                has_rendern_cursor = true;
                
                // Rebind the text shader
                text_shader->bind();
                
            }
            
            // Upload some uniforms
            text_shader->bindUniform(&font->characters[current_character].size, "size", UNIFORM_VEC2, 1);
            text_shader->bindUniform(&font->characters[current_character].position, "start", UNIFORM_VEC2, 1);
            
            SGL::renderRect(cursor_head + font->characters[current_character].offset * font_size_multiplier, glm::abs(font->characters[current_character].size) * (glm::vec2)font->font_atlas->size * font_size_multiplier);
            
            // Add the x-stride to the cursor
            cursor_head.x = cursor_head.x + font->characters[current_character].x_advance * font_size_multiplier;
            
        }
        
    }
    
    // If the cursor hasnt been rendered, render it
    if (!has_rendern_cursor)
        renderCursor(font, font_size_multiplier, cursor_head);
    
}

void STextRenderer::renderCursor(std::shared_ptr<SFont> font, float font_size_multiplier, const glm::vec2& position) {
    
    SUIRect cursor_frame;
    cursor_frame.origin = position / SGL::getScreenScale();
    cursor_frame.size = glm::vec2(CURSOR_WIDTH, font->line_height * font_size_multiplier / SGL::getScreenScale());
    
    SUI::renderRect(cursor_frame, glm::vec4(1.0));
    
}

void STextRenderer::renderCenteredTextLine(std::vector<SCharacterDrawCall>& character_draw_calls, std::shared_ptr<SFont> font,
                                           float font_size_multiplier, float& line_length, float width) {
    
    // Get the offset on the x to center the line
    float offset_x = (width * SGL::getScreenScale() - line_length) / 2.0;
    
    // render the centered line
    for (int j = 0; j < character_draw_calls.size(); j++) {
        
        // Upload some uniforms
        text_shader->bindUniform(&font->characters[character_draw_calls[j].character].size, "size", UNIFORM_VEC2, 1);
        text_shader->bindUniform(&font->characters[character_draw_calls[j].character].position, "start", UNIFORM_VEC2, 1);
        
        SGL::renderRect(glm::vec2(character_draw_calls[j].position.x + offset_x, character_draw_calls[j].position.y),
                        glm::abs(font->characters[character_draw_calls[j].character].size) * (glm::vec2)font->font_atlas->size * font_size_multiplier);
        
    }
    
    character_draw_calls.clear();
    line_length = 0.0;
    
}
