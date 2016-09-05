//
//  object.hpp
//  GLFW Test
//
//  Created by Logan Pazol on 8/16/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef object_hpp
#define object_hpp

#include <iostream>
#include <math.h>
#include <GLFW/glfw3.h>

#include "driver.hpp"
#include "SEventSystem.hpp"

class object {
    
        double position_x = 0;
        double position_y = 0;
    
        double scale = 1.0;
    
        SEventListener event_listener;
    
    public:
    
        object();
    
        void update(const SEvent& event);
        void render(double interpolation);
    
        double velocity_x = 0.0;
        double velocity_y = 0.0;
    
        SinDriver<double>scale_driver;
    
};

#endif /* object_hpp */
