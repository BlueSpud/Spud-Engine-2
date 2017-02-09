//
//  STime.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/5/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef STime_hpp
#define STime_hpp

#include <iostream>
#include <math.h>
#include "SSubsystem.hpp"

#include "SGL.hpp"
#include "SEventSystem.hpp"

#define TICKS_PER_SECOND 20

#define TIMER_LOOP_INFINITE -1

/******************************************************************************
 *  Definition for time system                                                *
 ******************************************************************************/

class STime : public SSubsystem {
    
    public:
    
        static void startup();
        static void shutdown();
    
        static double getHighResolutionTime();
    
        static void update(const SEvent& event);
    
    private:
    
        static double time_since_startup;
    
};

/******************************************************************************
 *  Definition for timer                                                      *
 ******************************************************************************/

class STimer {
    
    friend class boost::signals2::signal<void(SEvent*)>;
    
    public:
    
        STimer(boost::function<void()> _func, double duration, bool _loops);
    
        void start();
        void stop();
        void reset();
    
    private:
    
        SEventListener event_listener;
    
        int ticks_elapsed;
        int duration_ticks;
    
        bool loops;
        bool has_fired = false;
        bool running = true;
    
        void update(const SEvent& event);
        void fire();
    
        boost::function<void()> func;
    
};

/******************************************************************************
 *  Definition for stopwatch                                                  *
 ******************************************************************************/

class SStopwatch {
    
    public:
    
        void start();
        double stop();
		double getTime();
	
    private:
    
        double time_start;
        bool running = false;
    
};

#endif /* STime_hpp */
