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

// Forward declatations
class SUIWidget;

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
    
        /******************************************************************************
         *  Input functions                                                           *
         ******************************************************************************/
    
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void charCallback(GLFWwindow* window, unsigned int unicode_value);
        static void mouseCallback(GLFWwindow* window, int button, int action, int mods);
    
        static void moveMouse();
    
        static SUIWidget* current_widget_input;
    
    
};

#endif /* SUI_hpp */
