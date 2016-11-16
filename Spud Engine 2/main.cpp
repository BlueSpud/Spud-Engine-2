//
//  main.cpp
//  GLFW Test
//
//  Creatsed by Logan Pazol on 8/16/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include <btBulletDynamicsCommon.h>

#include "STime.hpp"
#include "SKeyboardSystem.hpp"
#include "SMouseSystem.hpp"
#include "SDeferredRenderingPipeline.hpp"
#include "SCamera.hpp"
#include "SHotLoadSystem.hpp"
#include "SFramebuffer.hpp"

#include "SConsole.hpp"

#include "SActor.hpp"
#include "SStaticMeshComponent.hpp"

double speed = 0.0;
double speed_x = 0.0;

double speed_r = 0.0;

SCamera camera;
SDeferredRenderingPipleline* deferred_pipeline;

void moveLight(int key) {
    
    // Temp function
    deferred_pipeline->light->transform = camera.transform;
    deferred_pipeline->light->needs_shadow_update = true;
    
}

void keyPress(int key) {
    
    switch (key) {
            
        case GLFW_KEY_W:
            speed += 0.4;
            break;
        
        case GLFW_KEY_S:
            speed += -0.4;
            break;
            
        case GLFW_KEY_D:
            speed_x += 0.4;
            break;
            
        case GLFW_KEY_A:
            speed_x += -0.4;
            break;
            
        case GLFW_KEY_LEFT:
            speed_r += -0.05;
            break;
            
        case GLFW_KEY_RIGHT:
            speed_r += 0.05;
            break;
    }
    
}

void keyRelease(int key) {
    
    switch (key) {
            
        case GLFW_KEY_W:
            speed -= 0.4;
            break;
            
        case GLFW_KEY_S:
            speed -= -0.4;
            break;
            
        case GLFW_KEY_D:
            speed_x -= 0.4;
            break;
            
        case GLFW_KEY_A:
            speed_x -= -0.4;
            break;
            
        case GLFW_KEY_LEFT:
            speed_r -= -0.05;
            break;
            
        case GLFW_KEY_RIGHT:
            speed_r -= 0.05;
            break;
    }
    
}

void mouseMove(const SEvent& event) {
    
    // Add rotation to the camera
    const SEventMouseMove& event_m = (SEventMouseMove&)event;
    camera.transform.rotation_velocity.y = event_m.mouse_vel.x * 0.01;
    camera.transform.rotation_velocity.x = -event_m.mouse_vel.y * 0.005;
    
}

int frames_counted = 0;

void FPS() {
    
    //SLog::verboseLog(SVerbosityLevel::Debug, "Current FPS: %i", frames_counted);
    frames_counted = 0;
    
}

