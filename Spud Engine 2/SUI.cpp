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

void SUI::mouseCallback(GLFWwindow* window, int button, int action, int mods) {}

void SUI::moveMouse() {

    // First we need to get the mouse position on the screen
    glm::vec2 mouse_position;
    SGL::getMousePos(&mouse_position.x, &mouse_position.y);
    
    SLog::verboseLog(SVerbosityLevel::Debug, "Mouse position: %f %f", mouse_position.x, mouse_position.y);


}
