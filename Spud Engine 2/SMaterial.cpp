//
//  SMaterial.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SMaterial.hpp"

/******************************************************************************
 *  Registration for supported material extensions                            *
 ******************************************************************************/

REGISTER_RESOURCE_CLASS(mat, SMaterial);

/******************************************************************************
 *  Functions for material instance                                           *
 ******************************************************************************/

void SMaterialInstance::useMaterial() {
    
    // Get the currently bound shader
    SShader* current_shader = SShader::getBoundShader();
    if (current_shader != parent_mat->shader) {
        
        // Bind the textures
        for (int i = 0; i < textures.size(); i++)
            textures[i]->bind(i);
        
        parent_mat->shader->bind();
        
    }
    
}

SMaterialInstance::SMaterialInstance() { /* stub */ }

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
            
            if (line.compare(0, 7, "shader:") == 0) {
                
                // Load up the shader
                std::string shader_path_s = line.substr(7, line.length() - 7);
                SLog::verboseLog(SVerbosityLevel::Debug, "Using shader %s", shader_path_s.c_str());
                
                SPath shader_path = SPath(shader_path_s);
                
                shader = (SShader*)SResourceManager::getResource(shader_path);
                
            }
            
            if (line.compare(0, 4, "req:") == 0) {
                
                // We found a required texture
                std::string tex_name = line.substr(4, line.length() - 4);
                req_textures.push_back(tex_name);
                
            }
            
        }
        
        return true;
        
    }

    return false;

}

SMaterialInstance* SMaterial::createMaterialInstance(std::map<std::string, STexture*>& _textures, int num_textures) {
    
    // Make sure we have enough textures
    if (_textures.size() >= req_textures.size()) {
        
        // Create an instance of this material, so we can use it
        SMaterialInstance* instance = new SMaterialInstance();
        instance->parent_mat = this;
        
        // Make sure what we had is what we need
        for (int i = 0; i < req_textures.size(); i++) {
            
            if (!_textures.count(req_textures[i].c_str())) {
                
                // Clean up
                delete instance;
                
                SLog::verboseLog(SVerbosityLevel::Critical, "Attempted to create material instance with enough, but not the proper textures! Missing: %s", req_textures[i].c_str());
                return nullptr;
                
            } else instance->textures.push_back(_textures[req_textures[i].c_str()]);
            
        }
    
        instances.push_back(instance);
    
        return instance;
        
    }
    
    SLog::verboseLog(SVerbosityLevel::Critical, "Attempted to create material instance without enough textures!");
    return nullptr;
    
}

void SMaterial::upload() {
    
    // Go through the textures and assign Ids for them
    for (int i = 0; i < req_textures[i].size(); i++)
        glUniform1i(SShader::getUniformLocation(shader, req_textures[i].c_str()), 0);
    
}

void SMaterial::unload() {

    // Delete all the instances that we made
    for (int i = 0; i < instances.size(); i++)
        delete instances[i];

}
