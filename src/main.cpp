//
//  main.cpp
//  GLFW Test
//
//  Created by Logan Pazol on 8/16/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SMainLoop.hpp"
#include "Rendering/Pipeline/Renderers/SDeferredRenderingPipeline.hpp"
#include "Resource/SHotLoadSystem.hpp"

#include "UI/Console/SConsole.hpp"
#include "UI/Widget/SUIButton.hpp"
#include "Resource/Resources/Rendering/SCursor.hpp"

#include "Resource/Resources/Rendering/Static Mesh/SStaticMesh.hpp"

#include "Sound/SSoundSystem.hpp"
#include "Sound/SSoundEmitter.hpp"

#include "Physics/SCharacterController.hpp"

#include "Resource/Resources/Rendering/SSkinnedMesh.hpp"
#include "Resource/Resources/Rendering/SAnimation.hpp"

#include "Level/SLevel.hpp"
#include "Rendering/Light/Reflections/SReflectionProbe.hpp"

double speed = 0.0;
double speed_x = 0.0;

SCamera camera;
SLight* light;
SSoundEmitter* sound_emitter;
SCharacterController* controller;
SObject* picked;

void hello(const std::vector<std::string>& args) {
    
    // Test command
    SLog::verboseLog(SVerbosityLevel::Debug, "Howdy!");
    
}

void loadMesh(const std::string& _path) {
	
	SPath path = SPath("Model/" + _path);
	if (SFileSystem::fileExitsAtPath(path)) {
		
		SLog::verboseLog(SVerbosityLevel::Debug, "Spawning mesh %s", _path.c_str());
		
		SStaticMesh* mesh = new SStaticMesh(SResourceManager::getResource<SModel>(path));
		mesh->setTranslation(camera.transform.translation);
	
		SLevelManager::spawnObject(mesh);
		
	} else SLog::verboseLog(SVerbosityLevel::Critical, "Model did not exist %s", _path.c_str());
	
}

void spawnMesh(const std::vector<std::string>& args) { boost::thread thread = boost::thread(&loadMesh, args[0]); }

void loadLevel(const std::vector<std::string>& args) {
	
	SPath path = SPath("Level/" + args[0]);
	if (SFileSystem::fileExitsAtPath(path)) {
	
		// Load a level
		SLevelManager::loadLevel(path);
		
	} else SLog::verboseLog(SVerbosityLevel::Critical, "Level did not exist %s", args[0].c_str());
	
}

void saveLevel(const std::vector<std::string>& args) {
	
	// Save level
	SLevelManager::saveLevel(SPath("Level/test.slevel"));
	
}


void spawnLight(const std::vector<std::string>& args) {

	SPointLight* new_light = new SPointLight();
	new_light->setTranslation(camera.transform.translation);
	
	if (args.size())
		new_light->setRadius(atof(args[0].c_str()));
	
	SLevelManager::spawnObject(new_light);
	
}

REGISTER_COMMAND(hello, &hello);
REGISTER_COMMAND(spawn_mesh, &spawnMesh);
REGISTER_COMMAND(load_level, &loadLevel);
REGISTER_COMMAND(save_level, &saveLevel);
REGISTER_COMMAND(spawn_light, &spawnLight);

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
			if (picked)
				picked->setTranslation(camera.transform.translation);
			
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
    camera.transform.rotation_velocity.y = (float)(mouse_vel.x * 0.01);
    camera.transform.rotation_velocity.x = (float)(-mouse_vel.y * 0.005);
    
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
	
	picked = SLevelManager::getObjectWithId(3);
	
}

