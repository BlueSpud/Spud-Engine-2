//
//  SModel.cpp
//  Spud Engine 2
//
//  Created by Logan Pazol on 9/2/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SModel.hpp"

/******************************************************************************
 *  Registration for supported model extensions                               *
 ******************************************************************************/

REGISTER_RESOURCE_CLASS(smdl, SModel)

/******************************************************************************
 *  Implementation for model                                                  *
 ******************************************************************************/

void SModel::render(const std::vector<SMaterial*>& instance_material) {
    
    // Bind the array and then render
    glBindVertexArray(array_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicies_id);
    
    int sum = 0;
    for (int i = 0; i < draw_calls.size(); i++) {
		
		instance_material[i]->bind(SGbufferShaderStatic);
        
        // Force an upload of the matricies, needs to be done for every material just in case there is a shader change
        SGL::flushMatrix(MAT_PROJECTION);
        SGL::flushMatrix(MAT_MODEL);
        SGL::flushMatrix(MAT_VIEW);
        
        // Render the array with an offset based on what we have already rendered
        glDrawElements(GL_TRIANGLES, draw_calls[i], GL_UNSIGNED_INT, (GLubyte*)(sizeof(unsigned int) * sum));
        sum += draw_calls[i];
        
    }
    
    // Dont need to unbind becasue the next thing that renders should bind
    //glBindVertexArray(0);
    
}

void SModel::render(SGbufferShader* shader) {
	
	// Bind the array and then render
	glBindVertexArray(array_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicies_id);
	
	// Bind the shader
	shader->bind(SGbufferShaderStatic);
	
	// Force an upload of the matricies, needs to be done for every material just in case there is a shader change
	SGL::flushMatrix(MAT_PROJECTION);
	SGL::flushMatrix(MAT_MODEL);
	SGL::flushMatrix(MAT_VIEW);
	
	// Render the array, because we are using a single shader, we dont need to use the draw calls for the materials
	glDrawElements(GL_TRIANGLES, index_count * 3, GL_UNSIGNED_INT, 0);
	
}

void SModel::getModelExtents(glm::vec3& _mins, glm::vec3& _maxes) {
    
    // Save out the extents of the model
    _mins = mins;
    _maxes = maxes;
    
}

