//
//  SUI.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/17/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SUI.hpp"
#include "SUIWidget.hpp"
#include "SConsole.hpp"

SShader* SUI::simple_shader_color;
SShader* SUI::simple_shader_texture;

SUIWidget* SUI::current_widget_input;
SUIGraph* SUI::current_ui_graph;

SUIWidget* SUI::hovering_widget;

/******************************************************************************
 *  Fucntions for ui graph                                                    *
 ******************************************************************************/

SUIGraph::~SUIGraph() {
    
    // The UI graph is responsible for the widget's memory once they are added, so we delete them
    for (std::list<SUIWidget*>::iterator i = widgets.begin(); i != widgets.end(); i++)
        delete *i;
    
}

void SUIGraph::addWidget(SUIWidget* widget) {

    // Add a widget to the graph
    widgets.push_back(widget);

}

void SUIGraph::removeWidget(SUIWidget* widget) {

    // Remove a widget
    widgets.remove(widget);

}

void SUIGraph::render(double interpolation) {
    
    // Go through all of the widgets and render them to the screen
    for (std::list<SUIWidget*>::iterator i = widgets.begin(); i != widgets.end(); i++)
        (*i)->render(interpolation);
    
}

/******************************************************************************
 *  Functions main ui class                                                   *
 ******************************************************************************/

void SUI::startup() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SUI startup");
    
    // Aquire the shaders that the UI might need to use
    simple_shader_color = (SShader*)SResourceManager::getResource(SPath("Shader/simple/simple_color.glsl"));
    simple_shader_texture = (SShader*)SResourceManager::getResource(SPath("Shader/simple/simple_texture.glsl"));
    
}

void SUI::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SUI shutdown");
    
    // If we have ui graph, delete it
    if (current_ui_graph)
        delete current_ui_graph;
    
}

void SUI::renderUI(double interpolation) {
    
    // Perhaps set up proper projection?
    
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Render out the current ui graph, if we have one
    if (current_ui_graph)
        current_ui_graph->render(interpolation);
    
    // Render the console to the screen, above all UI
    SConsole::render();
    
    glDisable(GL_BLEND);
    
    
}

void SUI::drawRect(SUIRect& rect, glm::vec4 color) {
    
    // Bind the simple color shader and the color
    simple_shader_color->bind();
    SUI::simple_shader_color->bindUniform(&color, "color", UNIFORM_VEC4, 1);
    
    // Draw the rect with opengl
    SGL::drawRect(rect.origin * SGL::getScreenScale(), rect.size * SGL::getScreenScale());
    
}

void SUI::drawRect(SUIRect& rect, STexture* texture) {
    
     // Bind the simple color shader and the texture
    simple_shader_texture->bind();
    texture->bind(0);
    
    // Draw the rect with opengl
    SGL::drawRect(rect.origin * SGL::getScreenScale(), rect.size * SGL::getScreenScale());
    
}

void SUI::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    // If we have a widget that can take keybaord input, we send it off
    if (current_widget_input)
        current_widget_input->input_listener.keyCallback(window, key, scancode, action, mods);

}

void SUI::charCallback(GLFWwindow* window, unsigned int unicode_value) {

    // If we have a widget that can take keybaord input, we send it off
    if (current_widget_input && current_widget_input->input_listener.char_func)
        current_widget_input->input_listener.char_func(unicode_value);

}

void SUI::mouseCallback(GLFWwindow* window, int button, int action, int mods) {

    // The hovering widget is the widget we would click, so if we have it, ask it to click
    if (hovering_widget) {
        
        // Logic for press
        if (action == INPUT_ACTION_DOWN && hovering_widget->has_event_pressed)
            hovering_widget->onPress(button);
        
        // Logic for release
        if (action == INPUT_ACTION_UP && hovering_widget->has_event_released)
            hovering_widget->onRelease(button);
        
    }

}

void SUI::moveMouse() {

    // First we need to get the mouse position on the screen
    glm::vec2 mouse_position;
    SGL::getMousePos(&mouse_position.x, &mouse_position.y);
    
    
    // Go through the widgets in reverse order and see if we're hovering on any
    SUIWidget* new_hover_widget = nullptr;
    
    if (current_ui_graph)
        for (std::list<SUIWidget*>::reverse_iterator i = current_ui_graph->widgets.rbegin(); i != current_ui_graph->widgets.rend(); i++) {
            
            if ((*i)->frame.origin.x > mouse_position.x || (*i)->frame.origin.y > mouse_position.y ||
                (*i)->frame.origin.x + (*i)->frame.size.x < mouse_position.x || (*i)->frame.origin.y + (*i)->frame.size.y < mouse_position.y)
                    continue;
            
            new_hover_widget = *i;
            break;
    
    }
    
    // We set the old hovering widget to false so when we leave it it will no longer be hovering
    if (hovering_widget) {
        
        hovering_widget->hovering = false;
        
        // If the new hovering widget is not equal to the old one we need to call release on the mouse buttons for it so it doesnt get stuck
        if (hovering_widget != new_hover_widget && hovering_widget->has_event_released)
            for (int i = GLFW_MOUSE_BUTTON_1; i < GLFW_MOUSE_BUTTON_8; i++)
                hovering_widget->onRelease(i);

    }
    
    // If we hovered over one, we need to do some stuff to it
    if (new_hover_widget) {
     
        new_hover_widget->hovering = true;
        hovering_widget = new_hover_widget;
        
        // If there is an event for hovering over the widget, call it
        if (hovering_widget->has_event_hover)
            hovering_widget->onHover();
    
    } else hovering_widget = nullptr;
    
}