int main(int argc, char* argv[]) {
    
    // Set verbosity level
    SLog::setVerbosityLevel(SVerbosityLevel::Debug);
    
    // Subsystem startup
    SEventSystem::startup();
    
    SGL::startup();
    STime::startup();
    
    SGLUploadSystem::startup();
    
    SKeyboardSystem::startup();
    SMouseSystem::startup();
    

    
    SFileSystem::startup();
    SFileSystem::getDefaultRootDirectory("/Users/Logan/Desktop/Spud Engine 2/a/a/");
    
    SResourceManager::startup();
    SHotLoadSystem::startup();
    
    STextRenderer::startup();
    SConsole::startup();
    
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glCullFace(GL_BACK);
    
    glEnable(GL_DEPTH_TEST);
    
    // TEMP CODE
    
    SEventListener e_listener;
    
    camera.transform.translation.y = 2.0;
    
    e_listener.listenToEvent(EVENT_MOUSE_MOVE, &mouseMove);
    
    SSimpleSceneGraph scene_graph;
    
    // Access the mesh
    SPath mesh_path = SPath("Model/shapes.smdl");
    SStaticMeshInstance* mesh = (SStaticMeshInstance*)SResourceManager::getResource(mesh_path);
    
    mesh->transform.scale = glm::vec3(2.0);
    
    scene_graph.addObject(mesh);
    
    SPath font_path = SPath("Font/Arial.font");
    
    glm::ivec2 window_framebuffer_size = SGL::getWindowFramebufferSize();
    
    SViewport viewport_2D = SViewport(window_framebuffer_size, glm::vec2());
    SViewport3D viewport_3D = SViewport3D(window_framebuffer_size / 2, glm::vec2(0), 45.0f, glm::vec2(0.1, 500.0));
    
    deferred_pipeline = new SDeferredRenderingPipleline(&viewport_2D, &viewport_3D);
    
    SKeyboardListener listener;
    listener.bind(&keyPress, GLFW_KEY_S, KEY_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_W, KEY_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_D, KEY_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_A, KEY_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_LEFT, KEY_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_RIGHT, KEY_ACTION_DOWN);
    
    listener.bind(&moveLight, GLFW_KEY_P, KEY_ACTION_DOWN);
    listener.bind(&SConsole::activate, GLFW_KEY_GRAVE_ACCENT, KEY_ACTION_DOWN);
    
    listener.bind(&keyRelease, GLFW_KEY_S, KEY_ACTION_UP);
    listener.bind(&keyRelease, GLFW_KEY_W, KEY_ACTION_UP);
    listener.bind(&keyRelease, GLFW_KEY_D, KEY_ACTION_UP);
    listener.bind(&keyRelease, GLFW_KEY_A, KEY_ACTION_UP);
    listener.bind(&keyRelease, GLFW_KEY_LEFT, KEY_ACTION_UP);
    listener.bind(&keyRelease, GLFW_KEY_RIGHT, KEY_ACTION_UP);
    
    listener.setHasFocus();
    
    // END TEMP CODE
    
    // Clear all the uploads
    SGLUploadSystem::processUploads();
    SGLUploadSystem::setUploadLimitPerFrame(10);
    
    double loopElapsedTime = 0.0;
    double time_tick = 1.0 / TICKS_PER_SECOND;
    int maxUpdateCount = 5;

    SStopwatch profiler;
    
    SLog::verboseLog(SVerbosityLevel::Debug, "Startup complete\n");
    
    glClearColor(0.2, 0.2, 0.2, 0.0);
    
    SStopwatch stopwatch;
    stopwatch.start();
    
    // Create a timer to display the FPS
    STimer fps_timer = STimer(&FPS, 1.0, TIMER_LOOP_INFINITE);
    
    
    // Main loop start
    while (SGL::windowIsGood()) {
        
        // Manage elapsed time for the loop
        double elapsed = stopwatch.stop();
        stopwatch.start();
        loopElapsedTime += elapsed;
        
        //SLog::verboseLog(SVerbosityLevel::Debug, "%i FPS", (int)(1.0 / elapsed));
        
        int loops = 0;
        profiler.start();
        while (loopElapsedTime >= time_tick && loops < maxUpdateCount) {
            
            // Take in input events from user
            glfwPollEvents();
            
            // Update camera position and calculate new velocity
            camera.transform.update();
            
            glm::vec3 forward = glm::vec3(sinf(camera.transform.rotation.y) * speed, 0, -cos(camera.transform.rotation.y) * speed);
            glm::vec3 strafe = glm::vec3(sinf(camera.transform.rotation.y + M_PI / 2) * speed_x, 0, -cos(camera.transform.rotation.y  + M_PI / 2) * speed_x);
            glm::vec3 fly = glm::vec3(0, sinf(camera.transform.rotation.x) * speed, 0);
            
            camera.transform.translation_velocity = strafe + forward + fly;
            
            // Post a tick event for everyone
            SEventTick e;
            SEventSystem::postEvent(EVENT_TICK, e);
            
            // Record that a game update was done
            loopElapsedTime -= time_tick;
            loops++;
            
        }
        
        SGL::setMouseInputMode(GLFW_CURSOR_DISABLED);
        
        //SLog::verboseLog(SVerbosityLevel::Debug, "Update took %fs", (float)profiler.stop());
        
        // If we were forced to stop updating the game and render a frame, we cant keep up
        if (loops == maxUpdateCount)
            SLog::verboseLog(SVerbosityLevel::Critical, "Cant keep up with %i ticks per second!", TICKS_PER_SECOND);
        
        profiler.start();
        
        // Upload stuff to the GPU
        SGLUploadSystem::processUploads();
        
        double interpolation = loopElapsedTime / time_tick;
        
        // Render using a deferred rendering pipline
        deferred_pipeline->render(scene_graph, camera, interpolation);
        
        
        // Temp enable blending function
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        SConsole::render();
        
        glDisable(GL_BLEND);
        
        //SLog::verboseLog(SVerbosityLevel::Debug, "Render took %fs", (float)profiler.stop());
        
        // Swap back and front buffer to display
        SGL::swapBuffers();
        
        // Save that we drew a frame
        frames_counted++;
        
    }
    
    // Clean up
    deferred_pipeline->unload();
    delete deferred_pipeline;
    
    // Subsystem shutdown
    SConsole::shutdown();
    STextRenderer::shutdown();
    
    SHotLoadSystem::shutdown();
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
    
    SMouseSystem::shutdown();
    SKeyboardSystem::shutdown();
    
    SGLUploadSystem::shutdown();
    
    STime::shutdown();
    SGL::shutdown();
    
    SEventSystem::shutdown();
    
    return 0;
}
