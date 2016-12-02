//
//  SSoundInstance.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/30/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SSoundInstance_hpp
#define SSoundInstance_hpp

#include "SSound.hpp"
#include "SSoundSystem.hpp"

/******************************************************************************
 *  Definition for positional sound modes                                     *
 ******************************************************************************/

enum SSoundPositionMode {
    
    SSoundPositionMode3D,
    SSoundPositionMode2D
    
};

/******************************************************************************
 *  Definition for sound instance                                             *
 ******************************************************************************/

class SSoundInstance : public SResource {
    
    friend class SSound;
    
    public:
    
        SSoundPositionMode sound_position_mode;
    
        void play();
        void stop();
    
        void pause();
        void reset();
    
        void setLoops(bool loops);
    
    protected:
    
        // FUNCTIONS SHOULD NEVER BE CALLED
        virtual bool load(const SPath& path);
        virtual void unload();
    
    private:
    
        void updateSoundPosition(const SEvent& event);
        SEventListener event_listener;
    
        bool playing = false;
    
        SSoundInstance(SSound* _parent_sound);
        SSound* parent_sound;
        ALuint source;
    
};

#endif /* SSoundInstance_hpp */
