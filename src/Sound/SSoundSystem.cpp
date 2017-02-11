//
//  SSoundSystem.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/30/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SSoundSystem.hpp"
#include "SCamera.hpp"

ALCdevice* SSoundSystem::device;
ALCcontext* SSoundSystem::context;

glm::vec3 SSoundSystem::listener_position;
glm::vec3 SSoundSystem::listener_velocity;

/******************************************************************************
 *  Implementation for sound system                                           *
 ******************************************************************************/

void SSoundSystem::startup() {
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SSoundSystem startup");
    
    // Get the sound device and if we didnt have a device let us know
    device = alcOpenDevice(NULL);
    if (!device)
        SLog::verboseLog(SVerbosityLevel::Critical, "Failed to create sound device!");
    
    // Create a sound context
    context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);
    
}

void SSoundSystem::shutdown() {
    
    // Audio de-init
    alcDestroyContext(context);
    alcCloseDevice(device);
    
    SLog::verboseLog(SVerbosityLevel::Debug, "SSoundSystem shutdown");
    
}

void SSoundSystem::updateListenerPosition(double interpolation) {
    
    // Calculate position for this frame and save it and velocity together
    listener_position = SCamera::current_camera->transform.translation +
                        SCamera::current_camera->transform.translation_velocity * (float)interpolation;
    
    listener_velocity = SCamera::current_camera->transform.translation_velocity;
    
    // Update the OpenAL listener positon and velocity
    alListenerfv(AL_POSITION, &listener_position[0]);
    alListenerfv(AL_VELOCITY, &listener_velocity[0]);
	
	// Calculate the up vector
	glm::vec3 at_up[] = {SCamera::current_camera->transform.getForwardVector(), glm::vec3()};
	
	float modified_yaw = SCamera::current_camera->transform.rotation.y - M_PI / 2;
	float modified_pitch = SCamera::current_camera->transform.rotation.x + M_PI / 2;
	float cos_pitch = cos(modified_pitch);
	
	at_up[1] = glm::normalize(glm::vec3(cos(modified_yaw) * cos_pitch,
							 sin(modified_pitch),
							 sin(modified_yaw) * cos_pitch));
	
	// Set the orientation
	alListenerfv(AL_ORIENTATION, &at_up[0][0]);
	
	
    // Send out an event so the sound emitters can update their position if they are following the camera
    SSoundEventListenerMove event;
    SEventSystem::postEvent(EVENT_SOUND_SOUND_LISTENER_MOVE, event);
    
}

glm::vec3 SSoundSystem::getListenerPosition() { return listener_position; }
glm::vec3 SSoundSystem::getListenerVelocity() { return listener_velocity; }
