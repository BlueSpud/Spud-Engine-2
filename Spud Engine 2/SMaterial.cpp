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

REGISTER_RESOURCE_CLASS(mat, SMaterial);

/******************************************************************************
 *  Functions for material                                                    *
 ******************************************************************************/

SResource* SMaterial::allocate() { return new SMaterial(); }

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
                
                textures[texture_name] = (STexture*)SResourceManager::getResource(texture_path);
                
            }
            
            if (line.compare(0, 2, "s:") == 0) {
                
                // Get the path of the shader and load it up
                SPath shader_path = SPath(line.substr(2, line.length() - 2));
                shader = (SShader*)SResourceManager::getResource(shader_path);
                
            }

            
        }
        
        return true;
        
    }

    return false;

}

void SMaterial::bind() {
    
    // Check if this is already the currently bound material
    //if (currently_bound_material != this) {
        
        currently_bound_material = this;
        
        // Bind the textures
        std::map<std::string, STexture*>::iterator i = textures.begin();
        int texture = 0;
        
        while (i != textures.end()) {
            
            i->second->bind(texture);
            
            texture++;
            i++;
            
        }
        
        // Make sure the shader is bound
        bool did_bind = shader->bind();
        
        // If the shader was bound, reupload texture IDs
        if (did_bind)
            uploadTextureIDs(shader);
        
        // Bind the uniforms
        for (int i = 0; i < uniforms.size(); i++)
            shader->bindUniform(uniforms[i]);
        
    // }
    
}

void SMaterial::uploadTextureIDs(SShader* shader) {
    
    // Go through the textures and assign Ids for them
    std::map<std::string, STexture*>::iterator i = textures.begin();
    int texture = 0;
    
    while (i != textures.end()) {
        
        glUniform1i(SShader::getUniformLocation(shader, i->first), texture);
        
        texture++;
        i++;
        
    }
    
    
}

void SMaterial::unload() { /* stub */ }
