//
//  SSoundEmitter.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/30/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SSoundEmitter.hpp"

/******************************************************************************
 *  Implementation for sound emitter                                          *
 ******************************************************************************/

SSoundEmitter::SSoundEmitter() {

    // Listen for the listener changing position
    event_listener.listenToEvent(EVENT_SOUND_SOUND_LISTENER_MOVE, EVENT_MEMBER(SSoundEmitter::updateSoundPosition));

}

SSoundEmitter::SSoundEmitter(SSound* _sound) {
    
    // Keep the parent
    sound = _sound;
    
    // Listen for the listener changing position
    event_listener.listenToEvent(EVENT_SOUND_SOUND_LISTENER_MOVE, EVENT_MEMBER(SSoundEmitter::updateSoundPosition));

    setSound(_sound);

}

SSoundEmitter::~SSoundEmitter() { unload(); }

void SSoundEmitter::setSound(SSound* _sound) {

    // Unload the last source if there was one
    unload();
    
    sound = _sound;
    
    // Generate a new source
    alGenSources(1, &source);
    
    alSourcei(source, AL_BUFFER, sound->buffer);
    
}

void SSoundEmitter::play() {

    // Play the sound and remember we are playing it
    if (!playing) {
        
        alSourcePlay(source);
        playing = true;
        
    }
    
}

void SSoundEmitter::stop() {
    
    // Stop the sound effect and it is no longer playing
    alSourceStop(source);
    playing = false;

}

void SSoundEmitter::pause() {

    // Pause the sound effect and it is no longer playing
    alSourcePause(source);
    playing = false;

}

void SSoundEmitter::reset() {

    // Stop the sound and play it if we were playing
    alSourceStop(source);
    
    if (playing)
        alSourcePlay(source);

}

void SSoundEmitter::setLoops(bool loops) {
    
    // Set if the sound loops
    alSourcei(source, AL_LOOPING, loops);
    
}

void SSoundEmitter::updateSoundPosition(const SEvent& event) {
    
    // Based on the mode that this sound is in, update the OpenAL position and sound accordingly
    if (sound_position_mode == SSoundPositionMode2D) {
        
        // The sound is following the camera, set the same position and velocity as the camera
        alSourcefv(source, AL_POSITION, &SSoundSystem::getListenerPosition()[0]);
        alSourcefv(source, AL_VELOCITY, &SSoundSystem::getListenerVelocity()[0]);
        
    } else {
        
        // The sound is a positional source, get it from the transform
		glm::vec3 translation = getTranslation();
		glm::vec3 velocity = getTranslationVel();
		
        alSourcefv(source, AL_POSITION, &translation[0]);
        alSourcefv(source, AL_VELOCITY, &velocity[0]);
        
    }
}

void SSoundEmitter::unload() {
    
    // If we had a source, we delete it
    if (source)
        alDeleteSources(1, &source);
    
}
