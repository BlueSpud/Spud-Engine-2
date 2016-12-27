//
//  SObject.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/18/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SObject.hpp"

/******************************************************************************
 *  Implementation for generic renderable object                              *
 ******************************************************************************/

SObject::SObject() {
    
    // Listen to the tick event for subclasses
    event_listener.listenToEvent(EVENT_TICK, EVENT_MEMBER(SObject::update));
    
}
