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
    glm::vec4 draw_color = standard_color;
    
    // Figure out which color we should be using
    if (hovering)
        draw_color = hover_color;
    
    if (pressed)
        draw_color = press_color;
    
    SUI::drawRect(frame, draw_color);
    
    // Draw the label on top of it
    if (font)
        STextRenderer::renderText(label, font, 15.0, frame.origin);

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
