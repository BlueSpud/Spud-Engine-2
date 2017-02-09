//
//  SSkinnedMesh.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 2/8/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "SSkinnedMesh.hpp"

SSkinnedMesh::SSkinnedMesh(SSkinnedModel* _parent_mesh) {
	
	// Store the parent mesh
	parent_mesh = _parent_mesh;
	
	// Copy over the material
	materials = parent_mesh->materials;
	
}

void SSkinnedMesh::render(bool render_material, double interpolation) {
	
	// Set the model matrix to the proper matrix for this model
	glm::mat4 transform_matrix = SGL::transformToMatrix(transform, interpolation);
	SGL::mulMatrix(transform_matrix, MAT_MODEL);
	
	// Calculate the matricies
	std::vector<glm::mat4> matricies;
	
	for (int i = 0; i < parent_mesh->bones.size(); i++) {
		
		// Calculate the world matrix (bone * parent) then calculate the full matrix with the bind pose
		glm::mat4 world_matrix = getMatrixForBone(i, timer.getTime());
		
		int parent_index = parent_mesh->bones[i].parent_index;
		if (parent_index != -1)
			world_matrix = world_matrix * matricies[parent_mesh->bones[i].parent_index];
		
		matricies.push_back(world_matrix);
		
	}
	
	// Go through and multiply by the bind pose
	for (int i = 0; i < parent_mesh->bones.size(); i++)
		matricies[i] = parent_mesh->bind_pose * parent_mesh->bones[i].bind_matrix * matricies[i];
	
	// Call render on the parent model
	parent_mesh->render(render_material, materials, matricies);
	
}

void SSkinnedMesh::update(const SEvent& event) {
	
	// Update the transform
	transform.update();
	
}

glm::mat4 SSkinnedMesh::getMatrixForBone(int bone, float time) {
	
	// If we have an animation, we can get it from the matrix, otherwise we use the default
	if (animation)
		return animation->getMatrixForBone(bone, time);
	else return parent_mesh->bones[bone].matrix;
	
}