bool SModel::load(const SPath& path) {
    
    file = SFileSystem::loadFile(path, true);
    
    if (file != nullptr) {
    
        // Read the header, the min and max of the bounding box
        glm::vec3 extents[2];
        file->read(extents, sizeof(glm::vec3) * 2);
        mins = extents[0];
        maxes = extents[1];
        
        // Grab the face count and vertex count
        file->read(&vertex_count, sizeof(unsigned int));
        
        // Allocate the arrays for the data
        verts = new glm::vec3[vertex_count];
        normals = new glm::vec3[vertex_count];
        tex_coords = new glm::vec2[vertex_count];
        tangents = new glm::vec3[vertex_count];
        
        // Read the verticies
        for (int i = 0; i < vertex_count; i++) {
                
            // Read position
            file->read(&verts[i], sizeof(glm::vec3));
                
            // Read normal
            file->read(&normals[i], sizeof(glm::vec3));
                
            // Read tangent
            file->read(&tangents[i], sizeof(glm::vec3));
                
            // Read tex coord
            file->read(&tex_coords[i], sizeof(glm::vec2));
                
        }
        
        // Create a vector for the indicies
        indicies = new std::vector<glm::ivec3>();
        
        while (true) {
            
            // Read the next token
            int token;
            file->read(&token, sizeof(int));
            
            // If end of file was reached, we are done
            if (token == END_OF_FILE_TOKEN)
                break;
            
            if (token == NEW_MATERIAL_TOKEN) {
                
                // Read the material name that we need
                unsigned int material_name_length;
                file->read(&material_name_length, sizeof(unsigned int));
                
                // Get the path of the material
                char* material_path_s = new char[material_name_length + 1];
                file->read(material_path_s, sizeof(char) * material_name_length);
                
                std::string material_path_std = material_path_s;
                material_path_std = material_path_std.substr(0, material_name_length);
                delete[] material_path_s;
                
                SPath material_path = SPath(material_path_std);
                materials.push_back(SResourceManager::getResource<SMaterial>(material_path));
                
                // Read the index count
                unsigned int index_count;
                file->read(&index_count, sizeof(unsigned int));
                
                // Save the number of indicies
                draw_calls.push_back(index_count * 3);
                
                for (int j = 0; j < index_count; j++) {
                    
                    // Read the indicies
                    indicies->push_back(glm::ivec3());
                    file->read(&indicies->back(), sizeof(glm::ivec3));
                }
                
            }
            
            if (token == COLLISION_TOKEN) {
                
                // Read the number of verticies
                unsigned int collision_vertex_count;
                file->read(&collision_vertex_count, sizeof(unsigned int));
                
                physx::PxVec3* collision_verticies = new physx::PxVec3[collision_vertex_count];
                
                for (int i = 0; i < collision_vertex_count; i++)
                    file->read(&collision_verticies[i], sizeof(physx::PxVec3));
                
                // Read the indicies
                unsigned int collision_index_count;
                file->read(&collision_index_count, sizeof(unsigned int));
                
                physx::PxU32* collision_indicies = new physx::PxU32[collision_index_count * 3];
                
                for (int i = 0; i < collision_index_count * 3; i++)
                    file->read(&collision_indicies[i], sizeof(physx::PxU32));
                
                // Create the mesh for PhysX
                physx::PxTriangleMeshDesc mesh_desc;
                mesh_desc.points.count = collision_vertex_count;
                mesh_desc.points.data = collision_verticies;
                mesh_desc.points.stride = sizeof(physx::PxVec3);
                
                mesh_desc.triangles.count = collision_index_count;
                mesh_desc.triangles.data = collision_indicies;
                mesh_desc.triangles.stride = sizeof(physx::PxU32) * 3;
                
                // Cook the mesh
                physx::PxDefaultMemoryOutputStream cooked_buffer;
                if (SPhysicsSystem::getCooking()->cookTriangleMesh(mesh_desc, cooked_buffer)) {
                    
                    // Cooking success, create the final meshs
                    physx::PxDefaultMemoryInputData read_buffer = physx::PxDefaultMemoryInputData(cooked_buffer.getData(),
                                                                                                  cooked_buffer.getSize());
                    
                    collision_mesh = PxGetPhysics().createTriangleMesh(read_buffer);
                    collision_geometry = new physx::PxTriangleMeshGeometry(collision_mesh);
                    
                }
                
                // Create a convex mesh for dynamic collision
                physx::PxConvexMeshDesc convex_desc;
                convex_desc.points.count = collision_vertex_count;
                convex_desc.points.data = collision_verticies;
                convex_desc.points.stride = sizeof(physx::PxVec3);
                convex_desc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
                convex_desc.vertexLimit = 255;
                
                // Cook the dynamic mesh
                physx::PxDefaultMemoryOutputStream convex_cooked_buffer;
                if (SPhysicsSystem::getCooking()->cookConvexMesh(convex_desc, convex_cooked_buffer)) {
                    
                    // Cooking sucess
                    physx::PxDefaultMemoryInputData read_buffer = physx::PxDefaultMemoryInputData(convex_cooked_buffer.getData(),
                                                                                                  convex_cooked_buffer.getSize());
                    
                    dynamic_collision_mesh = PxGetPhysics().createConvexMesh(read_buffer);
                    dynamic_collision_geometry = new physx::PxConvexMeshGeometry(dynamic_collision_mesh);
                    
                }
                
                // Clean up
                delete [] collision_verticies;
                delete [] collision_indicies;
                
            }
            
        }
		
        // Create an upload
        upload = new SModelUpload();
        upload->verts = verts;
        upload->normals = normals;
        upload->tex_coords = tex_coords;
        upload->tangents = tangents;
        
        upload->indicies = indicies;
		upload->indicies_id = &indicies_id;
        
        upload->face_count = index_count = (unsigned int)indicies->size();
        upload->vertex_count = (unsigned int)vertex_count;
		
		// Subtract 2 so we dont have the skinning data
		upload->m_buffer_count = m_buffer_count = buffer_count - SKINNING_DATA_COUNT;
		
        upload->array_id = &array_id;
        upload->buffer_ids = buffer_ids;
        upload->uploaded = &uploaded;
        
        SGLUploadSystem::addUpload(upload);

        return true;
        
    }

    return false;
    
}

