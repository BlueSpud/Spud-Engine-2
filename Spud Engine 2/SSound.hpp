//
//  SSound.hpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/30/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SSound_hpp
#define SSound_hpp

#include <AL/al.h>
#include <AL/alc.h>

#include "SResourceManager.hpp"

// Forward declarations
class SSoundEmitter;

/******************************************************************************
 *  Definition for .wav header                                                *
 ******************************************************************************/

struct SSoundWAVHeader {
    
    int file_size, riff_chunk_size;
    int wav_chunk_size, header_length;
    short format, channels; int sample_rate;
    int sample_bits_channels_8; short bits_channels, bits_per_sample;
    int data_length;
    
};

/******************************************************************************
 *  Definition for sound resource                                             *
 ******************************************************************************/

class SSound : public SResource {
    
    friend class SSoundEmitter;
    
    protected:
    
        virtual bool load(const SPath& path);
        virtual void unload();
    
    private:
    
        SSoundWAVHeader wav_header;
        char* data;
    
        int al_format;
        ALuint buffer;

};

#endif /* SSound_hpp */
