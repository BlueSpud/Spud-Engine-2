//
//  STextRenderer.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/15/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef STextRenderer_hpp
#define STextRenderer_hpp

#include "SSubsystem.hpp"
#include "SFont.hpp"
#include "SShader.hpp"

/******************************************************************************
 *  Definition for text renderer                                              *
 ******************************************************************************/

class STextRenderer : public SSubsystem {
    
    public:
    
        static void startup();
        static void shutdown();
    
        static void renderText(std::string text, SFont* font);
    
    private:
    
        static SFont* default_font;
        static SShader* text_shader;
    
    
};

#endif /* STextRenderer_hpp */
