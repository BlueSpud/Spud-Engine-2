//
//  SSoundEmitter.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/30/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SSoundEmitter_hpp
#define SSoundEmitter_hpp

#include "SObject.hpp"

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
 *  Definition for sound emitter                                              *
 ******************************************************************************/

class SSoundEmitter : public SObject {
    
    friend class SSound;
    
    public:
    
        SSoundEmitter();
        SSoundEmitter(SSound* _sound);
        virtual ~SSoundEmitter();
    
        void setSound(SSound* _sound);
    
        virtual void render(bool render_material, double interpolation) { /* intentionally blank */ };
        virtual void update(const SEvent& event) { /* intentionally blank */ };
    
        void play();
        void stop();
    
        void pause();
        void reset();
    
        void setLoops(bool loops);
    
        SSoundPositionMode sound_position_mode = SSoundPositionMode3D;
    
    private:
    
        void unload();
    
        void updateSoundPosition(const SEvent& event);
        SEventListener event_listener;
    
        bool playing = false;
        ALuint source;
        SSound* sound;
    
};

#endif /* SSoundEmitter_hpp */
