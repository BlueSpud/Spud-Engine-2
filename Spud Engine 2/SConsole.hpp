//
//  SConsole.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/15/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SConsole_hpp
#define SConsole_hpp

#include "STextRenderer.hpp"

/******************************************************************************
 *  Definition for console                                                    *
 ******************************************************************************/

class SConsole : public SSubsystem {
    
    public:
    
        static void startup();
        static void shutdown();
    
        static void activate(int arg);
    
        static void render();
    
    private:
    
        static SShader* color_shader;
        static SFont* console_font;
    
        static bool console_active;
    
};

#endif /* SConsole_hpp */
