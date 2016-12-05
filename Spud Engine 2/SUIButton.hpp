//
//  SUIButton.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/28/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SUIButton_hpp
#define SUIButton_hpp

#include "SUIWidget.hpp"
#include "SSoundEmitter.hpp"

/******************************************************************************
 *  Definition for simple button widget                                       *
 ******************************************************************************/

class SUIButton : public SUIWidget {
    
    public:
    
        SUIButton();
    
        virtual void render(double interpolation);
    
        std::string label;
        SFont* font = nullptr;
        float font_size = 15.0;
    
        STexture* background_image;
        STexture* hover_image;
        STexture* press_image;
    
        SSound* hover_sound;
        SSound* press_sound;
    
        boost::function<void(int)> action;
    
    protected:
    
        virtual void onHover();
        virtual void onPress(int button);
        virtual void onRelease(int button);
    
        bool pressed = false;
    
        SSoundEmitter* sound_emitter;
    
};

#endif /* SUIButton_hpp */
