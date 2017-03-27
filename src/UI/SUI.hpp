//
//  SUI.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/17/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SUI_hpp
#define SUI_hpp

#include <list>

#include "STexture.hpp"
#include "SShader.hpp"
#include "SFramebuffer.hpp"

#include "STextRenderer.hpp"

// Forward declatations
class SUIWidget;
class SUI;

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
 *  Definition for ui graph                                                   *
 ******************************************************************************/

class SUIGraph {
    
    friend class SUI;
    
    public:
    
        ~SUIGraph();
    
        void addWidget(SUIWidget* widget);
        void removeWidget(SUIWidget* widget);
    
        void render(double interpolation);
    
    private:
    
        std::list<SUIWidget*> widgets;
    
    
};

/******************************************************************************
 *  Definition for main ui class                                              *
 ******************************************************************************/

class SUI : public SSubsystem {
    
    public:
    
        static void startup();
        static void shutdown();
    
        /******************************************************************************
         *  rendering functions                                                         *
         ******************************************************************************/
    
        static void renderUI(double interpolation);
    
        static void renderRect(SUIRect& rect, glm::vec4 color);
        static void renderRect(SUIRect& rect, std::shared_ptr<STexture> texture);
    
        static std::shared_ptr<SShader> simple_shader_color;
        static std::shared_ptr<SShader> simple_shader_texture;
    
        /******************************************************************************
         *  Input functions                                                           *
         ******************************************************************************/
    
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void charCallback(GLFWwindow* window, unsigned int unicode_value);
        static void mouseCallback(GLFWwindow* window, int button, int action, int mods);
    
        static void moveMouse();
    
        static void onLoseInput();
    
        static SUIWidget* current_widget_input;
        static SUIGraph* current_ui_graph;
    
    private:
    
        static SUIWidget* hovering_widget;
    
    
};

#endif /* SUI_hpp */
