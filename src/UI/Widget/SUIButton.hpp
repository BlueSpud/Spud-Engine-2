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
#include "Sound/SSoundEmitter.hpp"

/******************************************************************************
 *  Definition for simple button widget                                       *
 ******************************************************************************/

class SUIButton : public SUIWidget {
    
    public:
    
        SUIButton();
    
        virtual void render(double interpolation);
    
        std::string label;
        std::shared_ptr<SFont> font = nullptr;
        float font_size = 15.0;
    
        std::shared_ptr<STexture> background_image;
        std::shared_ptr<STexture> hover_image;
        std::shared_ptr<STexture> press_image;

        std::shared_ptr<SSound> hover_sound;
        std::shared_ptr<SSound> press_sound;
    
        boost::function<void(int)> action;
    
    protected:
    
        virtual void onHover();
        virtual void onPress(int button);
        virtual void onRelease(int button);
    
        bool pressed = false;
    
        SSoundEmitter* sound_emitter;
    
};

#endif /* SUIButton_hpp */
