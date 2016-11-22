//
//  SUITextField.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/16/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SUITextField_hpp
#define SUITextField_hpp

#include "SUIWidget.hpp"
#include "STime.hpp"

/******************************************************************************
 *  Definition for tex field widget                                           *
 ******************************************************************************/

class SUITextField : public SUIWidget {
    
    public:
    
        SUITextField();
    
        void render(double interpolation);
    
        void startEditing();
    
        void pressSpecialKey(int key);
    
        SFont* font;
        float font_size = 15.0;
    
        glm::vec4 background_color;
    
        std::string& getText();
        void setText(std::string text_new);
    
        // Functions for different keys that are pressed
        std::function<void()> return_function;
        std::function<void()> escape_function;
    
    protected:
    
        virtual void charCallback(unsigned int key);
    
        SKeyboardListener keyboard_listener;
    
        int cursor_head = 0;
        std::string text;
    
        bool cursor_shown = true;
        STimer cursor_blink_timer;
        void blinkCursor();
        void resetCursorBlink();
    
    
};

#endif /* SUITextField_hpp */
