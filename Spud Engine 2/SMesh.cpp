//
//  SMesh.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/10/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SMesh.hpp"

/******************************************************************************
 *  Functions for mesh                                                        *
 ******************************************************************************/

SMesh::SMesh(const SPath& path) {
    
    // Grab the file off of the disk
    SFile* file = SFileSystem::loadFile(path);
    
    if (file) {
        
        SMaterial* mat = nullptr;
        std::map<std::string, STexture*>textures;
        
        // Read line by line and figure out what we need to do for each line, there is not a required order
        std::string line;
        std::string tex_name;
        
        while (file->getNextTokenWithDeliminator('\n', line)) {
            
            if (line.compare(0, 4, "mat:") == 0) {
                
                // Load in the material
                std::string mat_path_s = line.substr(4, line.length() - 4);
                mat = (SMaterial*)SResourceManager::getResource(SPath(mat_path_s));
                
            }
            
            if (line.compare(0, 6, "model:") == 0) {
                
                // We found the model that we needed
                std::string model_path_s = line.substr(6, line.length() - 6);
                model = (SModel*)SResourceManager::getResource(SPath(model_path_s));
                
            }
            
            // Used to get the name of the next texture loading
            if (line.compare(0, 6, "tex_n:") == 0)
                tex_name = line.substr(6, line.length() - 6);
            
            if (line.compare(0, 4, "tex:") == 0) {
                
                // We found a texture to load up
                std::string texture_path_s = line.substr(4, line.length() - 4);
                textures[tex_name] = (STexture*)SResourceManager::getResource(SPath(texture_path_s));

            }
            
        }
        
        // Now that we have everything we need to make the material instance
        mat_instance = mat->createMaterialInstance(textures);
        if (mat_instance == nullptr)
            SLog::verboseLog(SVerbosityLevel::Critical, "Failed to get material instance for mesh: %s", path.getPathAsString().c_str());
        else loaded = true;
        
    } else SLog::verboseLog(SVerbosityLevel::Critical, "Failed to load mesh: %s", path.getPathAsString().c_str());
    
}

SMesh::SMesh(SModel* _model, SMaterialInstance* _mat_instance) {

    // Save everything and check if we are valid
    model = _model;
    mat_instance = _mat_instance;
    
    if (model && mat_instance)
        loaded = true;
    
}

void SMesh::render(double interpolation) {

    // Make sure that we are loaded in
    if (loaded) {
        
        // Get the matrix for the transform and upload it
        glm::mat4 transform_matrix = SGL::transformToMatrix(transform);
        SGL::mulMatrix(transform_matrix, MAT_MODEL_MATRIX);
    
        // Bind the material
        mat_instance->useMaterial();
        
        // Force an upload of the matricies
        SGL::flushMatrix(MAT_PROJECTION_MATRIX);
        SGL::flushMatrix(MAT_MODEL_MATRIX);
        SGL::flushMatrix(MAT_VIEW_MATRIX);
        
        // Upload other uniforms
        SShader* material_shader = mat_instance->getShader();
        
        material_shader->bindUniform(&SGL::view_position, VEC3_VIEW_POSITION, UNIFORM_VEC3, 1);
    
        // Render the model
        model->render();
        
    }

}

void SMesh::update() { /* stub */ }
