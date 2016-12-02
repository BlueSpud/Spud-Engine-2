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

bool SSoundInstance::load(const SPath& path) { return true; }
void SSoundInstance::unload() { /* intentionally empty */ }
