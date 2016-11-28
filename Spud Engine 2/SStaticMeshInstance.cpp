//
//  SStaticMeshInstance.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/7/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SStaticMeshInstance.hpp"

/******************************************************************************
 *  Functions for static mesh instance                                        *
 ******************************************************************************/

SStaticMeshInstance::SStaticMeshInstance(SStaticMesh* _parent_mesh) {
    
    // Store the parent mesh
    parent_mesh = _parent_mesh;
    
    // Copy over the material
    materials = parent_mesh->materials;
    
}

void SStaticMeshInstance::render(bool render_material, double interpolation) {

    // Set the model matrix to the proper matrix for this model
    glm::mat4 transform_matrix = SGL::transformToMatrix(transform, interpolation);
    SGL::mulMatrix(transform_matrix, MAT_MODEL_MATRIX);
    
    // Call render on the parent model
    parent_mesh->render(render_material, materials);

}

void SStaticMeshInstance::setMaterial(SMaterial* new_material, int material_domain) {
    
    // Check if we have a material domain for the material we tried to replace
    if (material_domain < materials.size()) {
        
        // Set the new material
        materials[material_domain] = new_material;
        
    }
    
}

void SStaticMeshInstance::update() { /* intentionally empty */ }
bool SStaticMeshInstance::load(const SPath& path) { return true; }
void SStaticMeshInstance::unload() { /* intentionally empty */ }
