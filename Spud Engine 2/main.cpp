//
//  main.cpp
//  GLFW Test
//
//  Creatsed by Logan Pazol on 8/16/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include <btBulletDynamicsCommon.h>

#include "STime.hpp"
#include "SInputSystem.hpp"
#include "SDeferredRenderingPipeline.hpp"
#include "SCamera.hpp"
#include "SHotLoadSystem.hpp"
#include "SFramebuffer.hpp"

#include "SConsole.hpp"
#include "SUIButton.hpp"

#include "SActor.hpp"
#include "SStaticMeshComponent.hpp"

#include "SSoundSystem.hpp"
#include "SSoundInstance.hpp"

double speed = 0.0;
double speed_x = 0.0;

double speed_r = 0.0;

SCamera camera;
SDeferredRenderingPipleline* deferred_pipeline;
SSoundInstance* sound;

void moveLight(int key) {
    
    // Temp function
    deferred_pipeline->light->transform = camera.transform;
    deferred_pipeline->light->needs_shadow_update = true;
    
}

void hello(const std::vector<std::string>& args) {
    
    // Test command
    SLog::verboseLog(SVerbosityLevel::Debug, "Howdy!");
    
}

REGISTER_COMMAND(hello, &hello);

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
            
        case GLFW_KEY_G:
            
            SInputSystem::setInputMode(SInputModeUI);
            SGL::setMouseInputMode(GLFW_CURSOR_NORMAL);
            
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
        
    }
    
}

void mouseClick(int button) {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "CLICK");
    
}

