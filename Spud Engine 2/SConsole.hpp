//
//  SConsole.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/15/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SConsole_hpp
#define SConsole_hpp

#include "SUITextField.hpp"

/******************************************************************************
 *  Definition for console                                                    *
 ******************************************************************************/

class SConsole : public SSubsystem {
    
    public:
    
        static void startup();
        static void shutdown();
    
        static void activate(int arg);
        static void deactivateConsole();
    
        static void render();
    
        static void commitCommand();
    
    private:
    
        static SUIRect consoleFrame;
    
        static bool console_active;
    
        static SFont* console_font;
        static SUITextField* text_field;
    
};

/******************************************************************************
 *  Definition for console command registry                                   *
 ******************************************************************************/

class SConsoleCommandRegistry {
    
    friend class SConsole;
    
    public:
    
        bool registerCommand(const std::string& command_name,  void (*function)());
        static SConsoleCommandRegistry* instance();
    
    private:
    
        static std::map<size_t, void(*)()> commands;
        static std::hash<std::string> hasher;
    
    
};

// Define a macro that can be used to register a command
#define REGISTER_COMMAND(C, F) bool isRegistered_##F_##C =  SConsoleCommandRegistry::instance()->registerCommand(#C, F)

#endif /* SConsole_hpp */
