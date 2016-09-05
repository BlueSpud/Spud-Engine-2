//
//  SEventSystem.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/4/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SEventSystem_hpp
#define SEventSystem_hpp

#include <iostream>
#include <map>
#include <boost/signals2.hpp>

#include "SSubsystem.hpp"

struct SEvent {};

/******************************************************************************
 *  Definition for event listener                                             *
 ******************************************************************************/

class SEventListener {
    
    public:
    
        void listenToEvent(int event, const boost::function<void(const SEvent&)>& function);
        void stopListeningToEvent(int event, const boost::function<void(const SEvent&)>& function);
    
        ~SEventListener();
    
    private:
    
        std::map<int, boost::signals2::connection>connections;
    
};

/******************************************************************************
 *  Definition for event system                                               *
 ******************************************************************************/

class SEventSystem : public SSubsystem {
    
    friend class SEventListener;
    
    public:
    
        static void startup();
        static void shutdown();
    
        static void postEvent(int event_id, const SEvent& event);
    
    private:
    
        static boost::signals2::connection listenToEvent(int event, const boost::function<void(const SEvent&)>& function);
    
        static std::map<int, boost::signals2::signal<void(const SEvent&)>>signals;
        static boost::signals2::signal<void(const SEvent&)>& getSignalForEvent(int event);
    
    protected:
    
    
};

#endif /* SEventSystem_hpp */
