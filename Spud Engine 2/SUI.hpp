//
//  SUI.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/17/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SUI_hpp
#define SUI_hpp

#include "STexture.hpp"
#include "SShader.hpp"
#include "SFramebuffer.hpp"

#include "STextRenderer.hpp"
#include "SMouseSystem.hpp"
#include "SKeyboardSystem.hpp"

/******************************************************************************
 *  Definition for ui rect                                                    *
 ******************************************************************************/

struct SUIRect {
    
    SUIRect() {}
    SUIRect(glm::vec2 _origin, glm::vec2 _size) {
        
        size = _size;
        origin = _origin;
        
    }
    
    glm::vec2 origin;
    glm::vec2 size;
    
};

/******************************************************************************
 *  Definition main ui class                                                  *
 ******************************************************************************/

class SUI : public SSubsystem {
    
    public:
    
        static void startup();
        static void shutdown();
    
        /******************************************************************************
         *  Drawing functions                                                         *
         ******************************************************************************/
    
        static void drawRect(SUIRect& rect, glm::vec4 color);
    
        static SShader* simple_shader_color;
        static SShader* simple_shader_texture;
    
    
};

#endif /* SUI_hpp */
