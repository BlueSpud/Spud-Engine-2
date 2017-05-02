//
//  SSubsystem.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/1/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SSubsystem_hpp
#define SSubsystem_hpp

#include <iostream>

#include "Debug/SLog.hpp"
#include "Engine/Utility/SHash.hpp"

/******************************************************************************
 *  Definition for generic subsystem                                          *
 ******************************************************************************/

class SSubsystem {
    
    public:
    
        static void startup() { /* intentionally empty */}
        static void shutdown() { /* intentionally empty */}
    
};

/******************************************************************************
 *  Definition for subsystem manager                                          *
 ******************************************************************************/

#endif /* SSubsystem_hpp */
