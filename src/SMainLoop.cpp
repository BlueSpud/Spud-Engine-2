//
//  SMainLoop.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 12/12/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SMainLoop.hpp"

/******************************************************************************
 *  Implementation for main loop                                              *
 ******************************************************************************/

int SMainLoop::loop() {
    
    double loop_elapsed_time = 0.0;
    double real_time_per_tick = 1.0 / TICKS_PER_SECOND;
    
    SStopwatch stopwatch;
    stopwatch.start();
    
    // Main loop start
    while (SGL::windowIsGood()) {
        
        // Manage elapsed time for the loop
        double elapsed = stopwatch.stop();
        stopwatch.start();
        loop_elapsed_time += elapsed;
        
        //SLog::verboseLog(SVerbosityLevel::Debug, "%i FPS", (int)(1.0 / elapsed));
        
        int loops = 0;
        while (loop_elapsed_time >= real_time_per_tick && loops < MAX_UPDATES_BEFORE_RENDER) {
            
            // Take in input events from user
            glfwPollEvents();
            
            // Post a tick event for everyone
            SEventSystem::postEvent(EVENT_TICK, SEventTick());
            
            // Poll for the mouse movement
            SInputSystem::moveMouse();
            
            // Record that a game update was done
            loop_elapsed_time -= real_time_per_tick;
            loops++;
            
        }
        
        // If we were forced to stop updating the game and render a frame, we cant keep up
        if (loops == MAX_UPDATES_BEFORE_RENDER)
            SLog::verboseLog(SVerbosityLevel::Critical, "Unable to perform %i updates per second", TICKS_PER_SECOND);
        
        // Upload stuff to the GPU
        SGLUploadSystem::processUploads();
        
        double interpolation = loop_elapsed_time / real_time_per_tick;
		
        // Update physics
        SPhysicsSystem::updatePhysics(elapsed, interpolation);
		 
		SEventSystem::postEvent(EVENT_START_FRAME, SEventStartFrame());
		
        // Before we render we set where the listener is
        SSoundSystem::updateListenerPosition(interpolation);
        
        // Render a frame, UI and game, only if we are focused
		if (SGL::getWindowFocused()) {
			
			SRenderSystem::render(interpolation);
			SUI::renderUI(interpolation);
        
			// Swap back and front buffer to display
			SGL::swapBuffers();
			
		}
		
		// Send out an event to clean anything up
		SEventSystem::postEvent(EVENT_END_FRAME, SEventEndFrame());
		
    }
    
    return 0;
    
}
