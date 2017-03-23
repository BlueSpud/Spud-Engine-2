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

#include "SSkinnedMesh.hpp"
#include "SAnimation.hpp"

#include "SSerialization.hpp"
#include "SOctreeSceneGraph.hpp"

double speed = 0.0;
double speed_x = 0.0;

SCamera camera;
SLight* light;
SSoundEmitter* sound_emitter;
SCharacterController* controller;

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
			//	    controller->jump();
			
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
	glm::vec3 strafe = glm::vec3(cos(camera.transform.rotation.y) * cos(camera.transform.rotation.x),
								 0.0,
								 sin(camera.transform.rotation.y) * cos(camera.transform.rotation.x)) * (float)speed_x;

	glm::vec3 move_vector = strafe + camera.transform.getForwardVector(0.0) * (float)speed;
	
	if (glm::length(move_vector)) {
		
		camera.transform.translation_velocity = glm::normalize(move_vector) * 0.2f;
		//controller->setMoveDirection(move_vector * 35.0f);
		
	} else {
		
		camera.transform.translation_velocity = glm::vec3(0.0);
		//controller->setMoveDirection(glm::vec3(0.0));
		
	}
    
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
	
//    SSound* sound = SResourceManager::getResource<SSound>(SPath("Sound/Birds.wav"));
//    sound_emitter = new SSoundEmitter();
//    sound_emitter->setSound(sound);
//    sound_emitter->play();
//    sound_emitter->setLoops(true);
//    sound_emitter->transform.translation = camera.transform.translation;
	
    SSimpleSceneGraph* scene_graph = new SSimpleSceneGraph();
	
    SStaticMesh* mesh = new SStaticMesh(SResourceManager::getResource<SModel>(SPath("Model/floor.smdl")));
	mesh->transform.translation.y = 0.4;
	scene_graph->addObject(mesh);
	
	mesh = new SStaticMesh(SResourceManager::getResource<SModel>(SPath("Model/wall.smdl")));
	mesh->transform.scale = glm::vec3(0.25);
	mesh->transform.translation.y = 1.65;
	mesh->transform.translation.z = -1.20;
	mesh->transform.rotation.x = M_PI_2;
	mesh->setMaterial(SResourceManager::getResource<SMaterial>(SPath("Material/blank.mat")), 0);
	//scene_graph->addObject(mesh);
	
//	for (int i = 0; i < 1000; i++) {
//		
//		mesh = new SStaticMesh(SResourceManager::getResource<SModel>(SPath("Model/material_test.smdl")));
//		mesh->setMaterial(SResourceManager::getResource<SMaterial>(SPath("Material/machine_material_test.mat")), 0);
//		mesh->transform.translation.x = rand() % 100;
//		mesh->transform.translation.z = rand() % 100;
//		mesh->transform.translation.y = rand() % 100;
//		scene_graph->addObject(mesh);
//		
//	}
	
	
//	SSerializer serializer;
//	
//	glm::vec2 test = glm::vec2(0.0, 100.0);
//	serializer.addItem(&test);
//	
//	glm::vec2 test1 = glm::vec2(245, 123);
//	serializer.addItem(&test1);
//	
//	SDeserializer deserializer = SDeserializer(serializer.serialize());
//	
//	glm::vec2 test2;
//	deserializer.deserialize(&test2);
//	
//	glm::vec2 test3;
//	deserializer.deserialize(&test3);
//	deserializer.deserialize(&test3);
//	
//	std::cout << test2.x << " " << test2.y << std::endl;
//	std::cout << test3.x << " " << test3.y << std::endl;
	
//	mesh = new SStaticMesh(SResourceManager::getResource<SModel>(SPath("Model/sponza.smdl")));
//	mesh->transform.scale = glm::vec3(0.5);
//	scene_graph->addObject(mesh);
	
//	mesh = new SStaticMesh(SResourceManager::getResource<SModel>(SPath("Model/sponza.smdl")));
//	mesh->transform.scale = glm::vec3(0.5);
//	scene_graph->addObject(mesh);
	
//	SSkinnedMesh* skinned_mesh = new SSkinnedMesh(SResourceManager::getResource<SSkinnedModel>(SPath("Model/ak.smdl")));
//	
//	skinned_mesh->transform.rotation.x = -M_PI_2;
//	skinned_mesh->transform.translation = glm::vec3(0.0, 0.7, -0.5);
//	skinned_mesh->transform.scale = glm::vec3(0.4);
//	
//	scene_graph->addObject(skinned_mesh);
//	
//	SAnimation* animation = SResourceManager::getResource<SAnimation>(SPath("Model/ak_reload.sanim"));
//	skinned_mesh->animation = animation;
//	animation->loops = true;
//	skinned_mesh->timer.start();
	
