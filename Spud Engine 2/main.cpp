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
#include "SKeyboardSystem.hpp"
#include "SMesh.hpp"
#include "SCamera.hpp"

double speed = 0.0;

void keyPress(int key) {
    
    switch (key) {
            
        case GLFW_KEY_W:
            speed = 0.1;
            break;
        
        case GLFW_KEY_S:
            speed = -0.1;
            break;
    }
    
}

void keyRelease(int key) {
    
    speed = 0.0;
    
}

int main(int argc, char* argv[]) {
    
    // Set verbosity level
    SLog::setVerbosityLevel(SVerbosityLevel::Debug);
    
    // Subsystem startup
    SEventSystem::startup();
    
    SGL::startup();
    
    SKeyboardSystem::startup();
    
    STime::startup();
    
    SFileSystem::startup();
    SFileSystem::getDefaultRootDirectory(argv[0]);
    
    SResourceManager::startup();
    
    SCamera camera;
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glEnable(GL_DEPTH_TEST);
    
    SMesh mesh = SMesh(SPath("Mesh/tank.mesh"));
    
    SViewport3D viewport = SViewport3D(glm::vec2(WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2), glm::vec2(0), 45.0f, glm::vec2(0.1, 1000.0));
    SGL::setUpViewport(viewport);
    
    camera.transform.translation.y = 1.0;
    
    SKeyboardListener listener;
    listener.bind(&keyPress, GLFW_KEY_S, KEY_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_W, KEY_ACTION_DOWN);
    
    listener.bind(&keyRelease, GLFW_KEY_S, KEY_ACTION_UP);
    listener.bind(&keyRelease, GLFW_KEY_W, KEY_ACTION_UP);
    
    listener.setHasFocus();
    
    
    double loopElapsedTime = 0.0;
    double time_tick = 1.0 / TICKS_PER_SECOND;
    int maxUpdateCount = 5;
    
    SStopwatch stopwatch;
    stopwatch.start();
    
    double rot = 0.0;
    
    /* Loop until the user closes the window */
    while (SGL::windowIsGood()) {
        
        // Manage elapsed time for the loop
        double elapsed = stopwatch.stop();
        stopwatch.start();
        loopElapsedTime += elapsed;
        
        int loops = 0;
        while (loopElapsedTime >= time_tick && loops < maxUpdateCount) {

            SEventTick e;
            SEventSystem::postEvent(EVENT_TICK, e);
            
            /* Poll for and process events */
            glfwPollEvents();
            
            loopElapsedTime -= time_tick;
            
            rot += 0.02;
            
            camera.transform.translation.x += (sinf(camera.transform.rotation.y)) * speed;
            camera.transform.translation.z -= (cos(camera.transform.rotation.y)) * speed;
            
            loops++;
            
        }
        
        if (loops == maxUpdateCount)
            SLog::verboseLog(SVerbosityLevel::Critical, "Cant keep up with %i ticks per second!", TICKS_PER_SECOND);
        
        double interpolation = loopElapsedTime / time_tick;
        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 projection_matrix = SGL::getProjectionMatrix(viewport);
        
        SGL::loadMatrix(projection_matrix, MAT_PROJECTION_MATRIX);
        
        SGL::clearMatrix(MAT_MODELVIEW_MATRIX);
        camera.translateToCameraSpace();
        
        mesh.transform.rotation.y = rot + interpolation * 0.02;
        
        mesh.render(interpolation);
        
        /* Swap front and back buffers */
        SGL::swapBuffers();
        
    }
    
    glfwTerminate();
    
    // Subsystem shutdown
    SResourceManager::shutdown();
    
    // Write out the log to a file
    #ifdef __APPLE__
    
    // Because the root path is in the app bundle, which we cant write to, so we move the path to outside it
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
    
    SKeyboardSystem::shutdown();
    
    SGL::shutdown();
    
    SEventSystem::shutdown();
    
    return 0;
}
