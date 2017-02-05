//
//  main.cpp
//  GLFW Test
//
//  Creatsed by Logan Pazol on 8/16/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SMainLoop.hpp"
#include "SDeferredRenderingPipeline.hpp"
#include "SCamera.hpp"
#include "SHotLoadSystem.hpp"

#include "SConsole.hpp"
#include "SUIButton.hpp"
#include "SCursor.hpp"

#include "SActor.hpp"
#include "SStaticMeshComponent.hpp"

#include "SSoundSystem.hpp"
#include "SSoundEmitter.hpp"

#include "SRigidBody.hpp"
#include "SCharacterController.hpp"

double speed = 0.0;
double speed_x = 0.0;

SCamera camera;
SLight* light;
SSoundEmitter* sound_emitter;
//SCharacterController* controller;

void moveLight(int key) {
    
    light->transform.translation = camera.transform.translation;
	light->transform.rotation = camera.transform.rotation;
    
}

void hello(const std::vector<std::string>& args) {
    
    // Test command
    SLog::verboseLog(SVerbosityLevel::Debug, "Howdy!");
    
}

REGISTER_COMMAND(hello, &hello);

void keyPress(int key) {
    
    switch (key) {
            
        case GLFW_KEY_W:
            speed += 0.2;
            break;
        
        case GLFW_KEY_S:
            speed += -0.2;
            break;
            
        case GLFW_KEY_D:
            speed_x += 0.2;
            break;
            
        case GLFW_KEY_A:
            speed_x += -0.2;
            break;
            
        case GLFW_KEY_G:
            
            SInputSystem::setInputMode(SInputModeUI);
            SGL::setMouseInputMode(GLFW_CURSOR_NORMAL);
            
            break;
        
        case GLFW_KEY_SPACE:
    
			//if (controller->isOnGround())
			//    controller->jump();
            
        break;

    }
    
}

void keyRelease(int key) {
    
    switch (key) {
            
        case GLFW_KEY_W:
            speed -= 0.2;
            break;
            
        case GLFW_KEY_S:
            speed -= -0.2;
            break;
            
        case GLFW_KEY_D:
            speed_x -= 0.2;
            break;
            
        case GLFW_KEY_A:
            speed_x -= -0.2;
            break;
        
    }
    
}

