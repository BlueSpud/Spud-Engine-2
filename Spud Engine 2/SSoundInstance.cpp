//
//  SSoundInstance.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/30/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SSoundInstance.hpp"

SSoundInstance::SSoundInstance(SSound* _parent_sound) {
    
    // Keep the parent
    parent_sound = _parent_sound;
    
    // Listen for the listener changing position
    event_listener.listenToEvent(EVENT_SOUND_SOUND_LISTENER_MOVE, boost::bind(&SSoundInstance::updateSoundPosition, this, _1));
    
    // Generate a sound source
    alGenSources(1, &source);

    alSourcei(source, AL_BUFFER, parent_sound->buffer);

}

void SSoundInstance::play() {

    // Play the sound and remember we are playing it
    if (!playing) {
        
        alSourcePlay(source);
        playing = true;
        
    }
    
}

void SSoundInstance::stop() {
    
    // Stop the sound effect and it is no longer playing
    alSourceStop(source);
    playing = false;

}

void SSoundInstance::pause() {

    // Pause the sound effect and it is no longer playing
    alSourcePause(source);
    playing = false;

}

void SSoundInstance::reset() {

    // Stop the sound and play it if we were playing
    alSourceStop(source);
    
    if (playing)
        alSourcePlay(source);

}

void SSoundInstance::setLoops(bool loops) {
    
    // Set if the sound loops
    alSourcei(source, AL_LOOPING, loops);
    
}

void SSoundInstance::updateSoundPosition(const SEvent& event) {
    
    // Based on the mode that this sound is in, update the OpenAL position and sound accordingly
    if (sound_position_mode == SSoundPositionMode2D) {
        
        // The sound is following the camera, set the same position and velocity as the camera
        alSourcefv(source, AL_POSITION, &SSoundSystem::getListenerPosition()[0]);
        alSourcefv(source, AL_VELOCITY, &SSoundSystem::getListenerVelocity()[0]);
        
    }
    
}

bool SSoundInstance::load(const SPath& path) { return true; }
void SSoundInstance::unload() { /* intentionally empty */ }