void mouseMove(glm::vec2 mouse_vel) {
    
    // Add rotation to the camera
    camera.transform.rotation_velocity.y = mouse_vel.x * 0.01;
    camera.transform.rotation_velocity.x = -mouse_vel.y * 0.005;
    
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
    
    SSoundSystem::startup();
    SGL::startup();
    STime::startup();
    
    SGLUploadSystem::startup();
    
    SInputSystem::startup();

    SFileSystem::startup();
    SFileSystem::getDefaultRootDirectory("/Users/Logan/Desktop/Spud Engine 2/a/a/");
    
    SResourceManager::startup();
    SHotLoadSystem::startup();
    
    STextRenderer::startup();
    SUI::startup();
    
    SConsole::startup();
    
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glCullFace(GL_BACK);
    
    glEnable(GL_DEPTH_TEST);
    
    // TEMP CODE
    
    sound = (SSoundInstance*)SResourceManager::getResource(SPath("Sound/Birds.wav"));
    sound->play();
    sound->setLoops(true);
    sound->sound_position_mode = SSoundPositionMode2D;
    
    camera.transform.translation.y = 2.0;
    SCamera::current_camera = &camera;
    
    SSimpleSceneGraph scene_graph;
    
    // Access the mesh
    SStaticMeshInstance* mesh = (SStaticMeshInstance*)SResourceManager::getResource(SPath("Model/house.smdl"));
    mesh->transform.scale = glm::vec3(2.0);
    scene_graph.addObject(mesh);
    
    mesh = (SStaticMeshInstance*)SResourceManager::getResource(SPath("Model/house.smdl"));
    mesh->transform.scale = glm::vec3(2.0);
    mesh->transform.translation.z = 5.7;
    
    scene_graph.addObject(mesh);
    
    mesh = (SStaticMeshInstance*)SResourceManager::getResource(SPath("Model/plane.smdl"));
    scene_graph.addObject(mesh);
    
    glm::ivec2 window_framebuffer_size = SGL::getWindowFramebufferSize();
    
    SViewport viewport_2D = SViewport(window_framebuffer_size, glm::vec2());
    SViewport3D viewport_3D = SViewport3D(window_framebuffer_size / (int)SGL::getScreenScale(), glm::vec2(0), 45.0f, glm::vec2(0.1, 500.0));
    
    deferred_pipeline = new SDeferredRenderingPipleline(&viewport_2D, &viewport_3D);
    
    SInputListener listener;
    listener.bind(&keyPress, GLFW_KEY_S, INPUT_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_W, INPUT_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_D, INPUT_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_A, INPUT_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_LEFT, INPUT_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_RIGHT, INPUT_ACTION_DOWN);
    
    listener.bind(&moveLight, GLFW_KEY_P, INPUT_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_G, INPUT_ACTION_DOWN);
    listener.bind(&SConsole::activate, GLFW_KEY_GRAVE_ACCENT, INPUT_ACTION_UP);
    
    listener.bind(&keyRelease, GLFW_KEY_S, INPUT_ACTION_UP);
    listener.bind(&keyRelease, GLFW_KEY_W, INPUT_ACTION_UP);
    listener.bind(&keyRelease, GLFW_KEY_D, INPUT_ACTION_UP);
    listener.bind(&keyRelease, GLFW_KEY_A, INPUT_ACTION_UP);
    
    listener.bind(&mouseClick, GLFW_MOUSE_BUTTON_LEFT, INPUT_ACTION_DOWN);
    listener.bind(&mouseClick, GLFW_MOUSE_BUTTON_RIGHT, INPUT_ACTION_DOWN);
    listener.mouse_move_func = mouseMove;
    
    listener.setHasFocus();
    
    SUIGraph* ui_graph = new SUIGraph();
    
    SUIButton* button = new SUIButton();
    
    button->background_image = (STexture*)SResourceManager::getResource(SPath("Texture/ui/button/button_normal.png"));
    button->hover_image = (STexture*)SResourceManager::getResource(SPath("Texture/ui/button/button_hover.png"));
    button->press_image = (STexture*)SResourceManager::getResource(SPath("Texture/ui/button/button_press.png"));
    
    button->frame.origin = glm::vec2(5.0, 5.0);
    button->frame.size = glm::vec2(279, 37.5);
    
    button->font = (SFont*)SResourceManager::getResource(SPath("Font/Arial.font"));
    button->label = "FUNCTION CALLED ON RELEASE";
    button->font_size = 12.0;
    
    button->release_func = &mouseClick;
    
    ui_graph->addWidget(button);
    
    button = new SUIButton();
    
    button->background_image = (STexture*)SResourceManager::getResource(SPath("Texture/ui/button/button_normal.png"));
    button->hover_image = (STexture*)SResourceManager::getResource(SPath("Texture/ui/button/button_hover.png"));
    button->press_image = (STexture*)SResourceManager::getResource(SPath("Texture/ui/button/button_press.png"));
    
    button->frame.origin = glm::vec2(5.0, 47.5);
    button->frame.size = glm::vec2(279, 37.5);
    
    button->font = (SFont*)SResourceManager::getResource(SPath("Font/Arial.font"));
    button->label = "FUNCTION CALLED ON PRESS";
    button->font_size = 12.0;
    
    button->press_func = &mouseClick;
    
    ui_graph->addWidget(button);
    
    SUITextField* text_field = new SUITextField();
    
    text_field->font = button->font;
    text_field->font_size = CONSOLE_FONT_SIZE;
    
    text_field->background_color = glm::vec4(0.1, 0.1, 0.1, 1.0);
    
    text_field->frame.origin = glm::vec2(5.0, 95);
    text_field->frame.size = glm::vec2(279, 37.5);
    
    ui_graph->addWidget(text_field);
    
    SUI::current_ui_graph = ui_graph;
    
    
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
    
    // Disable the cursor by default
    SGL::setMouseInputMode(GLFW_CURSOR_DISABLED);
    
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
        
        //SLog::verboseLog(SVerbosityLevel::Debug, "Update took %fs", (float)profiler.stop());
        
        // If we were forced to stop updating the game and render a frame, we cant keep up
        if (loops == maxUpdateCount)
            SLog::verboseLog(SVerbosityLevel::Critical, "Unable to perform %i updates per second", TICKS_PER_SECOND);
        
        profiler.start();
        
        // Upload stuff to the GPU
        SGLUploadSystem::processUploads();
        
        double interpolation = loopElapsedTime / time_tick;
        
        // Before we render we set where the listener is
        SSoundSystem::updateListenerPosition(interpolation);
        
        // Render using a deferred rendering pipline
        deferred_pipeline->render(scene_graph, camera, interpolation);
        
        SUI::renderUI(interpolation);
        
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
    
    SUI::shutdown();
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
    
    SInputSystem::shutdown();
    
    SGLUploadSystem::shutdown();
    
    STime::shutdown();
    SGL::shutdown();
    SSoundSystem::shutdown();
    
    SEventSystem::shutdown();
    
    return 0;
}