int main(int argc, char* argv[]) {
    
    // Set verbosity level
    SLog::setVerbosityLevel(SVerbosityLevel::Debug);
    
    // Subsystem startup
    SEventSystem::startup();
    
    SSoundSystem::startup();
    SGL::startup();
	SGLUploadSystem::startup();
	
	SFileSystem::startup();
	SFileSystem::getDefaultRootDirectory("/Users/Logan/Desktop/Spud Engine 2/a/a/");
	
	SResourceManager::startup();
	SHotLoadSystem::startup();
	
    SRenderSystem::startup();
    
    STime::startup();
    
    SPhysicsSystem::startup();
	
    SInputSystem::startup();
    
    STextRenderer::startup();
    SUI::startup();
    
    SConsole::startup();
	
    // TEMP CODE
	
	//std::shared_ptr<SResource> pointer = std::shared_ptr<SResource>(SResourceManager::getResource<SModel>(SPath("Model/floor.smdl")));
	//std::shared_ptr<SModel> model_pointer = std::dynamic_pointer_cast<SModel>(pointer);
	
	//std::cout << model_pointer.use_count() << std::endl;
	
	camera.transform.translation.y = 2.0;
    SCamera::current_camera = &camera;
	
    std::shared_ptr<SSound> sound = SResourceManager::getResource<SSound>(SPath("Sound/Birds.wav"));
    sound_emitter = new SSoundEmitter();
    sound_emitter->setSound(sound);
    sound_emitter->play();
    sound_emitter->setLoops(true);
    sound_emitter->setTranslation(camera.transform.translation);
	
	// Access the level
	SLevelManager::loadLevel(SPath("Level/test.slevel"));
	
//	SStaticMesh* mesh = new SStaticMesh(SResourceManager::getResource<SModel>(SPath("Model/cerberus.smdl")));
//	mesh->setTranslation(glm::vec3(-1.0, 0.75, 0.0));
//	SLevelManager::spawnObject(mesh);

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
	
	
    glm::ivec2 window_framebuffer_size = SGL::getWindowFramebufferSize();
    
	SViewport viewport_2D = SViewport(window_framebuffer_size / (int)SGL::getScreenScale(), glm::vec2(0.0));
	SViewport screen_viewport = SViewport(window_framebuffer_size, glm::vec2(0.0));
    SViewport3D viewport_3D = SViewport3D(window_framebuffer_size / (int)SGL::getScreenScale(), glm::vec2(0.0), 45.0f, glm::vec2(0.1, 500.0));

	SDeferredRenderingPipeline* deferred = new SDeferredRenderingPipeline(&viewport_2D, &screen_viewport, &viewport_3D);
	SRenderSystem::rendering_pipeline = deferred;
	
//	SViewport viewport_2D_c = SViewport(glm::vec2(1024.0), glm::vec2(0.0));
//	SViewport screen_viewport_c = SViewport(glm::vec2(1024.0), glm::vec2(0.0));
//	SViewport3D viewport_3D_c = SViewport3D(glm::vec2(1024.0), glm::vec2(0.0), M_PI_2, glm::vec2(0.1, 500.0));
//	
//	SRenderSystem::cubemap_pipeline = new SDeferredRenderingPipeline(&viewport_2D_c, &screen_viewport_c, &viewport_3D_c);
	

    SInputListener listener;
    listener.bind(&keyPress, GLFW_KEY_S, INPUT_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_W, INPUT_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_D, INPUT_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_A, INPUT_ACTION_DOWN);
    listener.bind(&keyPress, GLFW_KEY_SPACE, INPUT_ACTION_DOWN);
	
    listener.bind(&keyPress, GLFW_KEY_G, INPUT_ACTION_DOWN);
    
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
    
	std::shared_ptr<SCursor> cursor = SResourceManager::getResource<SCursor>(SPath("Texture/ui/cursor/pointer.png"));
    cursor->bind();
    
    SEventListener event_listener;
    event_listener.listenToEvent(EVENT_TICK, &update);
	
	//controller = new SCharacterController(scene_graph->physics_graph, glm::vec2(0.2, 1.0), &camera.transform);
	//controller->movement_speed = 3.25;

    // END TEMP CODE
    
    // Clear all the uploads
    SGLUploadSystem::processUploads();
    SGLUploadSystem::setUploadLimitPerFrame(10);
	
	// Generate the BRDF
	SRenderSystem::generateBRDF();
    
    SLog::verboseLog(SVerbosityLevel::Debug, "Startup complete");
	
	// Generate a cubemap
	//SCubeMap* map = SRenderSystem::generateCubeMap(glm::vec3(-2.0, 0.2, 0.0));
	//deferred->environment_map = std::shared_ptr<SCubeMap>(map);
	
    // Do the main loop
    SMainLoop::loop();
	
	// TEMP CODE
	
	//SLevelManager::saveLevel(SPath("Level/out.slevel"));
	
	delete sound_emitter;
	//delete controller;
	
	// END TEMP CODE
    
    // Subsystem shutdown
    SConsole::shutdown();
    
    SUI::shutdown();
    STextRenderer::shutdown();
    
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
    
    SInputSystem::shutdown();
	
	// Clear any pending unloads
	SGLUploadSystem::setUploadLimitPerFrame(UPLOADS_INFINITE);
	SGLUploadSystem::processUploads();
    
    SPhysicsSystem::shutdown();
    
    STime::shutdown();
    
    SRenderSystem::shutdown();
	
	SHotLoadSystem::shutdown();
	SResourceManager::shutdown();
	
	SFileSystem::shutdown();
	
	SGLUploadSystem::shutdown();
	SGL::shutdown();
	SSoundSystem::shutdown();
	
    SEventSystem::shutdown();
	
	SLog::writeLogToFile(log_path);
	
    return 0;
}
