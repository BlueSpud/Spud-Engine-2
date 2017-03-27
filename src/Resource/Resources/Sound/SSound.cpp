//
//  SSound.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/30/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SSound.hpp"

/******************************************************************************
 *  Registration for supported sound extensions                               *
 ******************************************************************************/

REGISTER_RESOURCE_CLASS(wav, SSound)

/******************************************************************************
 *  Function for sound resource                                               *
 ******************************************************************************/

bool SSound::load(const SPath& path) {
    
    SFile* wav_file = SFileSystem::loadFile(path, true);
    
    if (wav_file) {
    
        // Create an array for the various 4 byte strings we encounter
        char string_array[4];
    
        // Read the header
        // ALL VALUES ARE IN BYTES ACCORDING TO THE WAV SPECIFICATION
        // Get RIFF header
        wav_file->read(&string_array[0], 4);
    
        // Get WAVE header
        wav_file->read(&wav_header.riff_chunk_size, 4);
        wav_file->read(&string_array[0], 4);
    
        wav_file->read(&wav_header.wav_chunk_size, 4);
        wav_file->read(&wav_header.header_length, 4);
    
        // Read the format, must be PCM so it must be 1
        wav_file->read(&wav_header.format, 2);
        if (wav_header.format != 1) {
        
            SLog::verboseLog(SVerbosityLevel::Critical, "%s was not a PCM file!", path.getPathAsString().c_str());
            return false;
        
        }
    
        // Read sample and channel information
        wav_file->read(&wav_header.channels, 2);
        wav_file->read(&wav_header.sample_rate, 4);
        wav_file->read(&wav_header.sample_bits_channels_8, 4);
        wav_file->read(&wav_header.bits_channels, 2);
        wav_file->read(&wav_header.bits_per_sample, 2);
        
        // Read the data header
        wav_file->read(&string_array[0], 4);
        wav_file->read(&wav_header.data_length, 4);
    
        // Create an array for all the data and read it
        data = new char[wav_header.data_length];
        wav_file->read(data, wav_header.data_length);
    
        // Get the format for OpenAL based on channels and bits per sample
        al_format = -1;
        if (wav_header.channels == 1) {
        
            // 1 channel, mono
            if (wav_header.bits_per_sample == 8)
                al_format = AL_FORMAT_MONO8;
            if (wav_header.bits_per_sample)
                al_format = AL_FORMAT_MONO16;
        
        
        } else {
        
            // 2 channels, stereo
            if (wav_header.bits_per_sample == 8)
                al_format = AL_FORMAT_STEREO8;
            if (wav_header.bits_per_sample)
                al_format = AL_FORMAT_STEREO16;
        
        }
    

        // Create an OpenAL buffer and delete the raw data
        alGenBuffers(1, &buffer);
        alBufferData(buffer, al_format, data, wav_header.data_length, wav_header.sample_rate);
        delete data;
		
		// Set that we are done with the file
		wav_file->endUse();
		
        return true;
        
    }
    
    return false;
    
}

void SSound::unload() { alDeleteBuffers(1, &buffer); }
