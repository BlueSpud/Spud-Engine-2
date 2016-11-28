//
//  SConsole.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/15/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SConsole.hpp"

/******************************************************************************
 *  Console static members                                                    *
 ******************************************************************************/

SUIRect SConsole::consoleFrame;

bool SConsole::console_active = false;

SFont* SConsole::console_font;
SUITextField* SConsole::text_field;

std::hash<std::string> SConsoleCommandRegistry::hasher;

/******************************************************************************
 *  Console command registry static members                                   *
 ******************************************************************************/

std::map<size_t, void(*)(const std::vector<std::string>&)> SConsoleCommandRegistry::commands;

/******************************************************************************
 *  Functions for console                                                     *
 ******************************************************************************/

void SConsole::startup() {
    
    // Load up the shader that we use for text rendering
    SLog::verboseLog(SVerbosityLevel::Debug, "SConsole startup");
    
    console_font = (SFont*)SResourceManager::getResource(SPath("Font/Arial.font"));
    
    text_field = new SUITextField();
    text_field->font = console_font;
    text_field->font_size = CONSOLE_FONT_SIZE;
    text_field->background_color = glm::vec4(0.1, 0.1, 0.1, 1.0);
    
    // Set up the text field callbacks
    text_field->escape_function = &deactivateConsole;
    text_field->return_function = &commitCommand;
    
    // Make the frame for the console
    glm::vec2 console_size = glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT * 0.75);
    consoleFrame.size = console_size;
    
    // Make the text field frame
    text_field->frame.origin = glm::vec2(0.0, console_size.y);
    text_field->frame.size = glm::vec2(console_size.x, console_font->getLineHeight(CONSOLE_FONT_SIZE) + CONSOLE_INDENT);
    
}

void SConsole::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SConsole shutdown");
    
    delete SConsoleCommandRegistry::instance();
    
}

void SConsole::activate(int arg) {
    
    // Give control of the keyboard to the text field
    text_field->startEditing();
    SInputSystem::setInputMode(SInputModeUI);
    
    // Show that the console is active
    console_active = true;
    
}

void SConsole::deactivateConsole() {
    
    // Give keyboard control back to the game
    SInputSystem::setInputMode(SInputModeGame);
    
    // Console is no longer active
    console_active = false;
    
}

void SConsole::render() {
    
    // If the console is not active, then we show nothing
    if (console_active) {
    
        // First render an entirely black quad
        SUI::drawRect(consoleFrame, glm::vec4(0.0, 0.0, 0.0, 1.0));
        
        // Get the number of lines in the log and the size of a line
        int line_count = SLog::getLineCount();
        float line_size = console_font->getLineHeight(CONSOLE_FONT_SIZE);
        
        // Figure out how many lines will fit
        int lines_displayed = floor((consoleFrame.size.y - CONSOLE_INDENT) / line_size);
        
        // Get the overall string of what needs to be written
        std::string displayed_lines;
        
        int start = line_count - lines_displayed + 1;
        if (start < 0)
            start = 0;
        
        for (int i = start; i < line_count; i++)
            displayed_lines = displayed_lines + SLog::getLine(i) + "\n";
        
        // Draw some text over it
        STextRenderer::renderText(displayed_lines, console_font, CONSOLE_FONT_SIZE, glm::vec2(CONSOLE_INDENT));
        
        // Render the text field
        text_field->render(0.0);
        
    }
    
}

void SConsole::commitCommand() {
    
    // Get the command out of the text field
    std::string& command = text_field->getText();
    
    if (command.length()) {
    
        SLog::log("[Console] %s", command.c_str());
    
        // Get the name of the command
        std::string command_name;
        std::stringstream command_stream = std::stringstream(command);
        std::getline(command_stream, command_name, ' ');
        
        // Hash the command so we can get it from the map
        size_t command_hash = SConsoleCommandRegistry::hasher(command_name);
        
        if (SConsoleCommandRegistry::instance()->commands.count(command_hash)) {
            
            // Parse the arguments
            std::vector<std::string> arguments;
            std::string arg_srt;
            
            while (std::getline(command_stream, arg_srt, ' '))
                arguments.push_back(arg_srt);
            
            // Call the function
            SConsoleCommandRegistry::instance()->commands[command_hash](arguments);
            
        } else SLog::log("[Console] Command not found");
    
        // Reset the text inside of the text field
        text_field->setText("");
        
    }
    
}

/******************************************************************************
 *  Functions for console command registry                                    *
 ******************************************************************************/

bool SConsoleCommandRegistry::registerCommand(const std::string& command_name, void (*function)(const std::vector<std::string>&)) {
    
    // Hash the name of the command and keep the function
    size_t command_hash = SConsoleCommandRegistry::hasher(command_name);
    commands[command_hash] = function;
    
    return true;

}

SConsoleCommandRegistry* SConsoleCommandRegistry::instance() {

    // Return a static instace of the manager
    static SConsoleCommandRegistry* _instance = NULL;
    if (_instance == NULL)
        _instance = new SConsoleCommandRegistry();
    
    return _instance;
    
}
