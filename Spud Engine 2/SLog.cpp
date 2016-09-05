//
//  SLog.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/5/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SLog.hpp"
#include "SFileSystem.hpp"

// Definition for printable names of verbosity
const char* verbosity_strs[] = {"Critical", "Warning", "Debug"};

SVerbosityLevel SLog::verbosity_level = SVerbosityLevel::Critical;
std::stringstream SLog::log_stream;

void SLog::setVerbosityLevel(SVerbosityLevel level) { verbosity_level = level; }

void SLog::verboseLog(SVerbosityLevel verbosity, const char* format, ...) {
    
    // Make sure we are above or at the level we need to display this message
    if (verbosity <= verbosity_level) {
    
        // Calculate length as string length + a max value we specify
        size_t length = strlen(format) + MAX_ARG_CHAR_LENGTH;
    
        // Get an array for args
        va_list args;
        va_start(args, format);

        // Write to the buffer
        char buffer[length];
        vsnprintf(buffer, length, format, args);
    
        va_end(args);
    
        // Output the message to the log stream, if we're in debug, output to cout as well
        std::string final_output = "[" + std::string(verbosity_strs[verbosity_level]) + "] " + buffer;
        
        log_stream << final_output << std::endl;
        if (verbosity_level == SVerbosityLevel::Debug)
            std::cout << final_output << std::endl;
        
    }
    
}

void SLog::writeLogToFile() {
    
    std::string log_str = log_stream.str();
    SPath log_path = SPath("Spud Engine.log");
    
    SFileSystem::writeStringToFile(log_path, log_str);
    
}
