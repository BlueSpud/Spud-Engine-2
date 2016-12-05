//
//  SUIButton.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/28/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SUIButton.hpp"

/******************************************************************************
 *  Functions for simple button widget                                        *
 ******************************************************************************/

SUIButton::SUIButton() {
    
    // Create the sound emitter and make sure it is 2D
    sound_emitter = new SSoundEmitter();
    sound_emitter->sound_position_mode = SSoundPositionMode2D;
    
}

void SUIButton::render(double interpolation) {

    // Figure out which image we should be using
    STexture* draw_image = background_image;
    
    if (hovering && hover_image)
        draw_image = hover_image;
    
    if (pressed && press_image)
        draw_image = press_image;
    
    SUI::drawRect(frame, draw_image);
    
    // Draw the label on top of it
    if (font) {
        
        // Get the padding
        float y_padding = (frame.size.y - font->getLineHeight(font_size)) / 2.0;
        STextRenderer::renderTextCentered(label, font, font_size, glm::vec2(frame.origin.x, frame.origin.y + y_padding), frame.size.x);
        
    }

}

void SUIButton::onHover() {

    // If there was a hover sound, we play it
    if (hover_sound) {
        
        sound_emitter->setSound(hover_sound);
        sound_emitter->reset();
        sound_emitter->play();
        
    }
    
}

void SUIButton::onPress(int button) { pressed = true; }

void SUIButton::onRelease(int button) {

    pressed = false;

    // If there is an aciton, call it, assiming we are hovering
    if (action && hovering) {
        
        action(button);

        // If there was a hover sound, we play it
        if (press_sound) {
            
            sound_emitter->setSound(press_sound);
            sound_emitter->reset();
            sound_emitter->play();
            
        }
        
    }
}
