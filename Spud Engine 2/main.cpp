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
#include "SSceneGraph.hpp"
#include "SCamera.hpp"

double speed = 0.0;
double speed_x = 0.0;

double speed_r = 0.0;

void keyPress(int key) {
    
    switch (key) {
            
        case GLFW_KEY_W:
            speed = 0.1;
            break;
        
        case GLFW_KEY_S:
            speed = -0.1;
            break;
            
        case GLFW_KEY_D:
            speed_x = 0.1;
            break;
            
        case GLFW_KEY_A:
            speed_x = -0.1;
            break;
            
        case GLFW_KEY_LEFT:
            speed_r = 0.1;
            break;
    }
    
}

void keyRelease(int key) {
    
    switch (key) {
            
        case GLFW_KEY_W:
            speed = 0;
            break;
            
        case GLFW_KEY_S:
            speed = 0;
            break;
            
        case GLFW_KEY_D:
            speed_x = 0;
            break;
            
        case GLFW_KEY_A:
            speed_x = 0;
            break;
            
        case GLFW_KEY_LEFT:
            speed_r = 0.0;
            break;
    }
    
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
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glEnable(GL_DEPTH_TEST);
    
    // TEMP CODE
    
    SCamera camera;
    
    SSimpleSceneGraph scene_graph;
    scene_graph.addObject(new SMesh(SPath("Mesh/tank.mesh")));
    
    SMesh* sub = new SMesh(SPath("Mesh/sub.mesh"));
    sub->transform.translation.x = 2.0;
    
    scene_graph.addObject(sub);
    
    SViewport3D viewport = SViewport3D(glm::vec2(WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2), glm::vec2(0), 45.0f, glm::vec2(0.1, 1000.0));
    SGL::setUpViewport(viewport);
    
    camera.transform.translation.y = 2.0;
    
    SKeyboardListener listener;
    listener.bind(&keyPress, GLFW_KEY_S, KEY_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_W, KEY_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_D, KEY_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_A, KEY_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_LEFT, KEY_ACTION_DOWN);
    
    listener.bind(&keyRelease, GLFW_KEY_S, KEY_ACTION_UP);
    listener.bind(&keyRelease, GLFW_KEY_W, KEY_ACTION_UP);
    listener.bind(&keyRelease, GLFW_KEY_D, KEY_ACTION_UP);
    listener.bind(&keyRelease, GLFW_KEY_A, KEY_ACTION_UP);
    listener.bind(&keyRelease, GLFW_KEY_LEFT, KEY_ACTION_UP);
    
    listener.setHasFocus();
    
    // END TEMP CODE
    
    double loopElapsedTime = 0.0;
    double time_tick = 1.0 / TICKS_PER_SECOND;
    int maxUpdateCount = 5;
    
    glClearColor(0.2, 0.2, 0.2, 1.0);
    
    SStopwatch stopwatch;
    stopwatch.start();
    
    SStopwatch profiler;
    
    /* Loop until the user closes the window */
    while (SGL::windowIsGood()) {
        
        // Manage elapsed time for the loop
        double elapsed = stopwatch.stop();
        stopwatch.start();
        loopElapsedTime += elapsed;
        
        int loops = 0;
        profiler.start();
        while (loopElapsedTime >= time_tick && loops < maxUpdateCount) {

            SEventTick e;
            SEventSystem::postEvent(EVENT_TICK, e);
            
            /* Poll for and process events */
            glfwPollEvents();
            
            loopElapsedTime -= time_tick;
            
            glm::vec3 forward = glm::vec3(sinf(camera.transform.rotation.y) * speed, 0, -cos(camera.transform.rotation.y) * speed);
            glm::vec3 strafe = glm::vec3(sinf(camera.transform.rotation.y + M_PI / 2) * speed_x, 0, -cos(camera.transform.rotation.y  + M_PI / 2) * speed_x);

            camera.transform.translation += strafe + forward;
            camera.transform.rotation.y += speed_r;
            
            loops++;
            
        }
        
        SLog::verboseLog(SVerbosityLevel::Debug, "Update took %fs", (float)profiler.stop());
        
        if (loops == maxUpdateCount)
            SLog::verboseLog(SVerbosityLevel::Critical, "Cant keep up with %i ticks per second!", TICKS_PER_SECOND);
        
        profiler.start();
        
        double interpolation = loopElapsedTime / time_tick;
        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 projection_matrix = SGL::getProjectionMatrix(viewport);
        
        SGL::loadMatrix(projection_matrix, MAT_PROJECTION_MATRIX);
        
        scene_graph.render(camera, interpolation);
        
         SLog::verboseLog(SVerbosityLevel::Debug, "Render took %fs", (float)profiler.stop());
        
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
