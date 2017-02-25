//
//  SSkinnedModel.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 2/8/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "SSkinnedModel.hpp"

/******************************************************************************
 *  Registration for supported model extensions                               *
 ******************************************************************************/

REGISTER_RESOURCE_CLASS(smdl, SSkinnedModel)

/******************************************************************************
 *  Implementation for skinned model                                          *
 ******************************************************************************/

void SSkinnedModel::render(const std::vector<SMaterial*>& instance_material, const std::vector<glm::mat4>& matricies) {
	
	// Bind the array and then render
	glBindVertexArray(array_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicies_id);
	
	int sum = 0;
	for (int i = 0; i < draw_calls.size(); i++) {
		
		// Bind the material
		instance_material[i]->bind(SGbufferShaderSkinned);
		
		// Force an upload of the matricies, needs to be done for every material just in case there is a shader change
		SGL::flushMatrix(MAT_PROJECTION);
		SGL::flushMatrix(MAT_MODEL);
		SGL::flushMatrix(MAT_VIEW);
		
		SGbufferShader* shader = instance_material[i]->getShader();
		
		const void* data = matricies.data();
		shader->bindUniform(SGbufferShaderSkinned, const_cast<void*>(data), "bones", UNIFORM_MAT4, (int)matricies.size());
		
		// render the array with an offset based on what we have already rendern
		glDrawElements(GL_TRIANGLES, draw_calls[i], GL_UNSIGNED_INT, (GLubyte*)(sizeof(unsigned int) * sum));
		sum += draw_calls[i];
		
	}
	
}

void SSkinnedModel::render(SGbufferShader* shader, const std::vector<glm::mat4>& matricies) {
	
	// Bind the array and then render
	glBindVertexArray(array_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicies_id);
	
	// Bind the shader
	shader->bind(SGbufferShaderSkinned);
	
	// Force an upload of the matricies, needs to be done for every material just in case there is a shader change
	SGL::flushMatrix(MAT_PROJECTION);
	SGL::flushMatrix(MAT_MODEL);
	SGL::flushMatrix(MAT_VIEW);
	
	// Upload the data
	const void* data = matricies.data();
	shader->bindUniform(SGbufferShaderSkinned, const_cast<void*>(data), "bones", UNIFORM_MAT4, (int)matricies.size());
		
	// render the array with an offset based on what we have already rendern
	glDrawElements(GL_TRIANGLES, index_count * 3, GL_UNSIGNED_INT, 0);
	
}

bool SSkinnedModel::load(const SPath& path) {
	
	// Load up the model file
	if (SModel::load(path)) {

	
		// Get the path to the skeleton
		SPath new_path = SPath(path);
	
		std::string file_name = new_path.getFilename();
		new_path.removeLastPathComponent();
		new_path.appendPathComponent(file_name + ".srig");
		
		SFile* file = SFileSystem::loadFile(new_path);
		if (file) {
			
			// Load up the skinning information
			bone_indicies = new glm::vec4[vertex_count];
			vertex_weights = new glm::vec4[vertex_count];
			
			// Read the bind pose
			file->read(&bind_pose, sizeof(glm::mat4));
			
			// Read the number of bones
			unsigned int bone_count;
			file->read(&bone_count, sizeof(unsigned int));
			
			for (int i = 0; i < bone_count; i++) {
				
				// Read the bone
				bones.push_back(SBone());
				SBone& bone = bones.back();
				
				// Read parent index
				file->read(&bone.parent_index, sizeof(int));
				
				// Read bind matrix
				file->read(&bone.bind_matrix, sizeof(glm::mat4));
				
				// Read transform matrix
				file->read(&bone.matrix, sizeof(glm::mat4));
				
			}
			
			// Parse the vertex weights
			for (int i = 0; i < vertex_count; i++) {
				
				// Read indicies
				file->read(&bone_indicies[i], sizeof(glm::vec4));
				
				// Read weight
				file->read(&vertex_weights[i], sizeof(glm::vec4));
				
			}
			
			// Change the upload to use the right number of buffers
			upload->m_buffer_count = m_buffer_count = buffer_count;
			upload->bone_indicies = bone_indicies;
			upload->vertex_weights = vertex_weights;
			
			return true;
			
		}
		
	}
	
	return false;
	
}

void SSkinnedModel::unload() {
	
	if (!uploaded) {
		
		// Cancel the uploads and delete the buffers
		upload->canceled = true;
		
		delete bone_indicies;
		delete vertex_weights;
		
	}
	
	// Unload the regular model
	SModel::unload();
	
}

void SSkinnedModel::hotload(const SPath& path) {}

