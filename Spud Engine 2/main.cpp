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
#include "SModel.hpp"

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
    
    SPath p = SPath("test.obj");
    SModel* mesh = (SModel*)SResourceManager::getResource(p);
 
    SGL::setKeyCallback(key_callback);
    
    a = new object();
    
    SViewport3D viewport = SViewport3D(glm::vec2(WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2), glm::vec2(0), 45.0f, glm::vec2(0.1, 1000.0));
    
    double loopElapsedTime = 0.0;
    double time_tick = 1.0 / TICKS_PER_SECOND;
    int maxUpdateCount = 5;
    
    int angle = 0;
    
    SStopwatch stopwatch;
    stopwatch.start();
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glEnable(GL_DEPTH_TEST);
    
    // Lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
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
            
            angle++;
            
            /* Poll for and process events */
            glfwPollEvents();
            
            loopElapsedTime -= time_tick;
            
            loops++;
            
        }
        
        if (loops == maxUpdateCount)
            SLog::verboseLog(SVerbosityLevel::Critical, "Cant keep up with %i ticks per second!", TICKS_PER_SECOND);
        
        double interpolation = loopElapsedTime / time_tick;
        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        SGL::setUp3DViewport(viewport);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        glTranslatef(-4, 0, -12);
        glRotated(angle + interpolation, 0, 1, 0);
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        
        
        glBindBuffer(GL_ARRAY_BUFFER, mesh->verts_id);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, mesh->normals_id);
        glNormalPointer(GL_FLOAT, 0, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, mesh->tex_coords_id);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
        
        
        glDrawArrays(GL_TRIANGLES, 0, mesh->face_count);
        
        glLoadIdentity();
        glTranslatef(4, 0, -12);
        glRotated(angle + interpolation, 0, 1, 0);
        
        glDrawArrays(GL_TRIANGLES, 0, mesh->face_count);
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        
        /* Swap front and back buffers */
        SGL::swapBuffers();
        
    }
    
    glfwTerminate();
    
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
