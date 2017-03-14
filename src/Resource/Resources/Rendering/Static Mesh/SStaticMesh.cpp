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
	
	// Keep maxes and mins for boudning box
	parent_mesh->getModelExtents(bounding_box.mins, bounding_box.maxes);
    
}

void SStaticMesh::render(double interpolation) {

    // Set the model matrix to the proper matrix for this model
    glm::mat4 transform_matrix = SGL::transformToMatrix(transform, interpolation);
    SGL::mulMatrix(transform_matrix, MAT_MODEL);
    
    // Call render on the parent model
	if (materials_identical)
		parent_mesh->render(parent_mesh->materials);
	else parent_mesh->render(materials);

}

void SStaticMesh::render(SGbufferShader* shader, double interpolation) {
	
	// Set the model matrix to the proper matrix for this model
	glm::mat4 transform_matrix = SGL::transformToMatrix(transform, interpolation);
	SGL::mulMatrix(transform_matrix, MAT_MODEL);

	parent_mesh->render(shader);
	
}

bool SStaticMesh::shouldBeRendered(const SFrustum& frustum) { return bounding_box.frustrumCull(frustum); }

void SStaticMesh::setMaterial(SMaterial* new_material, int material_domain) {
    
    // Check if we have a material domain for the material we tried to replace
    if (material_domain < materials.size()) {
        
        // Set the new material
        materials[material_domain] = new_material;
		materials_identical = false;
        
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