void mouseClick(int button) {
    
    SInputSystem::setInputMode(SInputModeGame);
    SGL::setMouseInputMode(GLFW_CURSOR_DISABLED);
    
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

void update(const SEvent& event) {
    
    // Update camera position and calculate new velocity
    camera.transform.update();
    
    glm::vec3 forward = glm::vec3(sinf(camera.transform.rotation.y) * speed, 0.0,
                                 -cos(camera.transform.rotation.y) * speed);

    glm::vec3 strafe =  glm::vec3(sinf(camera.transform.rotation.y + M_PI / 2) * speed_x, 0.0,
                                 -cos(camera.transform.rotation.y  + M_PI / 2) * speed_x);
	
    glm::vec3 fly = glm::vec3(0, sinf(camera.transform.rotation.x) * speed, 0);

	glm::vec3 move_vector = strafe + forward + fly;
	
	if (glm::length(move_vector)) {
		
		camera.transform.translation_velocity = glm::normalize(move_vector) * 0.2f;
		//controller->setMoveDirection((strafe + forward) * 35.0f);
		
	} else camera.transform.translation_velocity = glm::vec3(0.0);
    
}

int main(int argc, char* argv[]) {
    
    // Set verbosity level
    SLog::setVerbosityLevel(SVerbosityLevel::Debug);
    
    // Subsystem startup
    SEventSystem::startup();
    
    SSoundSystem::startup();
    SGL::startup();
    SRenderSystem::startup();
    
    STime::startup();
    
    SPhysicsSystem::startup();
    
    SGLUploadSystem::startup();
    
    SInputSystem::startup();

    SFileSystem::startup();
    SFileSystem::getDefaultRootDirectory("/Users/Logan/Desktop/Spud Engine 2/a/a/");
    
    SResourceManager::startup();
    SHotLoadSystem::startup();
    
    STextRenderer::startup();
    SUI::startup();
    
    SConsole::startup();
    
    // TEMP CODE
	camera.transform.translation.y = 2.0;
    SCamera::current_camera = &camera;
//    
//    SSound* sound = SResourceManager::getResource<SSound>(SPath("Sound/Birds.wav"));
//    sound_emitter = new SSoundEmitter();
//    sound_emitter->setSound(sound);
//    sound_emitter->play();
//    sound_emitter->setLoops(true);
//    sound_emitter->transform.translation = camera.transform.translation;
	
    SSimpleSceneGraph* scene_graph = new SSimpleSceneGraph();
	
    SStaticMesh* mesh = new SStaticMesh(SResourceManager::getResource<SModel>(SPath("Model/house.smdl")));
	scene_graph->addObject(mesh);
	
	mesh = new SStaticMesh(SResourceManager::getResource<SModel>(SPath("Model/house.smdl")));
	mesh->transform.translation.z = 2.8;
	
	scene_graph->addObject(mesh);
	
	mesh = new SStaticMesh(SResourceManager::getResource<SModel>(SPath("Model/plane.smdl")));
	scene_graph->addObject(mesh);
	
    glm::ivec2 window_framebuffer_size = SGL::getWindowFramebufferSize();
    
	SViewport viewport_2D = SViewport(window_framebuffer_size / (int)SGL::getScreenScale(), glm::vec2());
	SViewport screen_viewport = SViewport(window_framebuffer_size, glm::vec2());
    SViewport3D viewport_3D = SViewport3D(window_framebuffer_size / (int)SGL::getScreenScale(), glm::vec2(0), 45.0f, glm::vec2(0.1, 500.0));
    
    // Create the light graph
    SSimpleLightGraph* light_graph = new SSimpleLightGraph();
    
    light = new SDirectionalLight();
    light->transform.translation = glm::vec3(0.0, 1.5, 0.0);
    light->transform.rotation = glm::vec3(-0.541348, 7.37523, 0.0);
    light->casts_shadow = true;
    
    light_graph->addLight(light);
	
    SRenderSystem::rendering_pipeline = new SDeferredRenderingPipleline(&viewport_2D, &screen_viewport, &viewport_3D);
    scene_graph->makeCurrent();
    SRenderSystem::current_light_graph = light_graph;
    
    SInputListener listener;
    listener.bind(&keyPress, GLFW_KEY_S, INPUT_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_W, INPUT_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_D, INPUT_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_A, INPUT_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_SPACE, INPUT_ACTION_DOWN);
    
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
    
//    SUIButton* button = new SUIButton();
//    
//    button->background_image = SResourceManager::getResource<STexture>(SPath("Texture/ui/button/button_normal.png"));
//    button->hover_image = SResourceManager::getResource<STexture>(SPath("Texture/ui/button/button_hover.png"));
//    button->press_image = SResourceManager::getResource<STexture>(SPath("Texture/ui/button/button_press.png"));
//    
//    button->hover_sound = SResourceManager::getResource<SSound>(SPath("Sound/ui/button/button_hover.wav"));
//    button->press_sound = SResourceManager::getResource<SSound>(SPath("Sound/ui/button/button_press.wav"));
//    
//    button->frame.origin = glm::vec2(5.0, 5.0);
//    button->frame.size = glm::vec2(280, 50);
//    
//    button->font = SResourceManager::getResource<SFont>(SPath("Font/Arial.font"));
//    button->label = "Button 1";
//    button->font_size = 13.0;
//    
//    button->action = &mouseClick;
//    
//    ui_graph->addWidget(button);
//    
//    button = new SUIButton();
//    
//    button->background_image = SResourceManager::getResource<STexture>(SPath("Texture/ui/button/button_normal.png"));
//    button->hover_image = SResourceManager::getResource<STexture>(SPath("Texture/ui/button/button_hover.png"));
//    button->press_image = SResourceManager::getResource<STexture>(SPath("Texture/ui/button/button_press.png"));
//    
//    button->hover_sound = SResourceManager::getResource<SSound>(SPath("Sound/ui/button/button_hover.wav"));
//    button->press_sound = SResourceManager::getResource<SSound>(SPath("Sound/ui/button/button_press.wav"));
//    
//    button->frame.origin = glm::vec2(5.0, 60.0);
//    button->frame.size = glm::vec2(280, 50);
//    
//    button->font = SResourceManager::getResource<SFont>(SPath("Font/Arial.font"));
//    button->label = "Button 2";
//    button->font_size = 13.0;
//    
//    button->action = &mouseClick;
//    
//    ui_graph->addWidget(button);
//    
//    SUITextField* text_field = new SUITextField();
//    
//    text_field->font = button->font;
//    text_field->font_size = CONSOLE_FONT_SIZE;
//    
//    text_field->background_color = glm::vec4(0.1, 0.1, 0.1, 1.0);
//    
//    text_field->frame.origin = glm::vec2(5.0, 115);
//    text_field->frame.size = glm::vec2(280, 37.5);
//    
//    ui_graph->addWidget(text_field);
	
    SUI::current_ui_graph = ui_graph;
    
    SCursor* cursor = SResourceManager::getResource<SCursor>(SPath("Texture/ui/cursor/pointer.png"));
    cursor->bind();
    
    SEventListener event_listener;
    event_listener.listenToEvent(EVENT_TICK, &update);
    
    // END TEMP CODE
    
    // Clear all the uploads
    SGLUploadSystem::processUploads();
    SGLUploadSystem::setUploadLimitPerFrame(10);
    
    SLog::verboseLog(SVerbosityLevel::Debug, "Startup complete");
    
    // Do the main loop
    SMainLoop::loop();
    
    // Subsystem shutdown
    SConsole::shutdown();
    
    SUI::shutdown();
    STextRenderer::shutdown();
    
    SHotLoadSystem::shutdown();
    SResourceManager::shutdown();
    
    // Write out the log to a file
	SPath log_root_path = SPath(argv[0]);
	
    #ifdef __APPLE__
    
    // Because the root path is in the app bundle, which we cant write to, so we move the path to outside it
    log_root_path.removeLastPathComponent();
    log_root_path.removeLastPathComponent();
    log_root_path.removeLastPathComponent();
    log_root_path.removeLastPathComponent();
    
    #endif
	
	log_root_path.appendPathComponent("Spud Engine.log");
	std::string log_path = log_root_path.getPathAsAbsolutePath();
	
    SFileSystem::shutdown();
    
    SInputSystem::shutdown();
    
    SGLUploadSystem::shutdown();
    
    SPhysicsSystem::shutdown();
    
    STime::shutdown();
    
    SRenderSystem::shutdown();
    SGL::shutdown();
    SSoundSystem::shutdown();
    
    SEventSystem::shutdown();
	
	SLog::writeLogToFile(log_path);
	
    return 0;
}
