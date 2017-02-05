//
//  STime.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/5/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "STime.hpp"

double STime::time_since_startup;

/******************************************************************************
 *  Implementation for time system                                            *
 ******************************************************************************/

void STime::startup() {

    SLog::verboseLog(SVerbosityLevel::Debug, "STime startup");

    // Zero out the timer
    glfwSetTime(0.0);
    
}

void STime::shutdown() {

    SLog::verboseLog(SVerbosityLevel::Debug, "STime shutdown");

}

double STime::getHighResolutionTime() { return glfwGetTime(); }

void STime::update(const SEvent& event) {
    
    // Acumulate time since startup
    time_since_startup += getHighResolutionTime();
    
}

/******************************************************************************
 *  Implementation for timer                                                  *
 ******************************************************************************/

STimer::STimer(boost::function<void()> _func, double duration, bool _loops) {
    
    // Save the function
    func = _func;
    loops = _loops;
    
    // Get ready for updating
    event_listener.listenToEvent(EVENT_TICK, EVENT_MEMBER(STimer::update));
    
    duration_ticks = ceil(duration * TICKS_PER_SECOND);
    ticks_elapsed = 0;
    
}

// Timer control functions
void STimer::start() { running = true; }
void STimer::stop() { running = false; }
void STimer::reset() { ticks_elapsed = 0; }

void STimer::update(const SEvent& event) {
    
    // If the timer is running, add a tick on and if we have reached the duration we fire
    if (running) {
        
        ticks_elapsed++;
    
        if (ticks_elapsed >= duration_ticks)
            fire();
        
    }
    
}

void STimer::fire() {
    
    if (!has_fired || loops) {
        
        // Fire by calling the function
        func();
    
        // Reset ticks elapsed
        ticks_elapsed = 0;
        has_fired = true;
        
    }

}

/******************************************************************************
 *  Implementation for stopwatch                                              *
 ******************************************************************************/

void SStopwatch::start() {

    // Save the start time
    time_start = STime::getHighResolutionTime();
    running = true;

}

double SStopwatch::stop() {

    running = false;
    
    // Return elapsed time
    return STime::getHighResolutionTime() - time_start;
    
}
