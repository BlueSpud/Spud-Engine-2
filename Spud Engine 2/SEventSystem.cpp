//
//  SEventSystem.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/4/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SEventSystem.hpp"

/******************************************************************************
 *  Functions  for event listener                                             *
 ******************************************************************************/

void SEventListener::listenToEvent(int event, const boost::function<void(const SEvent&)>& function) {
    
    // Get the new connection
    boost::signals2::connection new_connection = SEventSystem::listenToEvent(event, function);
    connections[event] = new_connection;
    
}

void SEventListener::stopListeningToEvent(int event) {
    
    // If we are already connected, disconnect
    if (connections.count(event))
        connections[event].disconnect();
    
}

SEventListener::~SEventListener() {
    
    // Disconnect from all of the slots
    std::map<int, boost::signals2::connection>::iterator i = connections.begin();
    
    while (i != connections.end()) {
    
        i->second.disconnect();
        i++;
    }
}

/******************************************************************************
 *  Functions for event system                                                *
 ******************************************************************************/

std::map<int, boost::signals2::signal<void(const SEvent&)>>SEventSystem::signals;

void SEventSystem::startup() {

    SLog::verboseLog(SVerbosityLevel::Debug, "SEventSystem startup");
    
}

void SEventSystem::shutdown() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SEventSystem shutdown");

}

boost::signals2::connection SEventSystem::listenToEvent(int event, const boost::function<void(const SEvent&)>& function) {
    
    // Get the signal and connect it
    boost::signals2::signal<void(const SEvent&)>& signal = getSignalForEvent(event);
    
    return signal.connect(function);
}

void SEventSystem::postEvent(int event_id, const SEvent& event) {
    
    // Get the signal and send it off
    boost::signals2::signal<void(const SEvent&)>& signal = getSignalForEvent(event_id);
    signal(event);
    
}


boost::signals2::signal<void(const SEvent&)>& SEventSystem::getSignalForEvent(int event) {
    
    // If we dont have a signal for the event that we requested, create one
    if (!signals.count(event))
        signals[event] = boost::signals2::signal<void(const SEvent&)>();
    
    return signals[event];
    
}
