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

/******************************************************************************
 *  Definition for simple button widget                                       *
 ******************************************************************************/

class SUIButton : public SUIWidget {
    
    public:
    
        virtual void render(double interpolation);
    
        std::string label;
        SFont* font = nullptr;
        float font_size = 15.0;
    
        STexture* background_image;
        STexture* hover_image;
        STexture* press_image;
    
        boost::function<void(int)> press_func;
        boost::function<void(int)> release_func;
    
    protected:
    
        virtual void onHover();
        virtual void onPress(int button);
        virtual void onRelease(int button);
    
        bool pressed = false;
    
};

#endif /* SUIButton_hpp */
