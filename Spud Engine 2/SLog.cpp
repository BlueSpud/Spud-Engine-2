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
std::string verbosity_strs[] = {"Critical", "Warning", "Debug"};

SVerbosityLevel SLog::verbosity_level = SVerbosityLevel::Critical;
std::vector<std::string> SLog::log_lines;

/******************************************************************************
 *  Implementation for logging system                                         *
 ******************************************************************************/

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
        std::string final_output = "[" + verbosity_strs[verbosity] + "] " + buffer;
        
        log_lines.push_back(final_output);
        if (verbosity_level == SVerbosityLevel::Debug)
            std::cout << final_output << std::endl;
        
    }
    
}

void SLog::log(const char* format, ...) {
    
    // Calculate length as string length + a max value we specify
    size_t length = strlen(format) + MAX_ARG_CHAR_LENGTH;
    
    // Get an array for args
    va_list args;
    va_start(args, format);
    
    // Write to the buffer
    char buffer[length];
    vsnprintf(buffer, length, format, args);
    
    va_end(args);

    // Write the buffer
    log_lines.push_back(buffer);
    if (verbosity_level == SVerbosityLevel::Debug)
        std::cout << buffer << std::endl;
    
}

void SLog::writeLogToFile() {
    
    // Write out everything we have logged by concainating all the strings
    std::string final_log;
    for (int i = 0; i < log_lines.size(); i++)
        final_log = final_log + log_lines[i] + "\n";
    
    SPath log_path = SPath("Spud Engine.log");
    
    SFileSystem::writeStringToFile(log_path, final_log);
    
}

int SLog::getLineCount() { return (int)log_lines.size(); }

std::string& SLog::getLine(int line_number) {

    // Read a line from the log and return it
    return log_lines[line_number];
    
}