//	mesh = new SStaticMesh(SResourceManager::getResource<SModel>(SPath("Model/house.smdl")));
//	mesh->transform.translation.z = 2.8;
//	
//	scene_graph->addObject(mesh);
//	
//	mesh = new SStaticMesh(SResourceManager::getResource<SModel>(SPath("Model/material_test.smdl")));
//	mesh->transform.translation.z = -3.4;
//	mesh->transform.translation.y = 0.5;
//
//	scene_graph->addObject(mesh);
//	
//	mesh = new SStaticMesh(SResourceManager::getResource<SModel>(SPath("Model/plane.smdl")));
//	scene_graph->addObject(mesh);
	
    glm::ivec2 window_framebuffer_size = SGL::getWindowFramebufferSize();
    
	SViewport viewport_2D = SViewport(window_framebuffer_size / (int)SGL::getScreenScale(), glm::vec2());
	SViewport screen_viewport = SViewport(window_framebuffer_size, glm::vec2());
    SViewport3D viewport_3D = SViewport3D(window_framebuffer_size / (int)SGL::getScreenScale(), glm::vec2(0), 45.0f, glm::vec2(0.1, 500.0));
    
    // Create the light graph
    SSimpleLightGraph* light_graph = new SSimpleLightGraph();

//	for (int j = -1; j < 1; j++)
//		for (int i = -15; i < 16; i++) {
//		
//			light = new SPointLight();
//			light->transform.translation = glm::vec3(i / 10.0, 0.5, j * 2.0);
//			light->light_color = glm::vec3(rand() % 256 / 256.0, rand() % 256 / 256.0, rand() % 256 / 256.0);
//			
//			light_graph->addLight(light);
//		
//		}

//	light = new SDirectionalLight();
//	light->transform.translation = glm::vec3(0.0, 1.5, 0.0);
//	light->transform.rotation = glm::vec3(-0.541348, 7.37523, 0.0);
//	light->light_color = glm::vec3(0.5);
//	light->casts_shadow = true;
//
//	light_graph->addLight(light);
	
	light = new SSpotLight();
	light->transform.translation = glm::vec3(-5.0, 1.0, 0.0);
	light->casts_shadow = true;
	light->setRadius(10.0);
	//light_graph->addLight(light);
	
	// Test
	SSerializer s;
	light->serialize(s);
	mesh->serialize(s);
	SSerializedData* data = s.serialize();
	
	SDeserializer d = SDeserializer(data);
	size_t hash;
	d.deserialize(&hash);
	
	light = SLevelFactory::allocate<SLight>(hash);
	light->deserialize(d);
	light_graph->addLight(light);
	
	d.deserialize(&hash);
	
	mesh = SLevelFactory::allocate<SStaticMesh>(hash);
	mesh->deserialize(d);
	scene_graph->addObject(mesh);


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
//    listener.bind(&SConsole::activate, GLFW_KEY_GRAVE_ACCENT, INPUT_ACTION_UP);
    
    listener.bind(&keyRelease, GLFW_KEY_S, INPUT_ACTION_UP);
    listener.bind(&keyRelease, GLFW_KEY_W, INPUT_ACTION_UP);
    listener.bind(&keyRelease, GLFW_KEY_D, INPUT_ACTION_UP);
    listener.bind(&keyRelease, GLFW_KEY_A, INPUT_ACTION_UP);
    
    listener.bind(&mouseClick, GLFW_MOUSE_BUTTON_LEFT, INPUT_ACTION_DOWN);
    listener.bind(&mouseClick, GLFW_MOUSE_BUTTON_RIGHT, INPUT_ACTION_DOWN);
    listener.mouse_move_func = mouseMove;
    
    listener.setHasFocus();
    
//    SUIGraph* ui_graph = new SUIGraph();
//    
//    SUIButton* button = new SUIButton();
//    
//    button->background_image = SResourceManager::getResource<STexture>(SPath("Texture/ui/button/button_normal.dds"));
//    button->hover_image = SResourceManager::getResource<STexture>(SPath("Texture/ui/button/button_hover.dds"));
//    button->press_image = SResourceManager::getResource<STexture>(SPath("Texture/ui/button/button_press.dds"));
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
//    button->background_image = SResourceManager::getResource<STexture>(SPath("Texture/ui/button/button_normal.dds"));
//    button->hover_image = SResourceManager::getResource<STexture>(SPath("Texture/ui/button/button_hover.dds"));
//    button->press_image = SResourceManager::getResource<STexture>(SPath("Texture/ui/button/button_press.dds"));
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
//	
//    SUI::current_ui_graph = ui_graph;
    
    SCursor* cursor = SResourceManager::getResource<SCursor>(SPath("Texture/ui/cursor/pointer.png"));
    cursor->bind();
    
    SEventListener event_listener;
    event_listener.listenToEvent(EVENT_TICK, &update);
	
	//controller = new SCharacterController(scene_graph->physics_graph, glm::vec2(0.2, 1.0), &camera.transform);
	//controller->movement_speed = 3.25;
	
//	mesh = new SStaticMesh(SResourceManager::getResource<SModel>(SPath("Model/model.smdl")));
//	mesh->transform.translation.y = 0.0;
//	scene_graph->addObject(mesh);
	
    // END TEMP CODE
    
    // Clear all the uploads
    SGLUploadSystem::processUploads();
    SGLUploadSystem::setUploadLimitPerFrame(10);
    
    SLog::verboseLog(SVerbosityLevel::Debug, "Startup complete");
    
    // Do the main loop
    SMainLoop::loop();
	
	// TEMP CODE
	
	delete sound_emitter;
	//delete controller;
	
	// END TEMP CODE
    
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
	std::string log_path = log_root_path.getPathAsString();
	
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
