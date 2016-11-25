//
//  SUITextField.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/16/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SUITextField.hpp"

/******************************************************************************
 *  Fucntions for tex field widget                                            *
 ******************************************************************************/

SUITextField::SUITextField() : cursor_blink_timer(boost::bind(&SUITextField::blinkCursor, this), 0.5, TIMER_LOOP_INFINITE) {
    
    // Start listening for various things in the keyboard listener
    input_listener.char_func = boost::bind(&SUITextField::charCallback, this, _1);
    
    // Listen for some special characters
    input_listener.bind(boost::bind(&SUITextField::pressSpecialKey, this, _1), GLFW_KEY_BACKSPACE, INPUT_ACTION_DOWN);
    
    input_listener.bind(boost::bind(&SUITextField::pressSpecialKey, this, _1), GLFW_KEY_RIGHT, INPUT_ACTION_DOWN);
    input_listener.bind(boost::bind(&SUITextField::pressSpecialKey, this, _1), GLFW_KEY_LEFT, INPUT_ACTION_DOWN);
    
    input_listener.bind(boost::bind(&SUITextField::pressSpecialKey, this, _1), GLFW_KEY_ENTER, INPUT_ACTION_DOWN);
    input_listener.bind(boost::bind(&SUITextField::pressSpecialKey, this, _1), GLFW_KEY_ESCAPE, INPUT_ACTION_DOWN);
    
    // For text input, the keyboard listener repeats
    input_listener.repeats = true;
    
}

void SUITextField::render(double interpolation) {
    
    // Render a background
    SUI::drawRect(frame, background_color);
    
    // Calculate the padding in the y direction
    float y_padding = (frame.size.y - font->getLineHeight(font_size)) / 2.0;
    
    // Render the text field. give a slight x indent
    // If the cursor is shown we use the function with the cursor showing
    if (cursor_shown)
        STextRenderer::renderTextWithCursor(text, cursor_head, font, font_size, glm::vec2(frame.origin.x + font_size, frame.origin.y + y_padding));
    else STextRenderer::renderText(text, font, font_size, glm::vec2(frame.origin.x + font_size, frame.origin.y + y_padding));
    
}

void SUITextField::startEditing() {
    
    // Enable the keybaord listener
    SUI::current_widget_input = this;
    
}

std::string& SUITextField::getText() { return text; }
void SUITextField::setText(std::string text_new) {
    
    // If the cursor is outside the range of the text, move it to the end
    if (text_new.length() < cursor_head)
        cursor_head = (unsigned int)text_new.length();
    
    // Set the text to the new text
    text = text_new;


}

void SUITextField::charCallback(unsigned int key) {
    
    // Get the new character and place it where it needs to go
    char new_char = key;
    text.insert(cursor_head, 1, new_char);
    cursor_head++;
    
    resetCursorBlink();
    
}

void SUITextField::pressSpecialKey(int key) {
    
    switch (key) {
            
            case GLFW_KEY_BACKSPACE:
            
                if (text.length()) {
                
                    // The cursor is where we would place the next character, so we delete the one behind it
                    text.erase(cursor_head - 1);
                    cursor_head--;
                
                }
            
                resetCursorBlink();
            
            break;
            
            case GLFW_KEY_RIGHT:
            
                // Move the cursor head over to the right
                cursor_head++;
                if (cursor_head > text.length())
                    cursor_head = (int)text.length();
            
                resetCursorBlink();
            
            break;
            
            case GLFW_KEY_LEFT:
            
                // Move the cursor head over to the left
                cursor_head--;
                if (cursor_head < 0)
                    cursor_head = 0;
            
                resetCursorBlink();
            
            break;
            
            case GLFW_KEY_ENTER:
            
                // Call the return function if we have one
                if (return_function)
                    return_function();
            
            break;
            
            
            case GLFW_KEY_ESCAPE:
            
                // Call the escape function if we have one
                if (escape_function)
                    escape_function();
            
            break;
            
    }
    
}

void SUITextField::blinkCursor() { cursor_shown = !cursor_shown; }

void SUITextField::resetCursorBlink() {
    
    // Reset the timer and make the cursor visible
    cursor_blink_timer.reset();
    cursor_shown = true;
    
    
}
