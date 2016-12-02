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
    
    // Set that we have hover, press and release
    has_event_pressed = true;
    has_event_released = true;
    has_event_hover = true;
    
}

void SUIButton::render(double interpolation) {

    // If we are hovering change the color
    //glm::vec4 draw_color = standard_color;
    STexture* draw_image = background_image;
    
    // Figure out which color we should be using
    //if (hovering)
    //    draw_color = hover_color;
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

void SUIButton::onHover() { /* intentionally blank */ }

void SUIButton::onPress(int button) {

    pressed = true;
    
    // Call the designated fuction for press
    if (press_func)
        press_func(button);

}

void SUIButton::onRelease(int button) {

    pressed = false;
    
    // Call the designated function for release
    if (release_func)
        release_func(button);

}