void SModel::unload() {

    // Check if we havent already deleted these already, we might not have uploaded it yet
    if (uploaded) {
        
        // We had already uploaded the GPU
        glDeleteBuffers(m_buffer_count, buffer_ids);
		glDeleteBuffers(1, &indicies_id);
        glDeleteVertexArrays(1, &array_id);
        
    }
    
    delete collision_geometry;
    delete dynamic_collision_geometry;

}

void SModel::hotload(const SPath& path) {
    
    // Delete the last texture
    if (uploaded) {
        
        // Send a deletion command
        SModelUnload* unload = new SModelUnload();
        unload->array_id = array_id;
        
        // Copy the array
        for (int i = 0; i < buffer_count; i++)
            unload->buffer_ids[i] = buffer_ids[i];
		
		unload->buffer_count = m_buffer_count;
		unload->indicies_id = indicies_id;
        
        SGLUploadSystem::addUpload(unload);
        
    } else {
        
        // Cancel the upload we had already sent
        upload->canceled = true;
        
        // Free the stuff we have
        upload->unload();
        
    }
	
	// General clean up
	draw_calls.clear();
	materials.clear();
	
	delete collision_geometry;
	delete dynamic_collision_geometry;
	
    // Close the old file and then load the new one
    SFileSystem::closeFile(file);
    load(path);
    
}

/******************************************************************************
 *  Implementation for model upload                                           *
 ******************************************************************************/

void SModelUpload::upload() {

    // Generate the buffer for indicies
    
    // Generate the array and the buffers
    glGenVertexArrays(1, array_id);
    glGenBuffers(m_buffer_count, buffer_ids);
	glGenBuffers(1, indicies_id);
    
    // Bind the array and fill the buffers
    glBindVertexArray(*array_id);
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[buffer_position]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_count, verts, GL_STATIC_DRAW);
    
    // Tell OpenGL how to read this
    glEnableVertexAttribArray(buffer_position);
    glVertexAttribPointer(buffer_position, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[buffer_normal]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_count, normals, GL_STATIC_DRAW);

    // Tell OpenGL how to read this
    glEnableVertexAttribArray(buffer_normal);
    glVertexAttribPointer(buffer_normal, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[buffer_tex_coord]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertex_count, tex_coords, GL_STATIC_DRAW);
    
    // Tell OpenGL how to read this
    glEnableVertexAttribArray(buffer_tex_coord);
    glVertexAttribPointer(buffer_tex_coord, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    
    // Tell OpenGL how to read this
    glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[buffer_tangent]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_count, tangents, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(buffer_tangent);
    glVertexAttribPointer(buffer_tangent, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	
	// If this is a SSkinnedMesh, m_buffer_count will be buffer_count because it includes the skinning data
	if (m_buffer_count == buffer_count) {
		
		glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[buffer_bone_indicies]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * vertex_count, bone_indicies, GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(buffer_bone_indicies);
		glVertexAttribPointer(buffer_bone_indicies, 4, GL_FLOAT, GL_FALSE, 0, NULL);
		
		// Fill up the indicies buffer
		glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[buffer_weights]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * vertex_count, vertex_weights, GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(buffer_weights);
		glVertexAttribPointer(buffer_weights, 4, GL_FLOAT, GL_FALSE, 0, NULL);
		
	}
	
	// Fill up the indicies buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *indicies_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * face_count, indicies->data(), GL_STATIC_DRAW);
	
	//glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    unload();

    // Make sure we know if we uploaded
    *uploaded = true;

}

void SModelUpload::unload() {
    
    // Delete the buffers in regular RAM
    delete verts;
    delete normals;
    delete tex_coords;
    delete tangents;
    delete indicies;
	
	// If we had skinning data, delete it
	if (bone_indicies) {
		
		delete bone_indicies;
		delete vertex_weights;
		
	}
    
}

/******************************************************************************
 *  Implementation for model unload                                           *
 ******************************************************************************/

void SModelUnload::upload() {
    
    // Delete model data
    glDeleteBuffers(buffer_count, buffer_ids);
	glDeleteBuffers(1, &indicies_id);
    glDeleteVertexArrays(1, &array_id);
}

void SModelUnload::unload() { /* nothing */ }
