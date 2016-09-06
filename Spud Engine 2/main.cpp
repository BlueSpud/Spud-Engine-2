//
//  main.cpp
//  GLFW Test
//
//  Creatsed by Logan Pazol on 8/16/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include <iostream>
#include <vector>
#include <boost/thread.hpp>
#include <btBulletDynamicsCommon.h>

#include "object.hpp"
#include "SFileSystem.hpp"
#include "SResourceManager.hpp"
#include "SMesh.hpp"

object* a;

double velocity_constant = 5;

void key_release(int key) {
    
    switch (key) {
            
        case GLFW_KEY_W:
            
            a->velocity_y += velocity_constant;
            
            break;
        case GLFW_KEY_S:
            
            a->velocity_y -= velocity_constant;
            
            break;
            
            
        case GLFW_KEY_D:
            
            a->velocity_x -= velocity_constant;
            
            break;
            
        case GLFW_KEY_A:
            
            a->velocity_x += velocity_constant;
            
            break;
            
        case GLFW_KEY_R:
            
            a->scale_driver.reverse();
            
            break;
    }
    
}

void key_press(int key) {
    
    switch (key) {
            
        case GLFW_KEY_W:
            
            a->velocity_y -= velocity_constant;
            
            break;
            
        case GLFW_KEY_S:
            
            a->velocity_y += velocity_constant;
            
            break;
            
            
        case GLFW_KEY_D:
            
            a->velocity_x += velocity_constant;
            
            break;
            
        case GLFW_KEY_A:
            
            a->velocity_x -= velocity_constant;
            
            break;
            
        case GLFW_KEY_R:
            
            a->scale_driver.play();
            
            break;
            
    }
    
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    
    if (action == GLFW_PRESS)
        key_press(key);
    
    if (action == GLFW_RELEASE)
        key_release(key);
    
}

void test() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "Testing timer function");
    
}

int main(int argc, char* argv[]) {
    
    // Set verbosity level
    SLog::setVerbosityLevel(SVerbosityLevel::Debug);
    
    // Subsystem startup
    SEventSystem::startup();
    
    SGL::startup();
    
    STime::startup();
    
    SFileSystem::startup();
    SFileSystem::getDefaultRootDirectory(argv[0]);
    
    SResourceManager::startup();
    
    SPath p = SPath("test.txt");
    SResourceManager::getResource(p);
 
    SGL::setKeyCallback(key_callback);
    
    a = new object();
    
    double loopElapsedTime = 0.0;
    int maxUpdateCount = 5;
    
    SStopwatch stopwatch;
    stopwatch.start();
    
    STimer t = STimer(boost::bind(&test), 2.0, true);
    
    /* Loop until the user closes the window */
    while (SGL::windowGood()) {
        
        // Manage elapsed time for the loop
        double elapsed = stopwatch.stop();
        stopwatch.start();
        loopElapsedTime += elapsed;
        
        int loops = 0;
        while (loopElapsedTime >= TIME_PER_TICK && loops < maxUpdateCount) {

            SEventTick e;
            SEventSystem::postEvent(EVENT_TICK, e);
            
            /* Poll for and process events */
            glfwPollEvents();
            
            loopElapsedTime -= TIME_PER_TICK;
            
            loops++;
            
        }
        
        if (loops == maxUpdateCount)
            SLog::verboseLog(SVerbosityLevel::Critical, "Cant keep up with %i ticks per second!", TICKS_PER_SECOND);
        
        double interpolation = loopElapsedTime / TIME_PER_TICK;
        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, 1280, 800, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);

        a->render(interpolation);
        
        /* Swap front and back buffers */
        SGL::swapBuffers();
        
    }
    
    glfwTerminate();
    
    std::cout << "A test line of code\n";
    delete a;
    
    // Subsystem shutdown
    SResourceManager::shutdown();
    
    // Write out the log to a file, make a new path where we can edit if we are on macOS
    #ifdef __APPLE__
    
    SPath log_root_path = SPath(argv[0]);
    
    log_root_path.removeLastPathComponent();
    log_root_path.removeLastPathComponent();
    log_root_path.removeLastPathComponent();
    log_root_path.removeLastPathComponent();
    
    #endif
    
    SFileSystem::setRootDirectory(log_root_path);
    SLog::writeLogToFile();
    
    SFileSystem::shutdown();
    
    STime::shutdown();
    
    SGL::shutdown();
    
    SEventSystem::shutdown();
    
    return 0;
}
