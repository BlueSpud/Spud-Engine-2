//
//  SStaticMesh.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 11/7/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SStaticMesh.hpp"

/******************************************************************************
 *  Implementation for static mesh instance                                   *
 ******************************************************************************/

SStaticMesh::SStaticMesh(SModel* _parent_mesh) {
    
    // Store the parent mesh
    parent_mesh = _parent_mesh;
    
    // Copy over the material
    materials = parent_mesh->materials;
    
    // Check if there is a colliison mesh for the parent
    if (parent_mesh->collision_geometry)
        rigid_body = new SRigidBody(0.0, parent_mesh->collision_geometry, PxGetPhysics().createMaterial(0.5, 0.5, 0.1), &transform);
    
}

void SStaticMesh::render(bool render_material, double interpolation) {

    // Set the model matrix to the proper matrix for this model
    glm::mat4 transform_matrix = SGL::transformToMatrix(transform, interpolation);
    SGL::mulMatrix(transform_matrix, MAT_MODEL);
    
    // Call render on the parent model
    parent_mesh->render(render_material, materials);

}

void SStaticMesh::setMaterial(SMaterial* new_material, int material_domain) {
    
    // Check if we have a material domain for the material we tried to replace
    if (material_domain < materials.size()) {
        
        // Set the new material
        materials[material_domain] = new_material;
        
    }
    
}

void SStaticMesh::onMoveToSceneGraph(SPhysicsGraph* physics_graph) {
    
    // Add the rigidbody to the world
    if (rigid_body)
        rigid_body->addToPhysicsGraph(physics_graph);
    
}

void SStaticMesh::onRemoveFromSceneGraph(SPhysicsGraph* physics_graph) {
    
    // Remove the rigidbody to the world
    if (rigid_body)
        rigid_body->removeFromPhysicsGraph(physics_graph);
    
}

void SStaticMesh::update(const SEvent& event) {
    
    // Update the transform
    transform.update();
    
}
