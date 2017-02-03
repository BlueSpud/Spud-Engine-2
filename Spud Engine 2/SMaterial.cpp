//
//  SMaterial.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SMaterial.hpp"

SMaterial* SMaterial::currently_bound_material;

/******************************************************************************
 *  Registration for supported material extensions                            *
 ******************************************************************************/

REGISTER_RESOURCE_CLASS(mat, SMaterial)

/******************************************************************************
 *  Implementation for material                                               *
 ******************************************************************************/

bool SMaterial::load(const SPath& path) {

    SFile* file = SFileSystem::loadFile(path);
    
    if (file) {
        
        // Read line by line and figure out what we need to do for each line, there is not a required order
        std::string line;
        while (file->getNextTokenWithDeliminator('\n', line)) {
            
            if (line.compare(0, 2, "t:") == 0) {
                
                // A texture was specifed, get the name of the texture and the path
                std::string texture_name_and_path = line.substr(2, line.length() - 2);
                int split_index = (int)texture_name_and_path.find(":");
                
                std::string texture_name = texture_name_and_path.substr(0, split_index);
                SPath texture_path = SPath(texture_name_and_path.substr(split_index + 1, texture_name_and_path.length() - split_index));
                
                textures[texture_name] = SResourceManager::getResource<STexture>(texture_path);
                
            }
            
            if (line.compare(0, 2, "s:") == 0) {
                
                // Get the path of the shader and load it up
                SPath shader_path = SPath(line.substr(2, line.length() - 2));
                shader = SResourceManager::getResource<SGbufferShader>(shader_path);
                
            }

            
        }
        
        return true;
        
    }

    return false;

}

void SMaterial::bind(SGbufferShaderShaders shader_t) {
    
    // Make sure the shader is bound
    bool did_bind = shader->bind(shader_t);
    
    // Check if this is already the currently bound material
    if (currently_bound_material != this || did_bind) {
        
        currently_bound_material = this;
        
        // Bind the textures
        int texture = 0;
        for (std::map<std::string, STexture*>::iterator i = textures.begin(); i != textures.end(); i++) {
        
            i->second->bind(texture);
            texture++;
            
        }
        
        // If the shader was bound, reupload texture IDs
        if (did_bind)
            uploadTextureIDs(shader_t);
        
        // Bind the uniforms
        for (int i = 0; i < uniforms.size(); i++)
            shader->bindUniform(shader_t, uniforms[i]);
        
    }
    
}

void SMaterial::uploadTextureIDs(SGbufferShaderShaders shader_t) {
    
    // Go through the textures and assign Ids for them
    int texture = 0;
    for (std::map<std::string, STexture*>::iterator i = textures.begin(); i != textures.end(); i++) {
        
		shader->bindUniform(shader_t, &texture, i->first, UNIFORM_INT, 1);
        texture++;
        
    }
    
    
}

void SMaterial::unload() { /* intentionally empty */ }
