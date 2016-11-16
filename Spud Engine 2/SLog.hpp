//
//  SLog.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/5/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SLog_hpp
#define SLog_hpp

#include <iostream>
#include <sstream>
#include <cstdarg>

#define MAX_ARG_CHAR_LENGTH 1024;

/******************************************************************************
 *  Definition for logging system                                             *
 ******************************************************************************/

enum SVerbosityLevel {
    
    Critical = 0,
    Warning = 1,
    Debug = 2
    
};

class SLog {
    
    public:
    
        static void setVerbosityLevel(SVerbosityLevel level);
        static void verboseLog(SVerbosityLevel verbosity, const char* format, ...);
    
        static void writeLogToFile();
    
        static std::string getLogAsString();
    
    private:
    
        static std::stringstream log_stream;
        static SVerbosityLevel verbosity_level;
    
    
};

#endif /* SLog_hpp */
