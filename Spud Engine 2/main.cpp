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
#include "SMaterial.hpp"
#include "SCamera.hpp"

double velocity_constant = 0.1;
double direction = 1;

SCamera camera;

void key_release(int key) {
    
    
}

void key_press(int key) {
    
    
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
    
    SPath pasd = SPath("test.obj");
    SModel* mesh = (SModel*)SResourceManager::getResource(pasd);
 
    SGL::setKeyCallback(key_callback);
    
    double loopElapsedTime = 0.0;
    double time_tick = 1.0 / TICKS_PER_SECOND;
    int maxUpdateCount = 5;
    
    double cam_position = 0;
    
    SStopwatch stopwatch;
    stopwatch.start();
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glEnable(GL_DEPTH_TEST);
    
    SMaterial* mat = (SMaterial*)SResourceManager::getResource(SPath("Material/test.mat"));
    
    std::map<std::string, STexture*>textures;
    textures["tex_albedo"] = (STexture*)SResourceManager::getResource(SPath("tank.jpg"));
    textures["tex_overlay"] = (STexture*)SResourceManager::getResource(SPath("overlay.png"));
    SMaterialInstance* mat_instance = mat->createMaterialInstance(textures, 1);
    
    STransform transform;
    transform.translation.x = 2;
    
    SViewport3D viewport = SViewport3D(glm::vec2(WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2), glm::vec2(0), 45.0f, glm::vec2(0.1, 1000.0));
    SGL::setUpViewport(viewport);
    
    camera.transform.translation.y = 1.0;
    camera.transform.translation.z = 4.0;
    
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
            
            cam_position += velocity_constant * direction;
            
            /* Poll for and process events */
            glfwPollEvents();
            
            loopElapsedTime -= time_tick;
            
            loops++;
            
        }
        
        if (loops == maxUpdateCount)
            SLog::verboseLog(SVerbosityLevel::Critical, "Cant keep up with %i ticks per second!", TICKS_PER_SECOND);
        
        //double interpolation = loopElapsedTime / time_tick;
        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat_instance->useMaterial();
        
        glm::mat4 model_matrix = SGL::transformToMatrix(transform);
        glm::mat4 projection_matrix = SGL::getProjectionMatrix(viewport);
        
        SGL::loadMatrix(projection_matrix, MAT_PROJECTION_MATRIX);
        
        SGL::clearMatrix(MAT_MODELVIEW_MATRIX);
        camera.translateToCameraSpace();
        SGL::mulMatrix(model_matrix, MAT_MODELVIEW_MATRIX);
        
        mesh->render();
        
        /* Swap front and back buffers */
        SGL::swapBuffers();
        
    }
    
    glfwTerminate();
    
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
